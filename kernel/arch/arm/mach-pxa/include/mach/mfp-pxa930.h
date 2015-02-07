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
 * arch/arm/mach-pxa/include/mach/mfp-pxa930.h
 *
 * PXA930 specific MFP configuration definitions
 *
 * Copyright (C) 2007-2008 Marvell International Ltd.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef __ASM_ARCH_MFP_PXA9xx_H
#define __ASM_ARCH_MFP_PXA9xx_H

#include <mach/mfp.h>
#include <mach/mfp-pxa3xx.h>

/* GPIO */
#define GPIO46_GPIO		MFP_CFG(GPIO46, AF0)
#define GPIO49_GPIO		MFP_CFG(GPIO49, AF0)
#define GPIO50_GPIO		MFP_CFG(GPIO50, AF0)
#define GPIO51_GPIO		MFP_CFG(GPIO51, AF0)
#define GPIO52_GPIO		MFP_CFG(GPIO52, AF0)
#define GPIO56_GPIO		MFP_CFG(GPIO56, AF0)
#define GPIO58_GPIO		MFP_CFG(GPIO58, AF0)
#define GPIO59_GPIO		MFP_CFG(GPIO59, AF0)
#define GPIO60_GPIO		MFP_CFG(GPIO60, AF0)
#define GPIO61_GPIO		MFP_CFG(GPIO61, AF0)
#define GPIO62_GPIO		MFP_CFG(GPIO62, AF0)

#define GSIM_UCLK_GPIO_79	MFP_CFG(GSIM_UCLK, AF0)
#define GSIM_UIO_GPIO_80	MFP_CFG(GSIM_UIO, AF0)
#define GSIM_nURST_GPIO_81	MFP_CFG(GSIM_nURST, AF0)
#define GSIM_UDET_GPIO_82	MFP_CFG(GSIM_UDET, AF0)

#define DF_IO15_GPIO_28		MFP_CFG(DF_IO15, AF0)
#define DF_IO14_GPIO_29		MFP_CFG(DF_IO14, AF0)
#define DF_IO13_GPIO_30		MFP_CFG(DF_IO13, AF0)
#define DF_IO12_GPIO_31		MFP_CFG(DF_IO12, AF0)
#define DF_IO11_GPIO_32		MFP_CFG(DF_IO11, AF0)
#define DF_IO10_GPIO_33		MFP_CFG(DF_IO10, AF0)
#define DF_IO9_GPIO_34		MFP_CFG(DF_IO9, AF0)
#define DF_IO8_GPIO_35		MFP_CFG(DF_IO8, AF0)
#define DF_IO7_GPIO_36		MFP_CFG(DF_IO7, AF0)
#define DF_IO6_GPIO_37		MFP_CFG(DF_IO6, AF0)
#define DF_IO5_GPIO_38		MFP_CFG(DF_IO5, AF0)
#define DF_IO4_GPIO_39		MFP_CFG(DF_IO4, AF0)
#define DF_IO3_GPIO_40		MFP_CFG(DF_IO3, AF0)
#define DF_IO2_GPIO_41		MFP_CFG(DF_IO2, AF0)
#define DF_IO1_GPIO_42		MFP_CFG(DF_IO1, AF0)
#define DF_IO0_GPIO_43		MFP_CFG(DF_IO0, AF0)
#define DF_nCS0_GPIO_44		MFP_CFG(DF_nCS0, AF0)
#define DF_nCS1_GPIO_45		MFP_CFG(DF_nCS1, AF0)
#define DF_nWE_GPIO_46		MFP_CFG(DF_nWE, AF0)
#define DF_nRE_nOE_GPIO_47	MFP_CFG(DF_nRE_nOE, AF0)
#define DF_CLE_nOE_GPIO_48	MFP_CFG(DF_CLE_nOE, AF0)
#define DF_nADV1_ALE_GPIO_49	MFP_CFG(DF_nADV1_ALE, AF0)
#define DF_nADV2_ALE_GPIO_50	MFP_CFG(DF_nADV2_ALE, AF0)
#define DF_INT_RnB_GPIO_51	MFP_CFG(DF_INT_RnB, AF0)
#define DF_SCLK_E_GPIO_52	MFP_CFG(DF_SCLK_E, AF0)

#define DF_ADDR0_GPIO_53	MFP_CFG(DF_ADDR0, AF0)
#define DF_ADDR1_GPIO_54	MFP_CFG(DF_ADDR1, AF0)
#define DF_ADDR2_GPIO_55	MFP_CFG(DF_ADDR2, AF0)
#define DF_ADDR3_GPIO_56	MFP_CFG(DF_ADDR3, AF0)
#define nXCVREN_GPIO_57		MFP_CFG(nXCVREN, AF0)
#define nLUA_GPIO_58		MFP_CFG(nLUA, AF0)
#define nLLA_GPIO_59		MFP_CFG(nLLA, AF0)
#define nBE0_GPIO_60		MFP_CFG(nBE0, AF0)
#define nBE1_GPIO_61		MFP_CFG(nBE1, AF0)
#define RDY_GPIO_62		MFP_CFG(RDY, AF0)

