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
 * arch/blackfin/kernel/cplbinfo.c - display CPLB status
 *
 * Copyright 2004-2008 Analog Devices Inc.
 * Licensed under the GPL-2 or later.
 */

#include <linux/ctype.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

#include <asm/cplbinit.h>
#include <asm/blackfin.h>

static char const page_strtbl[][3] = { "1K", "4K", "1M", "4M" };
#define page(flags)    (((flags) & 0x30000) >> 16)
#define strpage(flags) page_strtbl[page(flags)]

struct cplbinfo_data {
	loff_t pos;
	char cplb_type;
	u32 mem_control;
	struct cplb_entry *tbl;
	int switched;
};

static void cplbinfo_print_header(struct seq_file *m)
{
	seq_printf(m, "Index\tAddress\t\tData\tSize\tU/RD\tU/WR\tS/WR\tSwitch\n");
}

static int cplbinfo_nomore(struct cplbinfo_data *cdata)
{
	return cdata->pos >= MAX_CPLBS;
}

static int cplbinfo_show(struct seq_file *m, void *p)
{
	struct cplbinfo_data *cdata;
	unsigned long data, addr;
	loff_t pos;

	cdata = p;
	pos = cdata->pos;
	addr = cdata->tbl[pos].addr;
	data = cdata->tbl[pos].data;

	seq_printf(m,
		"%d\t0x%08lx\t%05lx\t%s\t%c\t%c\t%c\t%c\n",
		(int)pos, addr, data, strpage(data),
		(data & CPLB_USER_RD) ? 'Y' : 'N',
		(data & CPLB_USER_WR) ? 'Y' : 'N',
		(data & CPLB_SUPV_WR) ? 'Y' : 'N',
		pos < cdata->switched ? 'N' : 'Y');

	return 0;
}

static void cplbinfo_seq_init(struct cplbinfo_data *cdata, unsigned int cpu)
{
	if (cdata->cplb_type == 'I') {
		cdata->mem_control = bfin_read_IMEM_CONTROL();
		cdata->tbl = icplb_tbl[cpu];
		cdata->switched = first_switched_icplb;
	} else {
		cdata->mem_control = bfin_read_DMEM_CONTROL();
		cdata->tbl = dcplb_tbl[cpu];
		cdata->switched = first_switched_dcplb;
	}
}

static void *cplbinfo_start(struct seq_file *m, loff_t *pos)
{
	struct cplbinfo_data *cdata = m->private;

	if (!*pos) {
		seq_printf(m, "%cCPLBs are %sabled: 0x%x\n", cdata->cplb_type,
			(cdata->mem_control & ENDCPLB ? "en" : "dis"),
			cdata->mem_control);
		cplbinfo_print_header(m);
	} else if (cplbinfo_nomore(cdata))
		return NULL;

	get_cpu();
	return cdata;
}

static void *cplbinfo_next(struct seq_file *m, void *p, loff_t *pos)
{
	struct cplbinfo_data *cdata = p;
	cdata->pos = ++(*pos);
	if (cplbinfo_nomore(cdata))
		return NULL;
	else
		return cdata;
}

static void cplbinfo_stop(struct seq_file *m, void *p)
{
	put_cpu();
}

static const struct seq_operations cplbinfo_sops = {
	.start = cplbinfo_start,
	.next  = cplbinfo_next,
	.stop  = cplbinfo_stop,
	.show  = cplbinfo_show,
};

static int cplbinfo_open(struct inode *inode, struct file *file)
{
	char buf[256], *path, *p;
	unsigned int cpu;
	char *s_cpu, *s_cplb;
	int ret;
	struct seq_file *m;
	struct cplbinfo_data *cdata;

	path = d_path(&file->f_path, buf, sizeof(buf));
	if (IS_ERR(path))
		return PTR_ERR(path);
	s_cpu = strstr(path, "/cpu");
	s_cplb = strrchr(path, '/');
	if (!s_cpu || !s_cplb)
		return -EINVAL;

	cpu = simple_strtoul(s_cpu + 4, &p, 10);
	if (!cpu_online(cpu))
		return -ENODEV;

	ret = seq_open_private(file, &cplbinfo_sops, sizeof(*cdata));
	if (ret)
		return ret;
	m = file->private_data;
	cdata = m->private;

	cdata->pos = 0;
	cdata->cplb_type = toupper(s_cplb[1]);
	cplbinfo_seq_init(cdata, cpu);

	return 0;
}

static const struct file_operations cplbinfo_fops = {
	.open    = cplbinfo_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release_private,
};

static int __init cplbinfo_init(void)
{
	struct proc_dir_entry *cplb_dir, *cpu_dir;
	char buf[10];
	unsigned int cpu;

	cplb_dir = proc_mkdir("cplbinfo", NULL);
	if (!cplb_dir)
		return -ENOMEM;

	for_each_possible_cpu(cpu) {
		sprintf(buf, "cpu%i", cpu);
		cpu_dir = proc_mkdir(buf, cplb_dir);
		if (!cpu_dir)
			return -ENOMEM;

		proc_create("icplb", S_IRUGO, cpu_dir, &cplbinfo_fops);
		proc_create("dcplb", S_IRUGO, cpu_dir, &cplbinfo_fops);
	}

	return 0;
}
late_initcall(cplbinfo_init);
