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
 * mxc_timer.h
 *
 * Copyright (C) 2008 Juergen Beisert (kernel@pengutronix.de)
 *
 * Platform independent (i.MX1, i.MX2, i.MX3) definition for timer handling.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#ifndef __PLAT_MXC_TIMER_H
#define __PLAT_MXC_TIMER_H

#include <linux/clk.h>
#include <mach/hardware.h>

#ifdef CONFIG_ARCH_MX1
#define TIMER_BASE		IO_ADDRESS(TIM1_BASE_ADDR)
#define TIMER_INTERRUPT		TIM1_INT

#define TCTL_VAL		TCTL_CLK_PCLK1
#define TCTL_IRQEN		(1<<4)
#define TCTL_FRR		(1<<8)
#define TCTL_CLK_PCLK1		(1<<1)
#define TCTL_CLK_PCLK1_4	(2<<1)
#define TCTL_CLK_TIN		(3<<1)
#define TCTL_CLK_32		(4<<1)

#define MXC_TCTL   0x00
#define MXC_TPRER  0x04
#define MXC_TCMP   0x08
#define MXC_TCR    0x0c
#define MXC_TCN    0x10
#define MXC_TSTAT  0x14
#define TSTAT_CAPT		(1<<1)
#define TSTAT_COMP		(1<<0)

static inline void gpt_irq_disable(void)
{
	unsigned int tmp;

	tmp = __raw_readl(TIMER_BASE + MXC_TCTL);
	__raw_writel(tmp & ~TCTL_IRQEN, TIMER_BASE + MXC_TCTL);
}

static inline void gpt_irq_enable(void)
{
	__raw_writel(__raw_readl(TIMER_BASE + MXC_TCTL) | TCTL_IRQEN,
				TIMER_BASE + MXC_TCTL);
}

static void gpt_irq_acknowledge(void)
{
	__raw_writel(0, TIMER_BASE + MXC_TSTAT);
}
#endif /* CONFIG_ARCH_MX1 */

#ifdef CONFIG_ARCH_MX2
#define TIMER_BASE		IO_ADDRESS(GPT1_BASE_ADDR)
#define TIMER_INTERRUPT		MXC_INT_GPT1

#define MXC_TCTL   0x00
#define TCTL_VAL		TCTL_CLK_PCLK1
#define TCTL_CLK_PCLK1		(1<<1)
#define TCTL_CLK_PCLK1_4	(2<<1)
#define TCTL_IRQEN		(1<<4)
#define TCTL_FRR		(1<<8)
#define MXC_TPRER  0x04
#define MXC_TCMP   0x08
#define MXC_TCR    0x0c
#define MXC_TCN    0x10
#define MXC_TSTAT  0x14
#define TSTAT_CAPT		(1<<1)
#define TSTAT_COMP		(1<<0)

static inline void gpt_irq_disable(void)
{
	unsigned int tmp;

	tmp = __raw_readl(TIMER_BASE + MXC_TCTL);
	__raw_writel(tmp & ~TCTL_IRQEN, TIMER_BASE + MXC_TCTL);
}

static inline void gpt_irq_enable(void)
{
	__raw_writel(__raw_readl(TIMER_BASE + MXC_TCTL) | TCTL_IRQEN,
				TIMER_BASE + MXC_TCTL);
}

static void gpt_irq_acknowledge(void)
{
	__raw_writel(TSTAT_CAPT | TSTAT_COMP, TIMER_BASE + MXC_TSTAT);
}
#endif /* CONFIG_ARCH_MX2 */

#ifdef CONFIG_ARCH_MX3
#define TIMER_BASE		IO_ADDRESS(GPT1_BASE_ADDR)
#define TIMER_INTERRUPT		MXC_INT_GPT

#define MXC_TCTL   0x00
#define TCTL_VAL		(TCTL_CLK_IPG | TCTL_WAITEN)
#define TCTL_CLK_IPG		(1<<6)
#define TCTL_FRR		(1<<9)
#define TCTL_WAITEN		(1<<3)

#define MXC_TPRER  0x04
#define MXC_TSTAT  0x08
#define TSTAT_OF1		(1<<0)
#define TSTAT_OF2		(1<<1)
#define TSTAT_OF3		(1<<2)
#define TSTAT_IF1		(1<<3)
#define TSTAT_IF2		(1<<4)
#define TSTAT_ROV		(1<<5)
#define MXC_IR     0x0c
#define MXC_TCMP   0x10
#define MXC_TCMP2  0x14
#define MXC_TCMP3  0x18
#define MXC_TCR    0x1c
#define MXC_TCN    0x24

static inline void gpt_irq_disable(void)
{
	__raw_writel(0, TIMER_BASE + MXC_IR);
}

static inline void gpt_irq_enable(void)
{
	__raw_writel(1<<0, TIMER_BASE + MXC_IR);
}

static inline void gpt_irq_acknowledge(void)
{
	__raw_writel(TSTAT_OF1, TIMER_BASE + MXC_TSTAT);
}
#endif /* CONFIG_ARCH_MX3 */

#define TCTL_SWR		(1<<15)
#define TCTL_CC			(1<<10)
#define TCTL_OM			(1<<9)
#define TCTL_CAP_RIS		(1<<6)
#define TCTL_CAP_FAL		(2<<6)
#define TCTL_CAP_RIS_FAL	(3<<6)
#define TCTL_CAP_ENA		(1<<5)
#define TCTL_TEN		(1<<0)

#endif
