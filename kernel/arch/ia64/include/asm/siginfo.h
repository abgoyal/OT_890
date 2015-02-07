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

#ifndef _ASM_IA64_SIGINFO_H
#define _ASM_IA64_SIGINFO_H

/*
 * Based on <asm-i386/siginfo.h>.
 *
 * Modified 1998-2002
 *	David Mosberger-Tang <davidm@hpl.hp.com>, Hewlett-Packard Co
 */

#define __ARCH_SI_PREAMBLE_SIZE	(4 * sizeof(int))

#define HAVE_ARCH_SIGINFO_T
#define HAVE_ARCH_COPY_SIGINFO
#define HAVE_ARCH_COPY_SIGINFO_TO_USER

#include <asm-generic/siginfo.h>

typedef struct siginfo {
	int si_signo;
	int si_errno;
	int si_code;
	int __pad0;

	union {
		int _pad[SI_PAD_SIZE];

		/* kill() */
		struct {
			pid_t _pid;		/* sender's pid */
			uid_t _uid;		/* sender's uid */
		} _kill;

		/* POSIX.1b timers */
		struct {
			timer_t _tid;		/* timer id */
			int _overrun;		/* overrun count */
			char _pad[sizeof(__ARCH_SI_UID_T) - sizeof(int)];
			sigval_t _sigval;	/* must overlay ._rt._sigval! */
			int _sys_private;	/* not to be passed to user */
		} _timer;

		/* POSIX.1b signals */
		struct {
			pid_t _pid;		/* sender's pid */
			uid_t _uid;		/* sender's uid */
			sigval_t _sigval;
		} _rt;

		/* SIGCHLD */
		struct {
			pid_t _pid;		/* which child */
			uid_t _uid;		/* sender's uid */
			int _status;		/* exit code */
			clock_t _utime;
			clock_t _stime;
		} _sigchld;

		/* SIGILL, SIGFPE, SIGSEGV, SIGBUS */
		struct {
			void __user *_addr;	/* faulting insn/memory ref. */
			int _imm;		/* immediate value for "break" */
			unsigned int _flags;	/* see below */
			unsigned long _isr;	/* isr */
		} _sigfault;

		/* SIGPOLL */
		struct {
			long _band;	/* POLL_IN, POLL_OUT, POLL_MSG (XPG requires a "long") */
			int _fd;
		} _sigpoll;
	} _sifields;
} siginfo_t;

#define si_imm		_sifields._sigfault._imm	/* as per UNIX SysV ABI spec */
#define si_flags	_sifields._sigfault._flags
/*
 * si_isr is valid for SIGILL, SIGFPE, SIGSEGV, SIGBUS, and SIGTRAP provided that
 * si_code is non-zero and __ISR_VALID is set in si_flags.
 */
#define si_isr		_sifields._sigfault._isr

/*
 * Flag values for si_flags:
 */
#define __ISR_VALID_BIT	0
#define __ISR_VALID	(1 << __ISR_VALID_BIT)

/*
 * SIGILL si_codes
 */
#define ILL_BADIADDR	(__SI_FAULT|9)	/* unimplemented instruction address */
#define __ILL_BREAK	(__SI_FAULT|10)	/* illegal break */
#define __ILL_BNDMOD	(__SI_FAULT|11)	/* bundle-update (modification) in progress */
#undef NSIGILL
#define NSIGILL		11

/*
 * SIGFPE si_codes
 */
#define __FPE_DECOVF	(__SI_FAULT|9)	/* decimal overflow */
#define __FPE_DECDIV	(__SI_FAULT|10)	/* decimal division by zero */
#define __FPE_DECERR	(__SI_FAULT|11)	/* packed decimal error */
#define __FPE_INVASC	(__SI_FAULT|12)	/* invalid ASCII digit */
#define __FPE_INVDEC	(__SI_FAULT|13)	/* invalid decimal digit */
#undef NSIGFPE
#define NSIGFPE		13

/*
 * SIGSEGV si_codes
 */
#define __SEGV_PSTKOVF	(__SI_FAULT|3)	/* paragraph stack overflow */
#undef NSIGSEGV
#define NSIGSEGV	3

#undef NSIGTRAP
#define NSIGTRAP	4

#ifdef __KERNEL__
#include <linux/string.h>

static inline void
copy_siginfo (siginfo_t *to, siginfo_t *from)
{
	if (from->si_code < 0)
		memcpy(to, from, sizeof(siginfo_t));
	else
		/* _sigchld is currently the largest know union member */
		memcpy(to, from, 4*sizeof(int) + sizeof(from->_sifields._sigchld));
}

#endif /* __KERNEL__ */

#endif /* _ASM_IA64_SIGINFO_H */
