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
 * include/asm-xtensa/bitops.h
 *
 * Atomic operations that C can't guarantee us.Useful for resource counting etc.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2001 - 2007 Tensilica Inc.
 */

#ifndef _XTENSA_BITOPS_H
#define _XTENSA_BITOPS_H

#ifdef __KERNEL__

#ifndef _LINUX_BITOPS_H
#error only <linux/bitops.h> can be included directly
#endif

#include <asm/processor.h>
#include <asm/byteorder.h>
#include <asm/system.h>

#ifdef CONFIG_SMP
# error SMP not supported on this architecture
#endif

#define smp_mb__before_clear_bit()	barrier()
#define smp_mb__after_clear_bit()	barrier()

#include <asm-generic/bitops/atomic.h>
#include <asm-generic/bitops/non-atomic.h>

#if XCHAL_HAVE_NSA

static inline unsigned long __cntlz (unsigned long x)
{
	int lz;
	asm ("nsau %0, %1" : "=r" (lz) : "r" (x));
	return lz;
}

/*
 * ffz: Find first zero in word. Undefined if no zero exists.
 * bit 0 is the LSB of addr; bit 32 is the LSB of (addr+1).
 */

static inline int ffz(unsigned long x)
{
	return 31 - __cntlz(~x & -~x);
}

/*
 * __ffs: Find first bit set in word. Return 0 for bit 0
 */

static inline int __ffs(unsigned long x)
{
	return 31 - __cntlz(x & -x);
}

/*
 * ffs: Find first bit set in word. This is defined the same way as
 * the libc and compiler builtin ffs routines, therefore
 * differs in spirit from the above ffz (man ffs).
 */

static inline int ffs(unsigned long x)
{
	return 32 - __cntlz(x & -x);
}

/*
 * fls: Find last (most-significant) bit set in word.
 * Note fls(0) = 0, fls(1) = 1, fls(0x80000000) = 32.
 */

static inline int fls (unsigned int x)
{
	return 32 - __cntlz(x);
}

/**
 * __fls - find last (most-significant) set bit in a long word
 * @word: the word to search
 *
 * Undefined if no set bit exists, so code should check against 0 first.
 */
static inline unsigned long __fls(unsigned long word)
{
	return 31 - __cntlz(word);
}
#else

/* Use the generic implementation if we don't have the nsa/nsau instructions. */

# include <asm-generic/bitops/ffs.h>
# include <asm-generic/bitops/__ffs.h>
# include <asm-generic/bitops/ffz.h>
# include <asm-generic/bitops/fls.h>
# include <asm-generic/bitops/__fls.h>

#endif

#include <asm-generic/bitops/fls64.h>
#include <asm-generic/bitops/find.h>
#include <asm-generic/bitops/ext2-non-atomic.h>

#ifdef __XTENSA_EL__
# define ext2_set_bit_atomic(lock,nr,addr)				\
	test_and_set_bit((nr), (unsigned long*)(addr))
# define ext2_clear_bit_atomic(lock,nr,addr)				\
	test_and_clear_bit((nr), (unsigned long*)(addr))
#elif defined(__XTENSA_EB__)
# define ext2_set_bit_atomic(lock,nr,addr)				\
	test_and_set_bit((nr) ^ 0x18, (unsigned long*)(addr))
# define ext2_clear_bit_atomic(lock,nr,addr)				\
	test_and_clear_bit((nr) ^ 0x18, (unsigned long*)(addr))
#else
# error processor byte order undefined!
#endif

#include <asm-generic/bitops/hweight.h>
#include <asm-generic/bitops/lock.h>
#include <asm-generic/bitops/sched.h>
#include <asm-generic/bitops/minix.h>

#endif	/* __KERNEL__ */

#endif	/* _XTENSA_BITOPS_H */
