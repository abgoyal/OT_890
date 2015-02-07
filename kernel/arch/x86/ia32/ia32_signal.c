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
 *  linux/arch/x86_64/ia32/ia32_signal.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  1997-11-28  Modified for POSIX.1b signals by Richard Henderson
 *  2000-06-20  Pentium III FXSR, SSE support by Gareth Hughes
 *  2000-12-*   x86-64 compatibility mode signal handling by Andi Kleen
 */

#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/ptrace.h>
#include <linux/unistd.h>
#include <linux/stddef.h>
#include <linux/personality.h>
#include <linux/compat.h>
#include <linux/binfmts.h>
#include <asm/ucontext.h>
#include <asm/uaccess.h>
#include <asm/i387.h>
#include <asm/ptrace.h>
#include <asm/ia32_unistd.h>
#include <asm/user32.h>
#include <asm/sigcontext32.h>
#include <asm/proto.h>
#include <asm/vdso.h>
#include <asm/sigframe.h>
#include <asm/sys_ia32.h>

#define DEBUG_SIG 0

#define _BLOCKABLE (~(sigmask(SIGKILL) | sigmask(SIGSTOP)))

#define FIX_EFLAGS	(X86_EFLAGS_AC | X86_EFLAGS_OF | \
			 X86_EFLAGS_DF | X86_EFLAGS_TF | X86_EFLAGS_SF | \
			 X86_EFLAGS_ZF | X86_EFLAGS_AF | X86_EFLAGS_PF | \
			 X86_EFLAGS_CF)

void signal_fault(struct pt_regs *regs, void __user *frame, char *where);

int copy_siginfo_to_user32(compat_siginfo_t __user *to, siginfo_t *from)
{
	int err;

	if (!access_ok(VERIFY_WRITE, to, sizeof(compat_siginfo_t)))
		return -EFAULT;

	/* If you change siginfo_t structure, please make sure that
	   this code is fixed accordingly.
	   It should never copy any pad contained in the structure
	   to avoid security leaks, but must copy the generic
	   3 ints plus the relevant union member.  */
	err = __put_user(from->si_signo, &to->si_signo);
	err |= __put_user(from->si_errno, &to->si_errno);
	err |= __put_user((short)from->si_code, &to->si_code);

	if (from->si_code < 0) {
		err |= __put_user(from->si_pid, &to->si_pid);
		err |= __put_user(from->si_uid, &to->si_uid);
		err |= __put_user(ptr_to_compat(from->si_ptr), &to->si_ptr);
	} else {
		/*
		 * First 32bits of unions are always present:
		 * si_pid === si_band === si_tid === si_addr(LS half)
		 */
		err |= __put_user(from->_sifields._pad[0],
				  &to->_sifields._pad[0]);
		switch (from->si_code >> 16) {
		case __SI_FAULT >> 16:
			break;
		case __SI_CHLD >> 16:
			err |= __put_user(from->si_utime, &to->si_utime);
			err |= __put_user(from->si_stime, &to->si_stime);
			err |= __put_user(from->si_status, &to->si_status);
			/* FALL THROUGH */
		default:
		case __SI_KILL >> 16:
			err |= __put_user(from->si_uid, &to->si_uid);
			break;
		case __SI_POLL >> 16:
			err |= __put_user(from->si_fd, &to->si_fd);
			break;
		case __SI_TIMER >> 16:
			err |= __put_user(from->si_overrun, &to->si_overrun);
			err |= __put_user(ptr_to_compat(from->si_ptr),
					  &to->si_ptr);
			break;
			 /* This is not generated by the kernel as of now.  */
		case __SI_RT >> 16:
		case __SI_MESGQ >> 16:
			err |= __put_user(from->si_uid, &to->si_uid);
			err |= __put_user(from->si_int, &to->si_int);
			break;
		}
	}
	return err;
}

