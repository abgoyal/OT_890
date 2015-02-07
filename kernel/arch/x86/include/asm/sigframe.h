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

#ifndef _ASM_X86_SIGFRAME_H
#define _ASM_X86_SIGFRAME_H

#include <asm/sigcontext.h>
#include <asm/siginfo.h>
#include <asm/ucontext.h>

#ifdef CONFIG_X86_32
#define sigframe_ia32		sigframe
#define rt_sigframe_ia32	rt_sigframe
#define sigcontext_ia32		sigcontext
#define _fpstate_ia32		_fpstate
#define ucontext_ia32		ucontext
#else /* !CONFIG_X86_32 */

#ifdef CONFIG_IA32_EMULATION
#include <asm/ia32.h>
#endif /* CONFIG_IA32_EMULATION */

#endif /* CONFIG_X86_32 */

#if defined(CONFIG_X86_32) || defined(CONFIG_IA32_EMULATION)
struct sigframe_ia32 {
	u32 pretcode;
	int sig;
	struct sigcontext_ia32 sc;
	/*
	 * fpstate is unused. fpstate is moved/allocated after
	 * retcode[] below. This movement allows to have the FP state and the
	 * future state extensions (xsave) stay together.
	 * And at the same time retaining the unused fpstate, prevents changing
	 * the offset of extramask[] in the sigframe and thus prevent any
	 * legacy application accessing/modifying it.
	 */
	struct _fpstate_ia32 fpstate_unused;
#ifdef CONFIG_IA32_EMULATION
	unsigned int extramask[_COMPAT_NSIG_WORDS-1];
#else /* !CONFIG_IA32_EMULATION */
	unsigned long extramask[_NSIG_WORDS-1];
#endif /* CONFIG_IA32_EMULATION */
	char retcode[8];
	/* fp state follows here */
};

struct rt_sigframe_ia32 {
	u32 pretcode;
	int sig;
	u32 pinfo;
	u32 puc;
#ifdef CONFIG_IA32_EMULATION
	compat_siginfo_t info;
#else /* !CONFIG_IA32_EMULATION */
	struct siginfo info;
#endif /* CONFIG_IA32_EMULATION */
	struct ucontext_ia32 uc;
	char retcode[8];
	/* fp state follows here */
};
#endif /* defined(CONFIG_X86_32) || defined(CONFIG_IA32_EMULATION) */

#ifdef CONFIG_X86_64
struct rt_sigframe {
	char __user *pretcode;
	struct ucontext uc;
	struct siginfo info;
	/* fp state follows here */
};
#endif /* CONFIG_X86_64 */

#endif /* _ASM_X86_SIGFRAME_H */
