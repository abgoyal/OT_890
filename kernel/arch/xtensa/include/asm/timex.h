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
 * include/asm-xtensa/timex.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2001 - 2005 Tensilica Inc.
 */

#ifndef _XTENSA_TIMEX_H
#define _XTENSA_TIMEX_H

#ifdef __KERNEL__

#include <asm/processor.h>
#include <linux/stringify.h>

#define _INTLEVEL(x)	XCHAL_INT ## x ## _LEVEL
#define INTLEVEL(x)	_INTLEVEL(x)

#if INTLEVEL(XCHAL_TIMER0_INTERRUPT) == 1
# define LINUX_TIMER     0
# define LINUX_TIMER_INT XCHAL_TIMER0_INTERRUPT
#elif INTLEVEL(XCHAL_TIMER1_INTERRUPT) == 1
# define LINUX_TIMER     1
# define LINUX_TIMER_INT XCHAL_TIMER1_INTERRUPT
#elif INTLEVEL(XCHAL_TIMER2_INTERRUPT) == 1
# define LINUX_TIMER     2
# define LINUX_TIMER_INT XCHAL_TIMER2_INTERRUPT
#else
# error "Bad timer number for Linux configurations!"
#endif

#define LINUX_TIMER_MASK        (1L << LINUX_TIMER_INT)

#define CLOCK_TICK_RATE 	1193180	/* (everyone is using this value) */
#define CLOCK_TICK_FACTOR       20 /* Factor of both 10^6 and CLOCK_TICK_RATE */

#ifdef CONFIG_XTENSA_CALIBRATE_CCOUNT
extern unsigned long ccount_per_jiffy;
extern unsigned long ccount_nsec;
#define CCOUNT_PER_JIFFY ccount_per_jiffy
#define NSEC_PER_CCOUNT  ccount_nsec
#else
#define CCOUNT_PER_JIFFY (CONFIG_XTENSA_CPU_CLOCK*(1000000UL/HZ))
#define NSEC_PER_CCOUNT (1000UL / CONFIG_XTENSA_CPU_CLOCK)
#endif


typedef unsigned long long cycles_t;

/*
 * Only used for SMP.
 */

extern cycles_t cacheflush_time;

#define get_cycles()	(0)


/*
 * Register access.
 */

#define WSR_CCOUNT(r)	  asm volatile ("wsr %0,"__stringify(CCOUNT) :: "a" (r))
#define RSR_CCOUNT(r)	  asm volatile ("rsr %0,"__stringify(CCOUNT) : "=a" (r))
#define WSR_CCOMPARE(x,r) asm volatile ("wsr %0,"__stringify(CCOMPARE)"+"__stringify(x) :: "a"(r))
#define RSR_CCOMPARE(x,r) asm volatile ("rsr %0,"__stringify(CCOMPARE)"+"__stringify(x) : "=a"(r))

static inline unsigned long get_ccount (void)
{
	unsigned long ccount;
	RSR_CCOUNT(ccount);
	return ccount;
}

static inline void set_ccount (unsigned long ccount)
{
	WSR_CCOUNT(ccount);
}

static inline unsigned long get_linux_timer (void)
{
	unsigned ccompare;
	RSR_CCOMPARE(LINUX_TIMER, ccompare);
	return ccompare;
}

static inline void set_linux_timer (unsigned long ccompare)
{
	WSR_CCOMPARE(LINUX_TIMER, ccompare);
}

#endif	/* __KERNEL__ */
#endif	/* _XTENSA_TIMEX_H */