int copy_siginfo_from_user32(siginfo_t *to, compat_siginfo_t __user *from)
{
	int err;
	u32 ptr32;

	if (!access_ok(VERIFY_READ, from, sizeof(compat_siginfo_t)))
		return -EFAULT;

	err = __get_user(to->si_signo, &from->si_signo);
	err |= __get_user(to->si_errno, &from->si_errno);
	err |= __get_user(to->si_code, &from->si_code);

	err |= __get_user(to->si_pid, &from->si_pid);
	err |= __get_user(to->si_uid, &from->si_uid);
	err |= __get_user(ptr32, &from->si_ptr);
	to->si_ptr = compat_ptr(ptr32);

	return err;
}

asmlinkage long sys32_sigsuspend(int history0, int history1, old_sigset_t mask)
{
	mask &= _BLOCKABLE;
	spin_lock_irq(&current->sighand->siglock);
	current->saved_sigmask = current->blocked;
	siginitset(&current->blocked, mask);
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);

	current->state = TASK_INTERRUPTIBLE;
	schedule();
	set_restore_sigmask();
	return -ERESTARTNOHAND;
}

asmlinkage long sys32_sigaltstack(const stack_ia32_t __user *uss_ptr,
				  stack_ia32_t __user *uoss_ptr,
				  struct pt_regs *regs)
{
	stack_t uss, uoss;
	int ret;
	mm_segment_t seg;

	if (uss_ptr) {
		u32 ptr;

		memset(&uss, 0, sizeof(stack_t));
		if (!access_ok(VERIFY_READ, uss_ptr, sizeof(stack_ia32_t)) ||
			    __get_user(ptr, &uss_ptr->ss_sp) ||
			    __get_user(uss.ss_flags, &uss_ptr->ss_flags) ||
			    __get_user(uss.ss_size, &uss_ptr->ss_size))
			return -EFAULT;
		uss.ss_sp = compat_ptr(ptr);
	}
	seg = get_fs();
	set_fs(KERNEL_DS);
	ret = do_sigaltstack(uss_ptr ? &uss : NULL, &uoss, regs->sp);
	set_fs(seg);
	if (ret >= 0 && uoss_ptr)  {
		if (!access_ok(VERIFY_WRITE, uoss_ptr, sizeof(stack_ia32_t)) ||
		    __put_user(ptr_to_compat(uoss.ss_sp), &uoss_ptr->ss_sp) ||
		    __put_user(uoss.ss_flags, &uoss_ptr->ss_flags) ||
		    __put_user(uoss.ss_size, &uoss_ptr->ss_size))
			ret = -EFAULT;
	}
	return ret;
}

/*
 * Do a signal return; undo the signal stack.
 */
#define COPY(x)			{		\
	err |= __get_user(regs->x, &sc->x);	\
}

#define COPY_SEG_CPL3(seg)	{			\
		unsigned short tmp;			\
		err |= __get_user(tmp, &sc->seg);	\
		regs->seg = tmp | 3;			\
}

#define RELOAD_SEG(seg)		{		\
	unsigned int cur, pre;			\
	err |= __get_user(pre, &sc->seg);	\
	savesegment(seg, cur);			\
	pre |= 3;				\
	if (pre != cur)				\
		loadsegment(seg, pre);		\
}

static int ia32_restore_sigcontext(struct pt_regs *regs,
				   struct sigcontext_ia32 __user *sc,
				   unsigned int *pax)
{
	unsigned int tmpflags, gs, oldgs, err = 0;
	void __user *buf;
	u32 tmp;

	/* Always make any pending restarted system calls return -EINTR */
	current_thread_info()->restart_block.fn = do_no_restart_syscall;

#if DEBUG_SIG
	printk(KERN_DEBUG "SIG restore_sigcontext: "
	       "sc=%p err(%x) eip(%x) cs(%x) flg(%x)\n",
	       sc, sc->err, sc->ip, sc->cs, sc->flags);
#endif

	/*
	 * Reload fs and gs if they have changed in the signal
	 * handler.  This does not handle long fs/gs base changes in
	 * the handler, but does not clobber them at least in the
	 * normal case.
	 */
	err |= __get_user(gs, &sc->gs);
	gs |= 3;
	savesegment(gs, oldgs);
	if (gs != oldgs)
		load_gs_index(gs);

	RELOAD_SEG(fs);
	RELOAD_SEG(ds);
	RELOAD_SEG(es);

	COPY(di); COPY(si); COPY(bp); COPY(sp); COPY(bx);
	COPY(dx); COPY(cx); COPY(ip);
	/* Don't touch extended registers */

	COPY_SEG_CPL3(cs);
	COPY_SEG_CPL3(ss);

	err |= __get_user(tmpflags, &sc->flags);
	regs->flags = (regs->flags & ~FIX_EFLAGS) | (tmpflags & FIX_EFLAGS);
	/* disable syscall checks */
	regs->orig_ax = -1;

	err |= __get_user(tmp, &sc->fpstate);
	buf = compat_ptr(tmp);
	err |= restore_i387_xstate_ia32(buf);

	err |= __get_user(*pax, &sc->ax);
	return err;
}

