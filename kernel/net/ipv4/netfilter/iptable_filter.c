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
 * This is the 1999 rewrite of IP Firewalling, aiming for kernel 2.3.x.
 *
 * Copyright (C) 1999 Paul `Rusty' Russell & Michael J. Neuling
 * Copyright (C) 2000-2004 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <net/ip.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Netfilter Core Team <coreteam@netfilter.org>");
MODULE_DESCRIPTION("iptables filter table");

#define FILTER_VALID_HOOKS ((1 << NF_INET_LOCAL_IN) | \
			    (1 << NF_INET_FORWARD) | \
			    (1 << NF_INET_LOCAL_OUT))

static struct
{
	struct ipt_replace repl;
	struct ipt_standard entries[3];
	struct ipt_error term;
} initial_table __net_initdata = {
	.repl = {
		.name = "filter",
		.valid_hooks = FILTER_VALID_HOOKS,
		.num_entries = 4,
		.size = sizeof(struct ipt_standard) * 3 + sizeof(struct ipt_error),
		.hook_entry = {
			[NF_INET_LOCAL_IN] = 0,
			[NF_INET_FORWARD] = sizeof(struct ipt_standard),
			[NF_INET_LOCAL_OUT] = sizeof(struct ipt_standard) * 2,
		},
		.underflow = {
			[NF_INET_LOCAL_IN] = 0,
			[NF_INET_FORWARD] = sizeof(struct ipt_standard),
			[NF_INET_LOCAL_OUT] = sizeof(struct ipt_standard) * 2,
		},
	},
	.entries = {
		IPT_STANDARD_INIT(NF_ACCEPT),	/* LOCAL_IN */
		IPT_STANDARD_INIT(NF_ACCEPT),	/* FORWARD */
		IPT_STANDARD_INIT(NF_ACCEPT),	/* LOCAL_OUT */
	},
	.term = IPT_ERROR_INIT,			/* ERROR */
};

static struct xt_table packet_filter = {
	.name		= "filter",
	.valid_hooks	= FILTER_VALID_HOOKS,
	.lock		= __RW_LOCK_UNLOCKED(packet_filter.lock),
	.me		= THIS_MODULE,
	.af		= AF_INET,
};

/* The work comes in here from netfilter.c. */
static unsigned int
ipt_local_in_hook(unsigned int hook,
		  struct sk_buff *skb,
		  const struct net_device *in,
		  const struct net_device *out,
		  int (*okfn)(struct sk_buff *))
{
	return ipt_do_table(skb, hook, in, out,
			    dev_net(in)->ipv4.iptable_filter);
}

static unsigned int
ipt_hook(unsigned int hook,
	 struct sk_buff *skb,
	 const struct net_device *in,
	 const struct net_device *out,
	 int (*okfn)(struct sk_buff *))
{
	return ipt_do_table(skb, hook, in, out,
			    dev_net(in)->ipv4.iptable_filter);
}

static unsigned int
ipt_local_out_hook(unsigned int hook,
		   struct sk_buff *skb,
		   const struct net_device *in,
		   const struct net_device *out,
		   int (*okfn)(struct sk_buff *))
{
	/* root is playing with raw sockets. */
	if (skb->len < sizeof(struct iphdr) ||
	    ip_hdrlen(skb) < sizeof(struct iphdr))
		return NF_ACCEPT;
	return ipt_do_table(skb, hook, in, out,
			    dev_net(out)->ipv4.iptable_filter);
}

static struct nf_hook_ops ipt_ops[] __read_mostly = {
	{
		.hook		= ipt_local_in_hook,
		.owner		= THIS_MODULE,
		.pf		= PF_INET,
		.hooknum	= NF_INET_LOCAL_IN,
		.priority	= NF_IP_PRI_FILTER,
	},
	{
		.hook		= ipt_hook,
		.owner		= THIS_MODULE,
		.pf		= PF_INET,
		.hooknum	= NF_INET_FORWARD,
		.priority	= NF_IP_PRI_FILTER,
	},
	{
		.hook		= ipt_local_out_hook,
		.owner		= THIS_MODULE,
		.pf		= PF_INET,
		.hooknum	= NF_INET_LOCAL_OUT,
		.priority	= NF_IP_PRI_FILTER,
	},
};

/* Default to forward because I got too much mail already. */
static int forward = NF_ACCEPT;
module_param(forward, bool, 0000);

static int __net_init iptable_filter_net_init(struct net *net)
{
	/* Register table */
	net->ipv4.iptable_filter =
		ipt_register_table(net, &packet_filter, &initial_table.repl);
	if (IS_ERR(net->ipv4.iptable_filter))
		return PTR_ERR(net->ipv4.iptable_filter);
	return 0;
}

static void __net_exit iptable_filter_net_exit(struct net *net)
{
	ipt_unregister_table(net->ipv4.iptable_filter);
}

static struct pernet_operations iptable_filter_net_ops = {
	.init = iptable_filter_net_init,
	.exit = iptable_filter_net_exit,
};

static int __init iptable_filter_init(void)
{
	int ret;

	if (forward < 0 || forward > NF_MAX_VERDICT) {
		printk("iptables forward must be 0 or 1\n");
		return -EINVAL;
	}

	/* Entry 1 is the FORWARD hook */
	initial_table.entries[1].target.verdict = -forward - 1;

	ret = register_pernet_subsys(&iptable_filter_net_ops);
	if (ret < 0)
		return ret;

	/* Register hooks */
	ret = nf_register_hooks(ipt_ops, ARRAY_SIZE(ipt_ops));
	if (ret < 0)
		goto cleanup_table;

	return ret;

 cleanup_table:
	unregister_pernet_subsys(&iptable_filter_net_ops);
	return ret;
}

static void __exit iptable_filter_fini(void)
{
	nf_unregister_hooks(ipt_ops, ARRAY_SIZE(ipt_ops));
	unregister_pernet_subsys(&iptable_filter_net_ops);
}

module_init(iptable_filter_init);
module_exit(iptable_filter_fini);
