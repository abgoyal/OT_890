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
 * Copyright (c) 2004-2008 Reyk Floeter <reyk@openbsd.org>
 * Copyright (c) 2006-2008 Nick Kossifidis <mickflemm@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/*
 * Common ar5xxx EEPROM data offsets (set these on AR5K_EEPROM_BASE)
 */
#define AR5K_EEPROM_MAGIC		0x003d	/* EEPROM Magic number */
#define AR5K_EEPROM_MAGIC_VALUE		0x5aa5	/* Default - found on EEPROM */
#define AR5K_EEPROM_MAGIC_5212		0x0000145c /* 5212 */
#define AR5K_EEPROM_MAGIC_5211		0x0000145b /* 5211 */
#define AR5K_EEPROM_MAGIC_5210		0x0000145a /* 5210 */

#define AR5K_EEPROM_REG_DOMAIN		0x00bf	/* EEPROM regdom */
#define AR5K_EEPROM_CHECKSUM		0x00c0	/* EEPROM checksum */
#define AR5K_EEPROM_INFO_BASE		0x00c0	/* EEPROM header */
#define AR5K_EEPROM_INFO_MAX		(0x400 - AR5K_EEPROM_INFO_BASE)
#define AR5K_EEPROM_INFO_CKSUM		0xffff
#define AR5K_EEPROM_INFO(_n)		(AR5K_EEPROM_INFO_BASE + (_n))

#define AR5K_EEPROM_VERSION		AR5K_EEPROM_INFO(1)	/* EEPROM Version */
#define AR5K_EEPROM_VERSION_3_0		0x3000	/* No idea what's going on before this version */
#define AR5K_EEPROM_VERSION_3_1		0x3001	/* ob/db values for 2Ghz (ar5211_rfregs) */
#define AR5K_EEPROM_VERSION_3_2		0x3002	/* different frequency representation (eeprom_bin2freq) */
#define AR5K_EEPROM_VERSION_3_3		0x3003	/* offsets changed, has 32 CTLs (see below) and ee_false_detect (eeprom_read_modes) */
#define AR5K_EEPROM_VERSION_3_4		0x3004	/* has ee_i_gain, ee_cck_ofdm_power_delta (eeprom_read_modes) */
#define AR5K_EEPROM_VERSION_4_0		0x4000	/* has ee_misc, ee_cal_pier, ee_turbo_max_power and ee_xr_power (eeprom_init) */
#define AR5K_EEPROM_VERSION_4_1		0x4001	/* has ee_margin_tx_rx (eeprom_init) */
#define AR5K_EEPROM_VERSION_4_2		0x4002	/* has ee_cck_ofdm_gain_delta (eeprom_init) */
#define AR5K_EEPROM_VERSION_4_3		0x4003	/* power calibration changes */
#define AR5K_EEPROM_VERSION_4_4		0x4004
#define AR5K_EEPROM_VERSION_4_5		0x4005
#define AR5K_EEPROM_VERSION_4_6		0x4006	/* has ee_scaled_cck_delta */
#define AR5K_EEPROM_VERSION_4_7		0x3007	/* 4007 ? */
#define AR5K_EEPROM_VERSION_4_9		0x4009	/* EAR futureproofing */
#define AR5K_EEPROM_VERSION_5_0		0x5000	/* Has 2413 PDADC calibration etc */
#define AR5K_EEPROM_VERSION_5_1		0x5001	/* Has capability values */
#define AR5K_EEPROM_VERSION_5_3		0x5003	/* Has spur mitigation tables */

#define AR5K_EEPROM_MODE_11A		0
#define AR5K_EEPROM_MODE_11B		1
#define AR5K_EEPROM_MODE_11G		2

