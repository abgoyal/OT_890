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
 * arch/s390/kernel/relocate_kernel64.S
 *
 * (C) Copyright IBM Corp. 2005
 *
 * Author(s): Rolf Adelsberger,
 *	      Heiko Carstens <heiko.carstens@de.ibm.com>
 *
 */

/*
 * moves the new kernel to its destination...
 * %r2 = pointer to first kimage_entry_t
 * %r3 = start address - where to jump to after the job is done...
 *
 * %r5 will be used as temp. storage
 * %r6 holds the destination address
 * %r7 = PAGE_SIZE
 * %r8 holds the source address
 * %r9 = PAGE_SIZE
 *
 * 0xf000 is a page_mask
 */

	.text
	.globl		relocate_kernel
	relocate_kernel:
		basr	%r13,0		# base address
	.base:
		stnsm	sys_msk-.base(%r13),0xfb	# disable DAT
		stctg	%c0,%c15,ctlregs-.base(%r13)
		stmg	%r0,%r15,gprregs-.base(%r13)
		lghi	%r0,3
		sllg	%r0,%r0,31
		stg	%r0,0x1d0(%r0)
		la	%r0,.back_pgm-.base(%r13)
		stg	%r0,0x1d8(%r0)
		la	%r1,load_psw-.base(%r13)
		mvc	0(8,%r0),0(%r1)
		la	%r0,.back-.base(%r13)
		st	%r0,4(%r0)
		oi	4(%r0),0x80
		lghi	%r0,0
		diag	%r0,%r0,0x308
	.back:
		lhi	%r1,1		# mode 1 = esame
		sigp	%r1,%r0,0x12	# switch to esame mode
		sam64			# switch to 64 bit addressing mode
		basr	%r13,0
	.back_base:
		oi	have_diag308-.back_base(%r13),0x01
		lctlg	%c0,%c15,ctlregs-.back_base(%r13)
		lmg	%r0,%r15,gprregs-.back_base(%r13)
		j	.top
	.back_pgm:
		lmg	%r0,%r15,gprregs-.base(%r13)
	.top:
		lghi	%r7,4096	# load PAGE_SIZE in r7
		lghi	%r9,4096	# load PAGE_SIZE in r9
		lg	%r5,0(%r2)	# read another word for indirection page
		aghi	%r2,8		# increment pointer
		tml	%r5,0x1		# is it a destination page?
		je	.indir_check	# NO, goto "indir_check"
		lgr	%r6,%r5		# r6 = r5
		nill	%r6,0xf000	# mask it out and...
		j	.top		# ...next iteration
	.indir_check:
		tml	%r5,0x2		# is it a indirection page?
		je	.done_test	# NO, goto "done_test"
		nill	%r5,0xf000	# YES, mask out,
		lgr	%r2,%r5		# move it into the right register,
		j	.top		# and read next...
	.done_test:
		tml	%r5,0x4		# is it the done indicator?
		je	.source_test	# NO! Well, then it should be the source indicator...
		j	.done		# ok, lets finish it here...
	.source_test:
		tml	%r5,0x8		# it should be a source indicator...
		je	.top		# NO, ignore it...
		lgr	%r8,%r5		# r8 = r5
		nill	%r8,0xf000	# masking
	0:	mvcle	%r6,%r8,0x0	# copy PAGE_SIZE bytes from r8 to r6 - pad with 0
		jo	0b
		j	.top
	.done:
		sgr	%r0,%r0		# clear register r0
		la	%r4,load_psw-.base(%r13)	# load psw-address into the register
		o	%r3,4(%r4)	# or load address into psw
		st	%r3,4(%r4)
		mvc	0(8,%r0),0(%r4)	# copy psw to absolute address 0
		tm	have_diag308-.base(%r13),0x01
		jno	.no_diag308
		diag	%r0,%r0,0x308
	.no_diag308:
		sam31			# 31 bit mode
		sr	%r1,%r1		# erase register r1
		sr	%r2,%r2		# erase register r2
		sigp	%r1,%r2,0x12	# set cpuid to zero
		lpsw	0		# hopefully start new kernel...

		.align	8
	load_psw:
		.long	0x00080000,0x80000000
	sys_msk:
		.quad	0
	ctlregs:
		.rept	16
		.quad	0
		.endr
	gprregs:
		.rept	16
		.quad	0
		.endr
	have_diag308:
		.byte	0
		.align	8
	relocate_kernel_end:
	.globl	relocate_kernel_len
	relocate_kernel_len:
		.quad	relocate_kernel_end - relocate_kernel