asmlinkage long sys32_sigreturn(struct pt_regs *regs)
{
	struct sigframe_ia32 __user *frame = (struct sigframe_ia32 __user *)(regs->sp-8);
	sigset_t set;
	unsigned int ax;

	if (!access_ok(VERIFY_READ, frame, sizeof(*frame)))
		goto badframe;
	if (__get_user(set.sig[0], &frame->sc.oldmask)
	    || (_COMPAT_NSIG_WORDS > 1
		&& __copy_from_user((((char *) &set.sig) + 4),
				    &frame->extramask,
				    sizeof(frame->extramask))))
		goto badframe;

	sigdelsetmask(&set, ~_BLOCKABLE);
	spin_lock_irq(&current->sighand->siglock);
	current->blocked = set;
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);

	if (ia32_restore_sigcontext(regs, &frame->sc, &ax))
		goto badframe;
	return ax;

badframe:
	signal_fault(regs, frame, "32bit sigreturn");
	return 0;
}

asmlinkage long sys32_rt_sigreturn(struct pt_regs *regs)
{
	struct rt_sigframe_ia32 __user *frame;
	sigset_t set;
	unsigned int ax;
	struct pt_regs tregs;

	frame = (struct rt_sigframe_ia32 __user *)(regs->sp - 4);

	if (!access_ok(VERIFY_READ, frame, sizeof(*frame)))
		goto badframe;
	if (__copy_from_user(&set, &frame->uc.uc_sigmask, sizeof(set)))
		goto badframe;

	sigdelsetmask(&set, ~_BLOCKABLE);
	spin_lock_irq(&current->sighand->siglock);
	current->blocked = set;
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);

	if (ia32_restore_sigcontext(regs, &frame->uc.uc_mcontext, &ax))
		goto badframe;

	tregs = *regs;
	if (sys32_sigaltstack(&frame->uc.uc_stack, NULL, &tregs) == -EFAULT)
		goto badframe;

	return ax;

badframe:
	signal_fault(regs, frame, "32bit rt sigreturn");
	return 0;
}

/*
 * Set up a signal frame.
 */

static int ia32_setup_sigcontext(struct sigcontext_ia32 __user *sc,
				 void __user *fpstate,
				 struct pt_regs *regs, unsigned int mask)
{
	int tmp, err = 0;

	savesegment(gs, tmp);
	err |= __put_user(tmp, (unsigned int __user *)&sc->gs);
	savesegment(fs, tmp);
	err |= __put_user(tmp, (unsigned int __user *)&sc->fs);
	savesegment(ds, tmp);
	err |= __put_user(tmp, (unsigned int __user *)&sc->ds);
	savesegment(es, tmp);
	err |= __put_user(tmp, (unsigned int __user *)&sc->es);

