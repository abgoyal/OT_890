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
 * locks.S: SMP low-level lock primitives on Sparc.
 *
 * Copyright (C) 1996 David S. Miller (davem@caip.rutgers.edu)
 * Copyright (C) 1998 Anton Blanchard (anton@progsoc.uts.edu.au)
 * Copyright (C) 1998 Jakub Jelinek   (jj@ultra.linux.cz)
 */

#include <asm/ptrace.h>
#include <asm/psr.h>
#include <asm/smp.h>
#include <asm/spinlock.h>

	.text
	.align	4

	/* Read/writer locks, as usual this is overly clever to make it
	 * as fast as possible.
	 */

	/* caches... */
___rw_read_enter_spin_on_wlock:
	orcc	%g2, 0x0, %g0
	be,a	___rw_read_enter
	 ldstub	[%g1 + 3], %g2
	b	___rw_read_enter_spin_on_wlock
	 ldub	[%g1 + 3], %g2
___rw_read_try_spin_on_wlock:
	andcc	%g2, 0xff, %g0
	be,a	___rw_read_try
	 ldstub	[%g1 + 3], %g2
	xnorcc	%g2, 0x0, %o0	/* if g2 is ~0, set o0 to 0 and bugger off */
	bne,a	___rw_read_enter_spin_on_wlock
	 ld	[%g1], %g2
	retl
	 mov	%g4, %o7
___rw_read_exit_spin_on_wlock:
	orcc	%g2, 0x0, %g0
	be,a	___rw_read_exit
	 ldstub	[%g1 + 3], %g2
	b	___rw_read_exit_spin_on_wlock
	 ldub	[%g1 + 3], %g2
___rw_write_enter_spin_on_wlock:
	orcc	%g2, 0x0, %g0
	be,a	___rw_write_enter
	 ldstub	[%g1 + 3], %g2
	b	___rw_write_enter_spin_on_wlock
	 ld	[%g1], %g2

	.globl	___rw_read_enter
___rw_read_enter:
	orcc	%g2, 0x0, %g0
	bne,a	___rw_read_enter_spin_on_wlock
	 ldub	[%g1 + 3], %g2
	ld	[%g1], %g2
	add	%g2, 1, %g2
	st	%g2, [%g1]
	retl
	 mov	%g4, %o7

	.globl	___rw_read_exit
___rw_read_exit:
	orcc	%g2, 0x0, %g0
	bne,a	___rw_read_exit_spin_on_wlock
	 ldub	[%g1 + 3], %g2
	ld	[%g1], %g2
	sub	%g2, 0x1ff, %g2
	st	%g2, [%g1]
	retl
	 mov	%g4, %o7

	.globl	___rw_read_try
___rw_read_try:
	orcc	%g2, 0x0, %g0
	bne	___rw_read_try_spin_on_wlock
	 ld	[%g1], %g2
	add	%g2, 1, %g2
	st	%g2, [%g1]
	set	1, %o1
	retl
	 mov	%g4, %o7

	.globl	___rw_write_enter
___rw_write_enter:
	orcc	%g2, 0x0, %g0
	bne	___rw_write_enter_spin_on_wlock
	 ld	[%g1], %g2
	andncc	%g2, 0xff, %g0
	bne,a	___rw_write_enter_spin_on_wlock
	 stb	%g0, [%g1 + 3]
	retl
	 mov	%g4, %o7
