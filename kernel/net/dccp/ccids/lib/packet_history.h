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
 *  Packet RX/TX history data structures and routines for TFRC-based protocols.
 *
 *  Copyright (c) 2007   The University of Aberdeen, Scotland, UK
 *  Copyright (c) 2005-6 The University of Waikato, Hamilton, New Zealand.
 *
 *  This code has been developed by the University of Waikato WAND
 *  research group. For further information please see http://www.wand.net.nz/
 *  or e-mail Ian McDonald - ian.mcdonald@jandi.co.nz
 *
 *  This code also uses code from Lulea University, rereleased as GPL by its
 *  authors:
 *  Copyright (c) 2003 Nils-Erik Mattsson, Joacim Haggmark, Magnus Erixzon
 *
 *  Changes to meet Linux coding standards, to make it meet latest ccid3 draft
 *  and to make it work as a loadable module in the DCCP stack written by
 *  Arnaldo Carvalho de Melo <acme@conectiva.com.br>.
 *
 *  Copyright (c) 2005 Arnaldo Carvalho de Melo <acme@conectiva.com.br>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _DCCP_PKT_HIST_
#define _DCCP_PKT_HIST_

#include <linux/list.h>
#include <linux/slab.h>
#include "tfrc.h"

struct tfrc_tx_hist_entry;

extern int  tfrc_tx_hist_add(struct tfrc_tx_hist_entry **headp, u64 seqno);
extern void tfrc_tx_hist_purge(struct tfrc_tx_hist_entry **headp);
extern u32  tfrc_tx_hist_rtt(struct tfrc_tx_hist_entry *head,
			     const u64 seqno, const ktime_t now);

/* Subtraction a-b modulo-16, respects circular wrap-around */
#define SUB16(a, b) (((a) + 16 - (b)) & 0xF)

/* Number of packets to wait after a missing packet (RFC 4342, 6.1) */
#define TFRC_NDUPACK 3

/**
 * tfrc_rx_hist_entry - Store information about a single received packet
 * @tfrchrx_seqno:	DCCP packet sequence number
 * @tfrchrx_ccval:	window counter value of packet (RFC 4342, 8.1)
 * @tfrchrx_ndp:	the NDP count (if any) of the packet
 * @tfrchrx_tstamp:	actual receive time of packet
 */
struct tfrc_rx_hist_entry {
	u64		 tfrchrx_seqno:48,
			 tfrchrx_ccval:4,
			 tfrchrx_type:4;
	u64		 tfrchrx_ndp:48;
	ktime_t		 tfrchrx_tstamp;
};

/**
 * tfrc_rx_hist  -  RX history structure for TFRC-based protocols
 *
 * @ring:		Packet history for RTT sampling and loss detection
 * @loss_count:		Number of entries in circular history
 * @loss_start:		Movable index (for loss detection)
 * @rtt_sample_prev:	Used during RTT sampling, points to candidate entry
 */
struct tfrc_rx_hist {
	struct tfrc_rx_hist_entry *ring[TFRC_NDUPACK + 1];
	u8			  loss_count:2,
				  loss_start:2;
#define rtt_sample_prev		  loss_start
};

/**
 * tfrc_rx_hist_index - index to reach n-th entry after loss_start
 */
static inline u8 tfrc_rx_hist_index(const struct tfrc_rx_hist *h, const u8 n)
{
	return (h->loss_start + n) & TFRC_NDUPACK;
}

/**
 * tfrc_rx_hist_last_rcv - entry with highest-received-seqno so far
 */
static inline struct tfrc_rx_hist_entry *
			tfrc_rx_hist_last_rcv(const struct tfrc_rx_hist *h)
{
	return h->ring[tfrc_rx_hist_index(h, h->loss_count)];
}

/**
 * tfrc_rx_hist_entry - return the n-th history entry after loss_start
 */
static inline struct tfrc_rx_hist_entry *
			tfrc_rx_hist_entry(const struct tfrc_rx_hist *h, const u8 n)
{
	return h->ring[tfrc_rx_hist_index(h, n)];
}

/**
 * tfrc_rx_hist_loss_prev - entry with highest-received-seqno before loss was detected
 */
static inline struct tfrc_rx_hist_entry *
			tfrc_rx_hist_loss_prev(const struct tfrc_rx_hist *h)
{
	return h->ring[h->loss_start];
}

/* indicate whether previously a packet was detected missing */
static inline bool tfrc_rx_hist_loss_pending(const struct tfrc_rx_hist *h)
{
	return h->loss_count > 0;
}

extern void tfrc_rx_hist_add_packet(struct tfrc_rx_hist *h,
				    const struct sk_buff *skb, const u64 ndp);

extern int tfrc_rx_hist_duplicate(struct tfrc_rx_hist *h, struct sk_buff *skb);

struct tfrc_loss_hist;
extern int  tfrc_rx_handle_loss(struct tfrc_rx_hist *h,
				struct tfrc_loss_hist *lh,
				struct sk_buff *skb, const u64 ndp,
				u32 (*first_li)(struct sock *sk),
				struct sock *sk);
extern u32 tfrc_rx_hist_sample_rtt(struct tfrc_rx_hist *h,
				   const struct sk_buff *skb);
extern int tfrc_rx_hist_alloc(struct tfrc_rx_hist *h);
extern void tfrc_rx_hist_purge(struct tfrc_rx_hist *h);

#endif /* _DCCP_PKT_HIST_ */