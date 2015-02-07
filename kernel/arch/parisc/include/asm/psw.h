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

#ifndef _PARISC_PSW_H


#define	PSW_I	0x00000001
#define	PSW_D	0x00000002
#define	PSW_P	0x00000004
#define	PSW_Q	0x00000008

#define	PSW_R	0x00000010
#define	PSW_F	0x00000020
#define	PSW_G	0x00000040	/* PA1.x only */
#define PSW_O	0x00000080	/* PA2.0 only */

/* ssm/rsm instructions number PSW_W and PSW_E differently */
#define PSW_SM_I	PSW_I	/* Enable External Interrupts */
#define PSW_SM_D	PSW_D
#define PSW_SM_P	PSW_P
#define PSW_SM_Q	PSW_Q	/* Enable Interrupt State Collection */
#define PSW_SM_R	PSW_R	/* Enable Recover Counter Trap */
#define PSW_SM_W	0x200	/* PA2.0 only : Enable Wide Mode */

#define PSW_SM_QUIET	PSW_SM_R+PSW_SM_Q+PSW_SM_P+PSW_SM_D+PSW_SM_I

#define PSW_CB	0x0000ff00

#define	PSW_M	0x00010000
#define	PSW_V	0x00020000
#define	PSW_C	0x00040000
#define	PSW_B	0x00080000

#define	PSW_X	0x00100000
#define	PSW_N	0x00200000
#define	PSW_L	0x00400000
#define	PSW_H	0x00800000

#define	PSW_T	0x01000000
#define	PSW_S	0x02000000
#define	PSW_E	0x04000000
#define PSW_W	0x08000000	/* PA2.0 only */
#define PSW_W_BIT       36      /* PA2.0 only */

#define	PSW_Z	0x40000000	/* PA1.x only */
#define	PSW_Y	0x80000000	/* PA1.x only */

#ifdef CONFIG_64BIT
#  define PSW_HI_CB 0x000000ff    /* PA2.0 only */
#endif

#ifdef CONFIG_64BIT
#  define USER_PSW_HI_MASK	PSW_HI_CB
#  define WIDE_PSW		PSW_W
#else 
#  define WIDE_PSW		0
#endif

/* Used when setting up for rfi */
#define KERNEL_PSW    (WIDE_PSW | PSW_C | PSW_Q | PSW_P | PSW_D)
#define REAL_MODE_PSW (WIDE_PSW | PSW_Q)
#define USER_PSW_MASK (WIDE_PSW | PSW_T | PSW_N | PSW_X | PSW_B | PSW_V | PSW_CB)
#define USER_PSW      (PSW_C | PSW_Q | PSW_P | PSW_D | PSW_I)

#endif
