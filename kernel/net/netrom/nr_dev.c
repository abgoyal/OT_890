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
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Copyright Jonathan Naylor G4KLX (g4klx@g4klx.demon.co.uk)
 */
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/sysctl.h>
#include <linux/string.h>
#include <linux/socket.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/in.h>
#include <linux/if_ether.h>	/* For the statistics structure. */

#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <linux/inet.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/if_arp.h>
#include <linux/skbuff.h>

#include <net/ip.h>
#include <net/arp.h>

#include <net/ax25.h>
#include <net/netrom.h>

/*
 *	Only allow IP over NET/ROM frames through if the netrom device is up.
 */

int nr_rx_ip(struct sk_buff *skb, struct net_device *dev)
{
	struct net_device_stats *stats = netdev_priv(dev);

	if (!netif_running(dev)) {
		stats->rx_dropped++;
		return 0;
	}

	stats->rx_packets++;
	stats->rx_bytes += skb->len;

	skb->protocol = htons(ETH_P_IP);

	/* Spoof incoming device */
	skb->dev      = dev;
	skb->mac_header = skb->network_header;
	skb_reset_network_header(skb);
	skb->pkt_type = PACKET_HOST;

	netif_rx(skb);

	return 1;
}

#ifdef CONFIG_INET

static int nr_rebuild_header(struct sk_buff *skb)
{
	unsigned char *bp = skb->data;

	if (arp_find(bp + 7, skb))
		return 1;

	bp[6] &= ~AX25_CBIT;
	bp[6] &= ~AX25_EBIT;
	bp[6] |= AX25_SSSID_SPARE;
	bp    += AX25_ADDR_LEN;

	bp[6] &= ~AX25_CBIT;
	bp[6] |= AX25_EBIT;
	bp[6] |= AX25_SSSID_SPARE;

	return 0;
}

#else

static int nr_rebuild_header(struct sk_buff *skb)
{
	return 1;
}

#endif

static int nr_header(struct sk_buff *skb, struct net_device *dev,
		     unsigned short type,
		     const void *daddr, const void *saddr, unsigned len)
{
	unsigned char *buff = skb_push(skb, NR_NETWORK_LEN + NR_TRANSPORT_LEN);

	memcpy(buff, (saddr != NULL) ? saddr : dev->dev_addr, dev->addr_len);
	buff[6] &= ~AX25_CBIT;
	buff[6] &= ~AX25_EBIT;
	buff[6] |= AX25_SSSID_SPARE;
	buff    += AX25_ADDR_LEN;

	if (daddr != NULL)
		memcpy(buff, daddr, dev->addr_len);
	buff[6] &= ~AX25_CBIT;
	buff[6] |= AX25_EBIT;
	buff[6] |= AX25_SSSID_SPARE;
	buff    += AX25_ADDR_LEN;

	*buff++ = sysctl_netrom_network_ttl_initialiser;

	*buff++ = NR_PROTO_IP;
	*buff++ = NR_PROTO_IP;
	*buff++ = 0;
	*buff++ = 0;
	*buff++ = NR_PROTOEXT;

	if (daddr != NULL)
		return 37;

	return -37;
}

static int __must_check nr_set_mac_address(struct net_device *dev, void *addr)
{
	struct sockaddr *sa = addr;
	int err;

	if (!memcmp(dev->dev_addr, sa->sa_data, dev->addr_len))
		return 0;

	if (dev->flags & IFF_UP) {
		err = ax25_listen_register((ax25_address *)sa->sa_data, NULL);
		if (err)
			return err;

		ax25_listen_release((ax25_address *)dev->dev_addr, NULL);
	}

	memcpy(dev->dev_addr, sa->sa_data, dev->addr_len);

	return 0;
}

static int nr_open(struct net_device *dev)
{
	int err;

	err = ax25_listen_register((ax25_address *)dev->dev_addr, NULL);
	if (err)
		return err;

	netif_start_queue(dev);

	return 0;
}

static int nr_close(struct net_device *dev)
{
	ax25_listen_release((ax25_address *)dev->dev_addr, NULL);
	netif_stop_queue(dev);
	return 0;
}

static int nr_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct nr_private *nr = netdev_priv(dev);
	struct net_device_stats *stats = &nr->stats;
	unsigned int len = skb->len;

	if (!nr_route_frame(skb, NULL)) {
		kfree_skb(skb);
		stats->tx_errors++;
		return 0;
	}

	stats->tx_packets++;
	stats->tx_bytes += len;

	return 0;
}

static struct net_device_stats *nr_get_stats(struct net_device *dev)
{
	struct nr_private *nr = netdev_priv(dev);

	return &nr->stats;
}

static const struct header_ops nr_header_ops = {
	.create	= nr_header,
	.rebuild= nr_rebuild_header,
};


void nr_setup(struct net_device *dev)
{
	dev->mtu		= NR_MAX_PACKET_SIZE;
	dev->hard_start_xmit	= nr_xmit;
	dev->open		= nr_open;
	dev->stop		= nr_close;

	dev->header_ops		= &nr_header_ops;
	dev->hard_header_len	= NR_NETWORK_LEN + NR_TRANSPORT_LEN;
	dev->addr_len		= AX25_ADDR_LEN;
	dev->type		= ARPHRD_NETROM;
	dev->set_mac_address    = nr_set_mac_address;

	/* New-style flags. */
	dev->flags		= IFF_NOARP;

	dev->get_stats 		= nr_get_stats;
}
