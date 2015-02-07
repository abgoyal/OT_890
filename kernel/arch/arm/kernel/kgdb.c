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
 * arch/arm/kernel/kgdb.c
 *
 * ARM KGDB support
 *
 * Copyright (c) 2002-2004 MontaVista Software, Inc
 * Copyright (c) 2008 Wind River Systems, Inc.
 *
 * Authors:  George Davis <davis_g@mvista.com>
 *           Deepak Saxena <dsaxena@plexity.net>
 */
#include <linux/kgdb.h>
#include <asm/traps.h>

/* Make a local copy of the registers passed into the handler (bletch) */
void pt_regs_to_gdb_regs(unsigned long *gdb_regs, struct pt_regs *kernel_regs)
{
	int regno;

	/* Initialize all to zero. */
	for (regno = 0; regno < GDB_MAX_REGS; regno++)
		gdb_regs[regno] = 0;

	gdb_regs[_R0]		= kernel_regs->ARM_r0;
	gdb_regs[_R1]		= kernel_regs->ARM_r1;
	gdb_regs[_R2]		= kernel_regs->ARM_r2;
	gdb_regs[_R3]		= kernel_regs->ARM_r3;
	gdb_regs[_R4]		= kernel_regs->ARM_r4;
	gdb_regs[_R5]		= kernel_regs->ARM_r5;
	gdb_regs[_R6]		= kernel_regs->ARM_r6;
	gdb_regs[_R7]		= kernel_regs->ARM_r7;
	gdb_regs[_R8]		= kernel_regs->ARM_r8;
	gdb_regs[_R9]		= kernel_regs->ARM_r9;
	gdb_regs[_R10]		= kernel_regs->ARM_r10;
	gdb_regs[_FP]		= kernel_regs->ARM_fp;
	gdb_regs[_IP]		= kernel_regs->ARM_ip;
	gdb_regs[_SPT]		= kernel_regs->ARM_sp;
	gdb_regs[_LR]		= kernel_regs->ARM_lr;
	gdb_regs[_PC]		= kernel_regs->ARM_pc;
	gdb_regs[_CPSR]		= kernel_regs->ARM_cpsr;
}

/* Copy local gdb registers back to kgdb regs, for later copy to kernel */
void gdb_regs_to_pt_regs(unsigned long *gdb_regs, struct pt_regs *kernel_regs)
{
	kernel_regs->ARM_r0	= gdb_regs[_R0];
	kernel_regs->ARM_r1	= gdb_regs[_R1];
	kernel_regs->ARM_r2	= gdb_regs[_R2];
	kernel_regs->ARM_r3	= gdb_regs[_R3];
	kernel_regs->ARM_r4	= gdb_regs[_R4];
	kernel_regs->ARM_r5	= gdb_regs[_R5];
	kernel_regs->ARM_r6	= gdb_regs[_R6];
	kernel_regs->ARM_r7	= gdb_regs[_R7];
	kernel_regs->ARM_r8	= gdb_regs[_R8];
	kernel_regs->ARM_r9	= gdb_regs[_R9];
	kernel_regs->ARM_r10	= gdb_regs[_R10];
	kernel_regs->ARM_fp	= gdb_regs[_FP];
	kernel_regs->ARM_ip	= gdb_regs[_IP];
	kernel_regs->ARM_sp	= gdb_regs[_SPT];
	kernel_regs->ARM_lr	= gdb_regs[_LR];
	kernel_regs->ARM_pc	= gdb_regs[_PC];
	kernel_regs->ARM_cpsr	= gdb_regs[_CPSR];
}

