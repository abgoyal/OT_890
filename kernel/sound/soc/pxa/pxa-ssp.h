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
 * ASoC PXA SSP port support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _PXA_SSP_H
#define _PXA_SSP_H

/* pxa DAI SSP IDs */
#define PXA_DAI_SSP1			0
#define PXA_DAI_SSP2			1
#define PXA_DAI_SSP3			2
#define PXA_DAI_SSP4			3

/* SSP clock sources */
#define PXA_SSP_CLK_PLL	0
#define PXA_SSP_CLK_EXT	1
#define PXA_SSP_CLK_NET	2
#define PXA_SSP_CLK_AUDIO	3
#define PXA_SSP_CLK_NET_PLL	4

/* SSP audio dividers */
#define PXA_SSP_AUDIO_DIV_ACDS		0
#define PXA_SSP_AUDIO_DIV_SCDB		1
#define PXA_SSP_DIV_SCR				2

/* SSP ACDS audio dividers values */
#define PXA_SSP_CLK_AUDIO_DIV_1		0
#define PXA_SSP_CLK_AUDIO_DIV_2		1
#define PXA_SSP_CLK_AUDIO_DIV_4		2
#define PXA_SSP_CLK_AUDIO_DIV_8		3
#define PXA_SSP_CLK_AUDIO_DIV_16	4
#define PXA_SSP_CLK_AUDIO_DIV_32	5

/* SSP divider bypass */
#define PXA_SSP_CLK_SCDB_4		0
#define PXA_SSP_CLK_SCDB_1		1
#define PXA_SSP_CLK_SCDB_8		2

#define PXA_SSP_PLL_OUT  0

extern struct snd_soc_dai pxa_ssp_dai[4];

#endif
