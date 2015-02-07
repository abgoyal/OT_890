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

/* arch/arm/mach-s3c2410/include/mach/regs-rtc.h
 *
 * Copyright (c) 2003 Simtec Electronics <linux@simtec.co.uk>
 *		      http://www.simtec.co.uk/products/SWLINUX/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * S3C2410 Internal RTC register definition
*/

#ifndef __ASM_ARCH_REGS_RTC_H
#define __ASM_ARCH_REGS_RTC_H __FILE__

#define S3C2410_RTCREG(x) (x)

#define S3C2410_RTCCON	      S3C2410_RTCREG(0x40)
#define S3C2410_RTCCON_RTCEN  (1<<0)
#define S3C2410_RTCCON_CLKSEL (1<<1)
#define S3C2410_RTCCON_CNTSEL (1<<2)
#define S3C2410_RTCCON_CLKRST (1<<3)

#define S3C2410_TICNT	      S3C2410_RTCREG(0x44)
#define S3C2410_TICNT_ENABLE  (1<<7)

#define S3C2410_RTCALM	      S3C2410_RTCREG(0x50)
#define S3C2410_RTCALM_ALMEN  (1<<6)
#define S3C2410_RTCALM_YEAREN (1<<5)
#define S3C2410_RTCALM_MONEN  (1<<4)
#define S3C2410_RTCALM_DAYEN  (1<<3)
#define S3C2410_RTCALM_HOUREN (1<<2)
#define S3C2410_RTCALM_MINEN  (1<<1)
#define S3C2410_RTCALM_SECEN  (1<<0)

#define S3C2410_RTCALM_ALL \
  S3C2410_RTCALM_ALMEN | S3C2410_RTCALM_YEAREN | S3C2410_RTCALM_MONEN |\
  S3C2410_RTCALM_DAYEN | S3C2410_RTCALM_HOUREN | S3C2410_RTCALM_MINEN |\
  S3C2410_RTCALM_SECEN


#define S3C2410_ALMSEC	      S3C2410_RTCREG(0x54)
#define S3C2410_ALMMIN	      S3C2410_RTCREG(0x58)
#define S3C2410_ALMHOUR	      S3C2410_RTCREG(0x5c)

#define S3C2410_ALMDATE	      S3C2410_RTCREG(0x60)
#define S3C2410_ALMMON	      S3C2410_RTCREG(0x64)
#define S3C2410_ALMYEAR	      S3C2410_RTCREG(0x68)

#define S3C2410_RTCRST	      S3C2410_RTCREG(0x6c)

#define S3C2410_RTCSEC	      S3C2410_RTCREG(0x70)
#define S3C2410_RTCMIN	      S3C2410_RTCREG(0x74)
#define S3C2410_RTCHOUR	      S3C2410_RTCREG(0x78)
#define S3C2410_RTCDATE	      S3C2410_RTCREG(0x7c)
#define S3C2410_RTCDAY	      S3C2410_RTCREG(0x80)
#define S3C2410_RTCMON	      S3C2410_RTCREG(0x84)
#define S3C2410_RTCYEAR	      S3C2410_RTCREG(0x88)


#endif /* __ASM_ARCH_REGS_RTC_H */