/* Chip Select */
#define DF_nCS0_nCS2		MFP_CFG_LPM(DF_nCS0, AF3, PULL_HIGH)
#define DF_nCS1_nCS3		MFP_CFG_LPM(DF_nCS1, AF3, PULL_HIGH)

/* AC97 */
#define GPIO83_BAC97_SYSCLK	MFP_CFG(GPIO83, AF3)
#define GPIO84_BAC97_SDATA_IN0	MFP_CFG(GPIO84, AF3)
#define GPIO85_BAC97_BITCLK	MFP_CFG(GPIO85, AF3)
#define GPIO86_BAC97_nRESET	MFP_CFG(GPIO86, AF3)
#define GPIO87_BAC97_SYNC	MFP_CFG(GPIO87, AF3)
#define GPIO88_BAC97_SDATA_OUT	MFP_CFG(GPIO88, AF3)

/* I2C */
#define GPIO39_CI2C_SCL		MFP_CFG_LPM(GPIO39, AF3, PULL_HIGH)
#define GPIO40_CI2C_SDA		MFP_CFG_LPM(GPIO40, AF3, PULL_HIGH)

#define GPIO51_CI2C_SCL		MFP_CFG_LPM(GPIO51, AF3, PULL_HIGH)
#define GPIO52_CI2C_SDA		MFP_CFG_LPM(GPIO52, AF3, PULL_HIGH)

#define GPIO63_CI2C_SCL		MFP_CFG_LPM(GPIO63, AF4, PULL_HIGH)
#define GPIO64_CI2C_SDA		MFP_CFG_LPM(GPIO64, AF4, PULL_HIGH)

#define GPIO77_CI2C_SCL		MFP_CFG_LPM(GPIO77, AF2, PULL_HIGH)
#define GPIO78_CI2C_SDA		MFP_CFG_LPM(GPIO78, AF2, PULL_HIGH)

#define GPIO89_CI2C_SCL		MFP_CFG_LPM(GPIO89, AF1, PULL_HIGH)
#define GPIO90_CI2C_SDA		MFP_CFG_LPM(GPIO90, AF1, PULL_HIGH)

#define GPIO95_CI2C_SCL		MFP_CFG_LPM(GPIO95, AF1, PULL_HIGH)
#define GPIO96_CI2C_SDA		MFP_CFG_LPM(GPIO96, AF1, PULL_HIGH)

#define GPIO97_CI2C_SCL		MFP_CFG_LPM(GPIO97, AF3, PULL_HIGH)
#define GPIO98_CI2C_SDA		MFP_CFG_LPM(GPIO98, AF3, PULL_HIGH)

/* QCI */
#define GPIO63_CI_DD_9		MFP_CFG_LPM(GPIO63, AF1, PULL_LOW)
#define GPIO64_CI_DD_8		MFP_CFG_LPM(GPIO64, AF1, PULL_LOW)
#define GPIO65_CI_DD_7		MFP_CFG_LPM(GPIO65, AF1, PULL_LOW)
#define GPIO66_CI_DD_6		MFP_CFG_LPM(GPIO66, AF1, PULL_LOW)
#define GPIO67_CI_DD_5		MFP_CFG_LPM(GPIO67, AF1, PULL_LOW)
#define GPIO68_CI_DD_4		MFP_CFG_LPM(GPIO68, AF1, PULL_LOW)
#define GPIO69_CI_DD_3		MFP_CFG_LPM(GPIO69, AF1, PULL_LOW)
#define GPIO70_CI_DD_2		MFP_CFG_LPM(GPIO70, AF1, PULL_LOW)
#define GPIO71_CI_DD_1		MFP_CFG_LPM(GPIO71, AF1, PULL_LOW)
#define GPIO72_CI_DD_0		MFP_CFG_LPM(GPIO72, AF1, PULL_LOW)
#define GPIO73_CI_HSYNC		MFP_CFG_LPM(GPIO73, AF1, PULL_LOW)
#define GPIO74_CI_VSYNC		MFP_CFG_LPM(GPIO74, AF1, PULL_LOW)
#define GPIO75_CI_MCLK		MFP_CFG_LPM(GPIO75, AF1, PULL_LOW)
#define GPIO76_CI_PCLK		MFP_CFG_LPM(GPIO76, AF1, PULL_LOW)

/* KEYPAD */
#define GPIO4_KP_DKIN_4		MFP_CFG_LPM(GPIO4, AF3, FLOAT)
#define GPIO5_KP_DKIN_5		MFP_CFG_LPM(GPIO5, AF3, FLOAT)
#define GPIO6_KP_DKIN_6		MFP_CFG_LPM(GPIO6, AF3, FLOAT)
#define GPIO7_KP_DKIN_7		MFP_CFG_LPM(GPIO7, AF3, FLOAT)
#define GPIO8_KP_DKIN_4		MFP_CFG_LPM(GPIO8, AF3, FLOAT)
#define GPIO9_KP_DKIN_5		MFP_CFG_LPM(GPIO9, AF3, FLOAT)
#define GPIO10_KP_DKIN_6	MFP_CFG_LPM(GPIO10, AF3, FLOAT)
#define GPIO11_KP_DKIN_7	MFP_CFG_LPM(GPIO11, AF3, FLOAT)

