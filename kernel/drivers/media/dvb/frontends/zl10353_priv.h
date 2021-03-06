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
 *  Driver for Zarlink DVB-T ZL10353 demodulator
 *
 *  Copyright (C) 2006, 2007 Christopher Pascoe <c.pascoe@itee.uq.edu.au>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _ZL10353_PRIV_
#define _ZL10353_PRIV_

#define ID_ZL10353	0x14

#define msb(x) (((x) >> 8) & 0xff)
#define lsb(x) ((x) & 0xff)

enum zl10353_reg_addr {
	INTERRUPT_0        = 0x00,
	INTERRUPT_1        = 0x01,
	INTERRUPT_2        = 0x02,
	INTERRUPT_3        = 0x03,
	INTERRUPT_4        = 0x04,
	INTERRUPT_5        = 0x05,
	STATUS_6           = 0x06,
	STATUS_7           = 0x07,
	STATUS_8           = 0x08,
	STATUS_9           = 0x09,
	AGC_GAIN_1         = 0x0A,
	AGC_GAIN_0         = 0x0B,
	SNR                = 0x10,
	RS_ERR_CNT_2       = 0x11,
	RS_ERR_CNT_1       = 0x12,
	RS_ERR_CNT_0       = 0x13,
	RS_UBC_1           = 0x14,
	RS_UBC_0           = 0x15,
	TPS_RECEIVED_1     = 0x1D,
	TPS_RECEIVED_0     = 0x1E,
	TPS_CURRENT_1      = 0x1F,
	TPS_CURRENT_0      = 0x20,
	RESET              = 0x55,
	AGC_TARGET         = 0x56,
	MCLK_RATIO         = 0x5C,
	ACQ_CTL            = 0x5E,
	TRL_NOMINAL_RATE_1 = 0x65,
	TRL_NOMINAL_RATE_0 = 0x66,
	INPUT_FREQ_1       = 0x6C,
	INPUT_FREQ_0       = 0x6D,
	TPS_GIVEN_1        = 0x6E,
	TPS_GIVEN_0        = 0x6F,
	TUNER_GO           = 0x70,
	FSM_GO             = 0x71,
	CHIP_ID            = 0x7F,
	CHAN_STEP_1        = 0xE4,
	CHAN_STEP_0        = 0xE5,
	OFDM_LOCK_TIME     = 0xE7,
	FEC_LOCK_TIME      = 0xE8,
	ACQ_DELAY          = 0xE9,
};

#endif                          /* _ZL10353_PRIV_ */
