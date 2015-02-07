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

/* (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2004 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/types.h>
#include <linux/ip.h>
#include <linux/netfilter.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <net/route.h>
#include <net/ip.h>

#include <linux/netfilter_ipv4.h>
#include <net/netfilter/ipv4/nf_defrag_ipv4.h>

/* Returns new sk_buff, or NULL */
static int nf_ct_ipv4_gather_frags(struct sk_buff *skb, u_int32_t user)
{
	int err;

	skb_orphan(skb);

	local_bh_disable();
	err = ip_defrag(skb, user);
	local_bh_enable();

	if (!err)
		ip_send_check(ip_hdr(skb));

	return err;
}

static unsigned int ipv4_conntrack_defrag(unsigned int hooknum,
					  struct sk_buff *skb,
					  const struct net_device *in,
					  const struct net_device *out,
					  int (*okfn)(struct sk_buff *))
{
#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
#if !defined(CONFIG_NF_NAT) && !defined(CONFIG_NF_NAT_MODULE)
	/* Previously seen (loopback)?  Ignore.  Do this before
	   fragment check. */
	if (skb->nfct)
		return NF_ACCEPT;
#endif
#endif
	/* Gather fragments. */
	if (ip_hdr(skb)->frag_off & htons(IP_MF | IP_OFFSET)) {
		if (nf_ct_ipv4_gather_frags(skb,
					    hooknum == NF_INET_PRE_ROUTING ?
					    IP_DEFRAG_CONNTRACK_IN :
					    IP_DEFRAG_CONNTRACK_OUT))
			return NF_STOLEN;
	}
	return NF_ACCEPT;
}

static struct nf_hook_ops ipv4_defrag_ops[] = {
	{
		.hook		= ipv4_conntrack_defrag,
		.owner		= THIS_MODULE,
		.pf		= PF_INET,
		.hooknum	= NF_INET_PRE_ROUTING,
		.priority	= NF_IP_PRI_CONNTRACK_DEFRAG,
	},
	{
		.hook           = ipv4_conntrack_defrag,
		.owner          = THIS_MODULE,
		.pf             = PF_INET,
		.hooknum        = NF_INET_LOCAL_OUT,
		.priority       = NF_IP_PRI_CONNTRACK_DEFRAG,
	},
};

static int __init nf_defrag_init(void)
{
	return nf_register_hooks(ipv4_defrag_ops, ARRAY_SIZE(ipv4_defrag_ops));
}

static void __exit nf_defrag_fini(void)
{
	nf_unregister_hooks(ipv4_defrag_ops, ARRAY_SIZE(ipv4_defrag_ops));
}

void nf_defrag_ipv4_enable(void)
{
}
EXPORT_SYMBOL_GPL(nf_defrag_ipv4_enable);

module_init(nf_defrag_init);
module_exit(nf_defrag_fini);

MODULE_LICENSE("GPL");