#define GPIO12_KP_DKIN_0	MFP_CFG_LPM(GPIO12, AF2, FLOAT)
#define GPIO13_KP_DKIN_1	MFP_CFG_LPM(GPIO13, AF2, FLOAT)
#define GPIO14_KP_DKIN_2	MFP_CFG_LPM(GPIO14, AF2, FLOAT)
#define GPIO15_KP_DKIN_3	MFP_CFG_LPM(GPIO15, AF2, FLOAT)

#define GPIO41_KP_DKIN_0	MFP_CFG_LPM(GPIO41, AF2, FLOAT)
#define GPIO42_KP_DKIN_1	MFP_CFG_LPM(GPIO42, AF2, FLOAT)
#define GPIO43_KP_DKIN_2	MFP_CFG_LPM(GPIO43, AF2, FLOAT)
#define GPIO44_KP_DKIN_3	MFP_CFG_LPM(GPIO44, AF2, FLOAT)
#define GPIO41_KP_DKIN_4	MFP_CFG_LPM(GPIO41, AF4, FLOAT)
#define GPIO42_KP_DKIN_5	MFP_CFG_LPM(GPIO42, AF4, FLOAT)

#define GPIO0_KP_MKIN_0		MFP_CFG_LPM(GPIO0, AF1, FLOAT)
#define GPIO2_KP_MKIN_1		MFP_CFG_LPM(GPIO2, AF1, FLOAT)
#define GPIO4_KP_MKIN_2		MFP_CFG_LPM(GPIO4, AF1, FLOAT)
#define GPIO6_KP_MKIN_3		MFP_CFG_LPM(GPIO6, AF1, FLOAT)
#define GPIO8_KP_MKIN_4		MFP_CFG_LPM(GPIO8, AF1, FLOAT)
#define GPIO10_KP_MKIN_5	MFP_CFG_LPM(GPIO10, AF1, FLOAT)
#define GPIO12_KP_MKIN_6	MFP_CFG_LPM(GPIO12, AF1, FLOAT)
#define GPIO14_KP_MKIN_7	MFP_CFG(GPIO14, AF1)
#define GPIO35_KP_MKIN_5	MFP_CFG(GPIO35, AF4)

#define GPIO1_KP_MKOUT_0	MFP_CFG_LPM(GPIO1, AF1, DRIVE_HIGH)
#define GPIO3_KP_MKOUT_1	MFP_CFG_LPM(GPIO3, AF1, DRIVE_HIGH)
#define GPIO5_KP_MKOUT_2	MFP_CFG_LPM(GPIO5, AF1, DRIVE_HIGH)
#define GPIO7_KP_MKOUT_3	MFP_CFG_LPM(GPIO7, AF1, DRIVE_HIGH)
#define GPIO9_KP_MKOUT_4	MFP_CFG_LPM(GPIO9, AF1, DRIVE_HIGH)
#define GPIO11_KP_MKOUT_5	MFP_CFG_LPM(GPIO11, AF1, DRIVE_HIGH)
#define GPIO13_KP_MKOUT_6	MFP_CFG_LPM(GPIO13, AF1, DRIVE_HIGH)
#define GPIO15_KP_MKOUT_7	MFP_CFG_LPM(GPIO15, AF1, DRIVE_HIGH)
#define GPIO36_KP_MKOUT_5	MFP_CFG_LPM(GPIO36, AF4, DRIVE_HIGH)

/* LCD */
#define GPIO17_LCD_FCLK_RD	MFP_CFG(GPIO17, AF1)
#define GPIO18_LCD_LCLK_A0	MFP_CFG(GPIO18, AF1)
#define GPIO19_LCD_PCLK_WR	MFP_CFG(GPIO19, AF1)
#define GPIO20_LCD_BIAS		MFP_CFG(GPIO20, AF1)
#define GPIO21_LCD_CS		MFP_CFG(GPIO21, AF1)
#define GPIO22_LCD_CS2		MFP_CFG(GPIO22, AF2)
#define GPIO22_LCD_VSYNC	MFP_CFG(GPIO22, AF1)
#define GPIO23_LCD_DD0		MFP_CFG(GPIO23, AF1)
#define GPIO24_LCD_DD1		MFP_CFG(GPIO24, AF1)
#define GPIO25_LCD_DD2		MFP_CFG(GPIO25, AF1)
#define GPIO26_LCD_DD3		MFP_CFG(GPIO26, AF1)
#define GPIO27_LCD_DD4		MFP_CFG(GPIO27, AF1)
#define GPIO28_LCD_DD5		MFP_CFG(GPIO28, AF1)
#define GPIO29_LCD_DD6		MFP_CFG(GPIO29, AF1)
#define GPIO30_LCD_DD7		MFP_CFG(GPIO30, AF1)
#define GPIO31_LCD_DD8		MFP_CFG(GPIO31, AF1)
#define GPIO32_LCD_DD9		MFP_CFG(GPIO32, AF1)
#define GPIO33_LCD_DD10		MFP_CFG(GPIO33, AF1)
#define GPIO34_LCD_DD11		MFP_CFG(GPIO34, AF1)
#define GPIO35_LCD_DD12		MFP_CFG(GPIO35, AF1)
#define GPIO36_LCD_DD13		MFP_CFG(GPIO36, AF1)
#define GPIO37_LCD_DD14		MFP_CFG(GPIO37, AF1)
#define GPIO38_LCD_DD15		MFP_CFG(GPIO38, AF1)
#define GPIO39_LCD_DD16		MFP_CFG(GPIO39, AF1)
#define GPIO40_LCD_DD17		MFP_CFG(GPIO40, AF1)
#define GPIO41_LCD_CS2		MFP_CFG(GPIO41, AF3)
#define GPIO42_LCD_VSYNC2	MFP_CFG(GPIO42, AF3)
#define GPIO44_LCD_DD7		MFP_CFG(GPIO44, AF1)

