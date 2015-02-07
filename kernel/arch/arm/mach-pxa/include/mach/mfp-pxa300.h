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
 * arch/arm/mach-pxa/include/mach/mfp-pxa300.h
 *
 * PXA300/PXA310 specific MFP configuration definitions
 *
 * Copyright (C) 2007 Marvell International Ltd.
 * 2007-08-21: eric miao <eric.miao@marvell.com>
 *             initial version
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef __ASM_ARCH_MFP_PXA300_H
#define __ASM_ARCH_MFP_PXA300_H

#include <mach/mfp.h>
#include <mach/mfp-pxa3xx.h>

/* GPIO */
#define GPIO46_GPIO		MFP_CFG(GPIO46, AF1)
#define GPIO49_GPIO		MFP_CFG(GPIO49, AF3)
#define GPIO50_GPIO		MFP_CFG(GPIO50, AF2)
#define GPIO51_GPIO		MFP_CFG(GPIO51, AF3)
#define GPIO52_GPIO		MFP_CFG(GPIO52, AF3)
#define GPIO56_GPIO		MFP_CFG(GPIO56, AF0)
#define GPIO58_GPIO		MFP_CFG(GPIO58, AF0)
#define GPIO59_GPIO		MFP_CFG(GPIO59, AF0)
#define GPIO60_GPIO		MFP_CFG(GPIO60, AF0)
#define GPIO61_GPIO		MFP_CFG(GPIO61, AF0)
#define GPIO62_GPIO		MFP_CFG(GPIO62, AF0)

#ifdef CONFIG_CPU_PXA310
#define GPIO7_2_GPIO		MFP_CFG(GPIO7_2, AF0)
#define GPIO8_2_GPIO		MFP_CFG(GPIO8_2, AF0)
#define GPIO9_2_GPIO		MFP_CFG(GPIO9_2, AF0)
#define GPIO10_2_GPIO		MFP_CFG(GPIO10_2, AF0)
#define GPIO11_2_GPIO		MFP_CFG(GPIO11_2, AF0)
#define GPIO12_2_GPIO		MFP_CFG(GPIO12_2, AF0)
#endif

/* Chip Select */
#define GPIO2_nCS3		MFP_CFG(GPIO2,  AF1)

/* AC97 */
#define GPIO23_AC97_nACRESET	MFP_CFG(GPIO23, AF1)
#define GPIO24_AC97_SYSCLK	MFP_CFG(GPIO24, AF1)
#define GPIO29_AC97_BITCLK	MFP_CFG(GPIO29, AF1)
#define GPIO25_AC97_SDATA_IN_0	MFP_CFG(GPIO25, AF1)
#define GPIO26_AC97_SDATA_IN_1	MFP_CFG(GPIO26, AF1)
#define GPIO17_AC97_SDATA_IN_2	MFP_CFG(GPIO17, AF3)
#define GPIO21_AC97_SDATA_IN_2	MFP_CFG(GPIO21, AF2)
#define GPIO18_AC97_SDATA_IN_3	MFP_CFG(GPIO18, AF3)
#define GPIO22_AC97_SDATA_IN_3	MFP_CFG(GPIO22, AF2)
#define GPIO27_AC97_SDATA_OUT	MFP_CFG(GPIO27, AF1)
#define GPIO28_AC97_SYNC	MFP_CFG(GPIO28, AF1)

/* I2C */
#define GPIO21_I2C_SCL		MFP_CFG_LPM(GPIO21, AF1, PULL_HIGH)
#define GPIO22_I2C_SDA		MFP_CFG_LPM(GPIO22, AF1, PULL_HIGH)

/* QCI */
#define GPIO39_CI_DD_0		MFP_CFG_DRV(GPIO39, AF1, DS04X)
#define GPIO40_CI_DD_1		MFP_CFG_DRV(GPIO40, AF1, DS04X)
#define GPIO41_CI_DD_2		MFP_CFG_DRV(GPIO41, AF1, DS04X)
#define GPIO42_CI_DD_3		MFP_CFG_DRV(GPIO42, AF1, DS04X)
#define GPIO43_CI_DD_4		MFP_CFG_DRV(GPIO43, AF1, DS04X)
#define GPIO44_CI_DD_5		MFP_CFG_DRV(GPIO44, AF1, DS04X)
#define GPIO45_CI_DD_6		MFP_CFG_DRV(GPIO45, AF1, DS04X)
#define GPIO46_CI_DD_7		MFP_CFG_DRV(GPIO46, AF0, DS04X)
#define GPIO47_CI_DD_8		MFP_CFG_DRV(GPIO47, AF1, DS04X)
#define GPIO48_CI_DD_9		MFP_CFG_DRV(GPIO48, AF1, DS04X)
#define GPIO52_CI_HSYNC		MFP_CFG_DRV(GPIO52, AF0, DS04X)
#define GPIO51_CI_VSYNC		MFP_CFG_DRV(GPIO51, AF0, DS04X)
#define GPIO49_CI_MCLK		MFP_CFG_DRV(GPIO49, AF0, DS04X)
#define GPIO50_CI_PCLK		MFP_CFG_DRV(GPIO50, AF0, DS04X)

