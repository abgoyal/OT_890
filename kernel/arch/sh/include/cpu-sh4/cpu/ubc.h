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
 * include/asm-sh/cpu-sh4/ubc.h
 *
 * Copyright (C) 1999 Niibe Yutaka
 * Copyright (C) 2003 Paul Mundt
 * Copyright (C) 2006 Lineo Solutions Inc. support SH4A UBC
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */
#ifndef __ASM_CPU_SH4_UBC_H
#define __ASM_CPU_SH4_UBC_H

#if defined(CONFIG_CPU_SH4A)
#define UBC_CBR0		0xff200000
#define UBC_CRR0		0xff200004
#define UBC_CAR0		0xff200008
#define UBC_CAMR0		0xff20000c
#define UBC_CBR1		0xff200020
#define UBC_CRR1		0xff200024
#define UBC_CAR1		0xff200028
#define UBC_CAMR1		0xff20002c
#define UBC_CDR1		0xff200030
#define UBC_CDMR1		0xff200034
#define UBC_CETR1		0xff200038
#define UBC_CCMFR		0xff200600
#define UBC_CBCR		0xff200620

/* CBR	*/
#define UBC_CBR_AIE		(0x01<<30)
#define UBC_CBR_ID_INST		(0x01<<4)
#define UBC_CBR_RW_READ		(0x01<<1)
#define UBC_CBR_CE		(0x01)

#define	UBC_CBR_AIV_MASK	(0x00FF0000)
#define	UBC_CBR_AIV_SHIFT	(16)
#define UBC_CBR_AIV_SET(asid)	(((asid)<<UBC_CBR_AIV_SHIFT) & UBC_CBR_AIV_MASK)

#define UBC_CBR_INIT		0x20000000

/* CRR	*/
#define UBC_CRR_RES		(0x01<<13)
#define UBC_CRR_PCB		(0x01<<1)
#define UBC_CRR_BIE		(0x01)

#define UBC_CRR_INIT		0x00002000

#else	/* CONFIG_CPU_SH4 */
#define UBC_BARA		0xff200000
#define UBC_BAMRA		0xff200004
#define UBC_BBRA		0xff200008
#define UBC_BASRA		0xff000014
#define UBC_BARB		0xff20000c
#define UBC_BAMRB		0xff200010
#define UBC_BBRB		0xff200014
#define UBC_BASRB		0xff000018
#define UBC_BDRB		0xff200018
#define UBC_BDMRB		0xff20001c
#define UBC_BRCR		0xff200020
#endif	/* CONFIG_CPU_SH4 */

#endif /* __ASM_CPU_SH4_UBC_H */

