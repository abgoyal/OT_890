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

/* MN10300 Low-level thread information
 *
 * Copyright (C) 2007 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version
 * 2 of the Licence, or (at your option) any later version.
 */

#ifndef _ASM_THREAD_INFO_H
#define _ASM_THREAD_INFO_H

#ifdef __KERNEL__

#include <asm/page.h>

#ifndef __ASSEMBLY__
#include <asm/processor.h>
#endif

#define PREEMPT_ACTIVE		0x10000000

#ifdef CONFIG_4KSTACKS
#define THREAD_SIZE		(4096)
#else
#define THREAD_SIZE		(8192)
#endif

#define STACK_WARN		(THREAD_SIZE / 8)

/*
 * low level task data that entry.S needs immediate access to
 * - this struct should fit entirely inside of one cache line
 * - this struct shares the supervisor stack pages
 * - if the contents of this structure are changed, the assembly constants
 *   must also be changed
 */
#ifndef __ASSEMBLY__

struct thread_info {
	struct task_struct	*task;		/* main task structure */
	struct exec_domain	*exec_domain;	/* execution domain */
	unsigned long		flags;		/* low level flags */
	__u32			cpu;		/* current CPU */
	__s32			preempt_count;	/* 0 => preemptable, <0 => BUG */

	mm_segment_t		addr_limit;	/* thread address space:
						   0-0xBFFFFFFF for user-thead
						   0-0xFFFFFFFF for kernel-thread
						*/
	struct restart_block    restart_block;

	__u8			supervisor_stack[0];
};

#else /* !__ASSEMBLY__ */

#ifndef __ASM_OFFSETS_H__
#include <asm/asm-offsets.h>
#endif

#endif

/*
 * macros/functions for gaining access to the thread information structure
 *
 * preempt_count needs to be 1 initially, until the scheduler is functional.
 */
#ifndef __ASSEMBLY__

#define INIT_THREAD_INFO(tsk)			\
{						\
	.task		= &tsk,			\
	.exec_domain	= &default_exec_domain,	\
	.flags		= 0,			\
	.cpu		= 0,			\
	.preempt_count	= 1,			\
	.addr_limit	= KERNEL_DS,		\
	.restart_block = {			\
		.fn = do_no_restart_syscall,	\
	},					\
}

#define init_thread_info	(init_thread_union.thread_info)
#define init_stack		(init_thread_union.stack)
#define init_uregs							\
	((struct pt_regs *)						\
	 ((unsigned long) init_stack + THREAD_SIZE - sizeof(struct pt_regs)))

extern struct thread_info *__current_ti;

/* how to get the thread information struct from C */
static inline __attribute__((const))
struct thread_info *current_thread_info(void)
{
	struct thread_info *ti;
	asm("mov sp,%0\n"
	    "and %1,%0\n"
	    : "=d" (ti)
	    : "i" (~(THREAD_SIZE - 1))
	    : "cc");
	return ti;
}

/* how to get the current stack pointer from C */
static inline unsigned long current_stack_pointer(void)
{
	unsigned long sp;
	asm("mov sp,%0; ":"=r" (sp));
	return sp;
}

#define __HAVE_ARCH_THREAD_INFO_ALLOCATOR

/* thread information allocation */
#ifdef CONFIG_DEBUG_STACK_USAGE
#define alloc_thread_info(tsk) kzalloc(THREAD_SIZE, GFP_KERNEL)
#else
#define alloc_thread_info(tsk) kmalloc(THREAD_SIZE, GFP_KERNEL)
#endif

#define free_thread_info(ti)	kfree((ti))
#define get_thread_info(ti)	get_task_struct((ti)->task)
#define put_thread_info(ti)	put_task_struct((ti)->task)

#else /* !__ASSEMBLY__ */

#ifndef __VMLINUX_LDS__
/* how to get the thread information struct from ASM */
.macro GET_THREAD_INFO reg
	mov	sp,\reg
	and	-THREAD_SIZE,\reg
.endm
#endif
#endif

/*
 * thread information flags
 * - these are process state flags that various assembly files may need to
 *   access
 * - pending work-to-be-done flags are in LSW
 * - other flags in MSW
 */
#define TIF_SYSCALL_TRACE	0	/* syscall trace active */
#define TIF_NOTIFY_RESUME	1	/* resumption notification requested */
#define TIF_SIGPENDING		2	/* signal pending */
#define TIF_NEED_RESCHED	3	/* rescheduling necessary */
#define TIF_SINGLESTEP		4	/* restore singlestep on return to user mode */
#define TIF_RESTORE_SIGMASK	5	/* restore signal mask in do_signal() */
#define TIF_POLLING_NRFLAG	16	/* true if poll_idle() is polling TIF_NEED_RESCHED */
#define TIF_MEMDIE		17	/* OOM killer killed process */
#define TIF_FREEZE		18	/* freezing for suspend */

#define _TIF_SYSCALL_TRACE	+(1 << TIF_SYSCALL_TRACE)
#define _TIF_NOTIFY_RESUME	+(1 << TIF_NOTIFY_RESUME)
#define _TIF_SIGPENDING		+(1 << TIF_SIGPENDING)
#define _TIF_NEED_RESCHED	+(1 << TIF_NEED_RESCHED)
#define _TIF_SINGLESTEP		+(1 << TIF_SINGLESTEP)
#define _TIF_RESTORE_SIGMASK	+(1 << TIF_RESTORE_SIGMASK)
#define _TIF_POLLING_NRFLAG	+(1 << TIF_POLLING_NRFLAG)
#define _TIF_FREEZE		+(1 << TIF_FREEZE)

#define _TIF_WORK_MASK		0x0000FFFE	/* work to do on interrupt/exception return */
#define _TIF_ALLWORK_MASK	0x0000FFFF	/* work to do on any return to u-space */

#endif /* __KERNEL__ */

#endif /* _ASM_THREAD_INFO_H */
