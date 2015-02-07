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
 * DVB USB Linux driver for Afatech AF9015 DVB-T USB2.0 receiver
 *
 * Copyright (C) 2007 Antti Palosaari <crope@iki.fi>
 *
 * Thanks to Afatech who kindly provided information.
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _AF9013_PRIV_
#define _AF9013_PRIV_

#define LOG_PREFIX "af9013"
extern int af9013_debug;

#define dprintk(var, level, args...) \
	    do { if ((var & level)) printk(args); } while (0)

#define debug_dump(b, l, func) {\
	int loop_; \
	for (loop_ = 0; loop_ < l; loop_++) \
		func("%02x ", b[loop_]); \
	func("\n");\
}

#define deb_info(args...) dprintk(af9013_debug, 0x01, args)

#undef err
#define err(f, arg...)  printk(KERN_ERR     LOG_PREFIX": " f "\n" , ## arg)
#undef info
#define info(f, arg...) printk(KERN_INFO    LOG_PREFIX": " f "\n" , ## arg)
#undef warn
#define warn(f, arg...) printk(KERN_WARNING LOG_PREFIX": " f "\n" , ## arg)

#define AF9013_DEFAULT_FIRMWARE     "dvb-fe-af9013.fw"

struct regdesc {
	u16 addr;
	u8  pos:4;
	u8  len:4;
	u8  val;
};

struct snr_table {
	u32 val;
	u8 snr;
};

/* QPSK SNR lookup table */
static struct snr_table qpsk_snr_table[] = {
	{ 0x0b4771,  0 },
	{ 0x0c1aed,  1 },
	{ 0x0d0d27,  2 },
	{ 0x0e4d19,  3 },
	{ 0x0e5da8,  4 },
	{ 0x107097,  5 },
	{ 0x116975,  6 },
	{ 0x1252d9,  7 },
	{ 0x131fa4,  8 },
	{ 0x13d5e1,  9 },
	{ 0x148e53, 10 },
	{ 0x15358b, 11 },
	{ 0x15dd29, 12 },
	{ 0x168112, 13 },
	{ 0x170b61, 14 },
	{ 0xffffff, 15 },
};

/* QAM16 SNR lookup table */
static struct snr_table qam16_snr_table[] = {
	{ 0x05eb62,  5 },
	{ 0x05fecf,  6 },
	{ 0x060b80,  7 },
	{ 0x062501,  8 },
	{ 0x064865,  9 },
	{ 0x069604, 10 },
	{ 0x06f356, 11 },
	{ 0x07706a, 12 },
	{ 0x0804d3, 13 },
	{ 0x089d1a, 14 },
	{ 0x093e3d, 15 },
	{ 0x09e35d, 16 },
	{ 0x0a7c3c, 17 },
	{ 0x0afaf8, 18 },
	{ 0x0b719d, 19 },
	{ 0xffffff, 20 },
};

/* QAM64 SNR lookup table */
static struct snr_table qam64_snr_table[] = {
	{ 0x03109b, 12 },
	{ 0x0310d4, 13 },
	{ 0x031920, 14 },
	{ 0x0322d0, 15 },
	{ 0x0339fc, 16 },
	{ 0x0364a1, 17 },
	{ 0x038bcc, 18 },
	{ 0x03c7d3, 19 },
	{ 0x0408cc, 20 },
	{ 0x043bed, 21 },
	{ 0x048061, 22 },
	{ 0x04be95, 23 },
	{ 0x04fa7d, 24 },
	{ 0x052405, 25 },
	{ 0x05570d, 26 },
	{ 0xffffff, 27 },
};

