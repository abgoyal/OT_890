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

/* p80211conv.h
*
* Ether/802.11 conversions and packet buffer routines
*
* Copyright (C) 1999 AbsoluteValue Systems, Inc.  All Rights Reserved.
* --------------------------------------------------------------------
*
* linux-wlan
*
*   The contents of this file are subject to the Mozilla Public
*   License Version 1.1 (the "License"); you may not use this file
*   except in compliance with the License. You may obtain a copy of
*   the License at http://www.mozilla.org/MPL/
*
*   Software distributed under the License is distributed on an "AS
*   IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
*   implied. See the License for the specific language governing
*   rights and limitations under the License.
*
*   Alternatively, the contents of this file may be used under the
*   terms of the GNU Public License version 2 (the "GPL"), in which
*   case the provisions of the GPL are applicable instead of the
*   above.  If you wish to allow the use of your version of this file
*   only under the terms of the GPL and not to allow others to use
*   your version of this file under the MPL, indicate your decision
*   by deleting the provisions above and replace them with the notice
*   and other provisions required by the GPL.  If you do not delete
*   the provisions above, a recipient may use your version of this
*   file under either the MPL or the GPL.
*
* --------------------------------------------------------------------
*
* Inquiries regarding the linux-wlan Open Source project can be
* made directly to:
*
* AbsoluteValue Systems Inc.
* info@linux-wlan.com
* http://www.linux-wlan.com
*
* --------------------------------------------------------------------
*
* Portions of the development of this software were funded by
* Intersil Corporation as part of PRISM(R) chipset product development.
*
* --------------------------------------------------------------------
*
* This file declares the functions, types and macros that perform
* Ethernet to/from 802.11 frame conversions.
*
* --------------------------------------------------------------------
*/

#ifndef _LINUX_P80211CONV_H
#define _LINUX_P80211CONV_H

/*================================================================*/
/* Constants */

#define WLAN_ETHADDR_LEN	6
#define WLAN_IEEE_OUI_LEN	3

#define WLAN_ETHCONV_ENCAP	1
#define WLAN_ETHCONV_RFC1042	2
#define WLAN_ETHCONV_8021h	3

#define WLAN_MIN_ETHFRM_LEN	60
#define WLAN_MAX_ETHFRM_LEN	1514
#define WLAN_ETHHDR_LEN		14

#define P80211CAPTURE_VERSION	0x80211001

/*================================================================*/
/* Macros */

#define	P80211_FRMMETA_MAGIC		0x802110

#define P80211SKB_FRMMETA(s) \
	(((((p80211_frmmeta_t*)((s)->cb))->magic)==P80211_FRMMETA_MAGIC) ? \
		((p80211_frmmeta_t*)((s)->cb)) : \
		(NULL))

#define P80211SKB_RXMETA(s) \
	(P80211SKB_FRMMETA((s)) ?  P80211SKB_FRMMETA((s))->rx : ((p80211_rxmeta_t*)(NULL)))

typedef struct p80211_rxmeta
{
	struct wlandevice	*wlandev;

	u64	mactime;	/* Hi-rez MAC-supplied time value */
	u64	hosttime;	/* Best-rez host supplied time value */

	unsigned int	rxrate;		/* Receive data rate in 100kbps */
	unsigned int	priority;	/* 0-15, 0=contention, 6=CF */
	int	signal;		/* An SSI, see p80211netdev.h */
	int	noise;		/* An SSI, see p80211netdev.h */
	unsigned int	channel;	/* Receive channel (mostly for snifs) */
	unsigned int	preamble;	/* P80211ENUM_preambletype_* */
	unsigned int	encoding;	/* P80211ENUM_encoding_* */

} p80211_rxmeta_t;

typedef struct p80211_frmmeta
{
	unsigned int			magic;
	p80211_rxmeta_t		*rx;
} p80211_frmmeta_t;

void p80211skb_free(struct wlandevice *wlandev, struct sk_buff *skb);
int p80211skb_rxmeta_attach(struct wlandevice *wlandev, struct sk_buff *skb);
void p80211skb_rxmeta_detach(struct sk_buff *skb);

/*================================================================*/
/* Types */

/*
 * Frame capture header.  (See doc/capturefrm.txt)
 */
typedef struct p80211_caphdr
{
	u32		version;
	u32		length;
	u64		mactime;
	u64		hosttime;
	u32		phytype;
	u32		channel;
	u32		datarate;
	u32		antenna;
	u32		priority;
	u32		ssi_type;
	s32		ssi_signal;
	s32		ssi_noise;
	u32		preamble;
	u32		encoding;
} p80211_caphdr_t;

/* buffer free method pointer type */
typedef void (* freebuf_method_t)(void *buf, int size);

typedef struct p80211_metawep {
	void  *data;
	u8 iv[4];
	u8 icv[4];
} p80211_metawep_t;

/* local ether header type */
typedef struct wlan_ethhdr
{
	u8	daddr[WLAN_ETHADDR_LEN];
	u8	saddr[WLAN_ETHADDR_LEN];
	u16	type;
} __WLAN_ATTRIB_PACK__ wlan_ethhdr_t;

/* local llc header type */
typedef struct wlan_llc
{
	u8	dsap;
	u8	ssap;
	u8	ctl;
} __WLAN_ATTRIB_PACK__ wlan_llc_t;

/* local snap header type */
typedef struct wlan_snap
{
	u8	oui[WLAN_IEEE_OUI_LEN];
	u16	type;
} __WLAN_ATTRIB_PACK__ wlan_snap_t;

/* Circular include trick */
struct wlandevice;

/*================================================================*/
/* Externs */

/*================================================================*/
/*Function Declarations */

int skb_p80211_to_ether( struct wlandevice *wlandev, u32 ethconv,
			 struct sk_buff *skb);
int skb_ether_to_p80211( struct wlandevice *wlandev, u32 ethconv,
			 struct sk_buff *skb, p80211_hdr_t *p80211_hdr,
			 p80211_metawep_t *p80211_wep );

int p80211_stt_findproto(u16 proto);
int p80211_stt_addproto(u16 proto);

#endif