/* Mini-LCD */
#define GPIO17_MLCD_FCLK	MFP_CFG(GPIO17, AF3)
#define GPIO18_MLCD_LCLK	MFP_CFG(GPIO18, AF3)
#define GPIO19_MLCD_PCLK	MFP_CFG(GPIO19, AF3)
#define GPIO20_MLCD_BIAS	MFP_CFG(GPIO20, AF3)
#define GPIO23_MLCD_DD0		MFP_CFG(GPIO23, AF3)
#define GPIO24_MLCD_DD1		MFP_CFG(GPIO24, AF3)
#define GPIO25_MLCD_DD2		MFP_CFG(GPIO25, AF3)
#define GPIO26_MLCD_DD3		MFP_CFG(GPIO26, AF3)
#define GPIO27_MLCD_DD4		MFP_CFG(GPIO27, AF3)
#define GPIO28_MLCD_DD5		MFP_CFG(GPIO28, AF3)
#define GPIO29_MLCD_DD6		MFP_CFG(GPIO29, AF3)
#define GPIO30_MLCD_DD7		MFP_CFG(GPIO30, AF3)
#define GPIO31_MLCD_DD8		MFP_CFG(GPIO31, AF3)
#define GPIO32_MLCD_DD9		MFP_CFG(GPIO32, AF3)
#define GPIO33_MLCD_DD10	MFP_CFG(GPIO33, AF3)
#define GPIO34_MLCD_DD11	MFP_CFG(GPIO34, AF3)
#define GPIO35_MLCD_DD12	MFP_CFG(GPIO35, AF3)
#define GPIO36_MLCD_DD13	MFP_CFG(GPIO36, AF3)
#define GPIO37_MLCD_DD14	MFP_CFG(GPIO37, AF3)
#define GPIO38_MLCD_DD15	MFP_CFG(GPIO38, AF3)
#define GPIO44_MLCD_DD7		MFP_CFG(GPIO44, AF5)

/* MMC1 */
#define GPIO10_MMC1_DAT3	MFP_CFG(GPIO10, AF4)
#define GPIO11_MMC1_DAT2	MFP_CFG(GPIO11, AF4)
#define GPIO12_MMC1_DAT1	MFP_CFG(GPIO12, AF4)
#define GPIO13_MMC1_DAT0	MFP_CFG(GPIO13, AF4)
#define GPIO14_MMC1_CMD		MFP_CFG(GPIO14, AF4)
#define GPIO15_MMC1_CLK		MFP_CFG(GPIO15, AF4)
#define GPIO55_MMC1_CMD		MFP_CFG(GPIO55, AF3)
#define GPIO56_MMC1_CLK		MFP_CFG(GPIO56, AF3)
#define GPIO57_MMC1_DAT0	MFP_CFG(GPIO57, AF3)
#define GPIO58_MMC1_DAT1	MFP_CFG(GPIO58, AF3)
#define GPIO59_MMC1_DAT2	MFP_CFG(GPIO59, AF3)
#define GPIO60_MMC1_DAT3	MFP_CFG(GPIO60, AF3)

#define DF_ADDR0_MMC1_CLK	MFP_CFG(DF_ADDR0, AF2)
#define DF_ADDR1_MMC1_CMD	MFP_CFG(DF_ADDR1, AF2)
#define DF_ADDR2_MMC1_DAT0	MFP_CFG(DF_ADDR2, AF2)
#define DF_ADDR3_MMC1_DAT1	MFP_CFG(DF_ADDR3, AF3)
#define nXCVREN_MMC1_DAT2	MFP_CFG(nXCVREN, AF2)

/* MMC2 */
#define GPIO31_MMC2_CMD		MFP_CFG(GPIO31, AF7)
#define GPIO32_MMC2_CLK		MFP_CFG(GPIO32, AF7)
#define GPIO33_MMC2_DAT0	MFP_CFG(GPIO33, AF7)
#define GPIO34_MMC2_DAT1	MFP_CFG(GPIO34, AF7)
#define GPIO35_MMC2_DAT2	MFP_CFG(GPIO35, AF7)
#define GPIO36_MMC2_DAT3	MFP_CFG(GPIO36, AF7)