#define AR5K_EEPROM_HDR			AR5K_EEPROM_INFO(2)	/* Header that contains the device caps */
#define AR5K_EEPROM_HDR_11A(_v)		(((_v) >> AR5K_EEPROM_MODE_11A) & 0x1)
#define AR5K_EEPROM_HDR_11B(_v)		(((_v) >> AR5K_EEPROM_MODE_11B) & 0x1)
#define AR5K_EEPROM_HDR_11G(_v)		(((_v) >> AR5K_EEPROM_MODE_11G) & 0x1)
#define AR5K_EEPROM_HDR_T_2GHZ_DIS(_v)	(((_v) >> 3) & 0x1)	/* Disable turbo for 2Ghz (?) */
#define AR5K_EEPROM_HDR_T_5GHZ_DBM(_v)	(((_v) >> 4) & 0x7f)	/* Max turbo power for a/XR mode (eeprom_init) */
#define AR5K_EEPROM_HDR_DEVICE(_v)	(((_v) >> 11) & 0x7)
#define AR5K_EEPROM_HDR_RFKILL(_v)	(((_v) >> 14) & 0x1)	/* Device has RFKill support */
#define AR5K_EEPROM_HDR_T_5GHZ_DIS(_v)	(((_v) >> 15) & 0x1)	/* Disable turbo for 5Ghz */

#define AR5K_EEPROM_RFKILL_GPIO_SEL	0x0000001c
#define AR5K_EEPROM_RFKILL_GPIO_SEL_S	2
#define AR5K_EEPROM_RFKILL_POLARITY	0x00000002
#define AR5K_EEPROM_RFKILL_POLARITY_S	1

/* Newer EEPROMs are using a different offset */
#define AR5K_EEPROM_OFF(_v, _v3_0, _v3_3) \
	(((_v) >= AR5K_EEPROM_VERSION_3_3) ? _v3_3 : _v3_0)

#define AR5K_EEPROM_ANT_GAIN(_v)	AR5K_EEPROM_OFF(_v, 0x00c4, 0x00c3)
#define AR5K_EEPROM_ANT_GAIN_5GHZ(_v)	((s8)(((_v) >> 8) & 0xff))
#define AR5K_EEPROM_ANT_GAIN_2GHZ(_v)	((s8)((_v) & 0xff))

/* Misc values available since EEPROM 4.0 */
#define AR5K_EEPROM_MISC0		AR5K_EEPROM_INFO(4)
#define AR5K_EEPROM_EARSTART(_v)	((_v) & 0xfff)
#define AR5K_EEPROM_HDR_XR2_DIS(_v)	(((_v) >> 12) & 0x1)
#define AR5K_EEPROM_HDR_XR5_DIS(_v)	(((_v) >> 13) & 0x1)
#define AR5K_EEPROM_EEMAP(_v)		(((_v) >> 14) & 0x3)

#define AR5K_EEPROM_MISC1			AR5K_EEPROM_INFO(5)
#define AR5K_EEPROM_TARGET_PWRSTART(_v)		((_v) & 0xfff)
#define AR5K_EEPROM_HAS32KHZCRYSTAL(_v)		(((_v) >> 14) & 0x1)
#define AR5K_EEPROM_HAS32KHZCRYSTAL_OLD(_v)	(((_v) >> 15) & 0x1)

#define AR5K_EEPROM_MISC2			AR5K_EEPROM_INFO(6)
#define AR5K_EEPROM_EEP_FILE_VERSION(_v)	(((_v) >> 8) & 0xff)
#define AR5K_EEPROM_EAR_FILE_VERSION(_v)	((_v) & 0xff)

#define AR5K_EEPROM_MISC3		AR5K_EEPROM_INFO(7)
#define AR5K_EEPROM_ART_BUILD_NUM(_v)	(((_v) >> 10) & 0x3f)
#define AR5K_EEPROM_EAR_FILE_ID(_v)	((_v) & 0xff)

#define AR5K_EEPROM_MISC4		AR5K_EEPROM_INFO(8)
#define AR5K_EEPROM_CAL_DATA_START(_v)	(((_v) >> 4) & 0xfff)
#define AR5K_EEPROM_MASK_R0(_v)		(((_v) >> 2) & 0x3)
#define AR5K_EEPROM_MASK_R1(_v)		((_v) & 0x3)

#define AR5K_EEPROM_MISC5		AR5K_EEPROM_INFO(9)
#define AR5K_EEPROM_COMP_DIS(_v)	((_v) & 0x1)
#define AR5K_EEPROM_AES_DIS(_v)		(((_v) >> 1) & 0x1)
#define AR5K_EEPROM_FF_DIS(_v)		(((_v) >> 2) & 0x1)
#define AR5K_EEPROM_BURST_DIS(_v)	(((_v) >> 3) & 0x1)
#define AR5K_EEPROM_MAX_QCU(_v)		(((_v) >> 4) & 0xf)
#define AR5K_EEPROM_HEAVY_CLIP_EN(_v)	(((_v) >> 8) & 0x1)
#define AR5K_EEPROM_KEY_CACHE_SIZE(_v)	(((_v) >> 12) & 0xf)

