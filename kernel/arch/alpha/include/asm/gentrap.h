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

#ifndef _ASMAXP_GENTRAP_H
#define _ASMAXP_GENTRAP_H

/*
 * Definitions for gentrap causes.  They are generated by user-level
 * programs and therefore should be compatible with the corresponding
 * OSF/1 definitions.
 */
#define GEN_INTOVF	-1	/* integer overflow */
#define GEN_INTDIV	-2	/* integer division by zero */
#define GEN_FLTOVF	-3	/* fp overflow */
#define GEN_FLTDIV	-4	/* fp division by zero */
#define GEN_FLTUND	-5	/* fp underflow */
#define GEN_FLTINV	-6	/* invalid fp operand */
#define GEN_FLTINE	-7	/* inexact fp operand */
#define GEN_DECOVF	-8	/* decimal overflow (for COBOL??) */
#define GEN_DECDIV	-9	/* decimal division by zero */
#define GEN_DECINV	-10	/* invalid decimal operand */
#define GEN_ROPRAND	-11	/* reserved operand */
#define GEN_ASSERTERR	-12	/* assertion error */
#define GEN_NULPTRERR	-13	/* null pointer error */
#define GEN_STKOVF	-14	/* stack overflow */
#define GEN_STRLENERR	-15	/* string length error */
#define GEN_SUBSTRERR	-16	/* substring error */
#define GEN_RANGERR	-17	/* range error */
#define GEN_SUBRNG	-18
#define GEN_SUBRNG1	-19	 
#define GEN_SUBRNG2	-20
#define GEN_SUBRNG3	-21	/* these report range errors for */
#define GEN_SUBRNG4	-22	/* subscripting (indexing) at levels 0..7 */
#define GEN_SUBRNG5	-23
#define GEN_SUBRNG6	-24
#define GEN_SUBRNG7	-25

/* the remaining codes (-26..-1023) are reserved. */

#endif /* _ASMAXP_GENTRAP_H */
