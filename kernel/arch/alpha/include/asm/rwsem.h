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

#ifndef _ALPHA_RWSEM_H
#define _ALPHA_RWSEM_H

/*
 * Written by Ivan Kokshaysky <ink@jurassic.park.msu.ru>, 2001.
 * Based on asm-alpha/semaphore.h and asm-i386/rwsem.h
 */

#ifndef _LINUX_RWSEM_H
#error "please don't include asm/rwsem.h directly, use linux/rwsem.h instead"
#endif

#ifdef __KERNEL__

#include <linux/compiler.h>
#include <linux/list.h>
#include <linux/spinlock.h>

struct rwsem_waiter;

extern struct rw_semaphore *rwsem_down_read_failed(struct rw_semaphore *sem);
extern struct rw_semaphore *rwsem_down_write_failed(struct rw_semaphore *sem);
extern struct rw_semaphore *rwsem_wake(struct rw_semaphore *);
extern struct rw_semaphore *rwsem_downgrade_wake(struct rw_semaphore *sem);

/*
 * the semaphore definition
 */
struct rw_semaphore {
	long			count;
#define RWSEM_UNLOCKED_VALUE		0x0000000000000000L
#define RWSEM_ACTIVE_BIAS		0x0000000000000001L
#define RWSEM_ACTIVE_MASK		0x00000000ffffffffL
#define RWSEM_WAITING_BIAS		(-0x0000000100000000L)
#define RWSEM_ACTIVE_READ_BIAS		RWSEM_ACTIVE_BIAS
#define RWSEM_ACTIVE_WRITE_BIAS		(RWSEM_WAITING_BIAS + RWSEM_ACTIVE_BIAS)
	spinlock_t		wait_lock;
	struct list_head	wait_list;
};

#define __RWSEM_INITIALIZER(name) \
	{ RWSEM_UNLOCKED_VALUE, SPIN_LOCK_UNLOCKED, \
	LIST_HEAD_INIT((name).wait_list) }

#define DECLARE_RWSEM(name) \
	struct rw_semaphore name = __RWSEM_INITIALIZER(name)

static inline void init_rwsem(struct rw_semaphore *sem)
{
	sem->count = RWSEM_UNLOCKED_VALUE;
	spin_lock_init(&sem->wait_lock);
	INIT_LIST_HEAD(&sem->wait_list);
}

static inline void __down_read(struct rw_semaphore *sem)
{
	long oldcount;
#ifndef	CONFIG_SMP
	oldcount = sem->count;
	sem->count += RWSEM_ACTIVE_READ_BIAS;
#else
	long temp;
	__asm__ __volatile__(
	"1:	ldq_l	%0,%1\n"
	"	addq	%0,%3,%2\n"
	"	stq_c	%2,%1\n"
	"	beq	%2,2f\n"
	"	mb\n"
	".subsection 2\n"
	"2:	br	1b\n"
	".previous"
	:"=&r" (oldcount), "=m" (sem->count), "=&r" (temp)
	:"Ir" (RWSEM_ACTIVE_READ_BIAS), "m" (sem->count) : "memory");
#endif
	if (unlikely(oldcount < 0))
		rwsem_down_read_failed(sem);
}

/*
 * trylock for reading -- returns 1 if successful, 0 if contention
 */
static inline int __down_read_trylock(struct rw_semaphore *sem)
{
	long old, new, res;

	res = sem->count;
	do {
		new = res + RWSEM_ACTIVE_READ_BIAS;
		if (new <= 0)
			break;
		old = res;
		res = cmpxchg(&sem->count, old, new);
	} while (res != old);
	return res >= 0 ? 1 : 0;
}

static inline void __down_write(struct rw_semaphore *sem)
{
	long oldcount;
#ifndef	CONFIG_SMP
	oldcount = sem->count;
	sem->count += RWSEM_ACTIVE_WRITE_BIAS;
#else
	long temp;
	__asm__ __volatile__(
	"1:	ldq_l	%0,%1\n"
	"	addq	%0,%3,%2\n"
	"	stq_c	%2,%1\n"
	"	beq	%2,2f\n"
	"	mb\n"
	".subsection 2\n"
	"2:	br	1b\n"
	".previous"
	:"=&r" (oldcount), "=m" (sem->count), "=&r" (temp)
	:"Ir" (RWSEM_ACTIVE_WRITE_BIAS), "m" (sem->count) : "memory");
#endif
	if (unlikely(oldcount))
		rwsem_down_write_failed(sem);
}

/*
 * trylock for writing -- returns 1 if successful, 0 if contention
 */
