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

/* strlen_user.S: Sparc optimized strlen_user code
 *
 * Return length of string in userspace including terminating 0
 * or 0 for error
 *
 * Copyright (C) 1991,1996 Free Software Foundation
 * Copyright (C) 1996 David S. Miller (davem@caip.rutgers.edu)
 * Copyright (C) 1996 Jakub Jelinek (jj@sunsite.mff.cuni.cz)
 */

#define LO_MAGIC 0x01010101
#define HI_MAGIC 0x80808080

10:
	ldub	[%o0], %o5
	cmp	%o5, 0
	be	1f
	 add	%o0, 1, %o0
	andcc	%o0, 3, %g0
	be	4f
	 or	%o4, %lo(HI_MAGIC), %o3
11:
	ldub	[%o0], %o5
	cmp	%o5, 0
	be	2f
	 add	%o0, 1, %o0
	andcc	%o0, 3, %g0
	be	5f
	 sethi	%hi(LO_MAGIC), %o4
12:
	ldub	[%o0], %o5
	cmp	%o5, 0
	be	3f
	 add	%o0, 1, %o0
	b	13f
	 or	%o4, %lo(LO_MAGIC), %o2
1:
	retl
	 mov	1, %o0
2:
	retl
	 mov	2, %o0
3:
	retl
	 mov	3, %o0

	.align 4
	.global __strlen_user, __strnlen_user
__strlen_user:
	sethi	%hi(32768), %o1
__strnlen_user:
	mov	%o1, %g1
	mov	%o0, %o1
	andcc	%o0, 3, %g0
	bne	10b
	 sethi	%hi(HI_MAGIC), %o4
	or	%o4, %lo(HI_MAGIC), %o3
4:
	sethi	%hi(LO_MAGIC), %o4
5:
	or	%o4, %lo(LO_MAGIC), %o2
13:
	ld	[%o0], %o5
2:
	sub	%o5, %o2, %o4
	andcc	%o4, %o3, %g0
	bne	82f
	 add	%o0, 4, %o0
	sub	%o0, %o1, %g2
81:	cmp	%g2, %g1
	blu	13b
	 mov	%o0, %o4
	ba,a	1f

	/* Check every byte. */
82:	srl	%o5, 24, %g5
	andcc	%g5, 0xff, %g0
	be	1f
	 add	%o0, -3, %o4
	srl	%o5, 16, %g5
	andcc	%g5, 0xff, %g0
	be	1f
	 add	%o4, 1, %o4
	srl	%o5, 8, %g5
	andcc	%g5, 0xff, %g0
	be	1f
	 add	%o4, 1, %o4
	andcc	%o5, 0xff, %g0
	bne	81b
	 sub	%o0, %o1, %g2

	add	%o4, 1, %o4
1:
	retl
	 sub	%o4, %o1, %o0

	.section .fixup,#alloc,#execinstr
	.align	4
9:
	retl
	 clr	%o0

	.section __ex_table,#alloc
	.align	4

	.word	10b, 9b
	.word	11b, 9b
	.word	12b, 9b
	.word	13b, 9b