#define GPIO101_MMC2_DAT3	MFP_CFG(GPIO101, AF1)
#define GPIO102_MMC2_DAT2	MFP_CFG(GPIO102, AF1)
#define GPIO103_MMC2_DAT1	MFP_CFG(GPIO103, AF1)
#define GPIO104_MMC2_DAT0	MFP_CFG(GPIO104, AF1)
#define GPIO105_MMC2_CMD	MFP_CFG(GPIO105, AF1)
#define GPIO106_MMC2_CLK	MFP_CFG(GPIO106, AF1)

#define DF_IO10_MMC2_DAT3	MFP_CFG(DF_IO10, AF3)
#define DF_IO11_MMC2_DAT2	MFP_CFG(DF_IO11, AF3)
#define DF_IO12_MMC2_DAT1	MFP_CFG(DF_IO12, AF3)
#define DF_IO13_MMC2_DAT0	MFP_CFG(DF_IO13, AF3)
#define DF_IO14_MMC2_CLK	MFP_CFG(DF_IO14, AF3)
#define DF_IO15_MMC2_CMD	MFP_CFG(DF_IO15, AF3)

/* BSSP1 */
#define GPIO12_BSSP1_CLK	MFP_CFG(GPIO12, AF3)
#define GPIO13_BSSP1_FRM	MFP_CFG(GPIO13, AF3)
#define GPIO14_BSSP1_RXD	MFP_CFG(GPIO14, AF3)
#define GPIO15_BSSP1_TXD	MFP_CFG(GPIO15, AF3)
#define GPIO97_BSSP1_CLK	MFP_CFG(GPIO97, AF5)
#define GPIO98_BSSP1_FRM	MFP_CFG(GPIO98, AF5)

/* BSSP2 */
#define GPIO84_BSSP2_SDATA_IN	MFP_CFG(GPIO84, AF1)
#define GPIO85_BSSP2_BITCLK	MFP_CFG(GPIO85, AF1)
#define GPIO86_BSSP2_SYSCLK	MFP_CFG(GPIO86, AF1)
#define GPIO87_BSSP2_SYNC	MFP_CFG(GPIO87, AF1)
#define GPIO88_BSSP2_DATA_OUT	MFP_CFG(GPIO88, AF1)
#define GPIO86_BSSP2_SDATA_IN	MFP_CFG(GPIO86, AF4)

/* BSSP3 */
#define GPIO79_BSSP3_CLK	MFP_CFG(GPIO79, AF1)
#define GPIO80_BSSP3_FRM	MFP_CFG(GPIO80, AF1)
#define GPIO81_BSSP3_TXD	MFP_CFG(GPIO81, AF1)
#define GPIO82_BSSP3_RXD	MFP_CFG(GPIO82, AF1)
#define GPIO83_BSSP3_SYSCLK	MFP_CFG(GPIO83, AF1)

/* BSSP4 */
#define GPIO43_BSSP4_CLK	MFP_CFG(GPIO43, AF4)
#define GPIO44_BSSP4_FRM	MFP_CFG(GPIO44, AF4)
#define GPIO45_BSSP4_TXD	MFP_CFG(GPIO45, AF4)
#define GPIO46_BSSP4_RXD	MFP_CFG(GPIO46, AF4)

#define GPIO51_BSSP4_CLK	MFP_CFG(GPIO51, AF4)
#define GPIO52_BSSP4_FRM	MFP_CFG(GPIO52, AF4)
#define GPIO53_BSSP4_TXD	MFP_CFG(GPIO53, AF4)
#define GPIO54_BSSP4_RXD	MFP_CFG(GPIO54, AF4)

/* GSSP1 */
#define GPIO79_GSSP1_CLK	MFP_CFG(GPIO79, AF2)
#define GPIO80_GSSP1_FRM	MFP_CFG(GPIO80, AF2)
#define GPIO81_GSSP1_TXD	MFP_CFG(GPIO81, AF2)
#define GPIO82_GSSP1_RXD	MFP_CFG(GPIO82, AF2)
#define GPIO83_GSSP1_SYSCLK	MFP_CFG(GPIO83, AF2)

#define GPIO93_GSSP1_CLK	MFP_CFG(GPIO93, AF4)
#define GPIO94_GSSP1_FRM	MFP_CFG(GPIO94, AF4)
#define GPIO95_GSSP1_TXD	MFP_CFG(GPIO95, AF4)
#define GPIO96_GSSP1_RXD	MFP_CFG(GPIO96, AF4)

/* GSSP2 */
#define GPIO47_GSSP2_CLK	MFP_CFG(GPIO47, AF4)
#define GPIO48_GSSP2_FRM	MFP_CFG(GPIO48, AF4)
#define GPIO49_GSSP2_RXD	MFP_CFG(GPIO49, AF4)
#define GPIO50_GSSP2_TXD	MFP_CFG(GPIO50, AF4)