#define AR5K_EEPROM_MISC6		AR5K_EEPROM_INFO(10)
#define AR5K_EEPROM_TX_CHAIN_DIS	((_v) & 0x8)
#define AR5K_EEPROM_RX_CHAIN_DIS	(((_v) >> 3) & 0x8)
#define AR5K_EEPROM_FCC_MID_EN		(((_v) >> 6) & 0x1)
#define AR5K_EEPROM_JAP_U1EVEN_EN	(((_v) >> 7) & 0x1)
#define AR5K_EEPROM_JAP_U2_EN		(((_v) >> 8) & 0x1)
#define AR5K_EEPROM_JAP_U1ODD_EN	(((_v) >> 9) & 0x1)
#define AR5K_EEPROM_JAP_11A_NEW_EN	(((_v) >> 10) & 0x1)

/* calibration settings */
#define AR5K_EEPROM_MODES_11A(_v)	AR5K_EEPROM_OFF(_v, 0x00c5, 0x00d4)
#define AR5K_EEPROM_MODES_11B(_v)	AR5K_EEPROM_OFF(_v, 0x00d0, 0x00f2)
#define AR5K_EEPROM_MODES_11G(_v)	AR5K_EEPROM_OFF(_v, 0x00da, 0x010d)
#define AR5K_EEPROM_CTL(_v)		AR5K_EEPROM_OFF(_v, 0x00e4, 0x0128)	/* Conformance test limits */
#define AR5K_EEPROM_GROUPS_START(_v)	AR5K_EEPROM_OFF(_v, 0x0100, 0x0150)	/* Start of Groups */
#define AR5K_EEPROM_GROUP1_OFFSET	0x0
#define AR5K_EEPROM_GROUP2_OFFSET	0x5
#define AR5K_EEPROM_GROUP3_OFFSET	0x37
#define AR5K_EEPROM_GROUP4_OFFSET	0x46
#define AR5K_EEPROM_GROUP5_OFFSET	0x55
#define AR5K_EEPROM_GROUP6_OFFSET	0x65
#define AR5K_EEPROM_GROUP7_OFFSET	0x69
#define AR5K_EEPROM_GROUP8_OFFSET	0x6f

#define AR5K_EEPROM_TARGET_PWR_OFF_11A(_v)	AR5K_EEPROM_OFF(_v, AR5K_EEPROM_GROUPS_START(_v) + \
								AR5K_EEPROM_GROUP5_OFFSET, 0x0000)
#define AR5K_EEPROM_TARGET_PWR_OFF_11B(_v)	AR5K_EEPROM_OFF(_v, AR5K_EEPROM_GROUPS_START(_v) + \
								AR5K_EEPROM_GROUP6_OFFSET, 0x0010)
#define AR5K_EEPROM_TARGET_PWR_OFF_11G(_v)	AR5K_EEPROM_OFF(_v, AR5K_EEPROM_GROUPS_START(_v) + \
								AR5K_EEPROM_GROUP7_OFFSET, 0x0014)

/* [3.1 - 3.3] */
#define AR5K_EEPROM_OBDB0_2GHZ		0x00ec
#define AR5K_EEPROM_OBDB1_2GHZ		0x00ed

