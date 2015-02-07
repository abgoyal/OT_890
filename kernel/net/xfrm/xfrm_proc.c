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
 * xfrm_proc.c
 *
 * Copyright (C)2006-2007 USAGI/WIDE Project
 *
 * Authors:	Masahide NAKAMURA <nakam@linux-ipv6.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <net/snmp.h>
#include <net/xfrm.h>

static struct snmp_mib xfrm_mib_list[] = {
	SNMP_MIB_ITEM("XfrmInError", LINUX_MIB_XFRMINERROR),
	SNMP_MIB_ITEM("XfrmInBufferError", LINUX_MIB_XFRMINBUFFERERROR),
	SNMP_MIB_ITEM("XfrmInHdrError", LINUX_MIB_XFRMINHDRERROR),
	SNMP_MIB_ITEM("XfrmInNoStates", LINUX_MIB_XFRMINNOSTATES),
	SNMP_MIB_ITEM("XfrmInStateProtoError", LINUX_MIB_XFRMINSTATEPROTOERROR),
	SNMP_MIB_ITEM("XfrmInStateModeError", LINUX_MIB_XFRMINSTATEMODEERROR),
	SNMP_MIB_ITEM("XfrmInStateSeqError", LINUX_MIB_XFRMINSTATESEQERROR),
	SNMP_MIB_ITEM("XfrmInStateExpired", LINUX_MIB_XFRMINSTATEEXPIRED),
	SNMP_MIB_ITEM("XfrmInStateMismatch", LINUX_MIB_XFRMINSTATEMISMATCH),
	SNMP_MIB_ITEM("XfrmInStateInvalid", LINUX_MIB_XFRMINSTATEINVALID),
	SNMP_MIB_ITEM("XfrmInTmplMismatch", LINUX_MIB_XFRMINTMPLMISMATCH),
	SNMP_MIB_ITEM("XfrmInNoPols", LINUX_MIB_XFRMINNOPOLS),
	SNMP_MIB_ITEM("XfrmInPolBlock", LINUX_MIB_XFRMINPOLBLOCK),
	SNMP_MIB_ITEM("XfrmInPolError", LINUX_MIB_XFRMINPOLERROR),
	SNMP_MIB_ITEM("XfrmOutError", LINUX_MIB_XFRMOUTERROR),
	SNMP_MIB_ITEM("XfrmOutBundleGenError", LINUX_MIB_XFRMOUTBUNDLEGENERROR),
	SNMP_MIB_ITEM("XfrmOutBundleCheckError", LINUX_MIB_XFRMOUTBUNDLECHECKERROR),
	SNMP_MIB_ITEM("XfrmOutNoStates", LINUX_MIB_XFRMOUTNOSTATES),
	SNMP_MIB_ITEM("XfrmOutStateProtoError", LINUX_MIB_XFRMOUTSTATEPROTOERROR),
	SNMP_MIB_ITEM("XfrmOutStateModeError", LINUX_MIB_XFRMOUTSTATEMODEERROR),
	SNMP_MIB_ITEM("XfrmOutStateSeqError", LINUX_MIB_XFRMOUTSTATESEQERROR),
	SNMP_MIB_ITEM("XfrmOutStateExpired", LINUX_MIB_XFRMOUTSTATEEXPIRED),
	SNMP_MIB_ITEM("XfrmOutPolBlock", LINUX_MIB_XFRMOUTPOLBLOCK),
	SNMP_MIB_ITEM("XfrmOutPolDead", LINUX_MIB_XFRMOUTPOLDEAD),
	SNMP_MIB_ITEM("XfrmOutPolError", LINUX_MIB_XFRMOUTPOLERROR),
	SNMP_MIB_SENTINEL
};

static int xfrm_statistics_seq_show(struct seq_file *seq, void *v)
{
	struct net *net = seq->private;
	int i;
	for (i=0; xfrm_mib_list[i].name; i++)
		seq_printf(seq, "%-24s\t%lu\n", xfrm_mib_list[i].name,
			   snmp_fold_field((void **)net->mib.xfrm_statistics,
					   xfrm_mib_list[i].entry));
	return 0;
}

static int xfrm_statistics_seq_open(struct inode *inode, struct file *file)
{
	return single_open_net(inode, file, xfrm_statistics_seq_show);
}

static struct file_operations xfrm_statistics_seq_fops = {
	.owner	 = THIS_MODULE,
	.open	 = xfrm_statistics_seq_open,
	.read	 = seq_read,
	.llseek	 = seq_lseek,
	.release = single_release_net,
};

int __net_init xfrm_proc_init(struct net *net)
{
	if (!proc_net_fops_create(net, "xfrm_stat", S_IRUGO,
				  &xfrm_statistics_seq_fops))
		return -ENOMEM;
	return 0;
}

void xfrm_proc_fini(struct net *net)
{
	proc_net_remove(net, "xfrm_stat");
}