static struct regdesc ofsm_init[] = {
	{ 0xd73a, 0, 8, 0xa1 },
	{ 0xd73b, 0, 8, 0x1f },
	{ 0xd73c, 4, 4, 0x0a },
	{ 0xd732, 3, 1, 0x00 },
	{ 0xd731, 4, 2, 0x03 },
	{ 0xd73d, 7, 1, 0x01 },
	{ 0xd740, 0, 1, 0x00 },
	{ 0xd740, 1, 1, 0x00 },
	{ 0xd740, 2, 1, 0x00 },
	{ 0xd740, 3, 1, 0x01 },
	{ 0xd3c1, 4, 1, 0x01 },
	{ 0xd3a2, 0, 8, 0x00 },
	{ 0xd3a3, 0, 8, 0x04 },
	{ 0xd305, 0, 8, 0x32 },
	{ 0xd306, 0, 8, 0x10 },
	{ 0xd304, 0, 8, 0x04 },
	{ 0x9112, 0, 1, 0x01 },
	{ 0x911d, 0, 1, 0x01 },
	{ 0x911a, 0, 1, 0x01 },
	{ 0x911b, 0, 1, 0x01 },
	{ 0x9bce, 0, 4, 0x02 },
	{ 0x9116, 0, 1, 0x01 },
	{ 0x9bd1, 0, 1, 0x01 },
	{ 0xd2e0, 0, 8, 0xd0 },
	{ 0xd2e9, 0, 4, 0x0d },
	{ 0xd38c, 0, 8, 0xfc },
	{ 0xd38d, 0, 8, 0x00 },
	{ 0xd38e, 0, 8, 0x7e },
	{ 0xd38f, 0, 8, 0x00 },
	{ 0xd390, 0, 8, 0x2f },
	{ 0xd145, 4, 1, 0x01 },
	{ 0xd1a9, 4, 1, 0x01 },
	{ 0xd158, 5, 3, 0x01 },
	{ 0xd159, 0, 6, 0x06 },
	{ 0xd167, 0, 8, 0x00 },
	{ 0xd168, 0, 4, 0x07 },
	{ 0xd1c3, 5, 3, 0x00 },
	{ 0xd1c4, 0, 6, 0x00 },
	{ 0xd1c5, 0, 7, 0x10 },
	{ 0xd1c6, 0, 3, 0x02 },
	{ 0xd080, 2, 5, 0x03 },
	{ 0xd081, 4, 4, 0x09 },
	{ 0xd098, 4, 4, 0x0f },
	{ 0xd098, 0, 4, 0x03 },
	{ 0xdbc0, 3, 1, 0x01 },
	{ 0xdbc0, 4, 1, 0x01 },
	{ 0xdbc7, 0, 8, 0x08 },
	{ 0xdbc8, 4, 4, 0x00 },
	{ 0xdbc9, 0, 5, 0x01 },
	{ 0xd280, 0, 8, 0xe0 },
	{ 0xd281, 0, 8, 0xff },
	{ 0xd282, 0, 8, 0xff },
	{ 0xd283, 0, 8, 0xc3 },
	{ 0xd284, 0, 8, 0xff },
	{ 0xd285, 0, 4, 0x01 },
	{ 0xd0f0, 0, 7, 0x1a },
	{ 0xd0f1, 4, 1, 0x01 },
	{ 0xd0f2, 0, 8, 0x0c },
	{ 0xd103, 0, 4, 0x08 },
	{ 0xd0f8, 0, 7, 0x20 },
	{ 0xd111, 5, 1, 0x00 },
	{ 0xd111, 6, 1, 0x00 },
	{ 0x910b, 0, 8, 0x0a },
	{ 0x9115, 0, 8, 0x02 },
	{ 0x910c, 0, 8, 0x02 },
	{ 0x910d, 0, 8, 0x08 },
	{ 0x910e, 0, 8, 0x0a },
	{ 0x9bf6, 0, 8, 0x06 },
	{ 0x9bf8, 0, 8, 0x02 },
	{ 0x9bf7, 0, 8, 0x05 },
	{ 0x9bf9, 0, 8, 0x0f },
	{ 0x9bfc, 0, 8, 0x13 },
	{ 0x9bd3, 0, 8, 0xff },
	{ 0x9bbe, 0, 1, 0x01 },
	{ 0x9bcc, 0, 1, 0x01 },
};

/* Panasonic ENV77H11D5 tuner init
   AF9013_TUNER_ENV77H11D5 = 129 */
static struct regdesc tuner_init_env77h11d5[] = {
	{ 0x9bd5, 0, 8, 0x01 },
	{ 0x9bd6, 0, 8, 0x03 },
	{ 0x9bbe, 0, 8, 0x01 },
	{ 0xd1a0, 1, 1, 0x01 },
	{ 0xd000, 0, 1, 0x01 },
	{ 0xd000, 1, 1, 0x00 },
	{ 0xd001, 1, 1, 0x01 },
	{ 0xd001, 0, 1, 0x00 },
	{ 0xd001, 5, 1, 0x00 },
	{ 0xd002, 0, 5, 0x19 },
	{ 0xd003, 0, 5, 0x1a },
	{ 0xd004, 0, 5, 0x19 },
	{ 0xd005, 0, 5, 0x1a },
	{ 0xd00e, 0, 5, 0x10 },
	{ 0xd00f, 0, 3, 0x04 },
	{ 0xd00f, 3, 3, 0x05 },
	{ 0xd010, 0, 3, 0x04 },
	{ 0xd010, 3, 3, 0x05 },
	{ 0xd016, 4, 4, 0x03 },
	{ 0xd01f, 0, 6, 0x0a },
	{ 0xd020, 0, 6, 0x0a },
	{ 0x9bda, 0, 8, 0x00 },
	{ 0x9be3, 0, 8, 0x00 },
	{ 0xd015, 0, 8, 0x50 },
	{ 0xd016, 0, 1, 0x00 },
	{ 0xd044, 0, 8, 0x46 },
	{ 0xd045, 0, 1, 0x00 },
	{ 0xd008, 0, 8, 0xdf },
	{ 0xd009, 0, 2, 0x02 },
	{ 0xd006, 0, 8, 0x44 },
	{ 0xd007, 0, 2, 0x01 },
	{ 0xd00c, 0, 8, 0xeb },
	{ 0xd00d, 0, 2, 0x02 },
	{ 0xd00a, 0, 8, 0xf4 },
	{ 0xd00b, 0, 2, 0x01 },
	{ 0x9bba, 0, 8, 0xf9 },
	{ 0x9bc3, 0, 8, 0xdf },
	{ 0x9bc4, 0, 8, 0x02 },
	{ 0x9bc5, 0, 8, 0xeb },
	{ 0x9bc6, 0, 8, 0x02 },
	{ 0x9bc9, 0, 8, 0x52 },
	{ 0xd011, 0, 8, 0x3c },
	{ 0xd012, 0, 2, 0x01 },
	{ 0xd013, 0, 8, 0xf7 },
	{ 0xd014, 0, 2, 0x02 },
	{ 0xd040, 0, 8, 0x0b },
	{ 0xd041, 0, 2, 0x02 },
	{ 0xd042, 0, 8, 0x4d },
	{ 0xd043, 0, 2, 0x00 },
	{ 0xd045, 1, 1, 0x00 },
	{ 0x9bcf, 0, 1, 0x01 },
	{ 0xd045, 2, 1, 0x01 },
	{ 0xd04f, 0, 8, 0x9a },
	{ 0xd050, 0, 1, 0x01 },
	{ 0xd051, 0, 8, 0x5a },
	{ 0xd052, 0, 1, 0x01 },
	{ 0xd053, 0, 8, 0x50 },
	{ 0xd054, 0, 8, 0x46 },
	{ 0x9bd7, 0, 8, 0x0a },
	{ 0x9bd8, 0, 8, 0x14 },
	{ 0x9bd9, 0, 8, 0x08 },
};