/* KEYPAD */
#define GPIO3_KP_DKIN_6		MFP_CFG_LPM(GPIO3,   AF2, FLOAT)
#define GPIO4_KP_DKIN_7		MFP_CFG_LPM(GPIO4,   AF2, FLOAT)
#define GPIO16_KP_DKIN_6	MFP_CFG_LPM(GPIO16,  AF6, FLOAT)
#define GPIO83_KP_DKIN_2	MFP_CFG_LPM(GPIO83,  AF5, FLOAT)
#define GPIO84_KP_DKIN_1	MFP_CFG_LPM(GPIO84,  AF5, FLOAT)
#define GPIO85_KP_DKIN_0	MFP_CFG_LPM(GPIO85,  AF3, FLOAT)
#define GPIO86_KP_DKIN_1	MFP_CFG_LPM(GPIO86,  AF3, FLOAT)
#define GPIO87_KP_DKIN_2	MFP_CFG_LPM(GPIO87,  AF3, FLOAT)
#define GPIO88_KP_DKIN_3	MFP_CFG_LPM(GPIO88,  AF3, FLOAT)
#define GPIO89_KP_DKIN_3	MFP_CFG_LPM(GPIO89,  AF3, FLOAT)
#define GPIO107_KP_DKIN_0	MFP_CFG_LPM(GPIO107, AF2, FLOAT)
#define GPIO108_KP_DKIN_1	MFP_CFG_LPM(GPIO108, AF2, FLOAT)
#define GPIO109_KP_DKIN_2	MFP_CFG_LPM(GPIO109, AF2, FLOAT)
#define GPIO110_KP_DKIN_3	MFP_CFG_LPM(GPIO110, AF2, FLOAT)
#define GPIO111_KP_DKIN_4	MFP_CFG_LPM(GPIO111, AF2, FLOAT)
#define GPIO112_KP_DKIN_5	MFP_CFG_LPM(GPIO112, AF2, FLOAT)
#define GPIO113_KP_DKIN_6	MFP_CFG_LPM(GPIO113, AF2, FLOAT)
#define GPIO114_KP_DKIN_7	MFP_CFG_LPM(GPIO114, AF2, FLOAT)
#define GPIO115_KP_DKIN_0	MFP_CFG_LPM(GPIO115, AF2, FLOAT)
#define GPIO116_KP_DKIN_1	MFP_CFG_LPM(GPIO116, AF2, FLOAT)
#define GPIO117_KP_DKIN_2	MFP_CFG_LPM(GPIO117, AF2, FLOAT)
#define GPIO118_KP_DKIN_3	MFP_CFG_LPM(GPIO118, AF2, FLOAT)
#define GPIO119_KP_DKIN_4	MFP_CFG_LPM(GPIO119, AF2, FLOAT)
#define GPIO120_KP_DKIN_5	MFP_CFG_LPM(GPIO120, AF2, FLOAT)
#define GPIO121_KP_DKIN_6	MFP_CFG_LPM(GPIO121, AF2, FLOAT)
#define GPIO122_KP_DKIN_5	MFP_CFG_LPM(GPIO122, AF2, FLOAT)
#define GPIO123_KP_DKIN_4	MFP_CFG_LPM(GPIO123, AF2, FLOAT)
#define GPIO124_KP_DKIN_3	MFP_CFG_LPM(GPIO124, AF2, FLOAT)
#define GPIO127_KP_DKIN_0	MFP_CFG_LPM(GPIO127, AF5, FLOAT)
#define GPIO0_2_KP_DKIN_0	MFP_CFG_LPM(GPIO0_2, AF2, FLOAT)
#define GPIO1_2_KP_DKIN_1	MFP_CFG_LPM(GPIO1_2, AF2, FLOAT)
#define GPIO2_2_KP_DKIN_6	MFP_CFG_LPM(GPIO2_2, AF2, FLOAT)
#define GPIO3_2_KP_DKIN_7	MFP_CFG_LPM(GPIO3_2, AF2, FLOAT)
#define GPIO4_2_KP_DKIN_1	MFP_CFG_LPM(GPIO4_2, AF2, FLOAT)
#define GPIO5_2_KP_DKIN_0	MFP_CFG_LPM(GPIO5_2, AF2, FLOAT)

#define GPIO5_KP_MKIN_0		MFP_CFG_LPM(GPIO5,   AF2, FLOAT)
#define GPIO6_KP_MKIN_1		MFP_CFG_LPM(GPIO6,   AF2, FLOAT)
#define GPIO9_KP_MKIN_6		MFP_CFG_LPM(GPIO9,   AF3, FLOAT)
#define GPIO10_KP_MKIN_7	MFP_CFG_LPM(GPIO10,  AF3, FLOAT)
#define GPIO70_KP_MKIN_6	MFP_CFG_LPM(GPIO70,  AF3, FLOAT)
#define GPIO71_KP_MKIN_7	MFP_CFG_LPM(GPIO71,  AF3, FLOAT)
#define GPIO100_KP_MKIN_6	MFP_CFG_LPM(GPIO100, AF7, FLOAT)
#define GPIO101_KP_MKIN_7	MFP_CFG_LPM(GPIO101, AF7, FLOAT)
#define GPIO112_KP_MKIN_6	MFP_CFG_LPM(GPIO112, AF4, FLOAT)
#define GPIO113_KP_MKIN_7	MFP_CFG_LPM(GPIO113, AF4, FLOAT)
#define GPIO115_KP_MKIN_0	MFP_CFG_LPM(GPIO115, AF1, FLOAT)
#define GPIO116_KP_MKIN_1	MFP_CFG_LPM(GPIO116, AF1, FLOAT)
#define GPIO117_KP_MKIN_2	MFP_CFG_LPM(GPIO117, AF1, FLOAT)
#define GPIO118_KP_MKIN_3	MFP_CFG_LPM(GPIO118, AF1, FLOAT)
#define GPIO119_KP_MKIN_4	MFP_CFG_LPM(GPIO119, AF1, FLOAT)
#define GPIO120_KP_MKIN_5	MFP_CFG_LPM(GPIO120, AF1, FLOAT)
#define GPIO125_KP_MKIN_2	MFP_CFG_LPM(GPIO125, AF2, FLOAT)
#define GPIO2_2_KP_MKIN_6	MFP_CFG_LPM(GPIO2_2, AF1, FLOAT)
#define GPIO3_2_KP_MKIN_7	MFP_CFG_LPM(GPIO3_2, AF1, FLOAT)

