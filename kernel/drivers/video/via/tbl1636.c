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
 * Copyright 1998-2008 VIA Technologies, Inc. All Rights Reserved.
 * Copyright 2001-2008 S3 Graphics, Inc. All Rights Reserved.

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTIES OR REPRESENTATIONS; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.See the GNU General Public License
 * for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "global.h"
struct IODATA COMMON_INIT_TBL_VT1636[] = {
/*  Index, Mask, Value */
	/* Set panel power sequence timing */
	{0x10, 0xC0, 0x00},
	/* T1: VDD on - Data on. Each increment is 1 ms. (50ms = 031h) */
	{0x0B, 0xFF, 0x40},
	/* T2: Data on - Backlight on. Each increment is 2 ms. (210ms = 068h) */
	{0x0C, 0xFF, 0x31},
	/* T3: Backlight off -Data off. Each increment is 2 ms. (210ms = 068h)*/
	{0x0D, 0xFF, 0x31},
	/* T4: Data off - VDD off. Each increment is 1 ms. (50ms = 031h) */
	{0x0E, 0xFF, 0x68},
	/* T5: VDD off - VDD on. Each increment is 100 ms. (500ms = 04h) */
	{0x0F, 0xFF, 0x68},
	/* LVDS output power up */
	{0x09, 0xA0, 0xA0},
	/* turn on back light */
	{0x10, 0x33, 0x13}
};

struct IODATA DUAL_CHANNEL_ENABLE_TBL_VT1636[] = {
/*  Index, Mask, Value */
	{0x08, 0xF0, 0xE0}	/* Input Data Mode Select */
};

struct IODATA SINGLE_CHANNEL_ENABLE_TBL_VT1636[] = {
/*  Index, Mask, Value */
	{0x08, 0xF0, 0x00}	/* Input Data Mode Select */
};

struct IODATA DITHERING_ENABLE_TBL_VT1636[] = {
/*  Index, Mask, Value */
	{0x0A, 0x70, 0x50}
};

struct IODATA DITHERING_DISABLE_TBL_VT1636[] = {
/*  Index, Mask, Value */
	{0x0A, 0x70, 0x00}
};

struct IODATA VDD_ON_TBL_VT1636[] = {
/*  Index, Mask, Value */
	{0x10, 0x20, 0x20}
};

struct IODATA VDD_OFF_TBL_VT1636[] = {
/*  Index, Mask, Value */
	{0x10, 0x20, 0x00}
};