/* Microtune MT2060 tuner init
   AF9013_TUNER_MT2060     = 130 */
static struct regdesc tuner_init_mt2060[] = {
	{ 0x9bd5, 0, 8, 0x01 },
	{ 0x9bd6, 0, 8, 0x07 },
	{ 0xd1a0, 1, 1, 0x01 },
	{ 0xd000, 0, 1, 0x01 },
	{ 0xd000, 1, 1, 0x00 },
	{ 0xd001, 1, 1, 0x01 },
	{ 0xd001, 0, 1, 0x00 },
	{ 0xd001, 5, 1, 0x00 },
	{ 0xd002, 0, 5, 0x19 },
	{ 0xd003, 0, 5, 0x1a },
	{ 0xd004, 0, 5, 0x19 },
	{ 0xd005, 0, 5, 0x1a },
	{ 0xd00e, 0, 5, 0x10 },
	{ 0xd00f, 0, 3, 0x04 },
	{ 0xd00f, 3, 3, 0x05 },
	{ 0xd010, 0, 3, 0x04 },
	{ 0xd010, 3, 3, 0x05 },
	{ 0xd016, 4, 4, 0x03 },
	{ 0xd01f, 0, 6, 0x0a },
	{ 0xd020, 0, 6, 0x0a },
	{ 0x9bda, 0, 8, 0x00 },
	{ 0x9be3, 0, 8, 0x00 },
	{ 0x9bbe, 0, 1, 0x00 },
	{ 0x9bcc, 0, 1, 0x00 },
	{ 0x9bb9, 0, 8, 0x75 },
	{ 0x9bcd, 0, 8, 0x24 },
	{ 0x9bff, 0, 8, 0x30 },
	{ 0xd015, 0, 8, 0x46 },
	{ 0xd016, 0, 1, 0x00 },
	{ 0xd044, 0, 8, 0x46 },
	{ 0xd045, 0, 1, 0x00 },
	{ 0xd008, 0, 8, 0x0f },
	{ 0xd009, 0, 2, 0x02 },
	{ 0xd006, 0, 8, 0x32 },
	{ 0xd007, 0, 2, 0x01 },
	{ 0xd00c, 0, 8, 0x36 },
	{ 0xd00d, 0, 2, 0x03 },
	{ 0xd00a, 0, 8, 0x35 },
	{ 0xd00b, 0, 2, 0x01 },
	{ 0x9bc7, 0, 8, 0x07 },
	{ 0x9bc8, 0, 8, 0x90 },
	{ 0x9bc3, 0, 8, 0x0f },
	{ 0x9bc4, 0, 8, 0x02 },
	{ 0x9bc5, 0, 8, 0x36 },
	{ 0x9bc6, 0, 8, 0x03 },
	{ 0x9bba, 0, 8, 0xc9 },
	{ 0x9bc9, 0, 8, 0x79 },
	{ 0xd011, 0, 8, 0x10 },
	{ 0xd012, 0, 2, 0x01 },
	{ 0xd013, 0, 8, 0x45 },
	{ 0xd014, 0, 2, 0x03 },
	{ 0xd040, 0, 8, 0x98 },
	{ 0xd041, 0, 2, 0x00 },
	{ 0xd042, 0, 8, 0xcf },
	{ 0xd043, 0, 2, 0x03 },
	{ 0xd045, 1, 1, 0x00 },
	{ 0x9bcf, 0, 1, 0x01 },
	{ 0xd045, 2, 1, 0x01 },
	{ 0xd04f, 0, 8, 0x9a },
	{ 0xd050, 0, 1, 0x01 },
	{ 0xd051, 0, 8, 0x5a },
	{ 0xd052, 0, 1, 0x01 },
	{ 0xd053, 0, 8, 0x50 },
	{ 0xd054, 0, 8, 0x46 },
	{ 0x9bd7, 0, 8, 0x0a },
	{ 0x9bd8, 0, 8, 0x14 },
	{ 0x9bd9, 0, 8, 0x08 },
	{ 0x9bd0, 0, 8, 0xcc },
	{ 0x9be4, 0, 8, 0xa0 },
	{ 0x9bbd, 0, 8, 0x8e },
	{ 0x9be2, 0, 8, 0x4d },
	{ 0x9bee, 0, 1, 0x01 },
};

/* Microtune MT2060 tuner init
   AF9013_TUNER_MT2060_2   = 147 */
