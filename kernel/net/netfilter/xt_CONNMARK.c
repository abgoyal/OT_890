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
 *	xt_CONNMARK - Netfilter module to modify the connection mark values
 *
 *	Copyright (C) 2002,2004 MARA Systems AB <http://www.marasystems.com>
 *	by Henrik Nordstrom <hno@marasystems.com>
 *	Copyright © CC Computer Consultants GmbH, 2007 - 2008
 *	Jan Engelhardt <jengelh@computergmbh.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <net/checksum.h>

MODULE_AUTHOR("Henrik Nordstrom <hno@marasystems.com>");
MODULE_DESCRIPTION("Xtables: connection mark modification");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_CONNMARK");
MODULE_ALIAS("ip6t_CONNMARK");

#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_CONNMARK.h>
#include <net/netfilter/nf_conntrack_ecache.h>

static unsigned int
connmark_tg_v0(struct sk_buff *skb, const struct xt_target_param *par)
{
	const struct xt_connmark_target_info *markinfo = par->targinfo;
	struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	u_int32_t diff;
	u_int32_t mark;
	u_int32_t newmark;

	ct = nf_ct_get(skb, &ctinfo);
	if (ct) {
		switch(markinfo->mode) {
		case XT_CONNMARK_SET:
			newmark = (ct->mark & ~markinfo->mask) | markinfo->mark;
			if (newmark != ct->mark) {
				ct->mark = newmark;
				nf_conntrack_event_cache(IPCT_MARK, ct);
			}
			break;
		case XT_CONNMARK_SAVE:
			newmark = (ct->mark & ~markinfo->mask) |
				  (skb->mark & markinfo->mask);
			if (ct->mark != newmark) {
				ct->mark = newmark;
				nf_conntrack_event_cache(IPCT_MARK, ct);
			}
			break;
		case XT_CONNMARK_RESTORE:
			mark = skb->mark;
			diff = (ct->mark ^ mark) & markinfo->mask;
			skb->mark = mark ^ diff;
			break;
		}
	}

	return XT_CONTINUE;
}

static unsigned int
connmark_tg(struct sk_buff *skb, const struct xt_target_param *par)
{
	const struct xt_connmark_tginfo1 *info = par->targinfo;
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct;
	u_int32_t newmark;

	ct = nf_ct_get(skb, &ctinfo);
	if (ct == NULL)
		return XT_CONTINUE;

	switch (info->mode) {
	case XT_CONNMARK_SET:
		newmark = (ct->mark & ~info->ctmask) ^ info->ctmark;
		if (ct->mark != newmark) {
			ct->mark = newmark;
			nf_conntrack_event_cache(IPCT_MARK, ct);
		}
		break;
	case XT_CONNMARK_SAVE:
		newmark = (ct->mark & ~info->ctmask) ^
		          (skb->mark & info->nfmask);
		if (ct->mark != newmark) {
			ct->mark = newmark;
			nf_conntrack_event_cache(IPCT_MARK, ct);
		}
		break;
	case XT_CONNMARK_RESTORE:
		newmark = (skb->mark & ~info->nfmask) ^
		          (ct->mark & info->ctmask);
		skb->mark = newmark;
		break;
	}

	return XT_CONTINUE;
}

static bool connmark_tg_check_v0(const struct xt_tgchk_param *par)
{
	const struct xt_connmark_target_info *matchinfo = par->targinfo;

	if (matchinfo->mode == XT_CONNMARK_RESTORE) {
		if (strcmp(par->table, "mangle") != 0) {
			printk(KERN_WARNING "CONNMARK: restore can only be "
			       "called from \"mangle\" table, not \"%s\"\n",
			       par->table);
			return false;
		}
	}
	if (matchinfo->mark > 0xffffffff || matchinfo->mask > 0xffffffff) {
		printk(KERN_WARNING "CONNMARK: Only supports 32bit mark\n");
		return false;
	}
	if (nf_ct_l3proto_try_module_get(par->family) < 0) {
		printk(KERN_WARNING "can't load conntrack support for "
				    "proto=%u\n", par->family);
		return false;
	}
	return true;
}

static bool connmark_tg_check(const struct xt_tgchk_param *par)
{
	if (nf_ct_l3proto_try_module_get(par->family) < 0) {
		printk(KERN_WARNING "cannot load conntrack support for "
		       "proto=%u\n", par->family);
		return false;
	}
	return true;
}

static void connmark_tg_destroy(const struct xt_tgdtor_param *par)
{
	nf_ct_l3proto_module_put(par->family);
}

#ifdef CONFIG_COMPAT
struct compat_xt_connmark_target_info {
	compat_ulong_t	mark, mask;
	u_int8_t	mode;
	u_int8_t	__pad1;
	u_int16_t	__pad2;
};

static void connmark_tg_compat_from_user_v0(void *dst, void *src)
{
	const struct compat_xt_connmark_target_info *cm = src;
	struct xt_connmark_target_info m = {
		.mark	= cm->mark,
		.mask	= cm->mask,
		.mode	= cm->mode,
	};
	memcpy(dst, &m, sizeof(m));
}

static int connmark_tg_compat_to_user_v0(void __user *dst, void *src)
{
	const struct xt_connmark_target_info *m = src;
	struct compat_xt_connmark_target_info cm = {
		.mark	= m->mark,
		.mask	= m->mask,
		.mode	= m->mode,
	};
	return copy_to_user(dst, &cm, sizeof(cm)) ? -EFAULT : 0;
}
#endif /* CONFIG_COMPAT */

static struct xt_target connmark_tg_reg[] __read_mostly = {
	{
		.name		= "CONNMARK",
		.revision	= 0,
		.family		= NFPROTO_UNSPEC,
		.checkentry	= connmark_tg_check_v0,
		.destroy	= connmark_tg_destroy,
		.target		= connmark_tg_v0,
		.targetsize	= sizeof(struct xt_connmark_target_info),
#ifdef CONFIG_COMPAT
		.compatsize	= sizeof(struct compat_xt_connmark_target_info),
		.compat_from_user = connmark_tg_compat_from_user_v0,
		.compat_to_user	= connmark_tg_compat_to_user_v0,
#endif
		.me		= THIS_MODULE
	},
	{
		.name           = "CONNMARK",
		.revision       = 1,
		.family         = NFPROTO_UNSPEC,
		.checkentry     = connmark_tg_check,
		.target         = connmark_tg,
		.targetsize     = sizeof(struct xt_connmark_tginfo1),
		.destroy        = connmark_tg_destroy,
		.me             = THIS_MODULE,
	},
};

static int __init connmark_tg_init(void)
{
	return xt_register_targets(connmark_tg_reg,
	       ARRAY_SIZE(connmark_tg_reg));
}

static void __exit connmark_tg_exit(void)
{
	xt_unregister_targets(connmark_tg_reg, ARRAY_SIZE(connmark_tg_reg));
}

module_init(connmark_tg_init);
module_exit(connmark_tg_exit);
