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

/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2000-2008 H. Peter Anvin - All Rights Reserved
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version; incorporated herein by reference.
 *
 * ----------------------------------------------------------------------- */

/*
 * x86 CPUID access device
 *
 * This device is accessed by lseek() to the appropriate CPUID level
 * and then read in chunks of 16 bytes.  A larger size means multiple
 * reads of consecutive levels.
 *
 * The lower 32 bits of the file position is used as the incoming %eax,
 * and the upper 32 bits of the file position as the incoming %ecx,
 * the latter intended for "counting" eax levels like eax=4.
 *
 * This driver uses /dev/cpu/%d/cpuid where %d is the minor number, and on
 * an SMP box will direct the access to CPU %d.
 */

#include <linux/module.h>

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/smp.h>
#include <linux/smp_lock.h>
#include <linux/major.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cpu.h>
#include <linux/notifier.h>
#include <linux/uaccess.h>

#include <asm/processor.h>
#include <asm/msr.h>
#include <asm/system.h>

static struct class *cpuid_class;

struct cpuid_regs {
	u32 eax, ebx, ecx, edx;
};

static void cpuid_smp_cpuid(void *cmd_block)
{
	struct cpuid_regs *cmd = (struct cpuid_regs *)cmd_block;

	cpuid_count(cmd->eax, cmd->ecx,
		    &cmd->eax, &cmd->ebx, &cmd->ecx, &cmd->edx);
}

static loff_t cpuid_seek(struct file *file, loff_t offset, int orig)
{
	loff_t ret;
	struct inode *inode = file->f_mapping->host;

	mutex_lock(&inode->i_mutex);
	switch (orig) {
	case 0:
		file->f_pos = offset;
		ret = file->f_pos;
		break;
	case 1:
		file->f_pos += offset;
		ret = file->f_pos;
		break;
	default:
		ret = -EINVAL;
	}
	mutex_unlock(&inode->i_mutex);
	return ret;
}

static ssize_t cpuid_read(struct file *file, char __user *buf,
			  size_t count, loff_t *ppos)
{
	char __user *tmp = buf;
	struct cpuid_regs cmd;
	int cpu = iminor(file->f_path.dentry->d_inode);
	u64 pos = *ppos;
	ssize_t bytes = 0;
	int err = 0;

	if (count % 16)
		return -EINVAL;	/* Invalid chunk size */

	for (; count; count -= 16) {
		cmd.eax = pos;
		cmd.ecx = pos >> 32;
		err = smp_call_function_single(cpu, cpuid_smp_cpuid, &cmd, 1);
		if (err)
			break;
		if (copy_to_user(tmp, &cmd, 16)) {
			err = -EFAULT;
			break;
		}
		tmp += 16;
		bytes += 16;
		*ppos = ++pos;
	}

	return bytes ? bytes : err;
}

static int cpuid_open(struct inode *inode, struct file *file)
{
	unsigned int cpu;
	struct cpuinfo_x86 *c;
	int ret = 0;

	lock_kernel();

	cpu = iminor(file->f_path.dentry->d_inode);
	if (cpu >= nr_cpu_ids || !cpu_online(cpu)) {
		ret = -ENXIO;	/* No such CPU */
		goto out;
	}
	c = &cpu_data(cpu);
	if (c->cpuid_level < 0)
		ret = -EIO;	/* CPUID not supported */
out:
	unlock_kernel();
	return ret;
}

/*
 * File operations we support
 */
static const struct file_operations cpuid_fops = {
	.owner = THIS_MODULE,
	.llseek = cpuid_seek,
	.read = cpuid_read,
	.open = cpuid_open,
};

static __cpuinit int cpuid_device_create(int cpu)
{
	struct device *dev;

	dev = device_create(cpuid_class, NULL, MKDEV(CPUID_MAJOR, cpu), NULL,
			    "cpu%d", cpu);
	return IS_ERR(dev) ? PTR_ERR(dev) : 0;
}

static void cpuid_device_destroy(int cpu)
{
	device_destroy(cpuid_class, MKDEV(CPUID_MAJOR, cpu));
}

static int __cpuinit cpuid_class_cpu_callback(struct notifier_block *nfb,
					      unsigned long action,
					      void *hcpu)
{
	unsigned int cpu = (unsigned long)hcpu;
	int err = 0;

	switch (action) {
	case CPU_UP_PREPARE:
		err = cpuid_device_create(cpu);
		break;
	case CPU_UP_CANCELED:
	case CPU_UP_CANCELED_FROZEN:
	case CPU_DEAD:
		cpuid_device_destroy(cpu);
		break;
	}
	return err ? NOTIFY_BAD : NOTIFY_OK;
}

static struct notifier_block __refdata cpuid_class_cpu_notifier =
{
	.notifier_call = cpuid_class_cpu_callback,
};

static int __init cpuid_init(void)
{
	int i, err = 0;
	i = 0;

	if (register_chrdev(CPUID_MAJOR, "cpu/cpuid", &cpuid_fops)) {
		printk(KERN_ERR "cpuid: unable to get major %d for cpuid\n",
		       CPUID_MAJOR);
		err = -EBUSY;
		goto out;
	}
	cpuid_class = class_create(THIS_MODULE, "cpuid");
	if (IS_ERR(cpuid_class)) {
		err = PTR_ERR(cpuid_class);
		goto out_chrdev;
	}
	for_each_online_cpu(i) {
		err = cpuid_device_create(i);
		if (err != 0)
			goto out_class;
	}
	register_hotcpu_notifier(&cpuid_class_cpu_notifier);

	err = 0;
	goto out;

out_class:
	i = 0;
	for_each_online_cpu(i) {
		cpuid_device_destroy(i);
	}
	class_destroy(cpuid_class);
out_chrdev:
	unregister_chrdev(CPUID_MAJOR, "cpu/cpuid");
out:
	return err;
}

static void __exit cpuid_exit(void)
{
	int cpu = 0;

	for_each_online_cpu(cpu)
		cpuid_device_destroy(cpu);
	class_destroy(cpuid_class);
	unregister_chrdev(CPUID_MAJOR, "cpu/cpuid");
	unregister_hotcpu_notifier(&cpuid_class_cpu_notifier);
}

module_init(cpuid_init);
module_exit(cpuid_exit);

MODULE_AUTHOR("H. Peter Anvin <hpa@zytor.com>");
MODULE_DESCRIPTION("x86 generic CPUID driver");
MODULE_LICENSE("GPL");