static struct regdesc tuner_init_mt2060_2[] = {
	{ 0x9bd5, 0, 8, 0x01 },
	{ 0x9bd6, 0, 8, 0x06 },
	{ 0x9bbe, 0, 8, 0x01 },
	{ 0xd1a0, 1, 1, 0x01 },
	{ 0xd000, 0, 1, 0x01 },
	{ 0xd000, 1, 1, 0x00 },
	{ 0xd001, 1, 1, 0x01 },
	{ 0xd001, 0, 1, 0x00 },
	{ 0xd001, 5, 1, 0x00 },
	{ 0xd002, 0, 5, 0x19 },
	{ 0xd003, 0, 5, 0x1a },
	{ 0xd004, 0, 5, 0x19 },
	{ 0xd005, 0, 5, 0x1a },
	{ 0xd00e, 0, 5, 0x10 },
	{ 0xd00f, 0, 3, 0x04 },
	{ 0xd00f, 3, 3, 0x05 },
	{ 0xd010, 0, 3, 0x04 },
	{ 0xd010, 3, 3, 0x05 },
	{ 0xd016, 4, 4, 0x03 },
	{ 0xd01f, 0, 6, 0x0a },
	{ 0xd020, 0, 6, 0x0a },
	{ 0xd015, 0, 8, 0x46 },
	{ 0xd016, 0, 1, 0x00 },
	{ 0xd044, 0, 8, 0x46 },
	{ 0xd045, 0, 1, 0x00 },
	{ 0xd008, 0, 8, 0x0f },
	{ 0xd009, 0, 2, 0x02 },
	{ 0xd006, 0, 8, 0x32 },
	{ 0xd007, 0, 2, 0x01 },
	{ 0xd00c, 0, 8, 0x36 },
	{ 0xd00d, 0, 2, 0x03 },
	{ 0xd00a, 0, 8, 0x35 },
	{ 0xd00b, 0, 2, 0x01 },
	{ 0x9bc7, 0, 8, 0x07 },
	{ 0x9bc8, 0, 8, 0x90 },
	{ 0x9bc3, 0, 8, 0x0f },
	{ 0x9bc4, 0, 8, 0x02 },
	{ 0x9bc5, 0, 8, 0x36 },
	{ 0x9bc6, 0, 8, 0x03 },
	{ 0x9bba, 0, 8, 0xc9 },
	{ 0x9bc9, 0, 8, 0x79 },
	{ 0xd011, 0, 8, 0x10 },
	{ 0xd012, 0, 2, 0x01 },
	{ 0xd013, 0, 8, 0x45 },
	{ 0xd014, 0, 2, 0x03 },
	{ 0xd040, 0, 8, 0x98 },
	{ 0xd041, 0, 2, 0x00 },
	{ 0xd042, 0, 8, 0xcf },
	{ 0xd043, 0, 2, 0x03 },
	{ 0xd045, 1, 1, 0x00 },
	{ 0x9bcf, 0, 8, 0x01 },
	{ 0xd045, 2, 1, 0x01 },
	{ 0xd04f, 0, 8, 0x9a },
	{ 0xd050, 0, 1, 0x01 },
	{ 0xd051, 0, 8, 0x5a },
	{ 0xd052, 0, 1, 0x01 },
	{ 0xd053, 0, 8, 0x96 },
	{ 0xd054, 0, 8, 0x46 },
	{ 0xd045, 7, 1, 0x00 },
	{ 0x9bd7, 0, 8, 0x0a },
	{ 0x9bd8, 0, 8, 0x14 },
	{ 0x9bd9, 0, 8, 0x08 },
};

/* MaxLinear MXL5003 tuner init
   AF9013_TUNER_MXL5003D   =   3 */
static struct regdesc tuner_init_mxl5003d[] = {
	{ 0x9bd5, 0, 8, 0x01 },
	{ 0x9bd6, 0, 8, 0x09 },
	{ 0xd1a0, 1, 1, 0x01 },
	{ 0xd000, 0, 1, 0x01 },
	{ 0xd000, 1, 1, 0x00 },
	{ 0xd001, 1, 1, 0x01 },
	{ 0xd001, 0, 1, 0x00 },
	{ 0xd001, 5, 1, 0x00 },
	{ 0xd002, 0, 5, 0x19 },
	{ 0xd003, 0, 5, 0x1a },
	{ 0xd004, 0, 5, 0x19 },
	{ 0xd005, 0, 5, 0x1a },
	{ 0xd00e, 0, 5, 0x10 },
	{ 0xd00f, 0, 3, 0x04 },
	{ 0xd00f, 3, 3, 0x05 },
	{ 0xd010, 0, 3, 0x04 },
	{ 0xd010, 3, 3, 0x05 },
	{ 0xd016, 4, 4, 0x03 },
	{ 0xd01f, 0, 6, 0x0a },
	{ 0xd020, 0, 6, 0x0a },
	{ 0x9bda, 0, 8, 0x00 },
	{ 0x9be3, 0, 8, 0x00 },
	{ 0x9bfc, 0, 8, 0x0f },
	{ 0x9bf6, 0, 8, 0x01 },
	{ 0x9bbe, 0, 1, 0x01 },
	{ 0xd015, 0, 8, 0x33 },
	{ 0xd016, 0, 1, 0x00 },
	{ 0xd044, 0, 8, 0x40 },
	{ 0xd045, 0, 1, 0x00 },
	{ 0xd008, 0, 8, 0x0f },
	{ 0xd009, 0, 2, 0x02 },
	{ 0xd006, 0, 8, 0x6c },
	{ 0xd007, 0, 2, 0x00 },
	{ 0xd00c, 0, 8, 0x3d },
	{ 0xd00d, 0, 2, 0x00 },
	{ 0xd00a, 0, 8, 0x45 },
	{ 0xd00b, 0, 2, 0x01 },
	{ 0x9bc7, 0, 8, 0x07 },
	{ 0x9bc8, 0, 8, 0x52 },
	{ 0x9bc3, 0, 8, 0x0f },
	{ 0x9bc4, 0, 8, 0x02 },
	{ 0x9bc5, 0, 8, 0x3d },
	{ 0x9bc6, 0, 8, 0x00 },
	{ 0x9bba, 0, 8, 0xa2 },
	{ 0x9bc9, 0, 8, 0xa0 },
	{ 0xd011, 0, 8, 0x56 },
	{ 0xd012, 0, 2, 0x00 },
	{ 0xd013, 0, 8, 0x50 },
	{ 0xd014, 0, 2, 0x00 },
	{ 0xd040, 0, 8, 0x56 },
	{ 0xd041, 0, 2, 0x00 },
	{ 0xd042, 0, 8, 0x50 },
	{ 0xd043, 0, 2, 0x00 },
	{ 0xd045, 1, 1, 0x00 },
	{ 0x9bcf, 0, 8, 0x01 },
	{ 0xd045, 2, 1, 0x01 },
	{ 0xd04f, 0, 8, 0x9a },
	{ 0xd050, 0, 1, 0x01 },
	{ 0xd051, 0, 8, 0x5a },
	{ 0xd052, 0, 1, 0x01 },
	{ 0xd053, 0, 8, 0x50 },
	{ 0xd054, 0, 8, 0x46 },
	{ 0x9bd7, 0, 8, 0x0a },
	{ 0x9bd8, 0, 8, 0x14 },
	{ 0x9bd9, 0, 8, 0x08 },
};

