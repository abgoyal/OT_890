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
 * arch/mips/dec/int-handler.S
 *
 * Copyright (C) 1995, 1996, 1997 Paul M. Antoine and Harald Koerfgen
 * Copyright (C) 2000, 2001, 2002, 2003, 2005  Maciej W. Rozycki
 *
 * Written by Ralf Baechle and Andreas Busse, modified for DECstation
 * support by Paul Antoine and Harald Koerfgen.
 *
 * completly rewritten:
 * Copyright (C) 1998 Harald Koerfgen
 *
 * Rewritten extensively for controller-driven IRQ support
 * by Maciej W. Rozycki.
 */

#include <asm/addrspace.h>
#include <asm/asm.h>
#include <asm/mipsregs.h>
#include <asm/regdef.h>
#include <asm/stackframe.h>

#include <asm/dec/interrupts.h>
#include <asm/dec/ioasic_addrs.h>
#include <asm/dec/ioasic_ints.h>
#include <asm/dec/kn01.h>
#include <asm/dec/kn02.h>
#include <asm/dec/kn02xa.h>
#include <asm/dec/kn03.h>

#define KN02_CSR_BASE		CKSEG1ADDR(KN02_SLOT_BASE + KN02_CSR)
#define KN02XA_IOASIC_BASE	CKSEG1ADDR(KN02XA_SLOT_BASE + IOASIC_IOCTL)
#define KN03_IOASIC_BASE	CKSEG1ADDR(KN03_SLOT_BASE + IOASIC_IOCTL)

		.text
		.set	noreorder
/*
 * plat_irq_dispatch: Interrupt handler for DECstations
 *
 * We follow the model in the Indy interrupt code by David Miller, where he
 * says: a lot of complication here is taken away because:
 *
 * 1) We handle one interrupt and return, sitting in a loop
 *    and moving across all the pending IRQ bits in the cause
 *    register is _NOT_ the answer, the common case is one
 *    pending IRQ so optimize in that direction.
 *
 * 2) We need not check against bits in the status register
 *    IRQ mask, that would make this routine slow as hell.
 *
 * 3) Linux only thinks in terms of all IRQs on or all IRQs
 *    off, nothing in between like BSD spl() brain-damage.
 *
 * Furthermore, the IRQs on the DECstations look basically (barring
 * software IRQs which we don't use at all) like...
 *
 * DS2100/3100's, aka kn01, aka Pmax:
 *
 *	MIPS IRQ	Source
 *      --------        ------
 *             0	Software (ignored)
 *             1        Software (ignored)
 *             2        SCSI
 *             3        Lance Ethernet
 *             4        DZ11 serial
 *             5        RTC
 *             6        Memory Controller & Video
 *             7        FPU
 *
 * DS5000/200, aka kn02, aka 3max:
 *
 *	MIPS IRQ	Source
 *      --------        ------
 *             0	Software (ignored)
 *             1        Software (ignored)
 *             2        TurboChannel
 *             3        RTC
 *             4        Reserved
 *             5        Memory Controller
 *             6        Reserved
 *             7        FPU
 *
 * DS5000/1xx's, aka kn02ba, aka 3min:
 *
 *	MIPS IRQ	Source
 *      --------        ------
 *             0	Software (ignored)
 *             1        Software (ignored)
 *             2        TurboChannel Slot 0
 *             3        TurboChannel Slot 1
 *             4        TurboChannel Slot 2
 *             5        TurboChannel Slot 3 (ASIC)
 *             6        Halt button
 *             7        FPU/R4k timer
 *
 * DS5000/2x's, aka kn02ca, aka maxine:
 *
 *	MIPS IRQ	Source
 *      --------        ------
 *             0	Software (ignored)
 *             1        Software (ignored)
 *             2        Periodic Interrupt (100usec)
 *             3        RTC
 *             4        I/O write timeout
 *             5        TurboChannel (ASIC)
 *             6        Halt Keycode from Access.Bus keyboard (CTRL-ALT-ENTER)
 *             7        FPU/R4k timer
 *
 * DS5000/2xx's, aka kn03, aka 3maxplus:
 *
 *	MIPS IRQ	Source
 *      --------        ------
 *             0	Software (ignored)
 *             1        Software (ignored)
 *             2        System Board (ASIC)
 *             3        RTC
 *             4        Reserved
 *             5        Memory
 *             6        Halt Button
 *             7        FPU/R4k timer
 *
 * We handle the IRQ according to _our_ priority (see setup.c),
 * then we just return.  If multiple IRQs are pending then we will
 * just take another exception, big deal.
 */
		.align	5
		NESTED(plat_irq_dispatch, PT_SIZE, ra)
		.set	noreorder

		/*
		 * Get pending Interrupts
		 */
		mfc0	t0,CP0_CAUSE		# get pending interrupts
		mfc0	t1,CP0_STATUS
#ifdef CONFIG_32BIT
		lw	t2,cpu_fpu_mask
#endif
		andi	t0,ST0_IM		# CAUSE.CE may be non-zero!
		and	t0,t1			# isolate allowed ones

		beqz	t0,spurious