#define GPIO69_GSSP2_CLK	MFP_CFG(GPIO69, AF4)
#define GPIO70_GSSP2_FRM	MFP_CFG(GPIO70, AF4)
#define GPIO71_GSSP2_RXD	MFP_CFG(GPIO71, AF4)
#define GPIO72_GSSP2_TXD	MFP_CFG(GPIO72, AF4)

#define GPIO84_GSSP2_RXD	MFP_CFG(GPIO84, AF2)
#define GPIO85_GSSP2_CLK	MFP_CFG(GPIO85, AF2)
#define GPIO86_GSSP2_SYSCLK	MFP_CFG(GPIO86, AF2)
#define GPIO87_GSSP2_FRM	MFP_CFG(GPIO87, AF2)
#define GPIO88_GSSP2_TXD	MFP_CFG(GPIO88, AF2)
#define GPIO86_GSSP2_RXD	MFP_CFG(GPIO86, AF5)

#define GPIO103_GSSP2_CLK	MFP_CFG(GPIO103, AF2)
#define GPIO104_GSSP2_FRM	MFP_CFG(GPIO104, AF2)
#define GPIO105_GSSP2_RXD	MFP_CFG(GPIO105, AF2)
#define GPIO106_GSSP2_TXD	MFP_CFG(GPIO106, AF2)

/* UART1 - FFUART */
#define GPIO47_UART1_DSR_N	MFP_CFG(GPIO47, AF1)
#define GPIO48_UART1_DTR_N	MFP_CFG(GPIO48, AF1)
#define GPIO49_UART1_RI		MFP_CFG(GPIO49, AF1)
#define GPIO50_UART1_DCD	MFP_CFG(GPIO50, AF1)
#define GPIO51_UART1_CTS	MFP_CFG(GPIO51, AF1)
#define GPIO52_UART1_RTS	MFP_CFG(GPIO52, AF1)
#define GPIO53_UART1_RXD	MFP_CFG(GPIO53, AF1)
#define GPIO54_UART1_TXD	MFP_CFG(GPIO54, AF1)

#define GPIO63_UART1_TXD	MFP_CFG(GPIO63, AF2)
#define GPIO64_UART1_RXD	MFP_CFG(GPIO64, AF2)
#define GPIO65_UART1_DSR	MFP_CFG(GPIO65, AF2)
#define GPIO66_UART1_DTR	MFP_CFG(GPIO66, AF2)
#define GPIO67_UART1_RI		MFP_CFG(GPIO67, AF2)
#define GPIO68_UART1_DCD	MFP_CFG(GPIO68, AF2)
#define GPIO69_UART1_CTS	MFP_CFG(GPIO69, AF2)
#define GPIO70_UART1_RTS	MFP_CFG(GPIO70, AF2)

/* UART2 - BTUART */
#define GPIO91_UART2_RXD	MFP_CFG(GPIO91, AF1)
#define GPIO92_UART2_TXD	MFP_CFG(GPIO92, AF1)
#define GPIO93_UART2_CTS	MFP_CFG(GPIO93, AF1)
#define GPIO94_UART2_RTS	MFP_CFG(GPIO94, AF1)

/* UART3 - STUART */
#define GPIO43_UART3_RTS	MFP_CFG(GPIO43, AF3)
#define GPIO44_UART3_CTS	MFP_CFG(GPIO44, AF3)
#define GPIO45_UART3_RXD	MFP_CFG(GPIO45, AF3)
#define GPIO46_UART3_TXD	MFP_CFG(GPIO46, AF3)

#define GPIO75_UART3_RTS	MFP_CFG(GPIO75, AF5)
#define GPIO76_UART3_CTS	MFP_CFG(GPIO76, AF5)
#define GPIO77_UART3_TXD	MFP_CFG(GPIO77, AF5)
#define GPIO78_UART3_RXD	MFP_CFG(GPIO78, AF5)

/* DFI */
#define DF_IO0_DF_IO0		MFP_CFG(DF_IO0, AF2)
#define DF_IO1_DF_IO1		MFP_CFG(DF_IO1, AF2)
#define DF_IO2_DF_IO2		MFP_CFG(DF_IO2, AF2)
#define DF_IO3_DF_IO3		MFP_CFG(DF_IO3, AF2)
#define DF_IO4_DF_IO4		MFP_CFG(DF_IO4, AF2)
#define DF_IO5_DF_IO5		MFP_CFG(DF_IO5, AF2)
#define DF_IO6_DF_IO6		MFP_CFG(DF_IO6, AF2)
#define DF_IO7_DF_IO7		MFP_CFG(DF_IO7, AF2)
#define DF_IO8_DF_IO8		MFP_CFG(DF_IO8, AF2)
#define DF_IO9_DF_IO9		MFP_CFG(DF_IO9, AF2)
#define DF_IO10_DF_IO10		MFP_CFG(DF_IO10, AF2)
#define DF_IO11_DF_IO11		MFP_CFG(DF_IO11, AF2)
#define DF_IO12_DF_IO12		MFP_CFG(DF_IO12, AF2)
#define DF_IO13_DF_IO13		MFP_CFG(DF_IO13, AF2)
#define DF_IO14_DF_IO14		MFP_CFG(DF_IO14, AF2)
#define DF_IO15_DF_IO15		MFP_CFG(DF_IO15, AF2)
#define DF_nADV1_ALE_DF_nADV1	MFP_CFG(DF_nADV1_ALE, AF2)
#define DF_nADV2_ALE_DF_nADV2	MFP_CFG(DF_nADV2_ALE, AF2)
#define DF_nCS0_DF_nCS0		MFP_CFG(DF_nCS0, AF2)
#define DF_nCS1_DF_nCS1		MFP_CFG(DF_nCS1, AF2)
#define DF_nRE_nOE_DF_nOE	MFP_CFG(DF_nRE_nOE, AF2)
#define DF_nWE_DF_nWE		MFP_CFG(DF_nWE, AF2)