/* MaxLinear MXL5005 tuner init
   AF9013_TUNER_MXL5005D   =  13
   AF9013_TUNER_MXL5005R   =  30 */
static struct regdesc tuner_init_mxl5005[] = {
	{ 0x9bd5, 0, 8, 0x01 },
	{ 0x9bd6, 0, 8, 0x07 },
	{ 0xd1a0, 1, 1, 0x01 },
	{ 0xd000, 0, 1, 0x01 },
	{ 0xd000, 1, 1, 0x00 },
	{ 0xd001, 1, 1, 0x01 },
	{ 0xd001, 0, 1, 0x00 },
	{ 0xd001, 5, 1, 0x00 },
	{ 0xd002, 0, 5, 0x19 },
	{ 0xd003, 0, 5, 0x1a },
	{ 0xd004, 0, 5, 0x19 },
	{ 0xd005, 0, 5, 0x1a },
	{ 0xd00e, 0, 5, 0x10 },
	{ 0xd00f, 0, 3, 0x04 },
	{ 0xd00f, 3, 3, 0x05 },
	{ 0xd010, 0, 3, 0x04 },
	{ 0xd010, 3, 3, 0x05 },
	{ 0xd016, 4, 4, 0x03 },
	{ 0xd01f, 0, 6, 0x0a },
	{ 0xd020, 0, 6, 0x0a },
	{ 0x9bda, 0, 8, 0x01 },
	{ 0x9be3, 0, 8, 0x01 },
	{ 0x9bbe, 0, 1, 0x01 },
	{ 0x9bcc, 0, 1, 0x01 },
	{ 0x9bb9, 0, 8, 0x00 },
	{ 0x9bcd, 0, 8, 0x28 },
	{ 0x9bff, 0, 8, 0x24 },
	{ 0xd015, 0, 8, 0x40 },
	{ 0xd016, 0, 1, 0x00 },
	{ 0xd044, 0, 8, 0x40 },
	{ 0xd045, 0, 1, 0x00 },
	{ 0xd008, 0, 8, 0x0f },
	{ 0xd009, 0, 2, 0x02 },
	{ 0xd006, 0, 8, 0x73 },
	{ 0xd007, 0, 2, 0x01 },
	{ 0xd00c, 0, 8, 0xfa },
	{ 0xd00d, 0, 2, 0x01 },
	{ 0xd00a, 0, 8, 0xff },
	{ 0xd00b, 0, 2, 0x01 },
	{ 0x9bc7, 0, 8, 0x23 },
	{ 0x9bc8, 0, 8, 0x55 },
	{ 0x9bc3, 0, 8, 0x01 },
	{ 0x9bc4, 0, 8, 0x02 },
	{ 0x9bc5, 0, 8, 0xfa },
	{ 0x9bc6, 0, 8, 0x01 },
	{ 0x9bba, 0, 8, 0xff },
	{ 0x9bc9, 0, 8, 0xff },
	{ 0x9bd3, 0, 8, 0x95 },
	{ 0xd011, 0, 8, 0x70 },
	{ 0xd012, 0, 2, 0x01 },
	{ 0xd013, 0, 8, 0xfb },
	{ 0xd014, 0, 2, 0x01 },
	{ 0xd040, 0, 8, 0x70 },
	{ 0xd041, 0, 2, 0x01 },
	{ 0xd042, 0, 8, 0xfb },
	{ 0xd043, 0, 2, 0x01 },
	{ 0xd045, 1, 1, 0x00 },
	{ 0x9bcf, 0, 1, 0x01 },
	{ 0xd045, 2, 1, 0x01 },
	{ 0xd04f, 0, 8, 0x9a },
	{ 0xd050, 0, 1, 0x01 },
	{ 0xd051, 0, 8, 0x5a },
	{ 0xd052, 0, 1, 0x01 },
	{ 0xd053, 0, 8, 0x50 },
	{ 0xd054, 0, 8, 0x46 },
	{ 0x9bd7, 0, 8, 0x0a },
	{ 0x9bd8, 0, 8, 0x14 },
	{ 0x9bd9, 0, 8, 0x08 },
	{ 0x9bd0, 0, 8, 0x93 },
	{ 0x9be4, 0, 8, 0xfe },
	{ 0x9bbd, 0, 8, 0x63 },
	{ 0x9be2, 0, 8, 0xfe },
	{ 0x9bee, 0, 1, 0x01 },
};

/* Quantek QT1010 tuner init
   AF9013_TUNER_QT1010     = 134
   AF9013_TUNER_QT1010A    = 162 */
