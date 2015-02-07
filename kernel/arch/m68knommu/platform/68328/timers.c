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

/***************************************************************************/

/*
 *  linux/arch/m68knommu/platform/68328/timers.c
 *
 *  Copyright (C) 1993 Hamish Macdonald
 *  Copyright (C) 1999 D. Jeff Dionne
 *  Copyright (C) 2001 Georges Menie, Ken Desmet
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 */

/***************************************************************************/

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/clocksource.h>
#include <asm/setup.h>
#include <asm/system.h>
#include <asm/pgtable.h>
#include <asm/machdep.h>
#include <asm/MC68VZ328.h>

/***************************************************************************/

#if defined(CONFIG_DRAGEN2)
/* with a 33.16 MHz clock, this will give usec resolution to the time functions */
#define CLOCK_SOURCE	TCTL_CLKSOURCE_SYSCLK
#define CLOCK_PRE	7
#define TICKS_PER_JIFFY	41450

#elif defined(CONFIG_XCOPILOT_BUGS)
/*
 * The only thing I know is that CLK32 is not available on Xcopilot
 * I have little idea about what frequency SYSCLK has on Xcopilot.
 * The values for prescaler and compare registers were simply
 * taken from the original source
 */
#define CLOCK_SOURCE	TCTL_CLKSOURCE_SYSCLK
#define CLOCK_PRE	2
#define TICKS_PER_JIFFY	0xd7e4

#else
/* default to using the 32Khz clock */
#define CLOCK_SOURCE	TCTL_CLKSOURCE_32KHZ
#define CLOCK_PRE	31
#define TICKS_PER_JIFFY	10
#endif

static u32 m68328_tick_cnt;

/***************************************************************************/

static irqreturn_t hw_tick(int irq, void *dummy)
{
	/* Reset Timer1 */
	TSTAT &= 0;

	m68328_tick_cnt += TICKS_PER_JIFFY;
	return arch_timer_interrupt(irq, dummy);
}

/***************************************************************************/

static struct irqaction m68328_timer_irq = {
	.name	 = "timer",
	.flags	 = IRQF_DISABLED | IRQF_TIMER,
	.handler = hw_tick,
};

/***************************************************************************/

static cycle_t m68328_read_clk(void)
{
	unsigned long flags;
	u32 cycles;

	local_irq_save(flags);
	cycles = m68328_tick_cnt + TCN;
	local_irq_restore(flags);

	return cycles;
}

/***************************************************************************/

static struct clocksource m68328_clk = {
	.name	= "timer",
	.rating	= 250,
	.read	= m68328_read_clk,
	.shift	= 20,
	.mask	= CLOCKSOURCE_MASK(32),
	.flags	= CLOCK_SOURCE_IS_CONTINUOUS,
};

/***************************************************************************/

void hw_timer_init(void)
{
	/* disable timer 1 */
	TCTL = 0;

	/* set ISR */
	setup_irq(TMR_IRQ_NUM, &m68328_timer_irq);

	/* Restart mode, Enable int, Set clock source */
	TCTL = TCTL_OM | TCTL_IRQEN | CLOCK_SOURCE;
	TPRER = CLOCK_PRE;
	TCMP = TICKS_PER_JIFFY;

	/* Enable timer 1 */
	TCTL |= TCTL_TEN;
	m68328_clk.mult = clocksource_hz2mult(TICKS_PER_JIFFY*HZ, m68328_clk.shift);
	clocksource_register(&m68328_clk);
}

/***************************************************************************/

void m68328_timer_gettod(int *year, int *mon, int *day, int *hour, int *min, int *sec)
{
	long now = RTCTIME;

	*year = *mon = *day = 1;
	*hour = (now >> 24) % 24;
	*min = (now >> 16) % 60;
	*sec = now % 60;
}

/***************************************************************************/
