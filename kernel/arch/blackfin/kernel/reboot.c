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
 * arch/blackfin/kernel/reboot.c - handle shutdown/reboot
 *
 * Copyright 2004-2007 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <linux/interrupt.h>
#include <asm/bfin-global.h>
#include <asm/reboot.h>
#include <asm/system.h>
#include <asm/bfrom.h>

/* A system soft reset makes external memory unusable so force
 * this function into L1.  We use the compiler ssync here rather
 * than SSYNC() because it's safe (no interrupts and such) and
 * we save some L1.  We do not need to force sanity in the SYSCR
 * register as the BMODE selection bit is cleared by the soft
 * reset while the Core B bit (on dual core parts) is cleared by
 * the core reset.
 */
__attribute__ ((__l1_text__, __noreturn__))
static void bfin_reset(void)
{
	/* Wait for completion of "system" events such as cache line
	 * line fills so that we avoid infinite stalls later on as
	 * much as possible.  This code is in L1, so it won't trigger
	 * any such event after this point in time.
	 */
	__builtin_bfin_ssync();

	/* The bootrom checks to see how it was reset and will
	 * automatically perform a software reset for us when
	 * it starts executing after the core reset.
	 */
	if (ANOMALY_05000353 || ANOMALY_05000386) {
		/* Initiate System software reset. */
		bfin_write_SWRST(0x7);

		/* Due to the way reset is handled in the hardware, we need
		 * to delay for 10 SCLKS.  The only reliable way to do this is
		 * to calculate the CCLK/SCLK ratio and multiply 10.  For now,
		 * we'll assume worse case which is a 1:15 ratio.
		 */
		asm(
			"LSETUP (1f, 1f) LC0 = %0\n"
			"1: nop;"
			:
			: "a" (15 * 10)
			: "LC0", "LB0", "LT0"
		);

		/* Clear System software reset */
		bfin_write_SWRST(0);

		/* The BF526 ROM will crash during reset */
#if defined(__ADSPBF522__) || defined(__ADSPBF524__) || defined(__ADSPBF526__)
		bfin_read_SWRST();
#endif

		/* Wait for the SWRST write to complete.  Cannot rely on SSYNC
		 * though as the System state is all reset now.
		 */
		asm(
			"LSETUP (1f, 1f) LC1 = %0\n"
			"1: nop;"
			:
			: "a" (15 * 1)
			: "LC1", "LB1", "LT1"
		);
	}

	while (1)
		/* Issue core reset */
		asm("raise 1");
}

__attribute__((weak))
void native_machine_restart(char *cmd)
{
}

void machine_restart(char *cmd)
{
	native_machine_restart(cmd);
	local_irq_disable();
	if (smp_processor_id())
		smp_call_function((void *)bfin_reset, 0, 1);
	else
		bfin_reset();
}

__attribute__((weak))
void native_machine_halt(void)
{
	idle_with_irq_disabled();
}

void machine_halt(void)
{
	native_machine_halt();
}

__attribute__((weak))
void native_machine_power_off(void)
{
	idle_with_irq_disabled();
}

void machine_power_off(void)
{
	native_machine_power_off();
}