static struct regdesc tuner_init_qt1010[] = {
	{ 0x9bd5, 0, 8, 0x01 },
	{ 0x9bd6, 0, 8, 0x09 },
	{ 0xd1a0, 1, 1, 0x01 },
	{ 0xd000, 0, 1, 0x01 },
	{ 0xd000, 1, 1, 0x00 },
	{ 0xd001, 1, 1, 0x01 },
	{ 0xd001, 0, 1, 0x00 },
	{ 0xd001, 5, 1, 0x00 },
	{ 0xd002, 0, 5, 0x19 },
	{ 0xd003, 0, 5, 0x1a },
	{ 0xd004, 0, 5, 0x19 },
	{ 0xd005, 0, 5, 0x1a },
	{ 0xd00e, 0, 5, 0x10 },
	{ 0xd00f, 0, 3, 0x04 },
	{ 0xd00f, 3, 3, 0x05 },
	{ 0xd010, 0, 3, 0x04 },
	{ 0xd010, 3, 3, 0x05 },
	{ 0xd016, 4, 4, 0x03 },
	{ 0xd01f, 0, 6, 0x0a },
	{ 0xd020, 0, 6, 0x0a },
	{ 0x9bda, 0, 8, 0x01 },
	{ 0x9be3, 0, 8, 0x01 },
	{ 0xd015, 0, 8, 0x46 },
	{ 0xd016, 0, 1, 0x00 },
	{ 0xd044, 0, 8, 0x46 },
	{ 0xd045, 0, 1, 0x00 },
	{ 0x9bbe, 0, 1, 0x01 },
	{ 0x9bcc, 0, 1, 0x01 },
	{ 0x9bb9, 0, 8, 0x00 },
	{ 0x9bcd, 0, 8, 0x28 },
	{ 0x9bff, 0, 8, 0x20 },
	{ 0xd008, 0, 8, 0x0f },
	{ 0xd009, 0, 2, 0x02 },
	{ 0xd006, 0, 8, 0x99 },
	{ 0xd007, 0, 2, 0x01 },
	{ 0xd00c, 0, 8, 0x0f },
	{ 0xd00d, 0, 2, 0x02 },
	{ 0xd00a, 0, 8, 0x50 },
	{ 0xd00b, 0, 2, 0x01 },
	{ 0x9bc7, 0, 8, 0x00 },
	{ 0x9bc8, 0, 8, 0x00 },
	{ 0x9bc3, 0, 8, 0x0f },
	{ 0x9bc4, 0, 8, 0x02 },
	{ 0x9bc5, 0, 8, 0x0f },
	{ 0x9bc6, 0, 8, 0x02 },
	{ 0x9bba, 0, 8, 0xc5 },
	{ 0x9bc9, 0, 8, 0xff },
	{ 0xd011, 0, 8, 0x58 },
	{ 0xd012, 0, 2, 0x02 },
	{ 0xd013, 0, 8, 0x89 },
	{ 0xd014, 0, 2, 0x01 },
	{ 0xd040, 0, 8, 0x58 },
	{ 0xd041, 0, 2, 0x02 },
	{ 0xd042, 0, 8, 0x89 },
	{ 0xd043, 0, 2, 0x01 },
	{ 0xd045, 1, 1, 0x00 },
	{ 0x9bcf, 0, 1, 0x01 },
	{ 0xd045, 2, 1, 0x01 },
	{ 0xd04f, 0, 8, 0x9a },
	{ 0xd050, 0, 1, 0x01 },
	{ 0xd051, 0, 8, 0x5a },
	{ 0xd052, 0, 1, 0x01 },
	{ 0xd053, 0, 8, 0x50 },
	{ 0xd054, 0, 8, 0x46 },
	{ 0x9bd7, 0, 8, 0x0a },
	{ 0x9bd8, 0, 8, 0x14 },
	{ 0x9bd9, 0, 8, 0x08 },
	{ 0x9bd0, 0, 8, 0xcd },
	{ 0x9be4, 0, 8, 0xbb },
	{ 0x9bbd, 0, 8, 0x93 },
	{ 0x9be2, 0, 8, 0x80 },
	{ 0x9bee, 0, 1, 0x01 },
};

/* Freescale MC44S803 tuner init
   AF9013_TUNER_MC44S803   = 133 */