#define AR5K_EEPROM_PROTECT		0x003f	/* EEPROM protect status */
#define AR5K_EEPROM_PROTECT_RD_0_31	0x0001	/* Read protection bit for offsets 0x0 - 0x1f */
#define AR5K_EEPROM_PROTECT_WR_0_31	0x0002	/* Write protection bit for offsets 0x0 - 0x1f */
#define AR5K_EEPROM_PROTECT_RD_32_63	0x0004	/* 0x20 - 0x3f */
#define AR5K_EEPROM_PROTECT_WR_32_63	0x0008
#define AR5K_EEPROM_PROTECT_RD_64_127	0x0010	/* 0x40 - 0x7f */
#define AR5K_EEPROM_PROTECT_WR_64_127	0x0020
#define AR5K_EEPROM_PROTECT_RD_128_191	0x0040	/* 0x80 - 0xbf (regdom) */
#define AR5K_EEPROM_PROTECT_WR_128_191	0x0080
#define AR5K_EEPROM_PROTECT_RD_192_207	0x0100	/* 0xc0 - 0xcf */
#define AR5K_EEPROM_PROTECT_WR_192_207	0x0200
#define AR5K_EEPROM_PROTECT_RD_208_223	0x0400	/* 0xd0 - 0xdf */
#define AR5K_EEPROM_PROTECT_WR_208_223	0x0800
#define AR5K_EEPROM_PROTECT_RD_224_239	0x1000	/* 0xe0 - 0xef */
#define AR5K_EEPROM_PROTECT_WR_224_239	0x2000
#define AR5K_EEPROM_PROTECT_RD_240_255	0x4000	/* 0xf0 - 0xff */
#define AR5K_EEPROM_PROTECT_WR_240_255	0x8000

/* Some EEPROM defines */
#define AR5K_EEPROM_EEP_SCALE		100
#define AR5K_EEPROM_EEP_DELTA		10
#define AR5K_EEPROM_N_MODES		3
#define AR5K_EEPROM_N_5GHZ_CHAN		10
#define AR5K_EEPROM_N_2GHZ_CHAN		3
#define AR5K_EEPROM_N_2GHZ_CHAN_2413	4
#define AR5K_EEPROM_MAX_CHAN		10
#define AR5K_EEPROM_N_PWR_POINTS_5111	11
#define AR5K_EEPROM_N_PCDAC		11
#define AR5K_EEPROM_N_PHASE_CAL		5
#define AR5K_EEPROM_N_TEST_FREQ		8
#define AR5K_EEPROM_N_EDGES		8
#define AR5K_EEPROM_N_INTERCEPTS	11
#define AR5K_EEPROM_FREQ_M(_v)		AR5K_EEPROM_OFF(_v, 0x7f, 0xff)
#define AR5K_EEPROM_PCDAC_M		0x3f
#define AR5K_EEPROM_PCDAC_START		1
#define AR5K_EEPROM_PCDAC_STOP		63
#define AR5K_EEPROM_PCDAC_STEP		1
#define AR5K_EEPROM_NON_EDGE_M		0x40
#define AR5K_EEPROM_CHANNEL_POWER	8
#define AR5K_EEPROM_N_OBDB		4
#define AR5K_EEPROM_OBDB_DIS		0xffff
#define AR5K_EEPROM_CHANNEL_DIS		0xff
#define AR5K_EEPROM_SCALE_OC_DELTA(_x)	(((_x) * 2) / 10)
#define AR5K_EEPROM_N_CTLS(_v)		AR5K_EEPROM_OFF(_v, 16, 32)
#define AR5K_EEPROM_MAX_CTLS		32
#define AR5K_EEPROM_N_XPD_PER_CHANNEL	4
#define AR5K_EEPROM_N_XPD0_POINTS	4
#define AR5K_EEPROM_N_XPD3_POINTS	3
#define AR5K_EEPROM_N_PD_GAINS		4
#define AR5K_EEPROM_N_PD_POINTS		5
#define AR5K_EEPROM_N_INTERCEPT_10_2GHZ	35
#define AR5K_EEPROM_N_INTERCEPT_10_5GHZ	55
#define AR5K_EEPROM_POWER_M		0x3f
#define AR5K_EEPROM_POWER_MIN		0
#define AR5K_EEPROM_POWER_MAX		3150
#define AR5K_EEPROM_POWER_STEP		50
#define AR5K_EEPROM_POWER_TABLE_SIZE	64
#define AR5K_EEPROM_N_POWER_LOC_11B	4
#define AR5K_EEPROM_N_POWER_LOC_11G	6
#define AR5K_EEPROM_I_GAIN		10
#define AR5K_EEPROM_CCK_OFDM_DELTA	15
#define AR5K_EEPROM_N_IQ_CAL		2

#define AR5K_EEPROM_READ(_o, _v) do {			\
	ret = ath5k_hw_eeprom_read(ah, (_o), &(_v));	\
	if (ret)					\
		return ret;				\
} while (0)

#define AR5K_EEPROM_READ_HDR(_o, _v)					\
	AR5K_EEPROM_READ(_o, ah->ah_capabilities.cap_eeprom._v);	\

