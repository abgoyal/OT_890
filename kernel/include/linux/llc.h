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

#ifndef __LINUX_LLC_H
#define __LINUX_LLC_H
/*
 * IEEE 802.2 User Interface SAPs for Linux, data structures and indicators.
 *
 * Copyright (c) 2001 by Jay Schulist <jschlst@samba.org>
 *
 * This program can be redistributed or modified under the terms of the
 * GNU General Public License as published by the Free Software Foundation.
 * This program is distributed without any warranty or implied warranty
 * of merchantability or fitness for a particular purpose.
 *
 * See the GNU General Public License for more details.
 */
#define __LLC_SOCK_SIZE__ 16	/* sizeof(sockaddr_llc), word align. */
struct sockaddr_llc {
	sa_family_t     sllc_family;	/* AF_LLC */
	sa_family_t	sllc_arphrd;	/* ARPHRD_ETHER */
	unsigned char   sllc_test;
	unsigned char   sllc_xid;
	unsigned char	sllc_ua;	/* UA data, only for SOCK_STREAM. */
	unsigned char   sllc_sap;
	unsigned char   sllc_mac[IFHWADDRLEN];
	unsigned char   __pad[__LLC_SOCK_SIZE__ - sizeof(sa_family_t) * 2 -
			      sizeof(unsigned char) * 4 - IFHWADDRLEN];
};

/* sockopt definitions. */
enum llc_sockopts {
	LLC_OPT_UNKNOWN = 0,
	LLC_OPT_RETRY,		/* max retrans attempts. */
	LLC_OPT_SIZE,		/* max PDU size (octets). */
	LLC_OPT_ACK_TMR_EXP,	/* ack expire time (secs). */
	LLC_OPT_P_TMR_EXP,	/* pf cycle expire time (secs). */
	LLC_OPT_REJ_TMR_EXP,	/* rej sent expire time (secs). */
	LLC_OPT_BUSY_TMR_EXP,	/* busy state expire time (secs). */
	LLC_OPT_TX_WIN,		/* tx window size. */
	LLC_OPT_RX_WIN,		/* rx window size. */
	LLC_OPT_MAX
};

#define LLC_OPT_MAX_RETRY	 100
#define LLC_OPT_MAX_SIZE	4196
#define LLC_OPT_MAX_WIN		 127
#define LLC_OPT_MAX_ACK_TMR_EXP	  60
#define LLC_OPT_MAX_P_TMR_EXP	  60
#define LLC_OPT_MAX_REJ_TMR_EXP	  60
#define LLC_OPT_MAX_BUSY_TMR_EXP  60

/* LLC SAP types. */
#define LLC_SAP_NULL	0x00		/* NULL SAP. 			*/
#define LLC_SAP_LLC	0x02		/* LLC Sublayer Management. 	*/
#define LLC_SAP_SNA	0x04		/* SNA Path Control. 		*/
#define LLC_SAP_PNM	0x0E		/* Proway Network Management.	*/	
#define LLC_SAP_IP	0x06		/* TCP/IP. 			*/
#define LLC_SAP_BSPAN	0x42		/* Bridge Spanning Tree Proto	*/
#define LLC_SAP_MMS	0x4E		/* Manufacturing Message Srv.	*/
#define LLC_SAP_8208	0x7E		/* ISO 8208			*/
#define LLC_SAP_3COM	0x80		/* 3COM. 			*/
#define LLC_SAP_PRO	0x8E		/* Proway Active Station List	*/
#define LLC_SAP_SNAP	0xAA		/* SNAP. 			*/
#define LLC_SAP_BANYAN	0xBC		/* Banyan. 			*/
#define LLC_SAP_IPX	0xE0		/* IPX/SPX. 			*/
#define LLC_SAP_NETBEUI	0xF0		/* NetBEUI. 			*/
#define LLC_SAP_LANMGR	0xF4		/* LanManager. 			*/
#define LLC_SAP_IMPL	0xF8		/* IMPL				*/
#define LLC_SAP_DISC	0xFC		/* Discovery			*/
#define LLC_SAP_OSI	0xFE		/* OSI Network Layers. 		*/
#define LLC_SAP_LAR	0xDC		/* LAN Address Resolution 	*/
#define LLC_SAP_RM	0xD4		/* Resource Management 		*/
#define LLC_SAP_GLOBAL	0xFF		/* Global SAP. 			*/

#ifdef __KERNEL__
#define LLC_SAP_DYN_START	0xC0
#define LLC_SAP_DYN_STOP	0xDE
#define LLC_SAP_DYN_TRIES	4

#define llc_ui_skb_cb(__skb) ((struct sockaddr_llc *)&((__skb)->cb[0]))
#endif /* __KERNEL__ */
#endif /* __LINUX_LLC_H */
