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
 *  ebt_log
 *
 *	Authors:
 *	Bart De Schuymer <bdschuym@pandora.be>
 *	Harald Welte <laforge@netfilter.org>
 *
 *  April, 2002
 *
 */
#include <linux/module.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/if_arp.h>
#include <linux/spinlock.h>
#include <net/netfilter/nf_log.h>
#include <linux/ipv6.h>
#include <net/ipv6.h>
#include <linux/in6.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_bridge/ebtables.h>
#include <linux/netfilter_bridge/ebt_log.h>
#include <linux/netfilter.h>

static DEFINE_SPINLOCK(ebt_log_lock);

static bool ebt_log_tg_check(const struct xt_tgchk_param *par)
{
	struct ebt_log_info *info = par->targinfo;

	if (info->bitmask & ~EBT_LOG_MASK)
		return false;
	if (info->loglevel >= 8)
		return false;
	info->prefix[EBT_LOG_PREFIX_SIZE - 1] = '\0';
	return true;
}

struct tcpudphdr
{
	__be16 src;
	__be16 dst;
};

struct arppayload
{
	unsigned char mac_src[ETH_ALEN];
	unsigned char ip_src[4];
	unsigned char mac_dst[ETH_ALEN];
	unsigned char ip_dst[4];
};

static void print_MAC(const unsigned char *p)
{
	int i;

	for (i = 0; i < ETH_ALEN; i++, p++)
		printk("%02x%c", *p, i == ETH_ALEN - 1 ? ' ':':');
}

static void
print_ports(const struct sk_buff *skb, uint8_t protocol, int offset)
{
	if (protocol == IPPROTO_TCP ||
	    protocol == IPPROTO_UDP ||
	    protocol == IPPROTO_UDPLITE ||
	    protocol == IPPROTO_SCTP ||
	    protocol == IPPROTO_DCCP) {
		const struct tcpudphdr *pptr;
		struct tcpudphdr _ports;

		pptr = skb_header_pointer(skb, offset,
					  sizeof(_ports), &_ports);
		if (pptr == NULL) {
			printk(" INCOMPLETE TCP/UDP header");
			return;
		}
		printk(" SPT=%u DPT=%u", ntohs(pptr->src), ntohs(pptr->dst));
	}
}