enum ath5k_ant_setting {
	AR5K_ANT_VARIABLE	= 0,	/* variable by programming */
	AR5K_ANT_FIXED_A	= 1,	/* fixed to 11a frequencies */
	AR5K_ANT_FIXED_B	= 2,	/* fixed to 11b frequencies */
	AR5K_ANT_MAX		= 3,
};

enum ath5k_ctl_mode {
	AR5K_CTL_11A = 0,
	AR5K_CTL_11B = 1,
	AR5K_CTL_11G = 2,
	AR5K_CTL_TURBO = 3,
	AR5K_CTL_108G = 4,
	AR5K_CTL_2GHT20 = 5,
	AR5K_CTL_5GHT20 = 6,
	AR5K_CTL_2GHT40 = 7,
	AR5K_CTL_5GHT40 = 8,
	AR5K_CTL_MODE_M = 15,
};

/* Per channel calibration data, used for power table setup */
struct ath5k_chan_pcal_info_rf5111 {
	/* Power levels in half dbm units
	 * for one power curve. */
	u8		pwr[AR5K_EEPROM_N_PWR_POINTS_5111];
	/* PCDAC table steps
	 * for the above values */
	u8		pcdac[AR5K_EEPROM_N_PWR_POINTS_5111];
	/* Starting PCDAC step */
	u8		pcdac_min;
	/* Final PCDAC step */
	u8		pcdac_max;
};

struct ath5k_chan_pcal_info_rf5112 {
	/* Power levels in quarter dBm units
	 * for lower (0) and higher (3)
	 * level curves */
	s8		pwr_x0[AR5K_EEPROM_N_XPD0_POINTS];
	s8		pwr_x3[AR5K_EEPROM_N_XPD3_POINTS];
	/* PCDAC table steps
	 * for the above values */
	u8	pcdac_x0[AR5K_EEPROM_N_XPD0_POINTS];
	u8	pcdac_x3[AR5K_EEPROM_N_XPD3_POINTS];
};

struct ath5k_chan_pcal_info_rf2413 {
	/* Starting pwr/pddac values */
	s8		pwr_i[AR5K_EEPROM_N_PD_GAINS];
	u8	pddac_i[AR5K_EEPROM_N_PD_GAINS];
	/* (pwr,pddac) points */
	s8		pwr[AR5K_EEPROM_N_PD_GAINS]
				[AR5K_EEPROM_N_PD_POINTS];
	u8	pddac[AR5K_EEPROM_N_PD_GAINS]
				[AR5K_EEPROM_N_PD_POINTS];
};

struct ath5k_chan_pcal_info {
	/* Frequency */
	u16	freq;
	/* Max available power */
	s8		max_pwr;
	union {
		struct ath5k_chan_pcal_info_rf5111 rf5111_info;
		struct ath5k_chan_pcal_info_rf5112 rf5112_info;
		struct ath5k_chan_pcal_info_rf2413 rf2413_info;
	};
};

/* Per rate calibration data for each mode, used for power table setup */
struct ath5k_rate_pcal_info {
	u16	freq; /* Frequency */
	/* Power level for 6-24Mbit/s rates */
	u16	target_power_6to24;
	/* Power level for 36Mbit rate */
	u16	target_power_36;
	/* Power level for 48Mbit rate */
	u16	target_power_48;
	/* Power level for 54Mbit rate */
	u16	target_power_54;
};

/* Power edges for conformance test limits */
struct ath5k_edge_power {
	u16 freq;
	u16 edge; /* in half dBm */
	bool flag;
};

/* EEPROM calibration data */
struct ath5k_eeprom_info {

	/* Header information */
	u16	ee_magic;
	u16	ee_protect;
	u16	ee_regdomain;
	u16	ee_version;
	u16	ee_header;
	u16	ee_ant_gain;
	u16	ee_misc0;
	u16	ee_misc1;
	u16	ee_misc2;
	u16	ee_misc3;
	u16	ee_misc4;
	u16	ee_misc5;
	u16	ee_misc6;
	u16	ee_cck_ofdm_gain_delta;
	u16	ee_cck_ofdm_power_delta;
	u16	ee_scaled_cck_delta;

	/* Used for tx thermal adjustment (eeprom_init, rfregs) */
	u16	ee_tx_clip;
	u16	ee_pwd_84;
	u16	ee_pwd_90;
	u16	ee_gain_select;

