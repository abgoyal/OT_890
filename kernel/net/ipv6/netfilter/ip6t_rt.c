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

/* Kernel module to match ROUTING parameters. */

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

#include <asm/byteorder.h>

#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include <linux/netfilter_ipv6/ip6t_rt.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Xtables: IPv6 Routing Header match");
MODULE_AUTHOR("Andras Kis-Szabo <kisza@sch.bme.hu>");

/* Returns 1 if the id is matched by the range, 0 otherwise */
static inline bool
segsleft_match(u_int32_t min, u_int32_t max, u_int32_t id, bool invert)
{
	bool r;
	pr_debug("rt segsleft_match:%c 0x%x <= 0x%x <= 0x%x",
		 invert ? '!' : ' ', min, id, max);
	r = (id >= min && id <= max) ^ invert;
	pr_debug(" result %s\n", r ? "PASS" : "FAILED");
	return r;
}

static bool rt_mt6(const struct sk_buff *skb, const struct xt_match_param *par)
{
	struct ipv6_rt_hdr _route;
	const struct ipv6_rt_hdr *rh;
	const struct ip6t_rt *rtinfo = par->matchinfo;
	unsigned int temp;
	unsigned int ptr;
	unsigned int hdrlen = 0;
	bool ret = false;
	struct in6_addr _addr;
	const struct in6_addr *ap;
	int err;

	err = ipv6_find_hdr(skb, &ptr, NEXTHDR_ROUTING, NULL);
	if (err < 0) {
		if (err != -ENOENT)
			*par->hotdrop = true;
		return false;
	}

	rh = skb_header_pointer(skb, ptr, sizeof(_route), &_route);
	if (rh == NULL) {
		*par->hotdrop = true;
		return false;
	}

	hdrlen = ipv6_optlen(rh);
	if (skb->len - ptr < hdrlen) {
		/* Pcket smaller than its length field */
		return false;
	}

	pr_debug("IPv6 RT LEN %u %u ", hdrlen, rh->hdrlen);
	pr_debug("TYPE %04X ", rh->type);
	pr_debug("SGS_LEFT %u %02X\n", rh->segments_left, rh->segments_left);

	pr_debug("IPv6 RT segsleft %02X ",
		 segsleft_match(rtinfo->segsleft[0], rtinfo->segsleft[1],
				rh->segments_left,
				!!(rtinfo->invflags & IP6T_RT_INV_SGS)));
	pr_debug("type %02X %02X %02X ",
		 rtinfo->rt_type, rh->type,
		 (!(rtinfo->flags & IP6T_RT_TYP) ||
		  ((rtinfo->rt_type == rh->type) ^
		   !!(rtinfo->invflags & IP6T_RT_INV_TYP))));
	pr_debug("len %02X %04X %02X ",
		 rtinfo->hdrlen, hdrlen,
		 !(rtinfo->flags & IP6T_RT_LEN) ||
		  ((rtinfo->hdrlen == hdrlen) ^
		   !!(rtinfo->invflags & IP6T_RT_INV_LEN)));
	pr_debug("res %02X %02X %02X ",
		 rtinfo->flags & IP6T_RT_RES,
		 ((const struct rt0_hdr *)rh)->reserved,
		 !((rtinfo->flags & IP6T_RT_RES) &&
		   (((const struct rt0_hdr *)rh)->reserved)));

	ret = (rh != NULL)
	      &&
	      (segsleft_match(rtinfo->segsleft[0], rtinfo->segsleft[1],
			      rh->segments_left,
			      !!(rtinfo->invflags & IP6T_RT_INV_SGS)))
	      &&
	      (!(rtinfo->flags & IP6T_RT_LEN) ||
	       ((rtinfo->hdrlen == hdrlen) ^
		!!(rtinfo->invflags & IP6T_RT_INV_LEN)))
	      &&
	      (!(rtinfo->flags & IP6T_RT_TYP) ||
	       ((rtinfo->rt_type == rh->type) ^
		!!(rtinfo->invflags & IP6T_RT_INV_TYP)));

	if (ret && (rtinfo->flags & IP6T_RT_RES)) {
		const u_int32_t *rp;
		u_int32_t _reserved;
		rp = skb_header_pointer(skb,
					ptr + offsetof(struct rt0_hdr,
						       reserved),
					sizeof(_reserved),
					&_reserved);

		ret = (*rp == 0);
	}

	pr_debug("#%d ", rtinfo->addrnr);
	if (!(rtinfo->flags & IP6T_RT_FST)) {
		return ret;
	} else if (rtinfo->flags & IP6T_RT_FST_NSTRICT) {
		pr_debug("Not strict ");
		if (rtinfo->addrnr > (unsigned int)((hdrlen - 8) / 16)) {
			pr_debug("There isn't enough space\n");
			return false;
		} else {
			unsigned int i = 0;

			pr_debug("#%d ", rtinfo->addrnr);
			for (temp = 0;
			     temp < (unsigned int)((hdrlen - 8) / 16);
			     temp++) {
				ap = skb_header_pointer(skb,
							ptr
							+ sizeof(struct rt0_hdr)
							+ temp * sizeof(_addr),
							sizeof(_addr),
							&_addr);

				BUG_ON(ap == NULL);

				if (ipv6_addr_equal(ap, &rtinfo->addrs[i])) {
					pr_debug("i=%d temp=%d;\n", i, temp);
					i++;
				}
				if (i == rtinfo->addrnr)
					break;
			}
			pr_debug("i=%d #%d\n", i, rtinfo->addrnr);
			if (i == rtinfo->addrnr)
				return ret;
			else
				return false;
		}
	} else {
		pr_debug("Strict ");
		if (rtinfo->addrnr > (unsigned int)((hdrlen - 8) / 16)) {
			pr_debug("There isn't enough space\n");
			return false;
		} else {
			pr_debug("#%d ", rtinfo->addrnr);
			for (temp = 0; temp < rtinfo->addrnr; temp++) {
				ap = skb_header_pointer(skb,
							ptr
							+ sizeof(struct rt0_hdr)
							+ temp * sizeof(_addr),
							sizeof(_addr),
							&_addr);
				BUG_ON(ap == NULL);

				if (!ipv6_addr_equal(ap, &rtinfo->addrs[temp]))
					break;
			}
			pr_debug("temp=%d #%d\n", temp, rtinfo->addrnr);
			if (temp == rtinfo->addrnr &&
			    temp == (unsigned int)((hdrlen - 8) / 16))
				return ret;
			else
				return false;
		}
	}

	return false;
}