#define GPIO7_KP_MKOUT_5	MFP_CFG_LPM(GPIO7,   AF1, DRIVE_HIGH)
#define GPIO11_KP_MKOUT_5	MFP_CFG_LPM(GPIO11,  AF3, DRIVE_HIGH)
#define GPIO12_KP_MKOUT_6	MFP_CFG_LPM(GPIO12,  AF3, DRIVE_HIGH)
#define GPIO13_KP_MKOUT_7	MFP_CFG_LPM(GPIO13,  AF3, DRIVE_HIGH)
#define GPIO19_KP_MKOUT_4	MFP_CFG_LPM(GPIO19,  AF3, DRIVE_HIGH)
#define GPIO20_KP_MKOUT_5	MFP_CFG_LPM(GPIO20,  AF3, DRIVE_HIGH)
#define GPIO38_KP_MKOUT_5	MFP_CFG_LPM(GPIO38,  AF5, DRIVE_HIGH)
#define GPIO53_KP_MKOUT_6	MFP_CFG_LPM(GPIO53,  AF5, DRIVE_HIGH)
#define GPIO78_KP_MKOUT_7	MFP_CFG_LPM(GPIO78,  AF5, DRIVE_HIGH)
#define GPIO85_KP_MKOUT_0	MFP_CFG_LPM(GPIO85,  AF2, DRIVE_HIGH)
#define GPIO86_KP_MKOUT_1	MFP_CFG_LPM(GPIO86,  AF2, DRIVE_HIGH)
#define GPIO87_KP_MKOUT_2	MFP_CFG_LPM(GPIO87,  AF2, DRIVE_HIGH)
#define GPIO88_KP_MKOUT_3	MFP_CFG_LPM(GPIO88,  AF2, DRIVE_HIGH)
#define GPIO104_KP_MKOUT_6	MFP_CFG_LPM(GPIO104, AF5, DRIVE_HIGH)
#define GPIO105_KP_MKOUT_7	MFP_CFG_LPM(GPIO105, AF5, DRIVE_HIGH)
#define GPIO121_KP_MKOUT_0	MFP_CFG_LPM(GPIO121, AF1, DRIVE_HIGH)
#define GPIO122_KP_MKOUT_1	MFP_CFG_LPM(GPIO122, AF1, DRIVE_HIGH)
#define GPIO123_KP_MKOUT_2	MFP_CFG_LPM(GPIO123, AF1, DRIVE_HIGH)
#define GPIO124_KP_MKOUT_3	MFP_CFG_LPM(GPIO124, AF1, DRIVE_HIGH)
#define GPIO125_KP_MKOUT_4	MFP_CFG_LPM(GPIO125, AF1, DRIVE_HIGH)
#define GPIO126_KP_MKOUT_7	MFP_CFG_LPM(GPIO126, AF4, DRIVE_HIGH)
#define GPIO5_2_KP_MKOUT_6	MFP_CFG_LPM(GPIO5_2, AF1, DRIVE_HIGH)
#define GPIO4_2_KP_MKOUT_5	MFP_CFG_LPM(GPIO4_2, AF1, DRIVE_HIGH)
#define GPIO6_2_KP_MKOUT_7	MFP_CFG_LPM(GPIO6_2, AF1, DRIVE_HIGH)

/* LCD */
#define GPIO54_LCD_LDD_0	MFP_CFG_DRV(GPIO54, AF1, DS01X)
#define GPIO55_LCD_LDD_1	MFP_CFG_DRV(GPIO55, AF1, DS01X)
#define GPIO56_LCD_LDD_2	MFP_CFG_DRV(GPIO56, AF1, DS01X)
#define GPIO57_LCD_LDD_3	MFP_CFG_DRV(GPIO57, AF1, DS01X)
#define GPIO58_LCD_LDD_4	MFP_CFG_DRV(GPIO58, AF1, DS01X)
#define GPIO59_LCD_LDD_5	MFP_CFG_DRV(GPIO59, AF1, DS01X)
#define GPIO60_LCD_LDD_6	MFP_CFG_DRV(GPIO60, AF1, DS01X)
#define GPIO61_LCD_LDD_7	MFP_CFG_DRV(GPIO61, AF1, DS01X)
#define GPIO62_LCD_LDD_8	MFP_CFG_DRV(GPIO62, AF1, DS01X)
#define GPIO63_LCD_LDD_9	MFP_CFG_DRV(GPIO63, AF1, DS01X)
#define GPIO64_LCD_LDD_10	MFP_CFG_DRV(GPIO64, AF1, DS01X)
#define GPIO65_LCD_LDD_11	MFP_CFG_DRV(GPIO65, AF1, DS01X)
#define GPIO66_LCD_LDD_12	MFP_CFG_DRV(GPIO66, AF1, DS01X)
#define GPIO67_LCD_LDD_13	MFP_CFG_DRV(GPIO67, AF1, DS01X)
#define GPIO68_LCD_LDD_14	MFP_CFG_DRV(GPIO68, AF1, DS01X)
#define GPIO69_LCD_LDD_15	MFP_CFG_DRV(GPIO69, AF1, DS01X)
#define GPIO70_LCD_LDD_16	MFP_CFG_DRV(GPIO70, AF1, DS01X)
#define GPIO71_LCD_LDD_17	MFP_CFG_DRV(GPIO71, AF1, DS01X)
#define GPIO62_LCD_CS_N		MFP_CFG_DRV(GPIO62, AF2, DS01X)
#define GPIO72_LCD_FCLK		MFP_CFG_DRV(GPIO72, AF1, DS01X)
#define GPIO73_LCD_LCLK		MFP_CFG_DRV(GPIO73, AF1, DS01X)
#define GPIO74_LCD_PCLK		MFP_CFG_DRV(GPIO74, AF1, DS02X)
#define GPIO75_LCD_BIAS		MFP_CFG_DRV(GPIO75, AF1, DS01X)
#define GPIO76_LCD_VSYNC	MFP_CFG_DRV(GPIO76, AF2, DS01X)

#define GPIO15_LCD_CS_N		MFP_CFG_DRV(GPIO15,  AF2, DS01X)
#define GPIO127_LCD_CS_N	MFP_CFG_DRV(GPIO127, AF1, DS01X)
#define GPIO63_LCD_VSYNC	MFP_CFG_DRV(GPIO63,  AF2, DS01X)

/* Mini-LCD */
#define GPIO72_MLCD_FCLK	MFP_CFG_DRV(GPIO72, AF7, DS08X)
#define GPIO73_MLCD_LCLK	MFP_CFG_DRV(GPIO73, AF7, DS08X)
#define GPIO54_MLCD_LDD_0	MFP_CFG_DRV(GPIO54, AF7, DS08X)
#define GPIO55_MLCD_LDD_1	MFP_CFG_DRV(GPIO55, AF7, DS08X)
#define GPIO56_MLCD_LDD_2	MFP_CFG_DRV(GPIO56, AF7, DS08X)
#define GPIO57_MLCD_LDD_3	MFP_CFG_DRV(GPIO57, AF7, DS08X)
#define GPIO58_MLCD_LDD_4	MFP_CFG_DRV(GPIO58, AF7, DS08X)
#define GPIO59_MLCD_LDD_5	MFP_CFG_DRV(GPIO59, AF7, DS08X)
#define GPIO60_MLCD_LDD_6	MFP_CFG_DRV(GPIO60, AF7, DS08X)
#define GPIO61_MLCD_LDD_7	MFP_CFG_DRV(GPIO61, AF7, DS08X)
#define GPIO62_MLCD_LDD_8	MFP_CFG_DRV(GPIO62, AF7, DS08X)
#define GPIO63_MLCD_LDD_9	MFP_CFG_DRV(GPIO63, AF7, DS08X)
#define GPIO64_MLCD_LDD_10	MFP_CFG_DRV(GPIO64, AF7, DS08X)
#define GPIO65_MLCD_LDD_11	MFP_CFG_DRV(GPIO65, AF7, DS08X)
#define GPIO66_MLCD_LDD_12	MFP_CFG_DRV(GPIO66, AF7, DS08X)
#define GPIO67_MLCD_LDD_13	MFP_CFG_DRV(GPIO67, AF7, DS08X)
#define GPIO68_MLCD_LDD_14	MFP_CFG_DRV(GPIO68, AF7, DS08X)
#define GPIO69_MLCD_LDD_15	MFP_CFG_DRV(GPIO69, AF7, DS08X)
#define GPIO74_MLCD_PCLK	MFP_CFG_DRV(GPIO74, AF7, DS08X)
#define GPIO75_MLCD_BIAS	MFP_CFG_DRV(GPIO75, AF2, DS08X)