static void
ebt_log_packet(u_int8_t pf, unsigned int hooknum,
   const struct sk_buff *skb, const struct net_device *in,
   const struct net_device *out, const struct nf_loginfo *loginfo,
   const char *prefix)
{
	unsigned int bitmask;

	spin_lock_bh(&ebt_log_lock);
	printk("<%c>%s IN=%s OUT=%s MAC source = ", '0' + loginfo->u.log.level,
	       prefix, in ? in->name : "", out ? out->name : "");

	print_MAC(eth_hdr(skb)->h_source);
	printk("MAC dest = ");
	print_MAC(eth_hdr(skb)->h_dest);

	printk("proto = 0x%04x", ntohs(eth_hdr(skb)->h_proto));

	if (loginfo->type == NF_LOG_TYPE_LOG)
		bitmask = loginfo->u.log.logflags;
	else
		bitmask = NF_LOG_MASK;

	if ((bitmask & EBT_LOG_IP) && eth_hdr(skb)->h_proto ==
	   htons(ETH_P_IP)){
		const struct iphdr *ih;
		struct iphdr _iph;

		ih = skb_header_pointer(skb, 0, sizeof(_iph), &_iph);
		if (ih == NULL) {
			printk(" INCOMPLETE IP header");
			goto out;
		}
		printk(" IP SRC=%pI4 IP DST=%pI4, IP tos=0x%02X, IP proto=%d",
		       &ih->saddr, &ih->daddr, ih->tos, ih->protocol);
		print_ports(skb, ih->protocol, ih->ihl*4);
		goto out;
	}

#if defined(CONFIG_BRIDGE_EBT_IP6) || defined(CONFIG_BRIDGE_EBT_IP6_MODULE)
	if ((bitmask & EBT_LOG_IP6) && eth_hdr(skb)->h_proto ==
	   htons(ETH_P_IPV6)) {
		const struct ipv6hdr *ih;
		struct ipv6hdr _iph;
		uint8_t nexthdr;
		int offset_ph;

		ih = skb_header_pointer(skb, 0, sizeof(_iph), &_iph);
		if (ih == NULL) {
			printk(" INCOMPLETE IPv6 header");
			goto out;
		}
		printk(" IPv6 SRC=%pI6 IPv6 DST=%pI6, IPv6 priority=0x%01X, Next Header=%d",
		       &ih->saddr, &ih->daddr, ih->priority, ih->nexthdr);
		nexthdr = ih->nexthdr;
		offset_ph = ipv6_skip_exthdr(skb, sizeof(_iph), &nexthdr);
		if (offset_ph == -1)
			goto out;
		print_ports(skb, nexthdr, offset_ph);
		goto out;
	}
#endif

	if ((bitmask & EBT_LOG_ARP) &&
	    ((eth_hdr(skb)->h_proto == htons(ETH_P_ARP)) ||
	     (eth_hdr(skb)->h_proto == htons(ETH_P_RARP)))) {
		const struct arphdr *ah;
		struct arphdr _arph;

		ah = skb_header_pointer(skb, 0, sizeof(_arph), &_arph);
		if (ah == NULL) {
			printk(" INCOMPLETE ARP header");
			goto out;
		}
		printk(" ARP HTYPE=%d, PTYPE=0x%04x, OPCODE=%d",
		       ntohs(ah->ar_hrd), ntohs(ah->ar_pro),
		       ntohs(ah->ar_op));

		/* If it's for Ethernet and the lengths are OK,
		 * then log the ARP payload */
		if (ah->ar_hrd == htons(1) &&
		    ah->ar_hln == ETH_ALEN &&
		    ah->ar_pln == sizeof(__be32)) {
			const struct arppayload *ap;
			struct arppayload _arpp;

			ap = skb_header_pointer(skb, sizeof(_arph),
						sizeof(_arpp), &_arpp);
			if (ap == NULL) {
				printk(" INCOMPLETE ARP payload");
				goto out;
			}
			printk(" ARP MAC SRC=");
			print_MAC(ap->mac_src);
			printk(" ARP IP SRC=%pI4", ap->ip_src);
			printk(" ARP MAC DST=");
			print_MAC(ap->mac_dst);
			printk(" ARP IP DST=%pI4", ap->ip_dst);
		}
	}
out:
	printk("\n");
	spin_unlock_bh(&ebt_log_lock);

}

static unsigned int
ebt_log_tg(struct sk_buff *skb, const struct xt_target_param *par)
{
	const struct ebt_log_info *info = par->targinfo;
	struct nf_loginfo li;

	li.type = NF_LOG_TYPE_LOG;
	li.u.log.level = info->loglevel;
	li.u.log.logflags = info->bitmask;

	if (info->bitmask & EBT_LOG_NFLOG)
		nf_log_packet(NFPROTO_BRIDGE, par->hooknum, skb, par->in,
		              par->out, &li, "%s", info->prefix);
	else
		ebt_log_packet(NFPROTO_BRIDGE, par->hooknum, skb, par->in,
		               par->out, &li, info->prefix);
	return EBT_CONTINUE;
}

static struct xt_target ebt_log_tg_reg __read_mostly = {
	.name		= "log",
	.revision	= 0,
	.family		= NFPROTO_BRIDGE,
	.target		= ebt_log_tg,
	.checkentry	= ebt_log_tg_check,
	.targetsize	= XT_ALIGN(sizeof(struct ebt_log_info)),
	.me		= THIS_MODULE,
};

static const struct nf_logger ebt_log_logger = {
	.name 		= "ebt_log",
	.logfn		= &ebt_log_packet,
	.me		= THIS_MODULE,
};

static int __init ebt_log_init(void)
{
	int ret;

	ret = xt_register_target(&ebt_log_tg_reg);
	if (ret < 0)
		return ret;
	nf_log_register(NFPROTO_BRIDGE, &ebt_log_logger);
	return 0;
}

static void __exit ebt_log_fini(void)
{
	nf_log_unregister(&ebt_log_logger);
	xt_unregister_target(&ebt_log_tg_reg);
}

module_init(ebt_log_init);
module_exit(ebt_log_fini);
MODULE_DESCRIPTION("Ebtables: Packet logging to syslog");
MODULE_LICENSE("GPL");
