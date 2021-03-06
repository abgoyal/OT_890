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

#ifndef _ALPHA_LOCAL_H
#define _ALPHA_LOCAL_H

#include <linux/percpu.h>
#include <asm/atomic.h>

typedef struct
{
	atomic_long_t a;
} local_t;

#define LOCAL_INIT(i)	{ ATOMIC_LONG_INIT(i) }
#define local_read(l)	atomic_long_read(&(l)->a)
#define local_set(l,i)	atomic_long_set(&(l)->a, (i))
#define local_inc(l)	atomic_long_inc(&(l)->a)
#define local_dec(l)	atomic_long_dec(&(l)->a)
#define local_add(i,l)	atomic_long_add((i),(&(l)->a))
#define local_sub(i,l)	atomic_long_sub((i),(&(l)->a))

static __inline__ long local_add_return(long i, local_t * l)
{
	long temp, result;
	__asm__ __volatile__(
	"1:	ldq_l %0,%1\n"
	"	addq %0,%3,%2\n"
	"	addq %0,%3,%0\n"
	"	stq_c %0,%1\n"
	"	beq %0,2f\n"
	".subsection 2\n"
	"2:	br 1b\n"
	".previous"
	:"=&r" (temp), "=m" (l->a.counter), "=&r" (result)
	:"Ir" (i), "m" (l->a.counter) : "memory");
	return result;
}

static __inline__ long local_sub_return(long i, local_t * l)
{
	long temp, result;
	__asm__ __volatile__(
	"1:	ldq_l %0,%1\n"
	"	subq %0,%3,%2\n"
	"	subq %0,%3,%0\n"
	"	stq_c %0,%1\n"
	"	beq %0,2f\n"
	".subsection 2\n"
	"2:	br 1b\n"
	".previous"
	:"=&r" (temp), "=m" (l->a.counter), "=&r" (result)
	:"Ir" (i), "m" (l->a.counter) : "memory");
	return result;
}

#define local_cmpxchg(l, o, n) \
	(cmpxchg_local(&((l)->a.counter), (o), (n)))
#define local_xchg(l, n) (xchg_local(&((l)->a.counter), (n)))

/**
 * local_add_unless - add unless the number is a given value
 * @l: pointer of type local_t
 * @a: the amount to add to l...
 * @u: ...unless l is equal to u.
 *
 * Atomically adds @a to @l, so long as it was not @u.
 * Returns non-zero if @l was not @u, and zero otherwise.
 */
#define local_add_unless(l, a, u)				\
({								\
	long c, old;						\
	c = local_read(l);					\
	for (;;) {						\
		if (unlikely(c == (u)))				\
			break;					\
		old = local_cmpxchg((l), c, c + (a));	\
		if (likely(old == c))				\
			break;					\
		c = old;					\
	}							\
	c != (u);						\
})
#define local_inc_not_zero(l) local_add_unless((l), 1, 0)

#define local_add_negative(a, l) (local_add_return((a), (l)) < 0)

#define local_dec_return(l) local_sub_return(1,(l))

#define local_inc_return(l) local_add_return(1,(l))

#define local_sub_and_test(i,l) (local_sub_return((i), (l)) == 0)

#define local_inc_and_test(l) (local_add_return(1, (l)) == 0)

#define local_dec_and_test(l) (local_sub_return(1, (l)) == 0)

/* Verify if faster than atomic ops */
#define __local_inc(l)		((l)->a.counter++)
#define __local_dec(l)		((l)->a.counter++)
#define __local_add(i,l)	((l)->a.counter+=(i))
#define __local_sub(i,l)	((l)->a.counter-=(i))

/* Use these for per-cpu local_t variables: on some archs they are
 * much more efficient than these naive implementations.  Note they take
 * a variable, not an address.
 */
#define cpu_local_read(l)	local_read(&__get_cpu_var(l))
#define cpu_local_set(l, i)	local_set(&__get_cpu_var(l), (i))

#define cpu_local_inc(l)	local_inc(&__get_cpu_var(l))
#define cpu_local_dec(l)	local_dec(&__get_cpu_var(l))
#define cpu_local_add(i, l)	local_add((i), &__get_cpu_var(l))
#define cpu_local_sub(i, l)	local_sub((i), &__get_cpu_var(l))

#define __cpu_local_inc(l)	__local_inc(&__get_cpu_var(l))
#define __cpu_local_dec(l)	__local_dec(&__get_cpu_var(l))
#define __cpu_local_add(i, l)	__local_add((i), &__get_cpu_var(l))
#define __cpu_local_sub(i, l)	__local_sub((i), &__get_cpu_var(l))

#endif /* _ALPHA_LOCAL_H */