static bool rt_mt6_check(const struct xt_mtchk_param *par)
{
	const struct ip6t_rt *rtinfo = par->matchinfo;

	if (rtinfo->invflags & ~IP6T_RT_INV_MASK) {
		pr_debug("ip6t_rt: unknown flags %X\n", rtinfo->invflags);
		return false;
	}
	if ((rtinfo->flags & (IP6T_RT_RES | IP6T_RT_FST_MASK)) &&
	    (!(rtinfo->flags & IP6T_RT_TYP) ||
	     (rtinfo->rt_type != 0) ||
	     (rtinfo->invflags & IP6T_RT_INV_TYP))) {
		pr_debug("`--rt-type 0' required before `--rt-0-*'");
		return false;
	}

	return true;
}

static struct xt_match rt_mt6_reg __read_mostly = {
	.name		= "rt",
	.family		= NFPROTO_IPV6,
	.match		= rt_mt6,
	.matchsize	= sizeof(struct ip6t_rt),
	.checkentry	= rt_mt6_check,
	.me		= THIS_MODULE,
};

static int __init rt_mt6_init(void)
{
	return xt_register_match(&rt_mt6_reg);
}

static void __exit rt_mt6_exit(void)
{
	xt_unregister_match(&rt_mt6_reg);
}

module_init(rt_mt6_init);
module_exit(rt_mt6_exit);