static inline int __down_write_trylock(struct rw_semaphore *sem)
{
	long ret = cmpxchg(&sem->count, RWSEM_UNLOCKED_VALUE,
			   RWSEM_ACTIVE_WRITE_BIAS);
	if (ret == RWSEM_UNLOCKED_VALUE)
		return 1;
	return 0;
}

static inline void __up_read(struct rw_semaphore *sem)
{
	long oldcount;
#ifndef	CONFIG_SMP
	oldcount = sem->count;
	sem->count -= RWSEM_ACTIVE_READ_BIAS;
#else
	long temp;
	__asm__ __volatile__(
	"	mb\n"
	"1:	ldq_l	%0,%1\n"
	"	subq	%0,%3,%2\n"
	"	stq_c	%2,%1\n"
	"	beq	%2,2f\n"
	".subsection 2\n"
	"2:	br	1b\n"
	".previous"
	:"=&r" (oldcount), "=m" (sem->count), "=&r" (temp)
	:"Ir" (RWSEM_ACTIVE_READ_BIAS), "m" (sem->count) : "memory");
#endif
	if (unlikely(oldcount < 0))
		if ((int)oldcount - RWSEM_ACTIVE_READ_BIAS == 0)
			rwsem_wake(sem);
}

static inline void __up_write(struct rw_semaphore *sem)
{
	long count;
#ifndef	CONFIG_SMP
	sem->count -= RWSEM_ACTIVE_WRITE_BIAS;
	count = sem->count;
#else
	long temp;
	__asm__ __volatile__(
	"	mb\n"
	"1:	ldq_l	%0,%1\n"
	"	subq	%0,%3,%2\n"
	"	stq_c	%2,%1\n"
	"	beq	%2,2f\n"
	"	subq	%0,%3,%0\n"
	".subsection 2\n"
	"2:	br	1b\n"
	".previous"
	:"=&r" (count), "=m" (sem->count), "=&r" (temp)
	:"Ir" (RWSEM_ACTIVE_WRITE_BIAS), "m" (sem->count) : "memory");
#endif
	if (unlikely(count))
		if ((int)count == 0)
			rwsem_wake(sem);
}

/*
 * downgrade write lock to read lock
 */
static inline void __downgrade_write(struct rw_semaphore *sem)
{
	long oldcount;
#ifndef	CONFIG_SMP
	oldcount = sem->count;
	sem->count -= RWSEM_WAITING_BIAS;
#else
	long temp;
	__asm__ __volatile__(
	"1:	ldq_l	%0,%1\n"
	"	addq	%0,%3,%2\n"
	"	stq_c	%2,%1\n"
	"	beq	%2,2f\n"
	"	mb\n"
	".subsection 2\n"
	"2:	br	1b\n"
	".previous"
	:"=&r" (oldcount), "=m" (sem->count), "=&r" (temp)
	:"Ir" (-RWSEM_WAITING_BIAS), "m" (sem->count) : "memory");
#endif
	if (unlikely(oldcount < 0))
		rwsem_downgrade_wake(sem);
}

static inline void rwsem_atomic_add(long val, struct rw_semaphore *sem)
{
#ifndef	CONFIG_SMP
	sem->count += val;
#else
	long temp;
	__asm__ __volatile__(
	"1:	ldq_l	%0,%1\n"
	"	addq	%0,%2,%0\n"
	"	stq_c	%0,%1\n"
	"	beq	%0,2f\n"
	".subsection 2\n"
	"2:	br	1b\n"
	".previous"
	:"=&r" (temp), "=m" (sem->count)
	:"Ir" (val), "m" (sem->count));
#endif
}

static inline long rwsem_atomic_update(long val, struct rw_semaphore *sem)
{
#ifndef	CONFIG_SMP
	sem->count += val;
	return sem->count;
#else
	long ret, temp;
	__asm__ __volatile__(
	"1:	ldq_l	%0,%1\n"
	"	addq 	%0,%3,%2\n"
	"	addq	%0,%3,%0\n"
	"	stq_c	%2,%1\n"
	"	beq	%2,2f\n"
	".subsection 2\n"
	"2:	br	1b\n"
	".previous"
	:"=&r" (ret), "=m" (sem->count), "=&r" (temp)
	:"Ir" (val), "m" (sem->count));

	return ret;
#endif
}

static inline int rwsem_is_locked(struct rw_semaphore *sem)
{
	return (sem->count != 0);
}

#endif /* __KERNEL__ */
#endif /* _ALPHA_RWSEM_H */