/* DFI - NAND */
#define DF_CLE_nOE_ND_CLE	MFP_CFG_LPM(DF_CLE_nOE, AF1, PULL_HIGH)
#define DF_INT_RnB_ND_INT_RnB	MFP_CFG_LPM(DF_INT_RnB, AF1, PULL_LOW)
#define DF_IO0_ND_IO0		MFP_CFG_LPM(DF_IO0, AF1, PULL_LOW)
#define DF_IO1_ND_IO1		MFP_CFG_LPM(DF_IO1, AF1, PULL_LOW)
#define DF_IO2_ND_IO2		MFP_CFG_LPM(DF_IO2, AF1, PULL_LOW)
#define DF_IO3_ND_IO3		MFP_CFG_LPM(DF_IO3, AF1, PULL_LOW)
#define DF_IO4_ND_IO4		MFP_CFG_LPM(DF_IO4, AF1, PULL_LOW)
#define DF_IO5_ND_IO5		MFP_CFG_LPM(DF_IO5, AF1, PULL_LOW)
#define DF_IO6_ND_IO6		MFP_CFG_LPM(DF_IO6, AF1, PULL_LOW)
#define DF_IO7_ND_IO7		MFP_CFG_LPM(DF_IO7, AF1, PULL_LOW)
#define DF_IO8_ND_IO8		MFP_CFG_LPM(DF_IO8, AF1, PULL_LOW)
#define DF_IO9_ND_IO9		MFP_CFG_LPM(DF_IO9, AF1, PULL_LOW)
#define DF_IO10_ND_IO10		MFP_CFG_LPM(DF_IO10, AF1, PULL_LOW)
#define DF_IO11_ND_IO11		MFP_CFG_LPM(DF_IO11, AF1, PULL_LOW)
#define DF_IO12_ND_IO12		MFP_CFG_LPM(DF_IO12, AF1, PULL_LOW)
#define DF_IO13_ND_IO13		MFP_CFG_LPM(DF_IO13, AF1, PULL_LOW)
#define DF_IO14_ND_IO14		MFP_CFG_LPM(DF_IO14, AF1, PULL_LOW)
#define DF_IO15_ND_IO15		MFP_CFG_LPM(DF_IO15, AF1, PULL_LOW)
#define DF_nADV1_ALE_ND_ALE	MFP_CFG_LPM(DF_nADV1_ALE, AF1, PULL_HIGH)
#define DF_nADV2_ALE_ND_ALE	MFP_CFG_LPM(DF_nADV2_ALE, AF1, PULL_HIGH)
#define	DF_nADV2_ALE_nCS3	MFP_CFG_LPM(DF_nADV2_ALE, AF3, PULL_HIGH)
#define DF_nCS0_ND_nCS0		MFP_CFG_LPM(DF_nCS0, AF1, PULL_HIGH)
#define DF_nCS1_ND_nCS1		MFP_CFG_LPM(DF_nCS1, AF1, PULL_HIGH)
#define DF_nRE_nOE_ND_nRE	MFP_CFG_LPM(DF_nRE_nOE, AF1, PULL_HIGH)
#define DF_nWE_ND_nWE		MFP_CFG_LPM(DF_nWE, AF1, PULL_HIGH)

/* PWM */
#define GPIO41_PWM0		MFP_CFG_LPM(GPIO41, AF1, PULL_LOW)
#define GPIO42_PWM1		MFP_CFG_LPM(GPIO42, AF1, PULL_LOW)
#define GPIO43_PWM3		MFP_CFG_LPM(GPIO43, AF1, PULL_LOW)
#define GPIO20_PWM0		MFP_CFG_LPM(GPIO20, AF2, PULL_LOW)
#define GPIO21_PWM2		MFP_CFG_LPM(GPIO21, AF3, PULL_LOW)
#define GPIO22_PWM3		MFP_CFG_LPM(GPIO22, AF3, PULL_LOW)
#define GPIO32_PWM0		MFP_CFG_LPM(GPIO32, AF4, PULL_LOW)