#ifdef CONFIG_32BIT
		 and	t2,t0
		bnez	t2,fpu			# handle FPU immediately
#endif

		/*
		 * Find irq with highest priority
		 */
		 PTR_LA	t1,cpu_mask_nr_tbl
1:		lw	t2,(t1)
		nop
		and	t2,t0
		beqz	t2,1b
		 addu	t1,2*PTRSIZE		# delay slot

		/*
		 * Do the low-level stuff
		 */
		lw	a0,(-PTRSIZE)(t1)
		nop
		bgez	a0,handle_it		# irq_nr >= 0?
						# irq_nr < 0: it is an address
		 nop
		jr	a0
						# a trick to save a branch:
		 lui	t2,(KN03_IOASIC_BASE>>16)&0xffff
						# upper part of IOASIC Address

/*
 * Handle "IRQ Controller" Interrupts
 * Masked Interrupts are still visible and have to be masked "by hand".
 */
		FEXPORT(kn02_io_int)		# 3max
		lui	t0,(KN02_CSR_BASE>>16)&0xffff
						# get interrupt status and mask
		lw	t0,(t0)
		nop
		andi	t1,t0,KN02_IRQ_ALL
		b	1f
		 srl	t0,16			# shift interrupt mask

		FEXPORT(kn02xa_io_int)		# 3min/maxine
		lui	t2,(KN02XA_IOASIC_BASE>>16)&0xffff
						# upper part of IOASIC Address

		FEXPORT(kn03_io_int)		# 3max+ (t2 loaded earlier)
		lw	t0,IO_REG_SIR(t2)	# get status: IOASIC sir
		lw	t1,IO_REG_SIMR(t2)	# get mask:   IOASIC simr
		nop

1:		and	t0,t1			# mask out allowed ones

		beqz	t0,spurious

		/*
		 * Find irq with highest priority
		 */
		 PTR_LA	t1,asic_mask_nr_tbl
2:		lw	t2,(t1)
		nop
		and	t2,t0
		beq	zero,t2,2b
		 addu	t1,2*PTRSIZE		# delay slot

		/*
		 * Do the low-level stuff
		 */
		lw	a0,%lo(-PTRSIZE)(t1)
		nop
		bgez	a0,handle_it		# irq_nr >= 0?
						# irq_nr < 0: it is an address
		 nop
		jr	a0
		 nop				# delay slot

/*
 * Dispatch low-priority interrupts.  We reconsider all status
 * bits again, which looks like a lose, but it makes the code
 * simple and O(log n), so it gets compensated.
 */
		FEXPORT(cpu_all_int)		# HALT, timers, software junk
		li	a0,DEC_CPU_IRQ_BASE
		srl	t0,CAUSEB_IP
		li	t1,CAUSEF_IP>>CAUSEB_IP	# mask
		b	1f
		 li	t2,4			# nr of bits / 2

		FEXPORT(kn02_all_int)		# impossible ?
		li	a0,KN02_IRQ_BASE
		li	t1,KN02_IRQ_ALL		# mask
		b	1f
		 li	t2,4			# nr of bits / 2

		FEXPORT(asic_all_int)		# various I/O ASIC junk
		li	a0,IO_IRQ_BASE
		li	t1,IO_IRQ_ALL		# mask
		b	1f
		 li	t2,8			# nr of bits / 2

/*
 * Dispatch DMA interrupts -- O(log n).
 */
		FEXPORT(asic_dma_int)		# I/O ASIC DMA events
		li	a0,IO_IRQ_BASE+IO_INR_DMA
		srl	t0,IO_INR_DMA
		li	t1,IO_IRQ_DMA>>IO_INR_DMA # mask
		li	t2,8			# nr of bits / 2

		/*
		 * Find irq with highest priority.
		 * Highest irq number takes precedence.
		 */
1:		srlv	t3,t1,t2
2:		xor	t1,t3
		and	t3,t0,t1
		beqz	t3,3f
		 nop
		move	t0,t3
		addu	a0,t2
3:		srl	t2,1
		bnez	t2,2b
		 srlv	t3,t1,t2

handle_it:
		j	dec_irq_dispatch
		 nop

#ifdef CONFIG_32BIT
fpu:
		j	handle_fpe_int
		 nop
#endif

spurious:
		j	spurious_interrupt
		 nop
		END(plat_irq_dispatch)

/*
 * Generic unimplemented interrupt routines -- cpu_mask_nr_tbl
 * and asic_mask_nr_tbl are initialized to point all interrupts here.
 * The tables are then filled in by machine-specific initialisation
 * in dec_setup().
 */
		FEXPORT(dec_intr_unimplemented)
		move	a1,t0			# cheats way of printing an arg!
		PANIC("Unimplemented cpu interrupt! CP0_CAUSE: 0x%08x");

		FEXPORT(asic_intr_unimplemented)
		move	a1,t0			# cheats way of printing an arg!
		PANIC("Unimplemented asic interrupt! ASIC ISR: 0x%08x");