/* MMC1 */
#define GPIO7_MMC1_CLK		MFP_CFG_LPM(GPIO7,  AF4, DRIVE_HIGH)
#define GPIO8_MMC1_CMD		MFP_CFG_LPM(GPIO8,  AF4, DRIVE_HIGH)
#define GPIO14_MMC1_CMD		MFP_CFG_LPM(GPIO14, AF5, DRIVE_HIGH)
#define GPIO15_MMC1_CMD		MFP_CFG_LPM(GPIO15, AF5, DRIVE_HIGH)
#define GPIO3_MMC1_DAT0		MFP_CFG_LPM(GPIO3,  AF4, DRIVE_HIGH)
#define GPIO4_MMC1_DAT1		MFP_CFG_LPM(GPIO4,  AF4, DRIVE_HIGH)
#define GPIO5_MMC1_DAT2		MFP_CFG_LPM(GPIO5,  AF4, DRIVE_HIGH)
#define GPIO6_MMC1_DAT3		MFP_CFG_LPM(GPIO6,  AF4, DRIVE_HIGH)

/* MMC2 */
#define GPIO9_MMC2_DAT0		MFP_CFG_LPM(GPIO9,  AF4, PULL_HIGH)
#define GPIO10_MMC2_DAT1	MFP_CFG_LPM(GPIO10, AF4, PULL_HIGH)
#define GPIO11_MMC2_DAT2	MFP_CFG_LPM(GPIO11, AF4, PULL_HIGH)
#define GPIO12_MMC2_DAT3	MFP_CFG_LPM(GPIO12, AF4, PULL_HIGH)
#define GPIO13_MMC2_CLK		MFP_CFG_LPM(GPIO13, AF4, PULL_HIGH)
#define GPIO14_MMC2_CMD		MFP_CFG_LPM(GPIO14, AF4, PULL_HIGH)
#define GPIO77_MMC2_DAT0	MFP_CFG_LPM(GPIO77, AF4, PULL_HIGH)
#define GPIO78_MMC2_DAT1	MFP_CFG_LPM(GPIO78, AF4, PULL_HIGH)
#define GPIO79_MMC2_DAT2	MFP_CFG_LPM(GPIO79, AF4, PULL_HIGH)
#define GPIO80_MMC2_DAT3	MFP_CFG_LPM(GPIO80, AF4, PULL_HIGH)
#define GPIO81_MMC2_CLK		MFP_CFG_LPM(GPIO81, AF4, PULL_HIGH)
#define GPIO82_MMC2_CMD		MFP_CFG_LPM(GPIO82, AF4, PULL_HIGH)

/* SSP1 */
#define GPIO89_SSP1_EXTCLK	MFP_CFG(GPIO89, AF1)
#define GPIO90_SSP1_SYSCLK	MFP_CFG(GPIO90, AF1)
#define GPIO15_SSP1_SCLK	MFP_CFG(GPIO15, AF6)
#define GPIO16_SSP1_FRM		MFP_CFG(GPIO16, AF2)
#define GPIO33_SSP1_SCLK	MFP_CFG(GPIO33, AF5)
#define GPIO34_SSP1_FRM		MFP_CFG(GPIO34, AF5)
#define GPIO85_SSP1_SCLK	MFP_CFG(GPIO85, AF1)
#define GPIO86_SSP1_FRM		MFP_CFG(GPIO86, AF1)
#define GPIO18_SSP1_TXD		MFP_CFG(GPIO18, AF7)
#define GPIO18_SSP1_RXD		MFP_CFG(GPIO18, AF2)
#define GPIO20_SSP1_TXD		MFP_CFG(GPIO20, AF2)
#define GPIO20_SSP1_RXD		MFP_CFG(GPIO20, AF7)
#define GPIO35_SSP1_TXD		MFP_CFG(GPIO35, AF5)
#define GPIO35_SSP1_RXD		MFP_CFG(GPIO35, AF4)
#define GPIO36_SSP1_TXD		MFP_CFG(GPIO36, AF5)
#define GPIO36_SSP1_RXD		MFP_CFG(GPIO36, AF6)
#define GPIO87_SSP1_TXD		MFP_CFG(GPIO87, AF1)
#define GPIO87_SSP1_RXD		MFP_CFG(GPIO87, AF6)
#define GPIO88_SSP1_TXD		MFP_CFG(GPIO88, AF6)
#define GPIO88_SSP1_RXD		MFP_CFG(GPIO88, AF1)

/* SSP2 */
#define GPIO29_SSP2_EXTCLK	MFP_CFG(GPIO29, AF2)
#define GPIO23_SSP2_SCLK	MFP_CFG(GPIO23, AF2)
#define GPIO17_SSP2_FRM		MFP_CFG(GPIO17, AF2)
#define GPIO25_SSP2_SCLK	MFP_CFG(GPIO25, AF2)
#define GPIO26_SSP2_FRM		MFP_CFG(GPIO26, AF2)
#define GPIO33_SSP2_SCLK	MFP_CFG(GPIO33, AF6)
#define GPIO34_SSP2_FRM		MFP_CFG(GPIO34, AF6)
#define GPIO64_SSP2_SCLK	MFP_CFG(GPIO64, AF2)
#define GPIO65_SSP2_FRM		MFP_CFG(GPIO65, AF2)
#define GPIO19_SSP2_TXD		MFP_CFG(GPIO19, AF2)
#define GPIO19_SSP2_RXD		MFP_CFG(GPIO19, AF7)
#define GPIO24_SSP2_TXD		MFP_CFG(GPIO24, AF5)
#define GPIO24_SSP2_RXD		MFP_CFG(GPIO24, AF4)
#define GPIO27_SSP2_TXD		MFP_CFG(GPIO27, AF2)
#define GPIO27_SSP2_RXD		MFP_CFG(GPIO27, AF5)
#define GPIO28_SSP2_TXD		MFP_CFG(GPIO28, AF5)
#define GPIO28_SSP2_RXD		MFP_CFG(GPIO28, AF2)
#define GPIO35_SSP2_TXD		MFP_CFG(GPIO35, AF7)
#define GPIO35_SSP2_RXD		MFP_CFG(GPIO35, AF6)
#define GPIO66_SSP2_TXD		MFP_CFG(GPIO66, AF4)
#define GPIO66_SSP2_RXD		MFP_CFG(GPIO66, AF2)
#define GPIO67_SSP2_TXD		MFP_CFG(GPIO67, AF2)
#define GPIO67_SSP2_RXD		MFP_CFG(GPIO67, AF4)
#define GPIO36_SSP2_TXD		MFP_CFG(GPIO36, AF7)

