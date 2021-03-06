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
 * arch/arm/plat-omap/include/mach/omap34xx.h
 *
 * This file contains the processor specific definitions of the TI OMAP34XX.
 *
 * Copyright (C) 2007 Texas Instruments.
 * Copyright (C) 2007 Nokia Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __ASM_ARCH_OMAP34XX_H
#define __ASM_ARCH_OMAP34XX_H

/*
 * Please place only base defines here and put the rest in device
 * specific headers.
 */

#define L4_34XX_BASE		0x48000000
#define L4_WK_34XX_BASE		0x48300000
#define L4_WK_OMAP_BASE		L4_WK_34XX_BASE
#define L4_PER_34XX_BASE	0x49000000
#define L4_PER_OMAP_BASE	L4_PER_34XX_BASE
#define L4_EMU_34XX_BASE	0x54000000
#define L4_EMU_BASE		L4_EMU_34XX_BASE
#define L3_34XX_BASE		0x68000000
#define L3_OMAP_BASE		L3_34XX_BASE

#define OMAP3430_32KSYNCT_BASE	0x48320000
#define OMAP3430_CM_BASE	0x48004800
#define OMAP3430_PRM_BASE	0x48306800
#define OMAP343X_SMS_BASE	0x6C000000
#define OMAP343X_SDRC_BASE	0x6D000000
#define OMAP34XX_GPMC_BASE	0x6E000000
#define OMAP343X_SCM_BASE	0x48002000
#define OMAP343X_CTRL_BASE	OMAP343X_SCM_BASE

#define OMAP34XX_IC_BASE	0x48200000
#define OMAP34XX_IVA_INTC_BASE	0x40000000
#define OMAP34XX_HSUSB_OTG_BASE	(L4_34XX_BASE + 0xAB000)
#define OMAP34XX_HSUSB_HOST_BASE	(L4_34XX_BASE + 0x64000)
#define OMAP34XX_USBTLL_BASE	(L4_34XX_BASE + 0x62000)


#if defined(CONFIG_ARCH_OMAP3430)

#define OMAP2_32KSYNCT_BASE		OMAP3430_32KSYNCT_BASE
#define OMAP2_CM_BASE			OMAP3430_CM_BASE
#define OMAP2_PRM_BASE			OMAP3430_PRM_BASE
#define OMAP2_VA_IC_BASE		IO_ADDRESS(OMAP34XX_IC_BASE)

#endif

#define OMAP34XX_DSP_BASE	0x58000000
#define OMAP34XX_DSP_MEM_BASE	(OMAP34XX_DSP_BASE + 0x0)
#define OMAP34XX_DSP_IPI_BASE	(OMAP34XX_DSP_BASE + 0x1000000)
#define OMAP34XX_DSP_MMU_BASE	(OMAP34XX_DSP_BASE + 0x2000000)
#endif /* __ASM_ARCH_OMAP34XX_H */

