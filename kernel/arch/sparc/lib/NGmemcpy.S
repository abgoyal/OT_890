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

/* NGmemcpy.S: Niagara optimized memcpy.
 *
 * Copyright (C) 2006, 2007 David S. Miller (davem@davemloft.net)
 */

#ifdef __KERNEL__
#include <asm/asi.h>
#include <asm/thread_info.h>
#define GLOBAL_SPARE	%g7
#define RESTORE_ASI(TMP)	\
	ldub	[%g6 + TI_CURRENT_DS], TMP;  \
	wr	TMP, 0x0, %asi;
#else
#define GLOBAL_SPARE	%g5
#define RESTORE_ASI(TMP)	\
	wr	%g0, ASI_PNF, %asi
#endif

#ifdef __sparc_v9__
#define SAVE_AMOUNT	128
#else
#define SAVE_AMOUNT	64
#endif

#ifndef STORE_ASI
#define STORE_ASI	ASI_BLK_INIT_QUAD_LDD_P
#endif

#ifndef EX_LD
#define EX_LD(x)	x
#endif

#ifndef EX_ST
#define EX_ST(x)	x
#endif

#ifndef EX_RETVAL
#define EX_RETVAL(x)	x
#endif

#ifndef LOAD
#ifndef MEMCPY_DEBUG
#define LOAD(type,addr,dest)	type [addr], dest
#else
#define LOAD(type,addr,dest)	type##a [addr] 0x80, dest
#endif
#endif

#ifndef LOAD_TWIN
#define LOAD_TWIN(addr_reg,dest0,dest1)	\
	ldda [addr_reg] ASI_BLK_INIT_QUAD_LDD_P, dest0
#endif

#ifndef STORE
#define STORE(type,src,addr)	type src, [addr]
#endif

#ifndef STORE_INIT
#ifndef SIMULATE_NIAGARA_ON_NON_NIAGARA
#define STORE_INIT(src,addr)	stxa src, [addr] %asi
#else
#define STORE_INIT(src,addr)	stx src, [addr + 0x00]
#endif
#endif

#ifndef FUNC_NAME
#define FUNC_NAME	NGmemcpy
#endif

#ifndef PREAMBLE
#define PREAMBLE
#endif

#ifndef XCC
#define XCC xcc
#endif

	.register	%g2,#scratch
	.register	%g3,#scratch

	.text
	.align		64

	.globl	FUNC_NAME
	.type	FUNC_NAME,#function