void
sleeping_thread_to_gdb_regs(unsigned long *gdb_regs, struct task_struct *task)
{
	struct pt_regs *thread_regs;
	int regno;

	/* Just making sure... */
	if (task == NULL)
		return;

	/* Initialize to zero */
	for (regno = 0; regno < GDB_MAX_REGS; regno++)
		gdb_regs[regno] = 0;

	/* Otherwise, we have only some registers from switch_to() */
	thread_regs		= task_pt_regs(task);
	gdb_regs[_R0]		= thread_regs->ARM_r0;
	gdb_regs[_R1]		= thread_regs->ARM_r1;
	gdb_regs[_R2]		= thread_regs->ARM_r2;
	gdb_regs[_R3]		= thread_regs->ARM_r3;
	gdb_regs[_R4]		= thread_regs->ARM_r4;
	gdb_regs[_R5]		= thread_regs->ARM_r5;
	gdb_regs[_R6]		= thread_regs->ARM_r6;
	gdb_regs[_R7]		= thread_regs->ARM_r7;
	gdb_regs[_R8]		= thread_regs->ARM_r8;
	gdb_regs[_R9]		= thread_regs->ARM_r9;
	gdb_regs[_R10]		= thread_regs->ARM_r10;
	gdb_regs[_FP]		= thread_regs->ARM_fp;
	gdb_regs[_IP]		= thread_regs->ARM_ip;
	gdb_regs[_SPT]		= thread_regs->ARM_sp;
	gdb_regs[_LR]		= thread_regs->ARM_lr;
	gdb_regs[_PC]		= thread_regs->ARM_pc;
	gdb_regs[_CPSR]		= thread_regs->ARM_cpsr;
}

static int compiled_break;

int kgdb_arch_handle_exception(int exception_vector, int signo,
			       int err_code, char *remcom_in_buffer,
			       char *remcom_out_buffer,
			       struct pt_regs *linux_regs)
{
	unsigned long addr;
	char *ptr;

	switch (remcom_in_buffer[0]) {
	case 'D':
	case 'k':
	case 'c':
		/*
		 * Try to read optional parameter, pc unchanged if no parm.
		 * If this was a compiled breakpoint, we need to move
		 * to the next instruction or we will just breakpoint
		 * over and over again.
		 */
		ptr = &remcom_in_buffer[1];
		if (kgdb_hex2long(&ptr, &addr))
			linux_regs->ARM_pc = addr;
		else if (compiled_break == 1)
			linux_regs->ARM_pc += 4;

		compiled_break = 0;

		return 0;
	}

	return -1;
}

static int kgdb_brk_fn(struct pt_regs *regs, unsigned int instr)
{
	kgdb_handle_exception(1, SIGTRAP, 0, regs);

	return 0;
}

static int kgdb_compiled_brk_fn(struct pt_regs *regs, unsigned int instr)
{
	compiled_break = 1;
	kgdb_handle_exception(1, SIGTRAP, 0, regs);

	return 0;
}

static struct undef_hook kgdb_brkpt_hook = {
	.instr_mask		= 0xffffffff,
	.instr_val		= KGDB_BREAKINST,
	.fn			= kgdb_brk_fn
};

static struct undef_hook kgdb_compiled_brkpt_hook = {
	.instr_mask		= 0xffffffff,
	.instr_val		= KGDB_COMPILED_BREAK,
	.fn			= kgdb_compiled_brk_fn
};

/**
 *	kgdb_arch_init - Perform any architecture specific initalization.
 *
 *	This function will handle the initalization of any architecture
 *	specific callbacks.
 */
int kgdb_arch_init(void)
{
	register_undef_hook(&kgdb_brkpt_hook);
	register_undef_hook(&kgdb_compiled_brkpt_hook);

	return 0;
}

/**
 *	kgdb_arch_exit - Perform any architecture specific uninitalization.
 *
 *	This function will handle the uninitalization of any architecture
 *	specific callbacks, for dynamic registration and unregistration.
 */
void kgdb_arch_exit(void)
{
	unregister_undef_hook(&kgdb_brkpt_hook);
	unregister_undef_hook(&kgdb_compiled_brkpt_hook);
}

/*
 * Register our undef instruction hooks with ARM undef core.
 * We regsiter a hook specifically looking for the KGB break inst
 * and we handle the normal undef case within the do_undefinstr
 * handler.
 */
struct kgdb_arch arch_kgdb_ops = {
#ifndef __ARMEB__
	.gdb_bpt_instr		= {0xfe, 0xde, 0xff, 0xe7}
#else /* ! __ARMEB__ */
	.gdb_bpt_instr		= {0xe7, 0xff, 0xde, 0xfe}
#endif
};
