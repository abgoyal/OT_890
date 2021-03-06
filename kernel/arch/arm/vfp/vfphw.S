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
 *  linux/arch/arm/vfp/vfphw.S
 *
 *  Copyright (C) 2004 ARM Limited.
 *  Written by Deep Blue Solutions Limited.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This code is called from the kernel's undefined instruction trap.
 * r9 holds the return address for successful handling.
 * lr holds the return address for unrecognised instructions.
 * r10 points at the start of the private FP workspace in the thread structure
 * sp points to a struct pt_regs (as defined in include/asm/proc/ptrace.h)
 */
#include <asm/thread_info.h>
#include <asm/vfpmacros.h>
#include "../kernel/entry-header.S"

	.macro	DBGSTR, str
#ifdef DEBUG
	stmfd	sp!, {r0-r3, ip, lr}
	add	r0, pc, #4
	bl	printk
	b	1f
	.asciz  "<7>VFP: \str\n"
	.balign 4
1:	ldmfd	sp!, {r0-r3, ip, lr}
#endif
	.endm

	.macro  DBGSTR1, str, arg
#ifdef DEBUG
	stmfd	sp!, {r0-r3, ip, lr}
	mov	r1, \arg
	add	r0, pc, #4
	bl	printk
	b	1f
	.asciz  "<7>VFP: \str\n"
	.balign 4
1:	ldmfd	sp!, {r0-r3, ip, lr}
#endif
	.endm

	.macro  DBGSTR3, str, arg1, arg2, arg3
#ifdef DEBUG
	stmfd	sp!, {r0-r3, ip, lr}
	mov	r3, \arg3
	mov	r2, \arg2
	mov	r1, \arg1
	add	r0, pc, #4
	bl	printk
	b	1f
	.asciz  "<7>VFP: \str\n"
	.balign 4
1:	ldmfd	sp!, {r0-r3, ip, lr}
#endif
	.endm


@ VFP hardware support entry point.
@
@  r0  = faulted instruction
@  r2  = faulted PC+4
@  r9  = successful return
@  r10 = vfp_state union
@  r11 = CPU number
@  lr  = failure return