FUNC_NAME:	/* %i0=dst, %i1=src, %i2=len */
	PREAMBLE
	save		%sp, -SAVE_AMOUNT, %sp
	srlx		%i2, 31, %g2
	cmp		%g2, 0
	tne		%xcc, 5
	mov		%i0, %o0
	cmp		%i2, 0
	be,pn		%XCC, 85f
	 or		%o0, %i1, %i3
	cmp		%i2, 16
	blu,a,pn	%XCC, 80f
	 or		%i3, %i2, %i3

	/* 2 blocks (128 bytes) is the minimum we can do the block
	 * copy with.  We need to ensure that we'll iterate at least
	 * once in the block copy loop.  At worst we'll need to align
	 * the destination to a 64-byte boundary which can chew up
	 * to (64 - 1) bytes from the length before we perform the
	 * block copy loop.
	 */
	cmp		%i2, (2 * 64)
	blu,pt		%XCC, 70f
	 andcc		%i3, 0x7, %g0

	/* %o0:	dst
	 * %i1:	src
	 * %i2:	len  (known to be >= 128)
	 *
	 * The block copy loops will use %i4/%i5,%g2/%g3 as
	 * temporaries while copying the data.
	 */

	LOAD(prefetch, %i1, #one_read)
	wr		%g0, STORE_ASI, %asi

	/* Align destination on 64-byte boundary.  */
	andcc		%o0, (64 - 1), %i4
	be,pt		%XCC, 2f
	 sub		%i4, 64, %i4
	sub		%g0, %i4, %i4	! bytes to align dst
	sub		%i2, %i4, %i2
1:	subcc		%i4, 1, %i4
	EX_LD(LOAD(ldub, %i1, %g1))
	EX_ST(STORE(stb, %g1, %o0))
	add		%i1, 1, %i1
	bne,pt		%XCC, 1b
	add		%o0, 1, %o0

	/* If the source is on a 16-byte boundary we can do
	 * the direct block copy loop.  If it is 8-byte aligned
	 * we can do the 16-byte loads offset by -8 bytes and the
	 * init stores offset by one register.
	 *
	 * If the source is not even 8-byte aligned, we need to do
	 * shifting and masking (basically integer faligndata).
	 *
	 * The careful bit with init stores is that if we store
	 * to any part of the cache line we have to store the whole
	 * cacheline else we can end up with corrupt L2 cache line
	 * contents.  Since the loop works on 64-bytes of 64-byte
	 * aligned store data at a time, this is easy to ensure.
	 */
2:
	andcc		%i1, (16 - 1), %i4
	andn		%i2, (64 - 1), %g1	! block copy loop iterator
	be,pt		%XCC, 50f
	 sub		%i2, %g1, %i2		! final sub-block copy bytes

	cmp		%i4, 8
	be,pt		%XCC, 10f
	 sub		%i1, %i4, %i1

	/* Neither 8-byte nor 16-byte aligned, shift and mask.  */
	and		%i4, 0x7, GLOBAL_SPARE
	sll		GLOBAL_SPARE, 3, GLOBAL_SPARE
	mov		64, %i5
	EX_LD(LOAD_TWIN(%i1, %g2, %g3))
	sub		%i5, GLOBAL_SPARE, %i5
	mov		16, %o4
	mov		32, %o5
	mov		48, %o7
	mov		64, %i3

	bg,pn	   	%XCC, 9f
	 nop

#define MIX_THREE_WORDS(WORD1, WORD2, WORD3, PRE_SHIFT, POST_SHIFT, TMP) \
	sllx		WORD1, POST_SHIFT, WORD1; \
	srlx		WORD2, PRE_SHIFT, TMP; \
	sllx		WORD2, POST_SHIFT, WORD2; \
	or		WORD1, TMP, WORD1; \
	srlx		WORD3, PRE_SHIFT, TMP; \
	or		WORD2, TMP, WORD2;

8:	EX_LD(LOAD_TWIN(%i1 + %o4, %o2, %o3))
	MIX_THREE_WORDS(%g2, %g3, %o2, %i5, GLOBAL_SPARE, %o1)
	LOAD(prefetch, %i1 + %i3, #one_read)

	EX_ST(STORE_INIT(%g2, %o0 + 0x00))
	EX_ST(STORE_INIT(%g3, %o0 + 0x08))

	EX_LD(LOAD_TWIN(%i1 + %o5, %g2, %g3))
	MIX_THREE_WORDS(%o2, %o3, %g2, %i5, GLOBAL_SPARE, %o1)

	EX_ST(STORE_INIT(%o2, %o0 + 0x10))
	EX_ST(STORE_INIT(%o3, %o0 + 0x18))

	EX_LD(LOAD_TWIN(%i1 + %o7, %o2, %o3))
	MIX_THREE_WORDS(%g2, %g3, %o2, %i5, GLOBAL_SPARE, %o1)

	EX_ST(STORE_INIT(%g2, %o0 + 0x20))
	EX_ST(STORE_INIT(%g3, %o0 + 0x28))

	EX_LD(LOAD_TWIN(%i1 + %i3, %g2, %g3))
	add		%i1, 64, %i1
	MIX_THREE_WORDS(%o2, %o3, %g2, %i5, GLOBAL_SPARE, %o1)

	EX_ST(STORE_INIT(%o2, %o0 + 0x30))
	EX_ST(STORE_INIT(%o3, %o0 + 0x38))

	subcc		%g1, 64, %g1
	bne,pt		%XCC, 8b
	 add		%o0, 64, %o0

	ba,pt		%XCC, 60f
	 add		%i1, %i4, %i1

9:	EX_LD(LOAD_TWIN(%i1 + %o4, %o2, %o3))
	MIX_THREE_WORDS(%g3, %o2, %o3, %i5, GLOBAL_SPARE, %o1)
	LOAD(prefetch, %i1 + %i3, #one_read)

	EX_ST(STORE_INIT(%g3, %o0 + 0x00))
	EX_ST(STORE_INIT(%o2, %o0 + 0x08))

	EX_LD(LOAD_TWIN(%i1 + %o5, %g2, %g3))
	MIX_THREE_WORDS(%o3, %g2, %g3, %i5, GLOBAL_SPARE, %o1)

	EX_ST(STORE_INIT(%o3, %o0 + 0x10))
	EX_ST(STORE_INIT(%g2, %o0 + 0x18))

	EX_LD(LOAD_TWIN(%i1 + %o7, %o2, %o3))
	MIX_THREE_WORDS(%g3, %o2, %o3, %i5, GLOBAL_SPARE, %o1)

	EX_ST(STORE_INIT(%g3, %o0 + 0x20))
	EX_ST(STORE_INIT(%o2, %o0 + 0x28))

	EX_LD(LOAD_TWIN(%i1 + %i3, %g2, %g3))
	add		%i1, 64, %i1
	MIX_THREE_WORDS(%o3, %g2, %g3, %i5, GLOBAL_SPARE, %o1)

	EX_ST(STORE_INIT(%o3, %o0 + 0x30))
	EX_ST(STORE_INIT(%g2, %o0 + 0x38))

	subcc		%g1, 64, %g1
	bne,pt		%XCC, 9b
	 add		%o0, 64, %o0

	ba,pt		%XCC, 60f
	 add		%i1, %i4, %i1

10:	/* Destination is 64-byte aligned, source was only 8-byte
	 * aligned but it has been subtracted by 8 and we perform
	 * one twin load ahead, then add 8 back into source when
	 * we finish the loop.
	 */
	EX_LD(LOAD_TWIN(%i1, %o4, %o5))
	mov	16, %o7
	mov	32, %g2
	mov	48, %g3
	mov	64, %o1
1:	EX_LD(LOAD_TWIN(%i1 + %o7, %o2, %o3))
	LOAD(prefetch, %i1 + %o1, #one_read)
	EX_ST(STORE_INIT(%o5, %o0 + 0x00))	! initializes cache line
	EX_ST(STORE_INIT(%o2, %o0 + 0x08))
	EX_LD(LOAD_TWIN(%i1 + %g2, %o4, %o5))
	EX_ST(STORE_INIT(%o3, %o0 + 0x10))
	EX_ST(STORE_INIT(%o4, %o0 + 0x18))
	EX_LD(LOAD_TWIN(%i1 + %g3, %o2, %o3))
	EX_ST(STORE_INIT(%o5, %o0 + 0x20))
	EX_ST(STORE_INIT(%o2, %o0 + 0x28))
	EX_LD(LOAD_TWIN(%i1 + %o1, %o4, %o5))
	add		%i1, 64, %i1
	EX_ST(STORE_INIT(%o3, %o0 + 0x30))
	EX_ST(STORE_INIT(%o4, %o0 + 0x38))
	subcc		%g1, 64, %g1
	bne,pt		%XCC, 1b
	 add		%o0, 64, %o0

	ba,pt		%XCC, 60f
	 add		%i1, 0x8, %i1

50:	/* Destination is 64-byte aligned, and source is 16-byte
	 * aligned.
	 */
	mov	16, %o7
	mov	32, %g2
	mov	48, %g3
	mov	64, %o1
1:	EX_LD(LOAD_TWIN(%i1 + %g0, %o4, %o5))
	EX_LD(LOAD_TWIN(%i1 + %o7, %o2, %o3))
	LOAD(prefetch, %i1 + %o1, #one_read)
	EX_ST(STORE_INIT(%o4, %o0 + 0x00))	! initializes cache line
	EX_ST(STORE_INIT(%o5, %o0 + 0x08))
	EX_LD(LOAD_TWIN(%i1 + %g2, %o4, %o5))
	EX_ST(STORE_INIT(%o2, %o0 + 0x10))
	EX_ST(STORE_INIT(%o3, %o0 + 0x18))
	EX_LD(LOAD_TWIN(%i1 + %g3, %o2, %o3))
	add	%i1, 64, %i1
	EX_ST(STORE_INIT(%o4, %o0 + 0x20))
	EX_ST(STORE_INIT(%o5, %o0 + 0x28))
	EX_ST(STORE_INIT(%o2, %o0 + 0x30))
	EX_ST(STORE_INIT(%o3, %o0 + 0x38))
	subcc	%g1, 64, %g1
	bne,pt	%XCC, 1b
	 add	%o0, 64, %o0
	/* fall through */

60:	
	membar		#Sync

	/* %i2 contains any final bytes still needed to be copied
	 * over. If anything is left, we copy it one byte at a time.
	 */
	RESTORE_ASI(%i3)
	brz,pt		%i2, 85f
	 sub		%o0, %i1, %i3
	ba,a,pt		%XCC, 90f

	.align		64
70: /* 16 < len <= 64 */
	bne,pn		%XCC, 75f
	 sub		%o0, %i1, %i3

72:
	andn		%i2, 0xf, %i4
	and		%i2, 0xf, %i2
1:	subcc		%i4, 0x10, %i4
	EX_LD(LOAD(ldx, %i1, %o4))
	add		%i1, 0x08, %i1
	EX_LD(LOAD(ldx, %i1, %g1))
	sub		%i1, 0x08, %i1
	EX_ST(STORE(stx, %o4, %i1 + %i3))
	add		%i1, 0x8, %i1
	EX_ST(STORE(stx, %g1, %i1 + %i3))
	bgu,pt		%XCC, 1b
	 add		%i1, 0x8, %i1
73:	andcc		%i2, 0x8, %g0
	be,pt		%XCC, 1f
	 nop
	sub		%i2, 0x8, %i2
	EX_LD(LOAD(ldx, %i1, %o4))
	EX_ST(STORE(stx, %o4, %i1 + %i3))
	add		%i1, 0x8, %i1
1:	andcc		%i2, 0x4, %g0
	be,pt		%XCC, 1f
	 nop
	sub		%i2, 0x4, %i2
	EX_LD(LOAD(lduw, %i1, %i5))
	EX_ST(STORE(stw, %i5, %i1 + %i3))
	add		%i1, 0x4, %i1
1:	cmp		%i2, 0
	be,pt		%XCC, 85f
	 nop
	ba,pt		%xcc, 90f
	 nop

75:
	andcc		%o0, 0x7, %g1
	sub		%g1, 0x8, %g1
	be,pn		%icc, 2f
	 sub		%g0, %g1, %g1
	sub		%i2, %g1, %i2

1:	subcc		%g1, 1, %g1
	EX_LD(LOAD(ldub, %i1, %i5))
	EX_ST(STORE(stb, %i5, %i1 + %i3))
	bgu,pt		%icc, 1b
	 add		%i1, 1, %i1

2:	add		%i1, %i3, %o0
	andcc		%i1, 0x7, %g1
	bne,pt		%icc, 8f
	 sll		%g1, 3, %g1

	cmp		%i2, 16
	bgeu,pt		%icc, 72b
	 nop
	ba,a,pt		%xcc, 73b

8:	mov		64, %i3
	andn		%i1, 0x7, %i1
	EX_LD(LOAD(ldx, %i1, %g2))
	sub		%i3, %g1, %i3
	andn		%i2, 0x7, %i4
	sllx		%g2, %g1, %g2
1:	add		%i1, 0x8, %i1
	EX_LD(LOAD(ldx, %i1, %g3))
	subcc		%i4, 0x8, %i4
	srlx		%g3, %i3, %i5
	or		%i5, %g2, %i5
	EX_ST(STORE(stx, %i5, %o0))
	add		%o0, 0x8, %o0
	bgu,pt		%icc, 1b
	 sllx		%g3, %g1, %g2

	srl		%g1, 3, %g1
	andcc		%i2, 0x7, %i2
	be,pn		%icc, 85f
	 add		%i1, %g1, %i1
	ba,pt		%xcc, 90f
	 sub		%o0, %i1, %i3

	.align		64
80: /* 0 < len <= 16 */
	andcc		%i3, 0x3, %g0
	bne,pn		%XCC, 90f
	 sub		%o0, %i1, %i3

1:
	subcc		%i2, 4, %i2
	EX_LD(LOAD(lduw, %i1, %g1))
	EX_ST(STORE(stw, %g1, %i1 + %i3))
	bgu,pt		%XCC, 1b
	 add		%i1, 4, %i1

85:	ret
	 restore	EX_RETVAL(%i0), %g0, %o0

	.align		32
90:
	subcc		%i2, 1, %i2
	EX_LD(LOAD(ldub, %i1, %g1))
	EX_ST(STORE(stb, %g1, %i1 + %i3))
	bgu,pt		%XCC, 90b
	 add		%i1, 1, %i1
	ret
	 restore	EX_RETVAL(%i0), %g0, %o0

	.size		FUNC_NAME, .-FUNC_NAME
