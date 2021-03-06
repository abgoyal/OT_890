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

#ifndef __ASM_SH_ATOMIC_LLSC_H
#define __ASM_SH_ATOMIC_LLSC_H

/*
 * To get proper branch prediction for the main line, we must branch
 * forward to code at the end of this object's .text section, then
 * branch back to restart the operation.
 */
static inline void atomic_add(int i, atomic_t *v)
{
	unsigned long tmp;

	__asm__ __volatile__ (
"1:	movli.l @%2, %0		! atomic_add	\n"
"	add	%1, %0				\n"
"	movco.l	%0, @%2				\n"
"	bf	1b				\n"
	: "=&z" (tmp)
	: "r" (i), "r" (&v->counter)
	: "t");
}

static inline void atomic_sub(int i, atomic_t *v)
{
	unsigned long tmp;

	__asm__ __volatile__ (
"1:	movli.l @%2, %0		! atomic_sub	\n"
"	sub	%1, %0				\n"
"	movco.l	%0, @%2				\n"
"	bf	1b				\n"
	: "=&z" (tmp)
	: "r" (i), "r" (&v->counter)
	: "t");
}

/*
 * SH-4A note:
 *
 * We basically get atomic_xxx_return() for free compared with
 * atomic_xxx(). movli.l/movco.l require r0 due to the instruction
 * encoding, so the retval is automatically set without having to
 * do any special work.
 */
static inline int atomic_add_return(int i, atomic_t *v)
{
	unsigned long temp;

	__asm__ __volatile__ (
"1:	movli.l @%2, %0		! atomic_add_return	\n"
"	add	%1, %0					\n"
"	movco.l	%0, @%2					\n"
"	bf	1b					\n"
"	synco						\n"
	: "=&z" (temp)
	: "r" (i), "r" (&v->counter)
	: "t");

	return temp;
}

static inline int atomic_sub_return(int i, atomic_t *v)
{
	unsigned long temp;

	__asm__ __volatile__ (
"1:	movli.l @%2, %0		! atomic_sub_return	\n"
"	sub	%1, %0					\n"
"	movco.l	%0, @%2					\n"
"	bf	1b					\n"
"	synco						\n"
	: "=&z" (temp)
	: "r" (i), "r" (&v->counter)
	: "t");

	return temp;
}

static inline void atomic_clear_mask(unsigned int mask, atomic_t *v)
{
	unsigned long tmp;

	__asm__ __volatile__ (
"1:	movli.l @%2, %0		! atomic_clear_mask	\n"
"	and	%1, %0					\n"
"	movco.l	%0, @%2					\n"
"	bf	1b					\n"
	: "=&z" (tmp)
	: "r" (~mask), "r" (&v->counter)
	: "t");
}

static inline void atomic_set_mask(unsigned int mask, atomic_t *v)
{
	unsigned long tmp;

	__asm__ __volatile__ (
"1:	movli.l @%2, %0		! atomic_set_mask	\n"
"	or	%1, %0					\n"
"	movco.l	%0, @%2					\n"
"	bf	1b					\n"
	: "=&z" (tmp)
	: "r" (mask), "r" (&v->counter)
	: "t");
}

#endif /* __ASM_SH_ATOMIC_LLSC_H */
