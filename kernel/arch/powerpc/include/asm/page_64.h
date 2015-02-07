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

#ifndef _ASM_POWERPC_PAGE_64_H
#define _ASM_POWERPC_PAGE_64_H

/*
 * Copyright (C) 2001 PPC64 Team, IBM Corp
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

/*
 * We always define HW_PAGE_SHIFT to 12 as use of 64K pages remains Linux
 * specific, every notion of page number shared with the firmware, TCEs,
 * iommu, etc... still uses a page size of 4K.
 */
#define HW_PAGE_SHIFT		12
#define HW_PAGE_SIZE		(ASM_CONST(1) << HW_PAGE_SHIFT)
#define HW_PAGE_MASK		(~(HW_PAGE_SIZE-1))

/*
 * PAGE_FACTOR is the number of bits factor between PAGE_SHIFT and
 * HW_PAGE_SHIFT, that is 4K pages.
 */
#define PAGE_FACTOR		(PAGE_SHIFT - HW_PAGE_SHIFT)

/* Segment size; normal 256M segments */
#define SID_SHIFT		28
#define SID_MASK		ASM_CONST(0xfffffffff)
#define ESID_MASK		0xfffffffff0000000UL
#define GET_ESID(x)		(((x) >> SID_SHIFT) & SID_MASK)

/* 1T segments */
#define SID_SHIFT_1T		40
#define SID_MASK_1T		0xffffffUL
#define ESID_MASK_1T		0xffffff0000000000UL
#define GET_ESID_1T(x)		(((x) >> SID_SHIFT_1T) & SID_MASK_1T)

#ifndef __ASSEMBLY__
#include <asm/cache.h>

typedef unsigned long pte_basic_t;

static __inline__ void clear_page(void *addr)
{
	unsigned long lines, line_size;

	line_size = ppc64_caches.dline_size;
	lines = ppc64_caches.dlines_per_page;

	__asm__ __volatile__(
	"mtctr	%1	# clear_page\n\
1:      dcbz	0,%0\n\
	add	%0,%0,%3\n\
	bdnz+	1b"
        : "=r" (addr)
        : "r" (lines), "0" (addr), "r" (line_size)
	: "ctr", "memory");
}

extern void copy_4K_page(void *to, void *from);

#ifdef CONFIG_PPC_64K_PAGES
static inline void copy_page(void *to, void *from)
{
	unsigned int i;
	for (i=0; i < (1 << (PAGE_SHIFT - 12)); i++) {
		copy_4K_page(to, from);
		to += 4096;
		from += 4096;
	}
}
#else /* CONFIG_PPC_64K_PAGES */
static inline void copy_page(void *to, void *from)
{
	copy_4K_page(to, from);
}
#endif /* CONFIG_PPC_64K_PAGES */

/* Log 2 of page table size */
extern u64 ppc64_pft_size;

/* Large pages size */
#ifdef CONFIG_HUGETLB_PAGE
extern unsigned int HPAGE_SHIFT;
#else
#define HPAGE_SHIFT PAGE_SHIFT
#endif
#define HPAGE_SIZE		((1UL) << HPAGE_SHIFT)
#define HPAGE_MASK		(~(HPAGE_SIZE - 1))
#define HUGETLB_PAGE_ORDER	(HPAGE_SHIFT - PAGE_SHIFT)
#define HUGE_MAX_HSTATE		3

#endif /* __ASSEMBLY__ */

#ifdef CONFIG_PPC_MM_SLICES

#define SLICE_LOW_SHIFT		28
#define SLICE_HIGH_SHIFT	40

#define SLICE_LOW_TOP		(0x100000000ul)
#define SLICE_NUM_LOW		(SLICE_LOW_TOP >> SLICE_LOW_SHIFT)
#define SLICE_NUM_HIGH		(PGTABLE_RANGE >> SLICE_HIGH_SHIFT)

#define GET_LOW_SLICE_INDEX(addr)	((addr) >> SLICE_LOW_SHIFT)
#define GET_HIGH_SLICE_INDEX(addr)	((addr) >> SLICE_HIGH_SHIFT)

#ifndef __ASSEMBLY__

struct slice_mask {
	u16 low_slices;
	u16 high_slices;
};

struct mm_struct;

extern unsigned long slice_get_unmapped_area(unsigned long addr,
					     unsigned long len,
					     unsigned long flags,
					     unsigned int psize,
					     int topdown,
					     int use_cache);

extern unsigned int get_slice_psize(struct mm_struct *mm,
				    unsigned long addr);

extern void slice_init_context(struct mm_struct *mm, unsigned int psize);
extern void slice_set_user_psize(struct mm_struct *mm, unsigned int psize);
extern void slice_set_range_psize(struct mm_struct *mm, unsigned long start,
				  unsigned long len, unsigned int psize);

#define slice_mm_new_context(mm)	((mm)->context.id == 0)

#endif /* __ASSEMBLY__ */
#else
#define slice_init()
#define get_slice_psize(mm, addr)	((mm)->context.user_psize)
#define slice_set_user_psize(mm, psize)		\
do {						\
	(mm)->context.user_psize = (psize);	\
	(mm)->context.sllp = SLB_VSID_USER | mmu_psize_defs[(psize)].sllp; \
} while (0)
#define slice_set_range_psize(mm, start, len, psize)	\
	slice_set_user_psize((mm), (psize))
#define slice_mm_new_context(mm)	1
#endif /* CONFIG_PPC_MM_SLICES */

#ifdef CONFIG_HUGETLB_PAGE

#define HAVE_ARCH_HUGETLB_UNMAPPED_AREA

#endif /* !CONFIG_HUGETLB_PAGE */

#ifdef MODULE
#define __page_aligned __attribute__((__aligned__(PAGE_SIZE)))
#else
#define __page_aligned \
	__attribute__((__aligned__(PAGE_SIZE), \
		__section__(".data.page_aligned")))
#endif

#define VM_DATA_DEFAULT_FLAGS \
	(test_thread_flag(TIF_32BIT) ? \
	 VM_DATA_DEFAULT_FLAGS32 : VM_DATA_DEFAULT_FLAGS64)

/*
 * This is the default if a program doesn't have a PT_GNU_STACK
 * program header entry. The PPC64 ELF ABI has a non executable stack
 * stack by default, so in the absense of a PT_GNU_STACK program header
 * we turn execute permission off.
 */
#define VM_STACK_DEFAULT_FLAGS32	(VM_READ | VM_WRITE | VM_EXEC | \
					 VM_MAYREAD | VM_MAYWRITE | VM_MAYEXEC)

#define VM_STACK_DEFAULT_FLAGS64	(VM_READ | VM_WRITE | \
					 VM_MAYREAD | VM_MAYWRITE | VM_MAYEXEC)

#define VM_STACK_DEFAULT_FLAGS \
	(test_thread_flag(TIF_32BIT) ? \
	 VM_STACK_DEFAULT_FLAGS32 : VM_STACK_DEFAULT_FLAGS64)

#include <asm-generic/page.h>

#endif /* _ASM_POWERPC_PAGE_64_H */