static struct regdesc tuner_init_mc44s803[] = {
	{ 0x9bd5, 0, 8, 0x01 },
	{ 0x9bd6, 0, 8, 0x06 },
	{ 0xd1a0, 1, 1, 0x01 },
	{ 0xd000, 0, 1, 0x01 },
	{ 0xd000, 1, 1, 0x00 },
	{ 0xd001, 1, 1, 0x01 },
	{ 0xd001, 0, 1, 0x00 },
	{ 0xd001, 5, 1, 0x00 },
	{ 0xd002, 0, 5, 0x19 },
	{ 0xd003, 0, 5, 0x1a },
	{ 0xd004, 0, 5, 0x19 },
	{ 0xd005, 0, 5, 0x1a },
	{ 0xd00e, 0, 5, 0x10 },
	{ 0xd00f, 0, 3, 0x04 },
	{ 0xd00f, 3, 3, 0x05 },
	{ 0xd010, 0, 3, 0x04 },
	{ 0xd010, 3, 3, 0x05 },
	{ 0xd016, 4, 4, 0x03 },
	{ 0xd01f, 0, 6, 0x0a },
	{ 0xd020, 0, 6, 0x0a },
	{ 0x9bda, 0, 8, 0x00 },
	{ 0x9be3, 0, 8, 0x00 },
	{ 0x9bf6, 0, 8, 0x01 },
	{ 0x9bf8, 0, 8, 0x02 },
	{ 0x9bf9, 0, 8, 0x02 },
	{ 0x9bfc, 0, 8, 0x1f },
	{ 0x9bbe, 0, 1, 0x01 },
	{ 0x9bcc, 0, 1, 0x01 },
	{ 0x9bb9, 0, 8, 0x00 },
	{ 0x9bcd, 0, 8, 0x24 },
	{ 0x9bff, 0, 8, 0x24 },
	{ 0xd015, 0, 8, 0x46 },
	{ 0xd016, 0, 1, 0x00 },
	{ 0xd044, 0, 8, 0x46 },
	{ 0xd045, 0, 1, 0x00 },
	{ 0xd008, 0, 8, 0x01 },
	{ 0xd009, 0, 2, 0x02 },
	{ 0xd006, 0, 8, 0x7b },
	{ 0xd007, 0, 2, 0x00 },
	{ 0xd00c, 0, 8, 0x7c },
	{ 0xd00d, 0, 2, 0x02 },
	{ 0xd00a, 0, 8, 0xfe },
	{ 0xd00b, 0, 2, 0x01 },
	{ 0x9bc7, 0, 8, 0x08 },
	{ 0x9bc8, 0, 8, 0x9a },
	{ 0x9bc3, 0, 8, 0x01 },
	{ 0x9bc4, 0, 8, 0x02 },
	{ 0x9bc5, 0, 8, 0x7c },
	{ 0x9bc6, 0, 8, 0x02 },
	{ 0x9bba, 0, 8, 0xfc },
	{ 0x9bc9, 0, 8, 0xaa },
	{ 0xd011, 0, 8, 0x6b },
	{ 0xd012, 0, 2, 0x00 },
	{ 0xd013, 0, 8, 0x88 },
	{ 0xd014, 0, 2, 0x02 },
	{ 0xd040, 0, 8, 0x6b },
	{ 0xd041, 0, 2, 0x00 },
	{ 0xd042, 0, 8, 0x7c },
	{ 0xd043, 0, 2, 0x02 },
	{ 0xd045, 1, 1, 0x00 },
	{ 0x9bcf, 0, 1, 0x01 },
	{ 0xd045, 2, 1, 0x01 },
	{ 0xd04f, 0, 8, 0x9a },
	{ 0xd050, 0, 1, 0x01 },
	{ 0xd051, 0, 8, 0x5a },
	{ 0xd052, 0, 1, 0x01 },
	{ 0xd053, 0, 8, 0x50 },
	{ 0xd054, 0, 8, 0x46 },
	{ 0x9bd7, 0, 8, 0x0a },
	{ 0x9bd8, 0, 8, 0x14 },
	{ 0x9bd9, 0, 8, 0x08 },
	{ 0x9bd0, 0, 8, 0x9e },
	{ 0x9be4, 0, 8, 0xff },
	{ 0x9bbd, 0, 8, 0x9e },
	{ 0x9be2, 0, 8, 0x25 },
	{ 0x9bee, 0, 1, 0x01 },
	{ 0xd73b, 3, 1, 0x00 },
};

/* unknown, probably for tin can tuner, tuner init
   AF9013_TUNER_UNKNOWN   = 140 */
static struct regdesc tuner_init_unknown[] = {
	{ 0x9bd5, 0, 8, 0x01 },
	{ 0x9bd6, 0, 8, 0x02 },
	{ 0xd1a0, 1, 1, 0x01 },
	{ 0xd000, 0, 1, 0x01 },
	{ 0xd000, 1, 1, 0x00 },
	{ 0xd001, 1, 1, 0x01 },
	{ 0xd001, 0, 1, 0x00 },
	{ 0xd001, 5, 1, 0x00 },
	{ 0xd002, 0, 5, 0x19 },
	{ 0xd003, 0, 5, 0x1a },
	{ 0xd004, 0, 5, 0x19 },
	{ 0xd005, 0, 5, 0x1a },
	{ 0xd00e, 0, 5, 0x10 },
	{ 0xd00f, 0, 3, 0x04 },
	{ 0xd00f, 3, 3, 0x05 },
	{ 0xd010, 0, 3, 0x04 },
	{ 0xd010, 3, 3, 0x05 },
	{ 0xd016, 4, 4, 0x03 },
	{ 0xd01f, 0, 6, 0x0a },
	{ 0xd020, 0, 6, 0x0a },
	{ 0x9bda, 0, 8, 0x01 },
	{ 0x9be3, 0, 8, 0x01 },
	{ 0xd1a0, 1, 1, 0x00 },
	{ 0x9bbe, 0, 1, 0x01 },
	{ 0x9bcc, 0, 1, 0x01 },
	{ 0x9bb9, 0, 8, 0x00 },
	{ 0x9bcd, 0, 8, 0x18 },
	{ 0x9bff, 0, 8, 0x2c },
	{ 0xd015, 0, 8, 0x46 },
	{ 0xd016, 0, 1, 0x00 },
	{ 0xd044, 0, 8, 0x46 },
	{ 0xd045, 0, 1, 0x00 },
	{ 0xd008, 0, 8, 0xdf },
	{ 0xd009, 0, 2, 0x02 },
	{ 0xd006, 0, 8, 0x44 },
	{ 0xd007, 0, 2, 0x01 },
	{ 0xd00c, 0, 8, 0x00 },
	{ 0xd00d, 0, 2, 0x02 },
	{ 0xd00a, 0, 8, 0xf6 },
	{ 0xd00b, 0, 2, 0x01 },
	{ 0x9bba, 0, 8, 0xf9 },
	{ 0x9bc8, 0, 8, 0xaa },
	{ 0x9bc3, 0, 8, 0xdf },
	{ 0x9bc4, 0, 8, 0x02 },
	{ 0x9bc5, 0, 8, 0x00 },
	{ 0x9bc6, 0, 8, 0x02 },
	{ 0x9bc9, 0, 8, 0xf0 },
	{ 0xd011, 0, 8, 0x3c },
	{ 0xd012, 0, 2, 0x01 },
	{ 0xd013, 0, 8, 0xf7 },
	{ 0xd014, 0, 2, 0x02 },
	{ 0xd040, 0, 8, 0x0b },
	{ 0xd041, 0, 2, 0x02 },
	{ 0xd042, 0, 8, 0x4d },
	{ 0xd043, 0, 2, 0x00 },
	{ 0xd045, 1, 1, 0x00 },
	{ 0x9bcf, 0, 1, 0x01 },
	{ 0xd045, 2, 1, 0x01 },
	{ 0xd04f, 0, 8, 0x9a },
	{ 0xd050, 0, 1, 0x01 },
	{ 0xd051, 0, 8, 0x5a },
	{ 0xd052, 0, 1, 0x01 },
	{ 0xd053, 0, 8, 0x50 },
	{ 0xd054, 0, 8, 0x46 },
	{ 0x9bd7, 0, 8, 0x0a },
	{ 0x9bd8, 0, 8, 0x14 },
	{ 0x9bd9, 0, 8, 0x08 },
};

