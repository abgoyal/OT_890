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

/* GENmemcpy.S: Generic sparc64 memcpy.
 *
 * Copyright (C) 2007 David S. Miller (davem@davemloft.net)
 */

#ifdef __KERNEL__
#define GLOBAL_SPARE	%g7
#else
#define GLOBAL_SPARE	%g5
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
#define LOAD(type,addr,dest)	type [addr], dest
#endif

#ifndef STORE
#define STORE(type,src,addr)	type src, [addr]
#endif

#ifndef FUNC_NAME
#define FUNC_NAME	GENmemcpy
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
FUNC_NAME:	/* %o0=dst, %o1=src, %o2=len */
	srlx		%o2, 31, %g2
	cmp		%g2, 0
	tne		%XCC, 5
	PREAMBLE
	mov		%o0, GLOBAL_SPARE

	cmp		%o2, 0
	be,pn		%XCC, 85f
	 or		%o0, %o1, %o3
	cmp		%o2, 16
	blu,a,pn	%XCC, 80f
	 or		%o3, %o2, %o3

	xor		%o0, %o1, %o4
	andcc		%o4, 0x7, %g0
	bne,a,pn	%XCC, 90f
	 sub		%o0, %o1, %o3

	and		%o0, 0x7, %o4
	sub		%o4, 0x8, %o4
	sub		%g0, %o4, %o4
	sub		%o2, %o4, %o2
1:	subcc		%o4, 1, %o4
	EX_LD(LOAD(ldub, %o1, %g1))
	EX_ST(STORE(stb, %g1, %o0))
	add		%o1, 1, %o1
	bne,pt		%XCC, 1b
	add		%o0, 1, %o0

	andn		%o2, 0x7, %g1
	sub		%o2, %g1, %o2
1:	subcc		%g1, 0x8, %g1
	EX_LD(LOAD(ldx, %o1, %g2))
	EX_ST(STORE(stx, %g2, %o0))
	add		%o1, 0x8, %o1
	bne,pt		%XCC, 1b
	 add		%o0, 0x8, %o0

	brz,pt		%o2, 85f
	 sub		%o0, %o1, %o3
	ba,a,pt		%XCC, 90f

	.align		64
80: /* 0 < len <= 16 */
	andcc		%o3, 0x3, %g0
	bne,pn		%XCC, 90f
	 sub		%o0, %o1, %o3

1:
	subcc		%o2, 4, %o2
	EX_LD(LOAD(lduw, %o1, %g1))
	EX_ST(STORE(stw, %g1, %o1 + %o3))
	bgu,pt		%XCC, 1b
	 add		%o1, 4, %o1

85:	retl
	 mov		EX_RETVAL(GLOBAL_SPARE), %o0

	.align		32
90:
	subcc		%o2, 1, %o2
	EX_LD(LOAD(ldub, %o1, %g1))
	EX_ST(STORE(stb, %g1, %o1 + %o3))
	bgu,pt		%XCC, 90b
	 add		%o1, 1, %o1
	retl
	 mov		EX_RETVAL(GLOBAL_SPARE), %o0

	.size		FUNC_NAME, .-FUNC_NAME
