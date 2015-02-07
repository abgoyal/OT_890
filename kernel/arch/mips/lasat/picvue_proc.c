/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/*
 * Picvue PVC160206 display driver
 *
 * Brian Murphy <brian.murphy@eicon.com>
 *
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>

#include <linux/proc_fs.h>
#include <linux/interrupt.h>

#include <linux/timer.h>
#include <linux/mutex.h>

#include "picvue.h"

static DEFINE_MUTEX(pvc_mutex);
static char pvc_lines[PVC_NLINES][PVC_LINELEN+1];
static int pvc_linedata[PVC_NLINES];
static struct proc_dir_entry *pvc_display_dir;
static char *pvc_linename[PVC_NLINES] = {"line1", "line2"};
#define DISPLAY_DIR_NAME "display"
static int scroll_dir, scroll_interval;

static struct timer_list timer;

static void pvc_display(unsigned long data)
{
	int i;

	pvc_clear();
	for (i = 0; i < PVC_NLINES; i++)
		pvc_write_string(pvc_lines[i], 0, i);
}

static DECLARE_TASKLET(pvc_display_tasklet, &pvc_display, 0);

static int pvc_proc_read_line(char *page, char **start,
			     off_t off, int count,
			     int *eof, void *data)
{
	char *origpage = page;
	int lineno = *(int *)data;

	if (lineno < 0 || lineno > PVC_NLINES) {
		printk(KERN_WARNING "proc_read_line: invalid lineno %d\n", lineno);
		return 0;
	}

	mutex_lock(&pvc_mutex);
	page += sprintf(page, "%s\n", pvc_lines[lineno]);
	mutex_unlock(&pvc_mutex);

	return page - origpage;
}

static int pvc_proc_write_line(struct file *file, const char *buffer,
			   unsigned long count, void *data)
{
	int origcount = count;
	int lineno = *(int *)data;

	if (lineno < 0 || lineno > PVC_NLINES) {
		printk(KERN_WARNING "proc_write_line: invalid lineno %d\n",
		       lineno);
		return origcount;
	}

	if (count > PVC_LINELEN)
		count = PVC_LINELEN;

	if (buffer[count-1] == '\n')
		count--;

	mutex_lock(&pvc_mutex);
	strncpy(pvc_lines[lineno], buffer, count);
	pvc_lines[lineno][count] = '\0';
	mutex_unlock(&pvc_mutex);

	tasklet_schedule(&pvc_display_tasklet);

	return origcount;
}

static int pvc_proc_write_scroll(struct file *file, const char *buffer,
			   unsigned long count, void *data)
{
	int origcount = count;
	int cmd = simple_strtol(buffer, NULL, 10);

	mutex_lock(&pvc_mutex);
	if (scroll_interval != 0)
		del_timer(&timer);

	if (cmd == 0) {
		scroll_dir = 0;
		scroll_interval = 0;
	} else {
		if (cmd < 0) {
			scroll_dir = -1;
			scroll_interval = -cmd;
		} else {
			scroll_dir = 1;
			scroll_interval = cmd;
		}
		add_timer(&timer);
	}
	mutex_unlock(&pvc_mutex);

	return origcount;
}

static int pvc_proc_read_scroll(char *page, char **start,
			     off_t off, int count,
			     int *eof, void *data)
{
	char *origpage = page;

	mutex_lock(&pvc_mutex);
	page += sprintf(page, "%d\n", scroll_dir * scroll_interval);
	mutex_unlock(&pvc_mutex);

	return page - origpage;
}


void pvc_proc_timerfunc(unsigned long data)
{
	if (scroll_dir < 0)
		pvc_move(DISPLAY|RIGHT);
	else if (scroll_dir > 0)
		pvc_move(DISPLAY|LEFT);

	timer.expires = jiffies + scroll_interval;
	add_timer(&timer);
}

static void pvc_proc_cleanup(void)
{
	int i;
	for (i = 0; i < PVC_NLINES; i++)
		remove_proc_entry(pvc_linename[i], pvc_display_dir);
	remove_proc_entry("scroll", pvc_display_dir);
	remove_proc_entry(DISPLAY_DIR_NAME, NULL);

	del_timer(&timer);
}

static int __init pvc_proc_init(void)
{
	struct proc_dir_entry *proc_entry;
	int i;

	pvc_display_dir = proc_mkdir(DISPLAY_DIR_NAME, NULL);
	if (pvc_display_dir == NULL)
		goto error;

	for (i = 0; i < PVC_NLINES; i++) {
		strcpy(pvc_lines[i], "");
		pvc_linedata[i] = i;
	}
	for (i = 0; i < PVC_NLINES; i++) {
		proc_entry = create_proc_entry(pvc_linename[i], 0644,
					       pvc_display_dir);
		if (proc_entry == NULL)
			goto error;

		proc_entry->read_proc = pvc_proc_read_line;
		proc_entry->write_proc = pvc_proc_write_line;
		proc_entry->data = &pvc_linedata[i];
	}
	proc_entry = create_proc_entry("scroll", 0644, pvc_display_dir);
	if (proc_entry == NULL)
		goto error;

	proc_entry->write_proc = pvc_proc_write_scroll;
	proc_entry->read_proc = pvc_proc_read_scroll;

	init_timer(&timer);
	timer.function = pvc_proc_timerfunc;

	return 0;
error:
	pvc_proc_cleanup();
	return -ENOMEM;
}

module_init(pvc_proc_init);
module_exit(pvc_proc_cleanup);
MODULE_LICENSE("GPL");