	err |= __put_user(regs->di, &sc->di);
	err |= __put_user(regs->si, &sc->si);
	err |= __put_user(regs->bp, &sc->bp);
	err |= __put_user(regs->sp, &sc->sp);
	err |= __put_user(regs->bx, &sc->bx);
	err |= __put_user(regs->dx, &sc->dx);
	err |= __put_user(regs->cx, &sc->cx);
	err |= __put_user(regs->ax, &sc->ax);
	err |= __put_user(current->thread.trap_no, &sc->trapno);
	err |= __put_user(current->thread.error_code, &sc->err);
	err |= __put_user(regs->ip, &sc->ip);
	err |= __put_user(regs->cs, (unsigned int __user *)&sc->cs);
	err |= __put_user(regs->flags, &sc->flags);
	err |= __put_user(regs->sp, &sc->sp_at_signal);
	err |= __put_user(regs->ss, (unsigned int __user *)&sc->ss);

	err |= __put_user(ptr_to_compat(fpstate), &sc->fpstate);

	/* non-iBCS2 extensions.. */
	err |= __put_user(mask, &sc->oldmask);
	err |= __put_user(current->thread.cr2, &sc->cr2);

	return err;
}

/*
 * Determine which stack to use..
 */
static void __user *get_sigframe(struct k_sigaction *ka, struct pt_regs *regs,
				 size_t frame_size,
				 void **fpstate)
{
	unsigned long sp;

	/* Default to using normal stack */
	sp = regs->sp;

	/* This is the X/Open sanctioned signal stack switching.  */
	if (ka->sa.sa_flags & SA_ONSTACK) {
		if (sas_ss_flags(sp) == 0)
			sp = current->sas_ss_sp + current->sas_ss_size;
	}

	/* This is the legacy signal stack switching. */
	else if ((regs->ss & 0xffff) != __USER32_DS &&
		!(ka->sa.sa_flags & SA_RESTORER) &&
		 ka->sa.sa_restorer)
		sp = (unsigned long) ka->sa.sa_restorer;

	if (used_math()) {
		sp = sp - sig_xstate_ia32_size;
		*fpstate = (struct _fpstate_ia32 *) sp;
		if (save_i387_xstate_ia32(*fpstate) < 0)
			return (void __user *) -1L;
	}

	sp -= frame_size;
	/* Align the stack pointer according to the i386 ABI,
	 * i.e. so that on function entry ((sp + 4) & 15) == 0. */
	sp = ((sp + 4) & -16ul) - 4;
	return (void __user *) sp;
}

int ia32_setup_frame(int sig, struct k_sigaction *ka,
		     compat_sigset_t *set, struct pt_regs *regs)
{
	struct sigframe_ia32 __user *frame;
	void __user *restorer;
	int err = 0;
	void __user *fpstate = NULL;

	/* copy_to_user optimizes that into a single 8 byte store */
	static const struct {
		u16 poplmovl;
		u32 val;
		u16 int80;
	} __attribute__((packed)) code = {
		0xb858,		 /* popl %eax ; movl $...,%eax */
		__NR_ia32_sigreturn,
		0x80cd,		/* int $0x80 */
	};

	frame = get_sigframe(ka, regs, sizeof(*frame), &fpstate);

	if (!access_ok(VERIFY_WRITE, frame, sizeof(*frame)))
		return -EFAULT;

	if (__put_user(sig, &frame->sig))
		return -EFAULT;

	if (ia32_setup_sigcontext(&frame->sc, fpstate, regs, set->sig[0]))
		return -EFAULT;

	if (_COMPAT_NSIG_WORDS > 1) {
		if (__copy_to_user(frame->extramask, &set->sig[1],
				   sizeof(frame->extramask)))
			return -EFAULT;
	}

	if (ka->sa.sa_flags & SA_RESTORER) {
		restorer = ka->sa.sa_restorer;
	} else {
		/* Return stub is in 32bit vsyscall page */
		if (current->mm->context.vdso)
			restorer = VDSO32_SYMBOL(current->mm->context.vdso,
						 sigreturn);
		else
			restorer = &frame->retcode;
	}
	err |= __put_user(ptr_to_compat(restorer), &frame->pretcode);

	/*
	 * These are actually not used anymore, but left because some
	 * gdb versions depend on them as a marker.
	 */
	err |= __put_user(*((u64 *)&code), (u64 *)frame->retcode);
	if (err)
		return -EFAULT;

	/* Set up registers for signal handler */
	regs->sp = (unsigned long) frame;
	regs->ip = (unsigned long) ka->sa.sa_handler;

	/* Make -mregparm=3 work */
	regs->ax = sig;
	regs->dx = 0;
	regs->cx = 0;

	loadsegment(ds, __USER32_DS);
	loadsegment(es, __USER32_DS);

	regs->cs = __USER32_CS;
	regs->ss = __USER32_DS;

#if DEBUG_SIG
	printk(KERN_DEBUG "SIG deliver (%s:%d): sp=%p pc=%lx ra=%u\n",
	       current->comm, current->pid, frame, regs->ip, frame->pretcode);
#endif

	return 0;
}

