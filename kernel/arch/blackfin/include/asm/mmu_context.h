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
 * File:         include/asm-blackfin/mmu_context.h
 * Based on:
 * Author:
 *
 * Created:
 * Description:
 *
 * Modified:
 *               Copyright 2004-2006 Analog Devices Inc.
 *
 * Bugs:         Enter bugs at http://blackfin.uclinux.org/
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
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __BLACKFIN_MMU_CONTEXT_H__
#define __BLACKFIN_MMU_CONTEXT_H__

#include <linux/gfp.h>
#include <linux/sched.h>
#include <asm/setup.h>
#include <asm/page.h>
#include <asm/pgalloc.h>
#include <asm/cplbinit.h>

/* Note: L1 stacks are CPU-private things, so we bluntly disable this
   feature in SMP mode, and use the per-CPU scratch SRAM bank only to
   store the PDA instead. */

extern void *current_l1_stack_save;
extern int nr_l1stack_tasks;
extern void *l1_stack_base;
extern unsigned long l1_stack_len;

extern int l1sram_free(const void*);
extern void *l1sram_alloc_max(void*);

static inline void free_l1stack(void)
{
	nr_l1stack_tasks--;
	if (nr_l1stack_tasks == 0)
		l1sram_free(l1_stack_base);
}

static inline unsigned long
alloc_l1stack(unsigned long length, unsigned long *stack_base)
{
	if (nr_l1stack_tasks == 0) {
		l1_stack_base = l1sram_alloc_max(&l1_stack_len);
		if (!l1_stack_base)
			return 0;
	}

	if (l1_stack_len < length) {
		if (nr_l1stack_tasks == 0)
			l1sram_free(l1_stack_base);
		return 0;
	}
	*stack_base = (unsigned long)l1_stack_base;
	nr_l1stack_tasks++;
	return l1_stack_len;
}

static inline int
activate_l1stack(struct mm_struct *mm, unsigned long sp_base)
{
	if (current_l1_stack_save)
		memcpy(current_l1_stack_save, l1_stack_base, l1_stack_len);
	mm->context.l1_stack_save = current_l1_stack_save = (void*)sp_base;
	memcpy(l1_stack_base, current_l1_stack_save, l1_stack_len);
	return 1;
}

#define deactivate_mm(tsk,mm)	do { } while (0)

#define activate_mm(prev, next) switch_mm(prev, next, NULL)

static inline void switch_mm(struct mm_struct *prev_mm, struct mm_struct *next_mm,
			     struct task_struct *tsk)
{
#ifdef CONFIG_MPU
	unsigned int cpu = smp_processor_id();
#endif
	if (prev_mm == next_mm)
		return;
#ifdef CONFIG_MPU
	if (prev_mm->context.page_rwx_mask == current_rwx_mask[cpu]) {
		flush_switched_cplbs(cpu);
		set_mask_dcplbs(next_mm->context.page_rwx_mask, cpu);
	}
#endif

#ifdef CONFIG_APP_STACK_L1
	/* L1 stack switching.  */
	if (!next_mm->context.l1_stack_save)
		return;
	if (next_mm->context.l1_stack_save == current_l1_stack_save)
		return;
	if (current_l1_stack_save) {
		memcpy(current_l1_stack_save, l1_stack_base, l1_stack_len);
	}
	current_l1_stack_save = next_mm->context.l1_stack_save;
	memcpy(l1_stack_base, current_l1_stack_save, l1_stack_len);
#endif
}

#ifdef CONFIG_MPU
static inline void protect_page(struct mm_struct *mm, unsigned long addr,
				unsigned long flags)
{
	unsigned long *mask = mm->context.page_rwx_mask;
	unsigned long page = addr >> 12;
	unsigned long idx = page >> 5;
	unsigned long bit = 1 << (page & 31);

	if (flags & VM_MAYREAD)
		mask[idx] |= bit;
	else
		mask[idx] &= ~bit;
	mask += page_mask_nelts;
	if (flags & VM_MAYWRITE)
		mask[idx] |= bit;
	else
		mask[idx] &= ~bit;
	mask += page_mask_nelts;
	if (flags & VM_MAYEXEC)
		mask[idx] |= bit;
	else
		mask[idx] &= ~bit;
}

static inline void update_protections(struct mm_struct *mm)
{
	unsigned int cpu = smp_processor_id();
	if (mm->context.page_rwx_mask == current_rwx_mask[cpu]) {
		flush_switched_cplbs(cpu);
		set_mask_dcplbs(mm->context.page_rwx_mask, cpu);
	}
}
#endif

static inline void enter_lazy_tlb(struct mm_struct *mm, struct task_struct *tsk)
{
}

/* Called when creating a new context during fork() or execve().  */
static inline int
init_new_context(struct task_struct *tsk, struct mm_struct *mm)
{
#ifdef CONFIG_MPU
	unsigned long p = __get_free_pages(GFP_KERNEL, page_mask_order);
	mm->context.page_rwx_mask = (unsigned long *)p;
	memset(mm->context.page_rwx_mask, 0,
	       page_mask_nelts * 3 * sizeof(long));
#endif
	return 0;
}

static inline void destroy_context(struct mm_struct *mm)
{
	struct sram_list_struct *tmp;
#ifdef CONFIG_MPU
	unsigned int cpu = smp_processor_id();
#endif

#ifdef CONFIG_APP_STACK_L1
	if (current_l1_stack_save == mm->context.l1_stack_save)
		current_l1_stack_save = 0;
	if (mm->context.l1_stack_save)
		free_l1stack();
#endif

	while ((tmp = mm->context.sram_list)) {
		mm->context.sram_list = tmp->next;
		sram_free(tmp->addr);
		kfree(tmp);
	}
#ifdef CONFIG_MPU
	if (current_rwx_mask[cpu] == mm->context.page_rwx_mask)
		current_rwx_mask[cpu] = NULL;
	free_pages((unsigned long)mm->context.page_rwx_mask, page_mask_order);
#endif
}

#endif