/* SSP3 */
#define GPIO69_SSP3_FRM		MFP_CFG_X(GPIO69, AF2, DS08X, DRIVE_LOW)
#define GPIO68_SSP3_SCLK	MFP_CFG_X(GPIO68, AF2, DS08X, FLOAT)
#define GPIO92_SSP3_FRM		MFP_CFG_X(GPIO92, AF1, DS08X, DRIVE_LOW)
#define GPIO91_SSP3_SCLK	MFP_CFG_X(GPIO91, AF1, DS08X, FLOAT)
#define GPIO70_SSP3_TXD		MFP_CFG_X(GPIO70, AF2, DS08X, DRIVE_LOW)
#define GPIO70_SSP3_RXD		MFP_CFG_X(GPIO70, AF5, DS08X, FLOAT)
#define GPIO71_SSP3_TXD		MFP_CFG_X(GPIO71, AF5, DS08X, DRIVE_LOW)
#define GPIO71_SSP3_RXD		MFP_CFG_X(GPIO71, AF2, DS08X, FLOAT)
#define GPIO93_SSP3_TXD		MFP_CFG_X(GPIO93, AF1, DS08X, DRIVE_LOW)
#define GPIO93_SSP3_RXD		MFP_CFG_X(GPIO93, AF5, DS08X, FLOAT)
#define GPIO94_SSP3_TXD		MFP_CFG_X(GPIO94, AF5, DS08X, DRIVE_LOW)
#define GPIO94_SSP3_RXD		MFP_CFG_X(GPIO94, AF1, DS08X, FLOAT)

/* SSP4 */
#define GPIO95_SSP4_SCLK	MFP_CFG_LPM(GPIO95, AF1, PULL_HIGH)
#define GPIO96_SSP4_FRM		MFP_CFG_LPM(GPIO96, AF1, PULL_HIGH)
#define GPIO97_SSP4_TXD		MFP_CFG_LPM(GPIO97, AF1, PULL_HIGH)
#define GPIO97_SSP4_RXD		MFP_CFG_LPM(GPIO97, AF5, PULL_HIGH)
#define GPIO98_SSP4_TXD		MFP_CFG_LPM(GPIO98, AF5, PULL_HIGH)
#define GPIO98_SSP4_RXD		MFP_CFG_LPM(GPIO98, AF1, PULL_HIGH)

/* UART1 */
#define GPIO32_UART1_CTS	MFP_CFG_LPM(GPIO32,  AF2, FLOAT)
#define GPIO37_UART1_CTS	MFP_CFG_LPM(GPIO37,  AF4, FLOAT)
#define GPIO79_UART1_CTS	MFP_CFG_LPM(GPIO79,  AF1, FLOAT)
#define GPIO84_UART1_CTS	MFP_CFG_LPM(GPIO84,  AF3, FLOAT)
#define GPIO101_UART1_CTS	MFP_CFG_LPM(GPIO101, AF1, FLOAT)
#define GPIO106_UART1_CTS	MFP_CFG_LPM(GPIO106, AF6, FLOAT)

#define GPIO32_UART1_RTS	MFP_CFG_LPM(GPIO32,  AF4, FLOAT)
#define GPIO37_UART1_RTS	MFP_CFG_LPM(GPIO37,  AF2, FLOAT)
#define GPIO79_UART1_RTS	MFP_CFG_LPM(GPIO79,  AF3, FLOAT)
#define GPIO84_UART1_RTS	MFP_CFG_LPM(GPIO84,  AF1, FLOAT)
#define GPIO101_UART1_RTS	MFP_CFG_LPM(GPIO101, AF6, FLOAT)
#define GPIO106_UART1_RTS	MFP_CFG_LPM(GPIO106, AF1, FLOAT)

#define GPIO34_UART1_DSR	MFP_CFG_LPM(GPIO34,  AF2, FLOAT)
#define GPIO36_UART1_DSR	MFP_CFG_LPM(GPIO36,  AF4, FLOAT)
#define GPIO81_UART1_DSR	MFP_CFG_LPM(GPIO81,  AF1, FLOAT)
#define GPIO83_UART1_DSR	MFP_CFG_LPM(GPIO83,  AF3, FLOAT)
#define GPIO103_UART1_DSR	MFP_CFG_LPM(GPIO103, AF1, FLOAT)
#define GPIO105_UART1_DSR	MFP_CFG_LPM(GPIO105, AF6, FLOAT)

#define GPIO34_UART1_DTR	MFP_CFG_LPM(GPIO34,  AF4, FLOAT)
#define GPIO36_UART1_DTR	MFP_CFG_LPM(GPIO36,  AF2, FLOAT)
#define GPIO81_UART1_DTR	MFP_CFG_LPM(GPIO81,  AF3, FLOAT)
#define GPIO83_UART1_DTR	MFP_CFG_LPM(GPIO83,  AF1, FLOAT)
#define GPIO103_UART1_DTR	MFP_CFG_LPM(GPIO103, AF6, FLOAT)
#define GPIO105_UART1_DTR	MFP_CFG_LPM(GPIO105, AF1, FLOAT)

#define GPIO35_UART1_RI		MFP_CFG_LPM(GPIO35,  AF2, FLOAT)
#define GPIO82_UART1_RI		MFP_CFG_LPM(GPIO82,  AF1, FLOAT)
#define GPIO104_UART1_RI	MFP_CFG_LPM(GPIO104, AF1, FLOAT)

