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

/**
 * @file oprofile_files.c
 *
 * @remark Copyright 2002 OProfile authors
 * @remark Read the file COPYING
 *
 * @author John Levon <levon@movementarian.org>
 */

#include <linux/fs.h>
#include <linux/oprofile.h>

#include "event_buffer.h"
#include "oprofile_stats.h"
#include "oprof.h"

#define BUFFER_SIZE_DEFAULT		131072
#define CPU_BUFFER_SIZE_DEFAULT		8192
#define BUFFER_WATERSHED_DEFAULT	32768	/* FIXME: tune */

unsigned long oprofile_buffer_size;
unsigned long oprofile_cpu_buffer_size;
unsigned long oprofile_buffer_watershed;

static ssize_t depth_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	return oprofilefs_ulong_to_user(oprofile_backtrace_depth, buf, count,
					offset);
}


static ssize_t depth_write(struct file *file, char const __user *buf, size_t count, loff_t *offset)
{
	unsigned long val;
	int retval;

	if (*offset)
		return -EINVAL;

	retval = oprofilefs_ulong_from_user(&val, buf, count);
	if (retval)
		return retval;

	retval = oprofile_set_backtrace(val);

	if (retval)
		return retval;
	return count;
}


static const struct file_operations depth_fops = {
	.read		= depth_read,
	.write		= depth_write
};


static ssize_t pointer_size_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	return oprofilefs_ulong_to_user(sizeof(void *), buf, count, offset);
}


static const struct file_operations pointer_size_fops = {
	.read		= pointer_size_read,
};


static ssize_t cpu_type_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	return oprofilefs_str_to_user(oprofile_ops.cpu_type, buf, count, offset);
}


static const struct file_operations cpu_type_fops = {
	.read		= cpu_type_read,
};


static ssize_t enable_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	return oprofilefs_ulong_to_user(oprofile_started, buf, count, offset);
}


static ssize_t enable_write(struct file *file, char const __user *buf, size_t count, loff_t *offset)
{
	unsigned long val;
	int retval;

	if (*offset)
		return -EINVAL;

	retval = oprofilefs_ulong_from_user(&val, buf, count);
	if (retval)
		return retval;

	if (val)
		retval = oprofile_start();
	else
		oprofile_stop();

	if (retval)
		return retval;
	return count;
}


static const struct file_operations enable_fops = {
	.read		= enable_read,
	.write		= enable_write,
};


static ssize_t dump_write(struct file *file, char const __user *buf, size_t count, loff_t *offset)
{
	wake_up_buffer_waiter();
	return count;
}


static const struct file_operations dump_fops = {
	.write		= dump_write,
};

void oprofile_create_files(struct super_block *sb, struct dentry *root)
{
	/* reinitialize default values */
	oprofile_buffer_size =		BUFFER_SIZE_DEFAULT;
	oprofile_cpu_buffer_size =	CPU_BUFFER_SIZE_DEFAULT;
	oprofile_buffer_watershed =	BUFFER_WATERSHED_DEFAULT;

	oprofilefs_create_file(sb, root, "enable", &enable_fops);
	oprofilefs_create_file_perm(sb, root, "dump", &dump_fops, 0666);
	oprofilefs_create_file(sb, root, "buffer", &event_buffer_fops);
	oprofilefs_create_ulong(sb, root, "buffer_size", &oprofile_buffer_size);
	oprofilefs_create_ulong(sb, root, "buffer_watershed", &oprofile_buffer_watershed);
	oprofilefs_create_ulong(sb, root, "cpu_buffer_size", &oprofile_cpu_buffer_size);
	oprofilefs_create_file(sb, root, "cpu_type", &cpu_type_fops);
	oprofilefs_create_file(sb, root, "backtrace_depth", &depth_fops);
	oprofilefs_create_file(sb, root, "pointer_size", &pointer_size_fops);
	oprofile_create_stats_files(sb, root);
	if (oprofile_ops.create_files)
		oprofile_ops.create_files(sb, root);
}