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
 * Optimised mutex implementation of include/asm-generic/mutex-dec.h algorithm
 */
#ifndef _ASM_POWERPC_MUTEX_H
#define _ASM_POWERPC_MUTEX_H

static inline int __mutex_cmpxchg_lock(atomic_t *v, int old, int new)
{
	int t;

	__asm__ __volatile__ (
"1:	lwarx	%0,0,%1		# mutex trylock\n\
	cmpw	0,%0,%2\n\
	bne-	2f\n"
	PPC405_ERR77(0,%1)
"	stwcx.	%3,0,%1\n\
	bne-	1b"
	ISYNC_ON_SMP
	"\n\
2:"
	: "=&r" (t)
	: "r" (&v->counter), "r" (old), "r" (new)
	: "cc", "memory");

	return t;
}

static inline int __mutex_dec_return_lock(atomic_t *v)
{
	int t;

	__asm__ __volatile__(
"1:	lwarx	%0,0,%1		# mutex lock\n\
	addic	%0,%0,-1\n"
	PPC405_ERR77(0,%1)
"	stwcx.	%0,0,%1\n\
	bne-	1b"
	ISYNC_ON_SMP
	: "=&r" (t)
	: "r" (&v->counter)
	: "cc", "memory");

	return t;
}

static inline int __mutex_inc_return_unlock(atomic_t *v)
{
	int t;

	__asm__ __volatile__(
	LWSYNC_ON_SMP
"1:	lwarx	%0,0,%1		# mutex unlock\n\
	addic	%0,%0,1\n"
	PPC405_ERR77(0,%1)
"	stwcx.	%0,0,%1 \n\
	bne-	1b"
	: "=&r" (t)
	: "r" (&v->counter)
	: "cc", "memory");

	return t;
}

/**
 *  __mutex_fastpath_lock - try to take the lock by moving the count
 *                          from 1 to a 0 value
 *  @count: pointer of type atomic_t
 *  @fail_fn: function to call if the original value was not 1
 *
 * Change the count from 1 to a value lower than 1, and call <fail_fn> if
 * it wasn't 1 originally. This function MUST leave the value lower than
 * 1 even when the "1" assertion wasn't true.
 */
static inline void
__mutex_fastpath_lock(atomic_t *count, void (*fail_fn)(atomic_t *))
{
	if (unlikely(__mutex_dec_return_lock(count) < 0))
		fail_fn(count);
}

/**
 *  __mutex_fastpath_lock_retval - try to take the lock by moving the count
 *                                 from 1 to a 0 value
 *  @count: pointer of type atomic_t
 *  @fail_fn: function to call if the original value was not 1
 *
 * Change the count from 1 to a value lower than 1, and call <fail_fn> if
 * it wasn't 1 originally. This function returns 0 if the fastpath succeeds,
 * or anything the slow path function returns.
 */
static inline int
__mutex_fastpath_lock_retval(atomic_t *count, int (*fail_fn)(atomic_t *))
{
	if (unlikely(__mutex_dec_return_lock(count) < 0))
		return fail_fn(count);
	return 0;
}

/**
 *  __mutex_fastpath_unlock - try to promote the count from 0 to 1
 *  @count: pointer of type atomic_t
 *  @fail_fn: function to call if the original value was not 0
 *
 * Try to promote the count from 0 to 1. If it wasn't 0, call <fail_fn>.
 * In the failure case, this function is allowed to either set the value to
 * 1, or to set it to a value lower than 1.
 */
static inline void
__mutex_fastpath_unlock(atomic_t *count, void (*fail_fn)(atomic_t *))
{
	if (unlikely(__mutex_inc_return_unlock(count) <= 0))
		fail_fn(count);
}

#define __mutex_slowpath_needs_to_unlock()		1

/**
 * __mutex_fastpath_trylock - try to acquire the mutex, without waiting
 *
 *  @count: pointer of type atomic_t
 *  @fail_fn: fallback function
 *
 * Change the count from 1 to 0, and return 1 (success), or if the count
 * was not 1, then return 0 (failure).
 */
static inline int
__mutex_fastpath_trylock(atomic_t *count, int (*fail_fn)(atomic_t *))
{
	if (likely(__mutex_cmpxchg_lock(count, 1, 0) == 1))
		return 1;
	return 0;
}

#endif
