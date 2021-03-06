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

/* Kernel module to match FRAG parameters. */

/* (C) 2001-2002 Andras Kis-Szabo <kisza@sch.bme.hu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ipv6.h>
#include <linux/types.h>
#include <net/checksum.h>
#include <net/ipv6.h>

#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include <linux/netfilter_ipv6/ip6t_frag.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Xtables: IPv6 fragment match");
MODULE_AUTHOR("Andras Kis-Szabo <kisza@sch.bme.hu>");

/* Returns 1 if the id is matched by the range, 0 otherwise */
static inline bool
id_match(u_int32_t min, u_int32_t max, u_int32_t id, bool invert)
{
	bool r;
	pr_debug("frag id_match:%c 0x%x <= 0x%x <= 0x%x", invert ? '!' : ' ',
		 min, id, max);
	r = (id >= min && id <= max) ^ invert;
	pr_debug(" result %s\n", r ? "PASS" : "FAILED");
	return r;
}

static bool
frag_mt6(const struct sk_buff *skb, const struct xt_match_param *par)
{
	struct frag_hdr _frag;
	const struct frag_hdr *fh;
	const struct ip6t_frag *fraginfo = par->matchinfo;
	unsigned int ptr;
	int err;

	err = ipv6_find_hdr(skb, &ptr, NEXTHDR_FRAGMENT, NULL);
	if (err < 0) {
		if (err != -ENOENT)
			*par->hotdrop = true;
		return false;
	}

	fh = skb_header_pointer(skb, ptr, sizeof(_frag), &_frag);
	if (fh == NULL) {
		*par->hotdrop = true;
		return false;
	}

	pr_debug("INFO %04X ", fh->frag_off);
	pr_debug("OFFSET %04X ", ntohs(fh->frag_off) & ~0x7);
	pr_debug("RES %02X %04X", fh->reserved, ntohs(fh->frag_off) & 0x6);
	pr_debug("MF %04X ", fh->frag_off & htons(IP6_MF));
	pr_debug("ID %u %08X\n", ntohl(fh->identification),
		 ntohl(fh->identification));

	pr_debug("IPv6 FRAG id %02X ",
		 id_match(fraginfo->ids[0], fraginfo->ids[1],
			  ntohl(fh->identification),
			  !!(fraginfo->invflags & IP6T_FRAG_INV_IDS)));
	pr_debug("res %02X %02X%04X %02X ",
		 fraginfo->flags & IP6T_FRAG_RES, fh->reserved,
		 ntohs(fh->frag_off) & 0x6,
		 !((fraginfo->flags & IP6T_FRAG_RES)
		   && (fh->reserved || (ntohs(fh->frag_off) & 0x06))));
	pr_debug("first %02X %02X %02X ",
		 fraginfo->flags & IP6T_FRAG_FST,
		 ntohs(fh->frag_off) & ~0x7,
		 !((fraginfo->flags & IP6T_FRAG_FST)
		   && (ntohs(fh->frag_off) & ~0x7)));
	pr_debug("mf %02X %02X %02X ",
		 fraginfo->flags & IP6T_FRAG_MF,
		 ntohs(fh->frag_off) & IP6_MF,
		 !((fraginfo->flags & IP6T_FRAG_MF)
		   && !((ntohs(fh->frag_off) & IP6_MF))));
	pr_debug("last %02X %02X %02X\n",
		 fraginfo->flags & IP6T_FRAG_NMF,
		 ntohs(fh->frag_off) & IP6_MF,
		 !((fraginfo->flags & IP6T_FRAG_NMF)
		   && (ntohs(fh->frag_off) & IP6_MF)));

	return (fh != NULL)
	       &&
	       id_match(fraginfo->ids[0], fraginfo->ids[1],
			ntohl(fh->identification),
			!!(fraginfo->invflags & IP6T_FRAG_INV_IDS))
	       &&
	       !((fraginfo->flags & IP6T_FRAG_RES)
		 && (fh->reserved || (ntohs(fh->frag_off) & 0x6)))
	       &&
	       !((fraginfo->flags & IP6T_FRAG_FST)
		 && (ntohs(fh->frag_off) & ~0x7))
	       &&
	       !((fraginfo->flags & IP6T_FRAG_MF)
		 && !(ntohs(fh->frag_off) & IP6_MF))
	       &&
	       !((fraginfo->flags & IP6T_FRAG_NMF)
		 && (ntohs(fh->frag_off) & IP6_MF));
}

static bool frag_mt6_check(const struct xt_mtchk_param *par)
{
	const struct ip6t_frag *fraginfo = par->matchinfo;

	if (fraginfo->invflags & ~IP6T_FRAG_INV_MASK) {
		pr_debug("ip6t_frag: unknown flags %X\n", fraginfo->invflags);
		return false;
	}
	return true;
}

static struct xt_match frag_mt6_reg __read_mostly = {
	.name		= "frag",
	.family		= NFPROTO_IPV6,
	.match		= frag_mt6,
	.matchsize	= sizeof(struct ip6t_frag),
	.checkentry	= frag_mt6_check,
	.me		= THIS_MODULE,
};

static int __init frag_mt6_init(void)
{
	return xt_register_match(&frag_mt6_reg);
}

static void __exit frag_mt6_exit(void)
{
	xt_unregister_match(&frag_mt6_reg);
}

module_init(frag_mt6_init);
module_exit(frag_mt6_exit);