/* CIR */
#define GPIO46_CIR_OUT		MFP_CFG(GPIO46, AF1)
#define GPIO77_CIR_OUT		MFP_CFG(GPIO77, AF3)

/* USB P2 */
#define GPIO0_USB_P2_7		MFP_CFG(GPIO0, AF3)
#define GPIO15_USB_P2_7		MFP_CFG(GPIO15, AF5)
#define GPIO16_USB_P2_7		MFP_CFG(GPIO16, AF2)
#define GPIO48_USB_P2_7		MFP_CFG(GPIO48, AF7)
#define GPIO49_USB_P2_7		MFP_CFG(GPIO49, AF6)
#define DF_IO9_USB_P2_7		MFP_CFG(DF_IO9, AF3)

#define GPIO48_USB_P2_8		MFP_CFG(GPIO48, AF2)
#define GPIO50_USB_P2_7		MFP_CFG_X(GPIO50, AF2, DS02X, FLOAT)
#define GPIO51_USB_P2_5		MFP_CFG(GPIO51, AF2)
#define GPIO47_USB_P2_4		MFP_CFG(GPIO47, AF2)
#define GPIO53_USB_P2_3		MFP_CFG(GPIO53, AF2)
#define GPIO54_USB_P2_6		MFP_CFG(GPIO54, AF2)
#define GPIO49_USB_P2_2		MFP_CFG(GPIO49, AF2)
#define GPIO52_USB_P2_1		MFP_CFG(GPIO52, AF2)

#define GPIO63_USB_P2_8		MFP_CFG(GPIO63, AF3)
#define GPIO64_USB_P2_7		MFP_CFG(GPIO64, AF3)
#define GPIO65_USB_P2_6		MFP_CFG(GPIO65, AF3)
#define GPIO66_USG_P2_5		MFP_CFG(GPIO66, AF3)
#define GPIO67_USB_P2_4		MFP_CFG(GPIO67, AF3)
#define GPIO68_USB_P2_3		MFP_CFG(GPIO68, AF3)
#define GPIO69_USB_P2_2		MFP_CFG(GPIO69, AF3)
#define GPIO70_USB_P2_1		MFP_CFG(GPIO70, AF3)

/* ULPI */
#define GPIO31_USB_ULPI_D0	MFP_CFG(GPIO31, AF4)
#define GPIO30_USB_ULPI_D1	MFP_CFG(GPIO30, AF7)
#define GPIO33_USB_ULPI_D2	MFP_CFG(GPIO33, AF5)
#define GPIO34_USB_ULPI_D3	MFP_CFG(GPIO34, AF5)
#define GPIO35_USB_ULPI_D4	MFP_CFG(GPIO35, AF5)
#define GPIO36_USB_ULPI_D5	MFP_CFG(GPIO36, AF5)
#define GPIO41_USB_ULPI_D6	MFP_CFG(GPIO41, AF5)
#define GPIO42_USB_ULPI_D7	MFP_CFG(GPIO42, AF5)
#define GPIO37_USB_ULPI_DIR	MFP_CFG(GPIO37, AF4)
#define GPIO38_USB_ULPI_CLK	MFP_CFG(GPIO38, AF4)
#define GPIO39_USB_ULPI_STP	MFP_CFG(GPIO39, AF4)
#define GPIO40_USB_ULPI_NXT	MFP_CFG(GPIO40, AF4)

#define GPIO3_CLK26MOUTDMD	MFP_CFG(GPIO3, AF3)
#define GPIO40_CLK26MOUTDMD	MFP_CFG(GPIO40, AF7)
#define GPIO94_CLK26MOUTDMD	MFP_CFG(GPIO94, AF5)
#define GPIO104_CLK26MOUTDMD	MFP_CFG(GPIO104, AF4)
#define DF_ADDR1_CLK26MOUTDMD	MFP_CFG(DF_ADDR2, AF3)
#define DF_ADDR3_CLK26MOUTDMD	MFP_CFG(DF_ADDR3, AF3)

#define GPIO14_CLK26MOUT	MFP_CFG(GPIO14, AF5)
#define GPIO38_CLK26MOUT	MFP_CFG(GPIO38, AF7)
#define GPIO92_CLK26MOUT	MFP_CFG(GPIO92, AF5)
#define GPIO105_CLK26MOUT	MFP_CFG(GPIO105, AF4)

#define GPIO2_CLK13MOUTDMD	MFP_CFG(GPIO2, AF3)
#define GPIO39_CLK13MOUTDMD	MFP_CFG(GPIO39, AF7)
#define GPIO50_CLK13MOUTDMD	MFP_CFG(GPIO50, AF3)
#define GPIO93_CLK13MOUTDMD	MFP_CFG(GPIO93, AF5)
#define GPIO103_CLK13MOUTDMD	MFP_CFG(GPIO103, AF4)
#define DF_ADDR2_CLK13MOUTDMD	MFP_CFG(DF_ADDR2, AF3)

/* 1 wire */
#define GPIO95_OW_DQ_IN		MFP_CFG(GPIO95, AF5)

#endif /* __ASM_ARCH_MFP_PXA9xx_H */
