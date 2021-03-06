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
 *  linux/include/asm-arm/arch-mt3351/irqs.h
 *
 *  Copyright (C) 2006 MediaTek Inc.
 */

#if !defined(IRQS_DRIVER_H)
#define IRQS_DRIVER_H



/*
 * CIRQ Line assigned to interrupt sources. Report to driver programmer.
 */
#define MT3351_GPI_FIQ_IRQ_LINE        0
#define MT3351_DMA_IRQ_LINE            1
#define MT3351_UART1_IRQ_LINE          2
#define MT3351_UART2_IRQ_LINE          3
#define MT3351_UART3_IRQ_LINE          4
#define MT3351_UART4_IRQ_LINE          5
#define MT3351_UART5_IRQ_LINE          6
#define MT3351_EIT_IRQ_LINE            7
#define MT3351_USB_IRQ_LINE            8
#define MT3351_USB1_IRQ_LINE           9
#define MT3351_RTC_IRQ_LINE            10
#define MT3351_MSDC_IRQ_LINE           11
#define MT3351_MSDC2_IRQ_LINE          12
#define MT3351_MSDC3_IRQ_LINE          13
#define MT3351_MSDC_EVENT_IRQ_LINE     14
#define MT3351_MSDC2_EVENT_IRQ_LINE    15
#define MT3351_MSDC3_EVENT_IRQ_LINE    16
#define MT3351_LCD_IRQ_LINE            17
#define MT3351_GPI_IRQ_LINE            18
#define MT3351_WDT_IRQ_LINE            19
#define MT3351_NFI_IRQ_LINE            20
#define MT3351_SPI_IRQ_LINE            21
#define MT3351_IMGDMA_IRQ_LINE         22
#define MT3351_ECC_IRQ_LINE		       23
#define MT3351_I2C_IRQ_LINE            24
#define MT3351_G2D_IRQ_LINE            25
#define MT3351_IMGPROC_IRQ_LINE        26
#define MT3351_CAMERA_IRQ_LINE         27 
#define MT3351_JPEG_DEC_IRQ_LINE       28
#define MT3351_JPEG_ENC_IRQ_LINE       29
#define MT3351_CRZ_IRQ_LINE            30
#define MT3351_DRZ_IRQ_LINE            31
#define MT3351_PRZ_IRQ_LINE            32
#define MT3351_PWM_IRQ_LINE            33
#define MT3351_DPI_IRQ_LINE            34
#define MT3351_DSP2MCU_IRQ_LINE        35
#define MT3351_EMI_IRQ_LINE            36
#define MT3351_SLEEPCTRL_IRQ_LINE      37
#define MT3351_KPAD_IRQ_LINE           38
#define MT3351_GPT_IRQ_LINE            39
#define MT3351_IFLT_IRQ_LINE           40
#define MT3351_IrDA_IRQ_LINE           41
#define MT3351_MTVSPI_IRQ_LINE         42
#define MT3351_DVFC_IRQ_LINE           43
#define MT3351_CHR_DET_IRQ_LINE        44
#define MT3351_BLS_IRQ_LINE		       45		//backlight
#define MT3351_LOW_BAT_IRQ_LINE        46		//Low voltage battery
#define MT3351_TS_IRQ_LINE        	   47		//touch screen
#define MT3351_48_IRQ_LINE             48
#define MT3351_49_IRQ_LINE             49
#define MT3351_50_IRQ_LINE             50
#define MT3351_51_IRQ_LINE             51
#define MT3351_52_IRQ_LINE             52
#define MT3351_53_IRQ_LINE             53
#define MT3351_VFE_IRQ_LINE            54
#define MT3351_ASM_IRQ_LINE            55
//Myron, not the final value
#define MT3351_56_IRQ_LINE             56 
#define MT3351_57_IRQ_LINE             57
#define MT3351_58_IRQ_LINE             58
#define MT3351_59_IRQ_LINE             59
#define MT3351_60_IRQ_LINE             60
#define MT3351_61_IRQ_LINE             61
#define MT3351_62_IRQ_LINE             62
#define MT3351_63_IRQ_LINE             63
#define MT3351_NUM_IRQ_LINE            64


#endif /*IRQS_H*/
