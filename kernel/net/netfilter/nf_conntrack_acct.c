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

/* Accouting handling for netfilter. */

/*
 * (C) 2008 Krzysztof Piotr Oledzki <ole@ans.pl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/netfilter.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_extend.h>
#include <net/netfilter/nf_conntrack_acct.h>

#ifdef CONFIG_NF_CT_ACCT
#define NF_CT_ACCT_DEFAULT 1
#else
#define NF_CT_ACCT_DEFAULT 0
#endif

static int nf_ct_acct __read_mostly = NF_CT_ACCT_DEFAULT;

module_param_named(acct, nf_ct_acct, bool, 0644);
MODULE_PARM_DESC(acct, "Enable connection tracking flow accounting.");

#ifdef CONFIG_SYSCTL
static struct ctl_table acct_sysctl_table[] = {
	{
		.ctl_name	= CTL_UNNUMBERED,
		.procname	= "nf_conntrack_acct",
		.data		= &init_net.ct.sysctl_acct,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec,
	},
	{}
};
#endif /* CONFIG_SYSCTL */

unsigned int
seq_print_acct(struct seq_file *s, const struct nf_conn *ct, int dir)
{
	struct nf_conn_counter *acct;

	acct = nf_conn_acct_find(ct);
	if (!acct)
		return 0;

	return seq_printf(s, "packets=%llu bytes=%llu ",
			  (unsigned long long)acct[dir].packets,
			  (unsigned long long)acct[dir].bytes);
};
EXPORT_SYMBOL_GPL(seq_print_acct);

static struct nf_ct_ext_type acct_extend __read_mostly = {
	.len	= sizeof(struct nf_conn_counter[IP_CT_DIR_MAX]),
	.align	= __alignof__(struct nf_conn_counter[IP_CT_DIR_MAX]),
	.id	= NF_CT_EXT_ACCT,
};

#ifdef CONFIG_SYSCTL
static int nf_conntrack_acct_init_sysctl(struct net *net)
{
	struct ctl_table *table;

	table = kmemdup(acct_sysctl_table, sizeof(acct_sysctl_table),
			GFP_KERNEL);
	if (!table)
		goto out;

	table[0].data = &net->ct.sysctl_acct;

	net->ct.acct_sysctl_header = register_net_sysctl_table(net,
			nf_net_netfilter_sysctl_path, table);
	if (!net->ct.acct_sysctl_header) {
		printk(KERN_ERR "nf_conntrack_acct: can't register to sysctl.\n");
		goto out_register;
	}
	return 0;

out_register:
	kfree(table);
out:
	return -ENOMEM;
}

static void nf_conntrack_acct_fini_sysctl(struct net *net)
{
	struct ctl_table *table;

	table = net->ct.acct_sysctl_header->ctl_table_arg;
	unregister_net_sysctl_table(net->ct.acct_sysctl_header);
	kfree(table);
}
#else
static int nf_conntrack_acct_init_sysctl(struct net *net)
{
	return 0;
}

static void nf_conntrack_acct_fini_sysctl(struct net *net)
{
}
#endif

int nf_conntrack_acct_init(struct net *net)
{
	int ret;

	net->ct.sysctl_acct = nf_ct_acct;

	if (net_eq(net, &init_net)) {
#ifdef CONFIG_NF_CT_ACCT
	printk(KERN_WARNING "CONFIG_NF_CT_ACCT is deprecated and will be removed soon. Please use\n");
		printk(KERN_WARNING "nf_conntrack.acct=1 kernel paramater, acct=1 nf_conntrack module option or\n");
		printk(KERN_WARNING "sysctl net.netfilter.nf_conntrack_acct=1 to enable it.\n");
#endif

		ret = nf_ct_extend_register(&acct_extend);
		if (ret < 0) {
			printk(KERN_ERR "nf_conntrack_acct: Unable to register extension\n");
			goto out_extend_register;
		}
	}

	ret = nf_conntrack_acct_init_sysctl(net);
	if (ret < 0)
		goto out_sysctl;

	return 0;

out_sysctl:
	if (net_eq(net, &init_net))
		nf_ct_extend_unregister(&acct_extend);
out_extend_register:
	return ret;
}

void nf_conntrack_acct_fini(struct net *net)
{
	nf_conntrack_acct_fini_sysctl(net);
	if (net_eq(net, &init_net))
		nf_ct_extend_unregister(&acct_extend);
}
