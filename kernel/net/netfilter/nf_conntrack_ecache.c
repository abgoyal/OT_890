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

/* Event cache for netfilter. */

/* (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2006 Netfilter Core Team <coreteam@netfilter.org>
 * (C) 2003,2004 USAGI/WIDE Project <http://www.linux-ipv6.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/types.h>
#include <linux/netfilter.h>
#include <linux/skbuff.h>
#include <linux/vmalloc.h>
#include <linux/stddef.h>
#include <linux/err.h>
#include <linux/percpu.h>
#include <linux/notifier.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>

ATOMIC_NOTIFIER_HEAD(nf_conntrack_chain);
EXPORT_SYMBOL_GPL(nf_conntrack_chain);

ATOMIC_NOTIFIER_HEAD(nf_ct_expect_chain);
EXPORT_SYMBOL_GPL(nf_ct_expect_chain);

/* deliver cached events and clear cache entry - must be called with locally
 * disabled softirqs */
static inline void
__nf_ct_deliver_cached_events(struct nf_conntrack_ecache *ecache)
{
	if (nf_ct_is_confirmed(ecache->ct) && !nf_ct_is_dying(ecache->ct)
	    && ecache->events) {
		struct nf_ct_event item = {
			.ct 	= ecache->ct,
			.pid	= 0,
			.report	= 0
		};

		atomic_notifier_call_chain(&nf_conntrack_chain,
					   ecache->events,
					   &item);
	}

	ecache->events = 0;
	nf_ct_put(ecache->ct);
	ecache->ct = NULL;
}

/* Deliver all cached events for a particular conntrack. This is called
 * by code prior to async packet handling for freeing the skb */
void nf_ct_deliver_cached_events(const struct nf_conn *ct)
{
	struct net *net = nf_ct_net(ct);
	struct nf_conntrack_ecache *ecache;

	local_bh_disable();
	ecache = per_cpu_ptr(net->ct.ecache, raw_smp_processor_id());
	if (ecache->ct == ct)
		__nf_ct_deliver_cached_events(ecache);
	local_bh_enable();
}
EXPORT_SYMBOL_GPL(nf_ct_deliver_cached_events);

/* Deliver cached events for old pending events, if current conntrack != old */
void __nf_ct_event_cache_init(struct nf_conn *ct)
{
	struct net *net = nf_ct_net(ct);
	struct nf_conntrack_ecache *ecache;

	/* take care of delivering potentially old events */
	ecache = per_cpu_ptr(net->ct.ecache, raw_smp_processor_id());
	BUG_ON(ecache->ct == ct);
	if (ecache->ct)
		__nf_ct_deliver_cached_events(ecache);
	/* initialize for this conntrack/packet */
	ecache->ct = ct;
	nf_conntrack_get(&ct->ct_general);
}
EXPORT_SYMBOL_GPL(__nf_ct_event_cache_init);

/* flush the event cache - touches other CPU's data and must not be called
 * while packets are still passing through the code */
void nf_ct_event_cache_flush(struct net *net)
{
	struct nf_conntrack_ecache *ecache;
	int cpu;

	for_each_possible_cpu(cpu) {
		ecache = per_cpu_ptr(net->ct.ecache, cpu);
		if (ecache->ct)
			nf_ct_put(ecache->ct);
	}
}

int nf_conntrack_ecache_init(struct net *net)
{
	net->ct.ecache = alloc_percpu(struct nf_conntrack_ecache);
	if (!net->ct.ecache)
		return -ENOMEM;
	return 0;
}

void nf_conntrack_ecache_fini(struct net *net)
{
	free_percpu(net->ct.ecache);
}

int nf_conntrack_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nf_conntrack_chain, nb);
}
EXPORT_SYMBOL_GPL(nf_conntrack_register_notifier);

int nf_conntrack_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nf_conntrack_chain, nb);
}
EXPORT_SYMBOL_GPL(nf_conntrack_unregister_notifier);

int nf_ct_expect_register_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_register(&nf_ct_expect_chain, nb);
}
EXPORT_SYMBOL_GPL(nf_ct_expect_register_notifier);

int nf_ct_expect_unregister_notifier(struct notifier_block *nb)
{
	return atomic_notifier_chain_unregister(&nf_ct_expect_chain, nb);
}
EXPORT_SYMBOL_GPL(nf_ct_expect_unregister_notifier);