#define GPIO33_UART1_DCD	MFP_CFG_LPM(GPIO33,  AF2, FLOAT)
#define GPIO80_UART1_DCD	MFP_CFG_LPM(GPIO80,  AF1, FLOAT)
#define GPIO102_UART1_DCD	MFP_CFG_LPM(GPIO102, AF1, FLOAT)

#define GPIO30_UART1_RXD	MFP_CFG_LPM(GPIO30,  AF2, FLOAT)
#define GPIO31_UART1_RXD	MFP_CFG_LPM(GPIO31,  AF4, FLOAT)
#define GPIO77_UART1_RXD	MFP_CFG_LPM(GPIO77,  AF1, FLOAT)
#define GPIO78_UART1_RXD	MFP_CFG_LPM(GPIO78,  AF3, FLOAT)
#define GPIO99_UART1_RXD	MFP_CFG_LPM(GPIO99,  AF1, FLOAT)
#define GPIO100_UART1_RXD	MFP_CFG_LPM(GPIO100, AF6, FLOAT)
#define GPIO102_UART1_RXD	MFP_CFG_LPM(GPIO102, AF6, FLOAT)
#define GPIO104_UART1_RXD	MFP_CFG_LPM(GPIO104, AF4, FLOAT)

#define GPIO30_UART1_TXD	MFP_CFG_LPM(GPIO30,  AF4, FLOAT)
#define GPIO31_UART1_TXD	MFP_CFG_LPM(GPIO31,  AF2, FLOAT)
#define GPIO77_UART1_TXD	MFP_CFG_LPM(GPIO77,  AF3, FLOAT)
#define GPIO78_UART1_TXD	MFP_CFG_LPM(GPIO78,  AF1, FLOAT)
#define GPIO99_UART1_TXD	MFP_CFG_LPM(GPIO99,  AF6, FLOAT)
#define GPIO100_UART1_TXD	MFP_CFG_LPM(GPIO100, AF1, FLOAT)
#define GPIO102_UART1_TXD	MFP_CFG_LPM(GPIO102, AF4, FLOAT)

/* UART2 */
#define GPIO15_UART2_CTS	MFP_CFG_LPM(GPIO15,  AF3, FLOAT)
#define GPIO16_UART2_CTS	MFP_CFG_LPM(GPIO16,  AF5, FLOAT)
#define GPIO111_UART2_CTS	MFP_CFG_LPM(GPIO111, AF3, FLOAT)
#define GPIO114_UART2_CTS	MFP_CFG_LPM(GPIO114, AF1, FLOAT)

#define GPIO15_UART2_RTS	MFP_CFG_LPM(GPIO15,  AF4, FLOAT)
#define GPIO16_UART2_RTS	MFP_CFG_LPM(GPIO16,  AF4, FLOAT)
#define GPIO114_UART2_RTS	MFP_CFG_LPM(GPIO114, AF3, FLOAT)
#define GPIO111_UART2_RTS	MFP_CFG_LPM(GPIO111, AF1, FLOAT)

#define GPIO18_UART2_RXD	MFP_CFG_LPM(GPIO18,  AF5, FLOAT)
#define GPIO19_UART2_RXD	MFP_CFG_LPM(GPIO19,  AF4, FLOAT)
#define GPIO112_UART2_RXD	MFP_CFG_LPM(GPIO112, AF1, FLOAT)
#define GPIO113_UART2_RXD	MFP_CFG_LPM(GPIO113, AF3, FLOAT)

#define GPIO18_UART2_TXD	MFP_CFG_LPM(GPIO18,  AF4, FLOAT)
#define GPIO19_UART2_TXD	MFP_CFG_LPM(GPIO19,  AF5, FLOAT)
#define GPIO112_UART2_TXD	MFP_CFG_LPM(GPIO112, AF3, FLOAT)
#define GPIO113_UART2_TXD	MFP_CFG_LPM(GPIO113, AF1, FLOAT)

/* UART3 */
#define GPIO91_UART3_CTS	MFP_CFG_LPM(GPIO91,  AF2, FLOAT)
#define GPIO92_UART3_CTS	MFP_CFG_LPM(GPIO92,  AF4, FLOAT)
#define GPIO107_UART3_CTS	MFP_CFG_LPM(GPIO107, AF1, FLOAT)
#define GPIO108_UART3_CTS	MFP_CFG_LPM(GPIO108, AF3, FLOAT)

#define GPIO91_UART3_RTS	MFP_CFG_LPM(GPIO91,  AF4, FLOAT)
#define GPIO92_UART3_RTS	MFP_CFG_LPM(GPIO92,  AF2, FLOAT)
#define GPIO107_UART3_RTS	MFP_CFG_LPM(GPIO107, AF3, FLOAT)
#define GPIO108_UART3_RTS	MFP_CFG_LPM(GPIO108, AF1, FLOAT)

#define GPIO7_UART3_RXD		MFP_CFG_LPM(GPIO7,   AF2, FLOAT)
#define GPIO8_UART3_RXD		MFP_CFG_LPM(GPIO8,   AF6, FLOAT)
#define GPIO93_UART3_RXD	MFP_CFG_LPM(GPIO93,  AF4, FLOAT)
#define GPIO94_UART3_RXD	MFP_CFG_LPM(GPIO94,  AF2, FLOAT)
#define GPIO109_UART3_RXD	MFP_CFG_LPM(GPIO109, AF3, FLOAT)
#define GPIO110_UART3_RXD	MFP_CFG_LPM(GPIO110, AF1, FLOAT)

#define GPIO7_UART3_TXD		MFP_CFG_LPM(GPIO7,   AF6, FLOAT)
#define GPIO8_UART3_TXD		MFP_CFG_LPM(GPIO8,   AF2, FLOAT)
#define GPIO93_UART3_TXD	MFP_CFG_LPM(GPIO93,  AF2, FLOAT)
#define GPIO94_UART3_TXD	MFP_CFG_LPM(GPIO94,  AF4, FLOAT)
#define GPIO109_UART3_TXD	MFP_CFG_LPM(GPIO109, AF1, FLOAT)
#define GPIO110_UART3_TXD	MFP_CFG_LPM(GPIO110, AF3, FLOAT)

/* USB Host */
#define GPIO0_2_USBH_PEN	MFP_CFG(GPIO0_2, AF1)
#define GPIO1_2_USBH_PWR	MFP_CFG(GPIO1_2, AF1)