/* NXP TDA18271 tuner init
   AF9013_TUNER_TDA18271   = 156 */
static struct regdesc tuner_init_tda18271[] = {
	{ 0x9bd5, 0, 8, 0x01 },
	{ 0x9bd6, 0, 8, 0x04 },
	{ 0xd1a0, 1, 1, 0x01 },
	{ 0xd000, 0, 1, 0x01 },
	{ 0xd000, 1, 1, 0x00 },
	{ 0xd001, 1, 1, 0x01 },
	{ 0xd001, 0, 1, 0x00 },
	{ 0xd001, 5, 1, 0x00 },
	{ 0xd002, 0, 5, 0x19 },
	{ 0xd003, 0, 5, 0x1a },
	{ 0xd004, 0, 5, 0x19 },
	{ 0xd005, 0, 5, 0x1a },
	{ 0xd00e, 0, 5, 0x10 },
	{ 0xd00f, 0, 3, 0x04 },
	{ 0xd00f, 3, 3, 0x05 },
	{ 0xd010, 0, 3, 0x04 },
	{ 0xd010, 3, 3, 0x05 },
	{ 0xd016, 4, 4, 0x03 },
	{ 0xd01f, 0, 6, 0x0a },
	{ 0xd020, 0, 6, 0x0a },
	{ 0x9bda, 0, 8, 0x01 },
	{ 0x9be3, 0, 8, 0x01 },
	{ 0xd1a0, 1, 1, 0x00 },
	{ 0x9bbe, 0, 1, 0x01 },
	{ 0x9bcc, 0, 1, 0x01 },
	{ 0x9bb9, 0, 8, 0x00 },
	{ 0x9bcd, 0, 8, 0x18 },
	{ 0x9bff, 0, 8, 0x2c },
	{ 0xd015, 0, 8, 0x46 },
	{ 0xd016, 0, 1, 0x00 },
	{ 0xd044, 0, 8, 0x46 },
	{ 0xd045, 0, 1, 0x00 },
	{ 0xd008, 0, 8, 0xdf },
	{ 0xd009, 0, 2, 0x02 },
	{ 0xd006, 0, 8, 0x44 },
	{ 0xd007, 0, 2, 0x01 },
	{ 0xd00c, 0, 8, 0x00 },
	{ 0xd00d, 0, 2, 0x02 },
	{ 0xd00a, 0, 8, 0xf6 },
	{ 0xd00b, 0, 2, 0x01 },
	{ 0x9bba, 0, 8, 0xf9 },
	{ 0x9bc8, 0, 8, 0xaa },
	{ 0x9bc3, 0, 8, 0xdf },
	{ 0x9bc4, 0, 8, 0x02 },
	{ 0x9bc5, 0, 8, 0x00 },
	{ 0x9bc6, 0, 8, 0x02 },
	{ 0x9bc9, 0, 8, 0xf0 },
	{ 0xd011, 0, 8, 0x3c },
	{ 0xd012, 0, 2, 0x01 },
	{ 0xd013, 0, 8, 0xf7 },
	{ 0xd014, 0, 2, 0x02 },
	{ 0xd040, 0, 8, 0x0b },
	{ 0xd041, 0, 2, 0x02 },
	{ 0xd042, 0, 8, 0x4d },
	{ 0xd043, 0, 2, 0x00 },
	{ 0xd045, 1, 1, 0x00 },
	{ 0x9bcf, 0, 1, 0x01 },
	{ 0xd045, 2, 1, 0x01 },
	{ 0xd04f, 0, 8, 0x9a },
	{ 0xd050, 0, 1, 0x01 },
	{ 0xd051, 0, 8, 0x5a },
	{ 0xd052, 0, 1, 0x01 },
	{ 0xd053, 0, 8, 0x50 },
	{ 0xd054, 0, 8, 0x46 },
	{ 0x9bd7, 0, 8, 0x0a },
	{ 0x9bd8, 0, 8, 0x14 },
	{ 0x9bd9, 0, 8, 0x08 },
	{ 0x9bd0, 0, 8, 0xa8 },
	{ 0x9be4, 0, 8, 0x7f },
	{ 0x9bbd, 0, 8, 0xa8 },
	{ 0x9be2, 0, 8, 0x20 },
	{ 0x9bee, 0, 1, 0x01 },
};

#endif /* _AF9013_PRIV_ */
