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
 *  linux/arch/arm/kernel/fiq.c
 *
 *  Copyright (C) 1998 Russell King
 *  Copyright (C) 1998, 1999 Phil Blundell
 *
 *  FIQ support written by Philip Blundell <philb@gnu.org>, 1998.
 *
 *  FIQ support re-written by Russell King to be more generic
 *
 * We now properly support a method by which the FIQ handlers can
 * be stacked onto the vector.  We still do not support sharing
 * the FIQ vector itself.
 *
 * Operation is as follows:
 *  1. Owner A claims FIQ:
 *     - default_fiq relinquishes control.
 *  2. Owner A:
 *     - inserts code.
 *     - sets any registers,
 *     - enables FIQ.
 *  3. Owner B claims FIQ:
 *     - if owner A has a relinquish function.
 *       - disable FIQs.
 *       - saves any registers.
 *       - returns zero.
 *  4. Owner B:
 *     - inserts code.
 *     - sets any registers,
 *     - enables FIQ.
 *  5. Owner B releases FIQ:
 *     - Owner A is asked to reacquire FIQ:
 *	 - inserts code.
 *	 - restores saved registers.
 *	 - enables FIQ.
 *  6. Goto 3
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/seq_file.h>

#include <asm/cacheflush.h>
#include <asm/fiq.h>
#include <asm/irq.h>
#include <asm/system.h>

static unsigned long no_fiq_insn;

/* Default reacquire function
 * - we always relinquish FIQ control
 * - we always reacquire FIQ control
 */
static int fiq_def_op(void *ref, int relinquish)
{
	if (!relinquish)
		set_fiq_handler(&no_fiq_insn, sizeof(no_fiq_insn));

	return 0;
}

static struct fiq_handler default_owner = {
	.name	= "default",
	.fiq_op = fiq_def_op,
};

static struct fiq_handler *current_fiq = &default_owner;

int show_fiq_list(struct seq_file *p, void *v)
{
	if (current_fiq != &default_owner)
		seq_printf(p, "FIQ:              %s\n", current_fiq->name);

	return 0;
}

void set_fiq_handler(void *start, unsigned int length)
{
	memcpy((void *)0xffff001c, start, length);
	flush_icache_range(0xffff001c, 0xffff001c + length);
	if (!vectors_high())
		flush_icache_range(0x1c, 0x1c + length);
}

/*
 * Taking an interrupt in FIQ mode is death, so both these functions
 * disable irqs for the duration.  Note - these functions are almost
 * entirely coded in assembly.
 */
void __naked set_fiq_regs(struct pt_regs *regs)
{
	register unsigned long tmp;
	asm volatile (
	"mov	ip, sp\n\
	stmfd	sp!, {fp, ip, lr, pc}\n\
	sub	fp, ip, #4\n\
	mrs	%0, cpsr\n\
	msr	cpsr_c, %2	@ select FIQ mode\n\
	mov	r0, r0\n\
	ldmia	%1, {r8 - r14}\n\
	msr	cpsr_c, %0	@ return to SVC mode\n\
	mov	r0, r0\n\
	ldmfd	sp, {fp, sp, pc}"
	: "=&r" (tmp)
	: "r" (&regs->ARM_r8), "I" (PSR_I_BIT | PSR_F_BIT | FIQ_MODE));
}

void __naked get_fiq_regs(struct pt_regs *regs)
{
	register unsigned long tmp;
	asm volatile (
	"mov	ip, sp\n\
	stmfd	sp!, {fp, ip, lr, pc}\n\
	sub	fp, ip, #4\n\
	mrs	%0, cpsr\n\
	msr	cpsr_c, %2	@ select FIQ mode\n\
	mov	r0, r0\n\
	stmia	%1, {r8 - r14}\n\
	msr	cpsr_c, %0	@ return to SVC mode\n\
	mov	r0, r0\n\
	ldmfd	sp, {fp, sp, pc}"
	: "=&r" (tmp)
	: "r" (&regs->ARM_r8), "I" (PSR_I_BIT | PSR_F_BIT | FIQ_MODE));
}

int claim_fiq(struct fiq_handler *f)
{
	int ret = 0;

	if (current_fiq) {
		ret = -EBUSY;

		if (current_fiq->fiq_op != NULL)
			ret = current_fiq->fiq_op(current_fiq->dev_id, 1);
	}

	if (!ret) {
		f->next = current_fiq;
		current_fiq = f;
	}

	return ret;
}

void release_fiq(struct fiq_handler *f)
{
	if (current_fiq != f) {
		printk(KERN_ERR "%s FIQ trying to release %s FIQ\n",
		       f->name, current_fiq->name);
		dump_stack();
		return;
	}

	do
		current_fiq = current_fiq->next;
	while (current_fiq->fiq_op(current_fiq->dev_id, 0));
}

void enable_fiq(int fiq)
{
	enable_irq(fiq + FIQ_START);
}

void disable_fiq(int fiq)
{
	disable_irq(fiq + FIQ_START);
}

EXPORT_SYMBOL(set_fiq_handler);
EXPORT_SYMBOL(set_fiq_regs);
EXPORT_SYMBOL(get_fiq_regs);
EXPORT_SYMBOL(claim_fiq);
EXPORT_SYMBOL(release_fiq);
EXPORT_SYMBOL(enable_fiq);
EXPORT_SYMBOL(disable_fiq);

void __init init_FIQ(void)
{
	no_fiq_insn = *(unsigned long *)0xffff001c;
}
