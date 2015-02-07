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

/*---------------------------------------------------------------------------+
 |  reg_constant.c                                                           |
 |                                                                           |
 | All of the constant FPU_REGs                                              |
 |                                                                           |
 | Copyright (C) 1992,1993,1994,1997                                         |
 |                     W. Metzenthen, 22 Parker St, Ormond, Vic 3163,        |
 |                     Australia.  E-mail   billm@suburbia.net               |
 |                                                                           |
 |                                                                           |
 +---------------------------------------------------------------------------*/

#include "fpu_system.h"
#include "fpu_emu.h"
#include "status_w.h"
#include "reg_constant.h"
#include "control_w.h"

#define MAKE_REG(s, e, l, h) { l, h, \
		((EXTENDED_Ebias+(e)) | ((SIGN_##s != 0)*0x8000)) }

FPU_REG const CONST_1 = MAKE_REG(POS, 0, 0x00000000, 0x80000000);
#if 0
FPU_REG const CONST_2 = MAKE_REG(POS, 1, 0x00000000, 0x80000000);
FPU_REG const CONST_HALF = MAKE_REG(POS, -1, 0x00000000, 0x80000000);
#endif /*  0  */
static FPU_REG const CONST_L2T = MAKE_REG(POS, 1, 0xcd1b8afe, 0xd49a784b);
static FPU_REG const CONST_L2E = MAKE_REG(POS, 0, 0x5c17f0bc, 0xb8aa3b29);
FPU_REG const CONST_PI = MAKE_REG(POS, 1, 0x2168c235, 0xc90fdaa2);
FPU_REG const CONST_PI2 = MAKE_REG(POS, 0, 0x2168c235, 0xc90fdaa2);
FPU_REG const CONST_PI4 = MAKE_REG(POS, -1, 0x2168c235, 0xc90fdaa2);
static FPU_REG const CONST_LG2 = MAKE_REG(POS, -2, 0xfbcff799, 0x9a209a84);
static FPU_REG const CONST_LN2 = MAKE_REG(POS, -1, 0xd1cf79ac, 0xb17217f7);

/* Extra bits to take pi/2 to more than 128 bits precision. */
FPU_REG const CONST_PI2extra = MAKE_REG(NEG, -66,
					0xfc8f8cbb, 0xece675d1);

/* Only the sign (and tag) is used in internal zeroes */
FPU_REG const CONST_Z = MAKE_REG(POS, EXP_UNDER, 0x0, 0x0);

/* Only the sign and significand (and tag) are used in internal NaNs */
/* The 80486 never generates one of these
FPU_REG const CONST_SNAN = MAKE_REG(POS, EXP_OVER, 0x00000001, 0x80000000);
 */
/* This is the real indefinite QNaN */
FPU_REG const CONST_QNaN = MAKE_REG(NEG, EXP_OVER, 0x00000000, 0xC0000000);

/* Only the sign (and tag) is used in internal infinities */
FPU_REG const CONST_INF = MAKE_REG(POS, EXP_OVER, 0x00000000, 0x80000000);

static void fld_const(FPU_REG const * c, int adj, u_char tag)
{
	FPU_REG *st_new_ptr;

	if (STACK_OVERFLOW) {
		FPU_stack_overflow();
		return;
	}
	push();
	reg_copy(c, st_new_ptr);
	st_new_ptr->sigl += adj;	/* For all our fldxxx constants, we don't need to
					   borrow or carry. */
	FPU_settag0(tag);
	clear_C1();
}

/* A fast way to find out whether x is one of RC_DOWN or RC_CHOP
   (and not one of RC_RND or RC_UP).
   */
#define DOWN_OR_CHOP(x)  (x & RC_DOWN)

static void fld1(int rc)
{
	fld_const(&CONST_1, 0, TAG_Valid);
}

static void fldl2t(int rc)
{
	fld_const(&CONST_L2T, (rc == RC_UP) ? 1 : 0, TAG_Valid);
}

static void fldl2e(int rc)
{
	fld_const(&CONST_L2E, DOWN_OR_CHOP(rc) ? -1 : 0, TAG_Valid);
}

static void fldpi(int rc)
{
	fld_const(&CONST_PI, DOWN_OR_CHOP(rc) ? -1 : 0, TAG_Valid);
}

static void fldlg2(int rc)
{
	fld_const(&CONST_LG2, DOWN_OR_CHOP(rc) ? -1 : 0, TAG_Valid);
}

static void fldln2(int rc)
{
	fld_const(&CONST_LN2, DOWN_OR_CHOP(rc) ? -1 : 0, TAG_Valid);
}

static void fldz(int rc)
{
	fld_const(&CONST_Z, 0, TAG_Zero);
}

typedef void (*FUNC_RC) (int);

static FUNC_RC constants_table[] = {
	fld1, fldl2t, fldl2e, fldpi, fldlg2, fldln2, fldz, (FUNC_RC) FPU_illegal
};

void fconst(void)
{
	(constants_table[FPU_rm]) (control_word & CW_RC);
}