/* USB P3 */
#define GPIO77_USB_P3_1		MFP_CFG(GPIO77,  AF2)
#define GPIO78_USB_P3_2		MFP_CFG(GPIO78,  AF2)
#define GPIO79_USB_P3_3		MFP_CFG(GPIO79,  AF2)
#define GPIO80_USB_P3_4		MFP_CFG(GPIO80,  AF2)
#define GPIO81_USB_P3_5		MFP_CFG(GPIO81,  AF2)
#define GPIO82_USB_P3_6		MFP_CFG(GPIO82,  AF2)

/* PWM */
#define GPIO17_PWM0_OUT		MFP_CFG(GPIO17, AF1)
#define GPIO18_PWM1_OUT		MFP_CFG(GPIO18, AF1)
#define GPIO19_PWM2_OUT		MFP_CFG(GPIO19, AF1)
#define GPIO20_PWM3_OUT		MFP_CFG(GPIO20, AF1)

/* CIR */
#define GPIO8_CIR_OUT		MFP_CFG(GPIO8, AF5)
#define GPIO16_CIR_OUT		MFP_CFG(GPIO16, AF3)

#define GPIO20_OW_DQ_IN		MFP_CFG(GPIO20, AF5)
#define GPIO126_OW_DQ		MFP_CFG(GPIO126, AF2)

#define GPIO0_DF_RDY		MFP_CFG(GPIO0, AF1)
#define GPIO7_CLK_BYPASS_XSC	MFP_CFG(GPIO7, AF7)
#define GPIO17_EXT_SYNC_MVT_0	MFP_CFG(GPIO17, AF6)
#define GPIO18_EXT_SYNC_MVT_1	MFP_CFG(GPIO18, AF6)
#define GPIO19_OST_CHOUT_MVT_0	MFP_CFG(GPIO19, AF6)
#define GPIO20_OST_CHOUT_MVT_1	MFP_CFG(GPIO20, AF6)
#define GPIO49_48M_CLK		MFP_CFG(GPIO49, AF2)
#define GPIO126_EXT_CLK		MFP_CFG(GPIO126, AF3)
#define GPIO127_CLK_BYPASS_GB	MFP_CFG(GPIO127, AF7)
#define GPIO71_EXT_MATCH_MVT	MFP_CFG(GPIO71, AF6)

#define GPIO3_uIO_IN		MFP_CFG(GPIO3, AF1)

#define GPIO4_uSIM_CARD_STATE	MFP_CFG(GPIO4, AF1)
#define GPIO5_uSIM_uCLK		MFP_CFG(GPIO5, AF1)
#define GPIO6_uSIM_uRST		MFP_CFG(GPIO6, AF1)
#define GPIO16_uSIM_UVS_0	MFP_CFG(GPIO16, AF1)

#define GPIO9_SCIO		MFP_CFG(GPIO9, AF1)
#define GPIO20_RTC_MVT		MFP_CFG(GPIO20, AF4)
#define GPIO126_RTC_MVT		MFP_CFG(GPIO126, AF1)

/*
 * PXA300 specific MFP configurations
 */
#ifdef CONFIG_CPU_PXA300
#define GPIO99_USB_P2_2		MFP_CFG(GPIO99, AF2)
#define GPIO99_USB_P2_5		MFP_CFG(GPIO99, AF3)
#define GPIO99_USB_P2_6		MFP_CFG(GPIO99, AF4)
#define GPIO100_USB_P2_2	MFP_CFG(GPIO100, AF4)
#define GPIO100_USB_P2_5	MFP_CFG(GPIO100, AF5)
#define GPIO101_USB_P2_1	MFP_CFG(GPIO101, AF2)
#define GPIO102_USB_P2_4	MFP_CFG(GPIO102, AF2)
#define GPIO104_USB_P2_3	MFP_CFG(GPIO104, AF2)
#define GPIO105_USB_P2_5	MFP_CFG(GPIO105, AF2)
#define GPIO100_USB_P2_6	MFP_CFG(GPIO100, AF2)
#define GPIO106_USB_P2_7	MFP_CFG(GPIO106, AF2)
#define GPIO103_USB_P2_8	MFP_CFG(GPIO103, AF2)