int ia32_setup_rt_frame(int sig, struct k_sigaction *ka, siginfo_t *info,
			compat_sigset_t *set, struct pt_regs *regs)
{
	struct rt_sigframe_ia32 __user *frame;
	void __user *restorer;
	int err = 0;
	void __user *fpstate = NULL;

	/* __copy_to_user optimizes that into a single 8 byte store */
	static const struct {
		u8 movl;
		u32 val;
		u16 int80;
		u8  pad;
	} __attribute__((packed)) code = {
		0xb8,
		__NR_ia32_rt_sigreturn,
		0x80cd,
		0,
	};

	frame = get_sigframe(ka, regs, sizeof(*frame), &fpstate);

	if (!access_ok(VERIFY_WRITE, frame, sizeof(*frame)))
		return -EFAULT;

	err |= __put_user(sig, &frame->sig);
	err |= __put_user(ptr_to_compat(&frame->info), &frame->pinfo);
	err |= __put_user(ptr_to_compat(&frame->uc), &frame->puc);
	err |= copy_siginfo_to_user32(&frame->info, info);
	if (err)
		return -EFAULT;

	/* Create the ucontext.  */
	if (cpu_has_xsave)
		err |= __put_user(UC_FP_XSTATE, &frame->uc.uc_flags);
	else
		err |= __put_user(0, &frame->uc.uc_flags);
	err |= __put_user(0, &frame->uc.uc_link);
	err |= __put_user(current->sas_ss_sp, &frame->uc.uc_stack.ss_sp);
	err |= __put_user(sas_ss_flags(regs->sp),
			  &frame->uc.uc_stack.ss_flags);
	err |= __put_user(current->sas_ss_size, &frame->uc.uc_stack.ss_size);
	err |= ia32_setup_sigcontext(&frame->uc.uc_mcontext, fpstate,
				     regs, set->sig[0]);
	err |= __copy_to_user(&frame->uc.uc_sigmask, set, sizeof(*set));
	if (err)
		return -EFAULT;

	if (ka->sa.sa_flags & SA_RESTORER)
		restorer = ka->sa.sa_restorer;
	else
		restorer = VDSO32_SYMBOL(current->mm->context.vdso,
					 rt_sigreturn);
	err |= __put_user(ptr_to_compat(restorer), &frame->pretcode);

	/*
	 * Not actually used anymore, but left because some gdb
	 * versions need it.
	 */
	err |= __put_user(*((u64 *)&code), (u64 *)frame->retcode);
	if (err)
		return -EFAULT;

	/* Set up registers for signal handler */
	regs->sp = (unsigned long) frame;
	regs->ip = (unsigned long) ka->sa.sa_handler;

	/* Make -mregparm=3 work */
	regs->ax = sig;
	regs->dx = (unsigned long) &frame->info;
	regs->cx = (unsigned long) &frame->uc;

	loadsegment(ds, __USER32_DS);
	loadsegment(es, __USER32_DS);

	regs->cs = __USER32_CS;
	regs->ss = __USER32_DS;

#if DEBUG_SIG
	printk(KERN_DEBUG "SIG deliver (%s:%d): sp=%p pc=%lx ra=%u\n",
	       current->comm, current->pid, frame, regs->ip, frame->pretcode);
#endif

	return 0;
}
