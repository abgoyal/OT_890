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

/******************************************************************************
 * Copyright (c) 2008 Isaku Yamahata <yamahata at valinux co jp>
 *                    VA Linux Systems Japan K.K.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef _ASM_IA64_PARAVIRT_PRIVOP_H
#define _ASM_IA64_PARAVIRT_PRIVOP_H

#ifdef CONFIG_PARAVIRT

#ifndef __ASSEMBLY__

#include <linux/types.h>
#include <asm/kregs.h> /* for IA64_PSR_I */

/******************************************************************************
 * replacement of intrinsics operations.
 */

struct pv_cpu_ops {
	void (*fc)(unsigned long addr);
	unsigned long (*thash)(unsigned long addr);
	unsigned long (*get_cpuid)(int index);
	unsigned long (*get_pmd)(int index);
	unsigned long (*getreg)(int reg);
	void (*setreg)(int reg, unsigned long val);
	void (*ptcga)(unsigned long addr, unsigned long size);
	unsigned long (*get_rr)(unsigned long index);
	void (*set_rr)(unsigned long index, unsigned long val);
	void (*set_rr0_to_rr4)(unsigned long val0, unsigned long val1,
			       unsigned long val2, unsigned long val3,
			       unsigned long val4);
	void (*ssm_i)(void);
	void (*rsm_i)(void);
	unsigned long (*get_psr_i)(void);
	void (*intrin_local_irq_restore)(unsigned long flags);
};

extern struct pv_cpu_ops pv_cpu_ops;

extern void ia64_native_setreg_func(int regnum, unsigned long val);
extern unsigned long ia64_native_getreg_func(int regnum);

/************************************************/
/* Instructions paravirtualized for performance */
/************************************************/

/* mask for ia64_native_ssm/rsm() must be constant.("i" constraing).
 * static inline function doesn't satisfy it. */
#define paravirt_ssm(mask)			\
	do {					\
		if ((mask) == IA64_PSR_I)	\
			pv_cpu_ops.ssm_i();	\
		else				\
			ia64_native_ssm(mask);	\
	} while (0)

#define paravirt_rsm(mask)			\
	do {					\
		if ((mask) == IA64_PSR_I)	\
			pv_cpu_ops.rsm_i();	\
		else				\
			ia64_native_rsm(mask);	\
	} while (0)

/* returned ip value should be the one in the caller,
 * not in __paravirt_getreg() */
#define paravirt_getreg(reg)					\
	({							\
		unsigned long res;				\
		if ((reg) == _IA64_REG_IP)			\
			res = ia64_native_getreg(_IA64_REG_IP); \
		else						\
			res = pv_cpu_ops.getreg(reg);		\
		res;						\
	})

/******************************************************************************
 * replacement of hand written assembly codes.
 */
struct pv_cpu_asm_switch {
	unsigned long switch_to;
	unsigned long leave_syscall;
	unsigned long work_processed_syscall;
	unsigned long leave_kernel;
};
void paravirt_cpu_asm_init(const struct pv_cpu_asm_switch *cpu_asm_switch);

#endif /* __ASSEMBLY__ */

#define IA64_PARAVIRT_ASM_FUNC(name)	paravirt_ ## name

#else

/* fallback for native case */
#define IA64_PARAVIRT_ASM_FUNC(name)	ia64_native_ ## name

#endif /* CONFIG_PARAVIRT */

/* these routines utilize privilege-sensitive or performance-sensitive
 * privileged instructions so the code must be replaced with
 * paravirtualized versions */
#define ia64_switch_to			IA64_PARAVIRT_ASM_FUNC(switch_to)
#define ia64_leave_syscall		IA64_PARAVIRT_ASM_FUNC(leave_syscall)
#define ia64_work_processed_syscall	\
	IA64_PARAVIRT_ASM_FUNC(work_processed_syscall)
#define ia64_leave_kernel		IA64_PARAVIRT_ASM_FUNC(leave_kernel)

#endif /* _ASM_IA64_PARAVIRT_PRIVOP_H */
