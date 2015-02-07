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

/* Header file for kernel module to match connection tracking information.
 * GPL (C) 2001  Marc Boucher (marc@mbsi.ca).
 */

#ifndef _IPT_CONNTRACK_H
#define _IPT_CONNTRACK_H

#include <linux/netfilter/xt_conntrack.h>

#define IPT_CONNTRACK_STATE_BIT(ctinfo) XT_CONNTRACK_STATE_BIT(ctinfo)
#define IPT_CONNTRACK_STATE_INVALID 	XT_CONNTRACK_STATE_INVALID

#define IPT_CONNTRACK_STATE_SNAT 	XT_CONNTRACK_STATE_SNAT
#define IPT_CONNTRACK_STATE_DNAT	XT_CONNTRACK_STATE_DNAT
#define IPT_CONNTRACK_STATE_UNTRACKED	XT_CONNTRACK_STATE_UNTRACKED

/* flags, invflags: */
#define IPT_CONNTRACK_STATE		XT_CONNTRACK_STATE
#define IPT_CONNTRACK_PROTO		XT_CONNTRACK_PROTO
#define IPT_CONNTRACK_ORIGSRC		XT_CONNTRACK_ORIGSRC
#define IPT_CONNTRACK_ORIGDST		XT_CONNTRACK_ORIGDST
#define IPT_CONNTRACK_REPLSRC		XT_CONNTRACK_REPLSRC
#define IPT_CONNTRACK_REPLDST		XT_CONNTRACK_REPLDST
#define IPT_CONNTRACK_STATUS		XT_CONNTRACK_STATUS
#define IPT_CONNTRACK_EXPIRES		XT_CONNTRACK_EXPIRES

#define ipt_conntrack_info		xt_conntrack_info
#endif /*_IPT_CONNTRACK_H*/