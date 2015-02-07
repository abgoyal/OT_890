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
 * arch/arm/mach-iop32x/include/mach/irqs.h
 *
 * Author:	Rory Bolt <rorybolt@pacbell.net>
 * Copyright:	(C) 2002 Rory Bolt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __IRQS_H
#define __IRQS_H

/*
 * IOP80321 chipset interrupts
 */
#define IRQ_IOP32X_DMA0_EOT	0
#define IRQ_IOP32X_DMA0_EOC	1
#define IRQ_IOP32X_DMA1_EOT	2
#define IRQ_IOP32X_DMA1_EOC	3
#define IRQ_IOP32X_AA_EOT	6
#define IRQ_IOP32X_AA_EOC	7
#define IRQ_IOP32X_CORE_PMON	8
#define IRQ_IOP32X_TIMER0	9
#define IRQ_IOP32X_TIMER1	10
#define IRQ_IOP32X_I2C_0	11
#define IRQ_IOP32X_I2C_1	12
#define IRQ_IOP32X_MESSAGING	13
#define IRQ_IOP32X_ATU_BIST	14
#define IRQ_IOP32X_PERFMON	15
#define IRQ_IOP32X_CORE_PMU	16
#define IRQ_IOP32X_BIU_ERR	17
#define IRQ_IOP32X_ATU_ERR	18
#define IRQ_IOP32X_MCU_ERR	19
#define IRQ_IOP32X_DMA0_ERR	20
#define IRQ_IOP32X_DMA1_ERR	21
#define IRQ_IOP32X_AA_ERR	23
#define IRQ_IOP32X_MSG_ERR	24
#define IRQ_IOP32X_SSP		25
#define IRQ_IOP32X_XINT0	27
#define IRQ_IOP32X_XINT1	28
#define IRQ_IOP32X_XINT2	29
#define IRQ_IOP32X_XINT3	30
#define IRQ_IOP32X_HPI		31

#define NR_IRQS			32


#endif
