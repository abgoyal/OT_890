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
 * linux/arch/m68knommu/platform/68328/ints.c
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 *
 * Copyright 1996 Roman Zippel
 * Copyright 1999 D. Jeff Dionne <jeff@rt-control.com>
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/traps.h>
#include <asm/io.h>
#include <asm/machdep.h>

#if defined(CONFIG_M68328)
#include <asm/MC68328.h>
#elif defined(CONFIG_M68EZ328)
#include <asm/MC68EZ328.h>
#elif defined(CONFIG_M68VZ328)
#include <asm/MC68VZ328.h>
#endif

/* assembler routines */
asmlinkage void system_call(void);
asmlinkage void buserr(void);
asmlinkage void trap(void);
asmlinkage void trap3(void);
asmlinkage void trap4(void);
asmlinkage void trap5(void);
asmlinkage void trap6(void);
asmlinkage void trap7(void);
asmlinkage void trap8(void);
asmlinkage void trap9(void);
asmlinkage void trap10(void);
asmlinkage void trap11(void);
asmlinkage void trap12(void);
asmlinkage void trap13(void);
asmlinkage void trap14(void);
asmlinkage void trap15(void);
asmlinkage void trap33(void);
asmlinkage void trap34(void);
asmlinkage void trap35(void);
asmlinkage void trap36(void);
asmlinkage void trap37(void);
asmlinkage void trap38(void);
asmlinkage void trap39(void);
asmlinkage void trap40(void);
asmlinkage void trap41(void);
asmlinkage void trap42(void);
asmlinkage void trap43(void);
asmlinkage void trap44(void);
asmlinkage void trap45(void);
asmlinkage void trap46(void);
asmlinkage void trap47(void);
asmlinkage irqreturn_t bad_interrupt(int, void *);
asmlinkage irqreturn_t inthandler(void);
asmlinkage irqreturn_t inthandler1(void);
asmlinkage irqreturn_t inthandler2(void);
asmlinkage irqreturn_t inthandler3(void);
asmlinkage irqreturn_t inthandler4(void);
asmlinkage irqreturn_t inthandler5(void);
asmlinkage irqreturn_t inthandler6(void);
asmlinkage irqreturn_t inthandler7(void);

extern e_vector *_ramvec;

/* The number of spurious interrupts */
volatile unsigned int num_spurious;

/*
 * This function should be called during kernel startup to initialize
 * the machine vector table.
 */
void __init init_vectors(void)
{
	int i;

	/* set up the vectors */
	for (i = 72; i < 256; ++i)
		_ramvec[i] = (e_vector) bad_interrupt;

	_ramvec[32] = system_call;

	_ramvec[65] = (e_vector) inthandler1;
	_ramvec[66] = (e_vector) inthandler2;
	_ramvec[67] = (e_vector) inthandler3;
	_ramvec[68] = (e_vector) inthandler4;
	_ramvec[69] = (e_vector) inthandler5;
	_ramvec[70] = (e_vector) inthandler6;
	_ramvec[71] = (e_vector) inthandler7;
 
	IVR = 0x40; /* Set DragonBall IVR (interrupt base) to 64 */

	/* turn off all interrupts */
	IMR = ~0;
}

/* The 68k family did not have a good way to determine the source
 * of interrupts until later in the family.  The EC000 core does
 * not provide the vector number on the stack, we vector everything
 * into one vector and look in the blasted mask register...
 * This code is designed to be fast, almost constant time, not clean!
 */
void process_int(int vec, struct pt_regs *fp)
{
	int irq;
	int mask;

	unsigned long pend = ISR;

	while (pend) {
		if (pend & 0x0000ffff) {
			if (pend & 0x000000ff) {
				if (pend & 0x0000000f) {
					mask = 0x00000001;
					irq = 0;
				} else {
					mask = 0x00000010;
					irq = 4;
				}
			} else {
				if (pend & 0x00000f00) {
					mask = 0x00000100;
					irq = 8;
				} else {
					mask = 0x00001000;
					irq = 12;
				}
			}
		} else {
			if (pend & 0x00ff0000) {
				if (pend & 0x000f0000) {
					mask = 0x00010000;
					irq = 16;
				} else {
					mask = 0x00100000;
					irq = 20;
				}
			} else {
				if (pend & 0x0f000000) {
					mask = 0x01000000;
					irq = 24;
				} else {
					mask = 0x10000000;
					irq = 28;
				}
			}
		}

		while (! (mask & pend)) {
			mask <<=1;
			irq++;
		}

		do_IRQ(irq, fp);
		pend &= ~mask;
	}
}

void enable_vector(unsigned int irq)
{
	IMR &= ~(1<<irq);
}

void disable_vector(unsigned int irq)
{
	IMR |= (1<<irq);
}

void ack_vector(unsigned int irq)
{
	/* Nothing needed */
}

