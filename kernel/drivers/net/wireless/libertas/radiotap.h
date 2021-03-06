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

#include <net/ieee80211_radiotap.h>

struct tx_radiotap_hdr {
	struct ieee80211_radiotap_header hdr;
	u8 rate;
	u8 txpower;
	u8 rts_retries;
	u8 data_retries;
} __attribute__ ((packed));

#define TX_RADIOTAP_PRESENT (				\
	(1 << IEEE80211_RADIOTAP_RATE) |		\
	(1 << IEEE80211_RADIOTAP_DBM_TX_POWER) |	\
	(1 << IEEE80211_RADIOTAP_RTS_RETRIES) |		\
	(1 << IEEE80211_RADIOTAP_DATA_RETRIES)  |	\
	0)

#define IEEE80211_FC_VERSION_MASK    0x0003
#define IEEE80211_FC_TYPE_MASK       0x000c
#define IEEE80211_FC_TYPE_MGT        0x0000
#define IEEE80211_FC_TYPE_CTL        0x0004
#define IEEE80211_FC_TYPE_DATA       0x0008
#define IEEE80211_FC_SUBTYPE_MASK    0x00f0
#define IEEE80211_FC_TOFROMDS_MASK   0x0300
#define IEEE80211_FC_TODS_MASK       0x0100
#define IEEE80211_FC_FROMDS_MASK     0x0200
#define IEEE80211_FC_NODS            0x0000
#define IEEE80211_FC_TODS            0x0100
#define IEEE80211_FC_FROMDS          0x0200
#define IEEE80211_FC_DSTODS          0x0300

struct rx_radiotap_hdr {
	struct ieee80211_radiotap_header hdr;
	u8 flags;
	u8 rate;
	u16 chan_freq;
	u16 chan_flags;
	u8 antenna;
	u8 antsignal;
	u16 rx_flags;
#if 0
	u8 pad[IEEE80211_RADIOTAP_HDRLEN - 18];
#endif
} __attribute__ ((packed));

#define RX_RADIOTAP_PRESENT (			\
	(1 << IEEE80211_RADIOTAP_FLAGS) |	\
	(1 << IEEE80211_RADIOTAP_RATE) |	\
	(1 << IEEE80211_RADIOTAP_CHANNEL) |	\
	(1 << IEEE80211_RADIOTAP_ANTENNA) |	\
	(1 << IEEE80211_RADIOTAP_DB_ANTSIGNAL) |\
	(1 << IEEE80211_RADIOTAP_RX_FLAGS) |	\
	0)