/* U2D UTMI */
#define GPIO38_UTM_CLK		MFP_CFG(GPIO38,  AF1)
#define GPIO26_U2D_RXERROR	MFP_CFG(GPIO26,  AF3)
#define GPIO50_U2D_RXERROR	MFP_CFG(GPIO50,  AF1)
#define GPIO89_U2D_RXERROR	MFP_CFG(GPIO89,  AF5)
#define GPIO24_UTM_RXVALID	MFP_CFG(GPIO24,  AF3)
#define GPIO48_UTM_RXVALID	MFP_CFG(GPIO48,  AF2)
#define GPIO87_UTM_RXVALID	MFP_CFG(GPIO87,  AF5)
#define GPIO25_UTM_RXACTIVE	MFP_CFG(GPIO25,  AF3)
#define GPIO47_UTM_RXACTIVE	MFP_CFG(GPIO47,  AF2)
#define GPIO49_UTM_RXACTIVE	MFP_CFG(GPIO49,  AF1)
#define GPIO88_UTM_RXACTIVE	MFP_CFG(GPIO88,  AF5)
#define GPIO53_UTM_TXREADY	MFP_CFG(GPIO53,  AF1)
#define GPIO67_UTM_LINESTATE_0	MFP_CFG(GPIO67,  AF3)
#define GPIO92_UTM_LINESTATE_0	MFP_CFG(GPIO92,  AF3)
#define GPIO104_UTM_LINESTATE_0	MFP_CFG(GPIO104, AF3)
#define GPIO109_UTM_LINESTATE_0	MFP_CFG(GPIO109, AF4)
#define GPIO68_UTM_LINESTATE_1	MFP_CFG(GPIO68,  AF3)
#define GPIO93_UTM_LINESTATE_1	MFP_CFG(GPIO93,  AF3)
#define GPIO105_UTM_LINESTATE_1	MFP_CFG(GPIO105, AF3)
#define GPIO27_U2D_OPMODE_0	MFP_CFG(GPIO27,  AF4)
#define GPIO51_U2D_OPMODE_0	MFP_CFG(GPIO51,  AF2)
#define GPIO90_U2D_OPMODE_0	MFP_CFG(GPIO90,  AF7)
#define GPIO28_U2D_OPMODE_1	MFP_CFG(GPIO28,  AF4)
#define GPIO52_U2D_OPMODE_1	MFP_CFG(GPIO52,  AF2)
#define GPIO106_U2D_OPMODE_1	MFP_CFG(GPIO106, AF3)
#define GPIO110_U2D_OPMODE_1	MFP_CFG(GPIO110, AF5)
#define GPIO76_U2D_RESET	MFP_CFG(GPIO76,  AF1)
#define GPIO95_U2D_RESET	MFP_CFG(GPIO95,  AF2)
#define GPIO100_U2D_RESET	MFP_CFG(GPIO100, AF3)
#define GPIO66_U2D_SUSPEND	MFP_CFG(GPIO66,  AF3)
#define GPIO98_U2D_SUSPEND	MFP_CFG(GPIO98,  AF2)
#define GPIO103_U2D_SUSPEND	MFP_CFG(GPIO103, AF3)
#define GPIO65_U2D_TERM_SEL	MFP_CFG(GPIO65,  AF5)
#define GPIO97_U2D_TERM_SEL	MFP_CFG(GPIO97,  AF3)
#define GPIO102_U2D_TERM_SEL	MFP_CFG(GPIO102, AF5)
#define GPIO29_U2D_TXVALID	MFP_CFG(GPIO29,  AF3)
#define GPIO52_U2D_TXVALID	MFP_CFG(GPIO52,  AF4)
#define GPIO69_U2D_TXVALID	MFP_CFG(GPIO69,  AF3)
#define GPIO85_U2D_TXVALID	MFP_CFG(GPIO85,  AF7)
#define GPIO64_U2D_XCVR_SEL	MFP_CFG(GPIO64,  AF5)
#define GPIO96_U2D_XCVR_SEL	MFP_CFG(GPIO96,  AF3)
#define GPIO101_U2D_XCVR_SEL	MFP_CFG(GPIO101, AF5)
#define GPIO30_UTM_PHYDATA_0	MFP_CFG(GPIO30,  AF3)
#define GPIO31_UTM_PHYDATA_1	MFP_CFG(GPIO31,  AF3)
#define GPIO32_UTM_PHYDATA_2	MFP_CFG(GPIO32,  AF3)
#define GPIO33_UTM_PHYDATA_3	MFP_CFG(GPIO33,  AF3)
#define GPIO34_UTM_PHYDATA_4	MFP_CFG(GPIO34,  AF3)
#define GPIO35_UTM_PHYDATA_5	MFP_CFG(GPIO35,  AF3)
#define GPIO36_UTM_PHYDATA_6	MFP_CFG(GPIO36,  AF3)
#define GPIO37_UTM_PHYDATA_7	MFP_CFG(GPIO37,  AF3)
#define GPIO39_UTM_PHYDATA_0	MFP_CFG(GPIO39,  AF3)
#define GPIO40_UTM_PHYDATA_1	MFP_CFG(GPIO40,  AF3)
#define GPIO41_UTM_PHYDATA_2	MFP_CFG(GPIO41,  AF3)
#define GPIO42_UTM_PHYDATA_3	MFP_CFG(GPIO42,  AF3)
#define GPIO43_UTM_PHYDATA_4	MFP_CFG(GPIO43,  AF3)
#define GPIO44_UTM_PHYDATA_5	MFP_CFG(GPIO44,  AF3)
#define GPIO45_UTM_PHYDATA_6	MFP_CFG(GPIO45,  AF3)
#define GPIO46_UTM_PHYDATA_7	MFP_CFG(GPIO46,  AF3)
#endif /* CONFIG_CPU_PXA300 */

/*
 * PXA310 specific MFP configurations
 */
#ifdef CONFIG_CPU_PXA310
/* USB P2 */
#define GPIO36_USB_P2_1		MFP_CFG(GPIO36, AF1)
#define GPIO30_USB_P2_2		MFP_CFG(GPIO30, AF1)
#define GPIO35_USB_P2_3		MFP_CFG(GPIO35, AF1)
#define GPIO32_USB_P2_4		MFP_CFG(GPIO32, AF1)
#define GPIO34_USB_P2_5		MFP_CFG(GPIO34, AF1)
#define GPIO31_USB_P2_6		MFP_CFG(GPIO31, AF1)

/* MMC1 */
#define GPIO24_MMC1_CMD		MFP_CFG(GPIO24, AF3)
#define GPIO29_MMC1_DAT0	MFP_CFG(GPIO29, AF3)

/* MMC3 */
#define GPIO103_MMC3_CLK	MFP_CFG(GPIO103, AF2)
#define GPIO105_MMC3_CMD	MFP_CFG(GPIO105, AF2)
#define GPIO11_2_MMC3_CLK	MFP_CFG(GPIO11_2, AF1)
#define GPIO12_2_MMC3_CMD	MFP_CFG(GPIO12_2, AF1)
#define GPIO7_2_MMC3_DAT0	MFP_CFG(GPIO7_2, AF1)
#define GPIO8_2_MMC3_DAT1	MFP_CFG(GPIO8_2, AF1)
#define GPIO9_2_MMC3_DAT2	MFP_CFG(GPIO9_2, AF1)
#define GPIO10_2_MMC3_DAT3	MFP_CFG(GPIO10_2, AF1)

/* ULPI */
#define GPIO38_ULPI_CLK		MFP_CFG(GPIO38, AF1)
#define GPIO30_ULPI_DATA_OUT_0	MFP_CFG(GPIO30, AF3)
#define GPIO31_ULPI_DATA_OUT_1	MFP_CFG(GPIO31, AF3)
#define GPIO32_ULPI_DATA_OUT_2	MFP_CFG(GPIO32, AF3)
#define GPIO33_ULPI_DATA_OUT_3	MFP_CFG(GPIO33, AF3)
#define GPIO34_ULPI_DATA_OUT_4	MFP_CFG(GPIO34, AF3)
#define GPIO35_ULPI_DATA_OUT_5	MFP_CFG(GPIO35, AF3)
#define GPIO36_ULPI_DATA_OUT_6	MFP_CFG(GPIO36, AF3)
#define GPIO37_ULPI_DATA_OUT_7	MFP_CFG(GPIO37, AF3)
#define GPIO33_ULPI_OTG_INTR	MFP_CFG(GPIO33, AF1)

#define ULPI_DIR	MFP_CFG_DRV(ULPI_DIR, MFP_AF0, MFP_DS01X)
#define ULPI_NXT	MFP_CFG_DRV(ULPI_NXT, MFP_AF0, MFP_DS01X)
#define ULPI_STP	MFP_CFG_DRV(ULPI_STP, MFP_AF0, MFP_DS01X)
#endif /* CONFIG_CPU_PXA310 */

#endif /* __ASM_ARCH_MFP_PXA300_H */