	/* RF Calibration settings (reset, rfregs) */
	u16	ee_i_cal[AR5K_EEPROM_N_MODES];
	u16	ee_q_cal[AR5K_EEPROM_N_MODES];
	u16	ee_fixed_bias[AR5K_EEPROM_N_MODES];
	u16	ee_turbo_max_power[AR5K_EEPROM_N_MODES];
	u16	ee_xr_power[AR5K_EEPROM_N_MODES];
	u16	ee_switch_settling[AR5K_EEPROM_N_MODES];
	u16	ee_atn_tx_rx[AR5K_EEPROM_N_MODES];
	u16	ee_ant_control[AR5K_EEPROM_N_MODES][AR5K_EEPROM_N_PCDAC];
	u16	ee_ob[AR5K_EEPROM_N_MODES][AR5K_EEPROM_N_OBDB];
	u16	ee_db[AR5K_EEPROM_N_MODES][AR5K_EEPROM_N_OBDB];
	u16	ee_tx_end2xlna_enable[AR5K_EEPROM_N_MODES];
	u16	ee_tx_end2xpa_disable[AR5K_EEPROM_N_MODES];
	u16	ee_tx_frm2xpa_enable[AR5K_EEPROM_N_MODES];
	u16	ee_thr_62[AR5K_EEPROM_N_MODES];
	u16	ee_xlna_gain[AR5K_EEPROM_N_MODES];
	u16	ee_xpd[AR5K_EEPROM_N_MODES];
	u16	ee_x_gain[AR5K_EEPROM_N_MODES];
	u16	ee_i_gain[AR5K_EEPROM_N_MODES];
	u16	ee_margin_tx_rx[AR5K_EEPROM_N_MODES];
	u16	ee_switch_settling_turbo[AR5K_EEPROM_N_MODES];
	u16	ee_margin_tx_rx_turbo[AR5K_EEPROM_N_MODES];
	u16	ee_atn_tx_rx_turbo[AR5K_EEPROM_N_MODES];

	/* Power calibration data */
	u16	ee_false_detect[AR5K_EEPROM_N_MODES];

	/* Number of pd gain curves per mode (RF2413) */
	u8 ee_pd_gains[AR5K_EEPROM_N_MODES];

	u8 ee_n_piers[AR5K_EEPROM_N_MODES];
	struct ath5k_chan_pcal_info	ee_pwr_cal_a[AR5K_EEPROM_N_5GHZ_CHAN];
	struct ath5k_chan_pcal_info	ee_pwr_cal_b[AR5K_EEPROM_N_2GHZ_CHAN];
	struct ath5k_chan_pcal_info	ee_pwr_cal_g[AR5K_EEPROM_N_2GHZ_CHAN];

	/* Per rate target power levels */
	u16	ee_rate_target_pwr_num[AR5K_EEPROM_N_MODES];
	struct ath5k_rate_pcal_info	ee_rate_tpwr_a[AR5K_EEPROM_N_5GHZ_CHAN];
	struct ath5k_rate_pcal_info	ee_rate_tpwr_b[AR5K_EEPROM_N_2GHZ_CHAN];
	struct ath5k_rate_pcal_info	ee_rate_tpwr_g[AR5K_EEPROM_N_2GHZ_CHAN];

	/* Conformance test limits (Unused) */
	u16	ee_ctls;
	u16	ee_ctl[AR5K_EEPROM_MAX_CTLS];
	struct ath5k_edge_power ee_ctl_pwr[AR5K_EEPROM_N_EDGES * AR5K_EEPROM_MAX_CTLS];

	/* Noise Floor Calibration settings */
	s16	ee_noise_floor_thr[AR5K_EEPROM_N_MODES];
	s8	ee_adc_desired_size[AR5K_EEPROM_N_MODES];
	s8	ee_pga_desired_size[AR5K_EEPROM_N_MODES];
	s8	ee_adc_desired_size_turbo[AR5K_EEPROM_N_MODES];
	s8	ee_pga_desired_size_turbo[AR5K_EEPROM_N_MODES];
	s8	ee_pd_gain_overlap;

	u32	ee_antenna[AR5K_EEPROM_N_MODES][AR5K_ANT_MAX];
};
