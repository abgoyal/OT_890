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
 * Generate definitions needed by assembly language modules.
 * This code generates raw asm output which is post-processed
 * to extract and format the required data.
 */

#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/personality.h>
#include <linux/kbuild.h>
#include <asm/ucontext.h>
#include <asm/processor.h>
#include <asm/thread_info.h>
#include <asm/ptrace.h>
#include "sigframe.h"
#include "mn10300-serial.h"

void foo(void)
{
	OFFSET(SIGCONTEXT_d0, sigcontext, d0);
	OFFSET(SIGCONTEXT_d1, sigcontext, d1);
	BLANK();

	OFFSET(TI_task,			thread_info, task);
	OFFSET(TI_exec_domain,		thread_info, exec_domain);
	OFFSET(TI_flags,		thread_info, flags);
	OFFSET(TI_cpu,			thread_info, cpu);
	OFFSET(TI_preempt_count,	thread_info, preempt_count);
	OFFSET(TI_addr_limit,		thread_info, addr_limit);
	OFFSET(TI_restart_block,	thread_info, restart_block);
	BLANK();

	OFFSET(REG_D0,			pt_regs, d0);
	OFFSET(REG_D1,			pt_regs, d1);
	OFFSET(REG_D2,			pt_regs, d2);
	OFFSET(REG_D3,			pt_regs, d3);
	OFFSET(REG_A0,			pt_regs, a0);
	OFFSET(REG_A1,			pt_regs, a1);
	OFFSET(REG_A2,			pt_regs, a2);
	OFFSET(REG_A3,			pt_regs, a3);
	OFFSET(REG_E0,			pt_regs, e0);
	OFFSET(REG_E1,			pt_regs, e1);
	OFFSET(REG_E2,			pt_regs, e2);
	OFFSET(REG_E3,			pt_regs, e3);
	OFFSET(REG_E4,			pt_regs, e4);
	OFFSET(REG_E5,			pt_regs, e5);
	OFFSET(REG_E6,			pt_regs, e6);
	OFFSET(REG_E7,			pt_regs, e7);
	OFFSET(REG_SP,			pt_regs, sp);
	OFFSET(REG_EPSW,		pt_regs, epsw);
	OFFSET(REG_PC,			pt_regs, pc);
	OFFSET(REG_LAR,			pt_regs, lar);
	OFFSET(REG_LIR,			pt_regs, lir);
	OFFSET(REG_MDR,			pt_regs, mdr);
	OFFSET(REG_MCVF,		pt_regs, mcvf);
	OFFSET(REG_MCRL,		pt_regs, mcrl);
	OFFSET(REG_MCRH,		pt_regs, mcrh);
	OFFSET(REG_MDRQ,		pt_regs, mdrq);
	OFFSET(REG_ORIG_D0,		pt_regs, orig_d0);
	OFFSET(REG_NEXT,		pt_regs, next);
	DEFINE(REG__END,		sizeof(struct pt_regs));
	BLANK();

	OFFSET(THREAD_UREGS,		thread_struct, uregs);
	OFFSET(THREAD_PC,		thread_struct, pc);
	OFFSET(THREAD_SP,		thread_struct, sp);
	OFFSET(THREAD_A3,		thread_struct, a3);
	OFFSET(THREAD_USP,		thread_struct, usp);
	OFFSET(THREAD_FRAME,		thread_struct, __frame);
	BLANK();

	DEFINE(CLONE_VM_asm,		CLONE_VM);
	DEFINE(CLONE_FS_asm,		CLONE_FS);
	DEFINE(CLONE_FILES_asm,		CLONE_FILES);
	DEFINE(CLONE_SIGHAND_asm,	CLONE_SIGHAND);
	DEFINE(CLONE_UNTRACED_asm,	CLONE_UNTRACED);
	DEFINE(SIGCHLD_asm,		SIGCHLD);
	BLANK();

	OFFSET(EXEC_DOMAIN_handler,	exec_domain, handler);
	OFFSET(RT_SIGFRAME_sigcontext,	rt_sigframe, uc.uc_mcontext);

	DEFINE(PAGE_SIZE_asm,		PAGE_SIZE);

	OFFSET(__rx_buffer,		mn10300_serial_port, rx_buffer);
	OFFSET(__rx_inp,		mn10300_serial_port, rx_inp);
	OFFSET(__rx_outp,		mn10300_serial_port, rx_outp);
	OFFSET(__tx_info_buffer,	mn10300_serial_port, uart.info);
	OFFSET(__tx_xchar,		mn10300_serial_port, tx_xchar);
	OFFSET(__tx_break,		mn10300_serial_port, tx_break);
	OFFSET(__intr_flags,		mn10300_serial_port, intr_flags);
	OFFSET(__rx_icr,		mn10300_serial_port, rx_icr);
	OFFSET(__tx_icr,		mn10300_serial_port, tx_icr);
	OFFSET(__tm_icr,		mn10300_serial_port, _tmicr);
	OFFSET(__iobase,		mn10300_serial_port, _iobase);

	DEFINE(__UART_XMIT_SIZE,	UART_XMIT_SIZE);
	OFFSET(__xmit_buffer,		uart_info, xmit.buf);
	OFFSET(__xmit_head,		uart_info, xmit.head);
	OFFSET(__xmit_tail,		uart_info, xmit.tail);
}