ENTRY(vfp_support_entry)
	DBGSTR3	"instr %08x pc %08x state %p", r0, r2, r10

	VFPFMRX	r1, FPEXC		@ Is the VFP enabled?
	DBGSTR1	"fpexc %08x", r1
	tst	r1, #FPEXC_EN
	bne	look_for_VFP_exceptions	@ VFP is already enabled

	DBGSTR1 "enable %x", r10
	ldr	r3, last_VFP_context_address
	orr	r1, r1, #FPEXC_EN	@ user FPEXC has the enable bit set
	ldr	r4, [r3, r11, lsl #2]	@ last_VFP_context pointer
	bic	r5, r1, #FPEXC_EX	@ make sure exceptions are disabled
	cmp	r4, r10
	beq	check_for_exception	@ we are returning to the same
					@ process, so the registers are
					@ still there.  In this case, we do
					@ not want to drop a pending exception.

	VFPFMXR	FPEXC, r5		@ enable VFP, disable any pending
					@ exceptions, so we can get at the
					@ rest of it

#ifndef CONFIG_SMP
	@ Save out the current registers to the old thread state
	@ No need for SMP since this is not done lazily

	DBGSTR1	"save old state %p", r4
	cmp	r4, #0
	beq	no_old_VFP_process
	VFPFSTMIA r4, r5		@ save the working registers
	VFPFMRX	r5, FPSCR		@ current status
	tst	r1, #FPEXC_EX		@ is there additional state to save?
	beq	1f
	VFPFMRX	r6, FPINST		@ FPINST (only if FPEXC.EX is set)
	tst	r1, #FPEXC_FP2V		@ is there an FPINST2 to read?
	beq	1f
	VFPFMRX	r8, FPINST2		@ FPINST2 if needed (and present)
1:
	stmia	r4, {r1, r5, r6, r8}	@ save FPEXC, FPSCR, FPINST, FPINST2
					@ and point r4 at the word at the
					@ start of the register dump
#endif

no_old_VFP_process:
	DBGSTR1	"load state %p", r10
	str	r10, [r3, r11, lsl #2]	@ update the last_VFP_context pointer
					@ Load the saved state back into the VFP
	VFPFLDMIA r10, r5		@ reload the working registers while
					@ FPEXC is in a safe state
	ldmia	r10, {r1, r5, r6, r8}	@ load FPEXC, FPSCR, FPINST, FPINST2
	tst	r1, #FPEXC_EX		@ is there additional state to restore?
	beq	1f
	VFPFMXR	FPINST, r6		@ restore FPINST (only if FPEXC.EX is set)
	tst	r1, #FPEXC_FP2V		@ is there an FPINST2 to write?
	beq	1f
	VFPFMXR	FPINST2, r8		@ FPINST2 if needed (and present)
1:
	VFPFMXR	FPSCR, r5		@ restore status

check_for_exception:
	tst	r1, #FPEXC_EX
	bne	process_exception	@ might as well handle the pending
					@ exception before retrying branch
					@ out before setting an FPEXC that
					@ stops us reading stuff
	VFPFMXR	FPEXC, r1		@ restore FPEXC last
	sub	r2, r2, #4
	str	r2, [sp, #S_PC]		@ retry the instruction
	mov	pc, r9			@ we think we have handled things


look_for_VFP_exceptions:
	@ Check for synchronous or asynchronous exception
	tst	r1, #FPEXC_EX | FPEXC_DEX
	bne	process_exception
	@ On some implementations of the VFP subarch 1, setting FPSCR.IXE
	@ causes all the CDP instructions to be bounced synchronously without
	@ setting the FPEXC.EX bit
	VFPFMRX	r5, FPSCR
	tst	r5, #FPSCR_IXE
	bne	process_exception

	@ Fall into hand on to next handler - appropriate coproc instr
	@ not recognised by VFP

	DBGSTR	"not VFP"
	mov	pc, lr

process_exception:
	DBGSTR	"bounce"
	mov	r2, sp			@ nothing stacked - regdump is at TOS
	mov	lr, r9			@ setup for a return to the user code.

	@ Now call the C code to package up the bounce to the support code
	@   r0 holds the trigger instruction
	@   r1 holds the FPEXC value
	@   r2 pointer to register dump
	b	VFP_bounce		@ we have handled this - the support
					@ code will raise an exception if
					@ required. If not, the user code will
					@ retry the faulted instruction
ENDPROC(vfp_support_entry)

#if defined(CONFIG_SMP) || defined(CONFIG_PM)
ENTRY(vfp_save_state)
	@ Save the current VFP state
	@ r0 - save location
	@ r1 - FPEXC
	DBGSTR1	"save VFP state %p", r0
	VFPFSTMIA r0, r2		@ save the working registers
	VFPFMRX	r2, FPSCR		@ current status
	tst	r1, #FPEXC_EX		@ is there additional state to save?
	beq	1f
	VFPFMRX	r3, FPINST		@ FPINST (only if FPEXC.EX is set)
	tst	r1, #FPEXC_FP2V		@ is there an FPINST2 to read?
	beq	1f
	VFPFMRX	r12, FPINST2		@ FPINST2 if needed (and present)
1:
	stmia	r0, {r1, r2, r3, r12}	@ save FPEXC, FPSCR, FPINST, FPINST2
	mov	pc, lr
ENDPROC(vfp_save_state)
#endif

last_VFP_context_address:
	.word	last_VFP_context

ENTRY(vfp_get_float)
	add	pc, pc, r0, lsl #3
	mov	r0, r0
	.irp	dr,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
	mrc	p10, 0, r0, c\dr, c0, 0	@ fmrs	r0, s0
	mov	pc, lr
	mrc	p10, 0, r0, c\dr, c0, 4	@ fmrs	r0, s1
	mov	pc, lr
	.endr
ENDPROC(vfp_get_float)

ENTRY(vfp_put_float)
	add	pc, pc, r1, lsl #3
	mov	r0, r0
	.irp	dr,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
	mcr	p10, 0, r0, c\dr, c0, 0	@ fmsr	r0, s0
	mov	pc, lr
	mcr	p10, 0, r0, c\dr, c0, 4	@ fmsr	r0, s1
	mov	pc, lr
	.endr
ENDPROC(vfp_put_float)

ENTRY(vfp_get_double)
	add	pc, pc, r0, lsl #3
	mov	r0, r0
	.irp	dr,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
	fmrrd	r0, r1, d\dr
	mov	pc, lr
	.endr
#ifdef CONFIG_VFPv3
	@ d16 - d31 registers
	.irp	dr,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
	mrrc	p11, 3, r0, r1, c\dr	@ fmrrd	r0, r1, d\dr
	mov	pc, lr
	.endr
#endif

	@ virtual register 16 (or 32 if VFPv3) for compare with zero
	mov	r0, #0
	mov	r1, #0
	mov	pc, lr
ENDPROC(vfp_get_double)

ENTRY(vfp_put_double)
	add	pc, pc, r2, lsl #3
	mov	r0, r0
	.irp	dr,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
	fmdrr	d\dr, r0, r1
	mov	pc, lr
	.endr
#ifdef CONFIG_VFPv3
	@ d16 - d31 registers
	.irp	dr,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
	mcrr	p11, 3, r1, r2, c\dr	@ fmdrr	r1, r2, d\dr
	mov	pc, lr
	.endr
#endif
ENDPROC(vfp_put_double)
