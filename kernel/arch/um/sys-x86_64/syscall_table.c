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
 * System call table for UML/x86-64, copied from arch/x86_64/kernel/syscall.c
 * with some changes for UML.
 */

#include <linux/linkage.h>
#include <linux/sys.h>
#include <linux/cache.h>
#include <kern_constants.h>

#define __NO_STUBS

/*
 * Below you can see, in terms of #define's, the differences between the x86-64
 * and the UML syscall table.
 */

/* Not going to be implemented by UML, since we have no hardware. */
#define stub_iopl sys_ni_syscall
#define sys_ioperm sys_ni_syscall

/*
 * The UML TLS problem. Note that x86_64 does not implement this, so the below
 * is needed only for the ia32 compatibility.
 */

/* On UML we call it this way ("old" means it's not mmap2) */
#define sys_mmap old_mmap
/*
 * On x86-64 sys_uname is actually sys_newuname plus a compatibility trick.
 * See arch/x86_64/kernel/sys_x86_64.c
 */
#define sys_uname sys_uname64

#define stub_clone sys_clone
#define stub_fork sys_fork
#define stub_vfork sys_vfork
#define stub_execve sys_execve
#define stub_rt_sigsuspend sys_rt_sigsuspend
#define stub_sigaltstack sys_sigaltstack
#define stub_rt_sigreturn sys_rt_sigreturn

#define __SYSCALL(nr, sym) extern asmlinkage void sym(void) ;
#undef _ASM_X86_UNISTD_64_H
#include "../../x86/include/asm/unistd_64.h"

#undef __SYSCALL
#define __SYSCALL(nr, sym) [ nr ] = sym,
#undef _ASM_X86_UNISTD_64_H

typedef void (*sys_call_ptr_t)(void);

extern void sys_ni_syscall(void);

/*
 * We used to have a trick here which made sure that holes in the
 * x86_64 table were filled in with sys_ni_syscall, but a comment in
 * unistd_64.h says that holes aren't allowed, so the trick was
 * removed.
 * The trick looked like this
 *	[0 ... UM_NR_syscall_max] = &sys_ni_syscall
 * before including unistd_64.h - the later initializations overwrote
 * the sys_ni_syscall filler.
 */

sys_call_ptr_t sys_call_table[] __cacheline_aligned = {
#include "../../x86/include/asm/unistd_64.h"
};

int syscall_table_size = sizeof(sys_call_table);
