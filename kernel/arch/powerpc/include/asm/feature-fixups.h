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

#ifndef __ASM_POWERPC_FEATURE_FIXUPS_H
#define __ASM_POWERPC_FEATURE_FIXUPS_H

/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifdef __ASSEMBLY__

/*
 * Feature section common macros
 *
 * Note that the entries now contain offsets between the table entry
 * and the code rather than absolute code pointers in order to be
 * useable with the vdso shared library. There is also an assumption
 * that values will be negative, that is, the fixup table has to be
 * located after the code it fixes up.
 */
#if defined(CONFIG_PPC64) && !defined(__powerpc64__)
/* 64 bits kernel, 32 bits code (ie. vdso32) */
#define FTR_ENTRY_LONG		.llong
#define FTR_ENTRY_OFFSET	.long 0xffffffff; .long
#else
/* 64 bit kernel 64 bit code, or 32 bit kernel 32 bit code */
#define FTR_ENTRY_LONG		PPC_LONG
#define FTR_ENTRY_OFFSET	PPC_LONG
#endif

#define START_FTR_SECTION(label)	label##1:

#define FTR_SECTION_ELSE_NESTED(label)			\
label##2:						\
	.pushsection __ftr_alt_##label,"a";		\
	.align 2;					\
label##3:

#define MAKE_FTR_SECTION_ENTRY(msk, val, label, sect)	\
label##4:						\
	.popsection;					\
	.pushsection sect,"a";				\
	.align 3;					\
label##5:					       	\
	FTR_ENTRY_LONG msk;				\
	FTR_ENTRY_LONG val;				\
	FTR_ENTRY_OFFSET label##1b-label##5b;		\
	FTR_ENTRY_OFFSET label##2b-label##5b;	 	\
	FTR_ENTRY_OFFSET label##3b-label##5b;		\
	FTR_ENTRY_OFFSET label##4b-label##5b;	 	\
	.popsection;


/* CPU feature dependent sections */
#define BEGIN_FTR_SECTION_NESTED(label)	START_FTR_SECTION(label)
#define BEGIN_FTR_SECTION		START_FTR_SECTION(97)

#define END_FTR_SECTION_NESTED(msk, val, label) 		\
	FTR_SECTION_ELSE_NESTED(label)				\
	MAKE_FTR_SECTION_ENTRY(msk, val, label, __ftr_fixup)

#define END_FTR_SECTION(msk, val)		\
	END_FTR_SECTION_NESTED(msk, val, 97)

#define END_FTR_SECTION_IFSET(msk)	END_FTR_SECTION((msk), (msk))
#define END_FTR_SECTION_IFCLR(msk)	END_FTR_SECTION((msk), 0)

/* CPU feature sections with alternatives, use BEGIN_FTR_SECTION to start */
#define FTR_SECTION_ELSE	FTR_SECTION_ELSE_NESTED(97)
#define ALT_FTR_SECTION_END_NESTED(msk, val, label)	\
	MAKE_FTR_SECTION_ENTRY(msk, val, label, __ftr_fixup)
#define ALT_FTR_SECTION_END_NESTED_IFSET(msk, label)	\
	ALT_FTR_SECTION_END_NESTED(msk, msk, label)
#define ALT_FTR_SECTION_END_NESTED_IFCLR(msk, label)	\
	ALT_FTR_SECTION_END_NESTED(msk, 0, label)
#define ALT_FTR_SECTION_END(msk, val)	\
	ALT_FTR_SECTION_END_NESTED(msk, val, 97)
#define ALT_FTR_SECTION_END_IFSET(msk)	\
	ALT_FTR_SECTION_END_NESTED_IFSET(msk, 97)
#define ALT_FTR_SECTION_END_IFCLR(msk)	\
	ALT_FTR_SECTION_END_NESTED_IFCLR(msk, 97)

/* MMU feature dependent sections */
#define BEGIN_MMU_FTR_SECTION_NESTED(label)	START_FTR_SECTION(label)
#define BEGIN_MMU_FTR_SECTION			START_FTR_SECTION(97)

#define END_MMU_FTR_SECTION_NESTED(msk, val, label) 		\
	FTR_SECTION_ELSE_NESTED(label)				\
	MAKE_FTR_SECTION_ENTRY(msk, val, label, __mmu_ftr_fixup)

#define END_MMU_FTR_SECTION(msk, val)		\
	END_MMU_FTR_SECTION_NESTED(msk, val, 97)

#define END_MMU_FTR_SECTION_IFSET(msk)	END_MMU_FTR_SECTION((msk), (msk))
#define END_MMU_FTR_SECTION_IFCLR(msk)	END_MMU_FTR_SECTION((msk), 0)

/* MMU feature sections with alternatives, use BEGIN_FTR_SECTION to start */
#define MMU_FTR_SECTION_ELSE_NESTED(label)	FTR_SECTION_ELSE_NESTED(label)
#define MMU_FTR_SECTION_ELSE	MMU_FTR_SECTION_ELSE_NESTED(97)
#define ALT_MMU_FTR_SECTION_END_NESTED(msk, val, label)	\
	MAKE_FTR_SECTION_ENTRY(msk, val, label, __mmu_ftr_fixup)
#define ALT_MMU_FTR_SECTION_END_NESTED_IFSET(msk, label)	\
	ALT_MMU_FTR_SECTION_END_NESTED(msk, msk, label)
#define ALT_MMU_FTR_SECTION_END_NESTED_IFCLR(msk, label)	\
	ALT_MMU_FTR_SECTION_END_NESTED(msk, 0, label)
#define ALT_MMU_FTR_SECTION_END(msk, val)	\
	ALT_MMU_FTR_SECTION_END_NESTED(msk, val, 97)
#define ALT_MMU_FTR_SECTION_END_IFSET(msk)	\
	ALT_MMU_FTR_SECTION_END_NESTED_IFSET(msk, 97)
#define ALT_MMU_FTR_SECTION_END_IFCLR(msk)	\
	ALT_MMU_FTR_SECTION_END_NESTED_IFCLR(msk, 97)

/* Firmware feature dependent sections */
#define BEGIN_FW_FTR_SECTION_NESTED(label)	START_FTR_SECTION(label)
#define BEGIN_FW_FTR_SECTION			START_FTR_SECTION(97)

#define END_FW_FTR_SECTION_NESTED(msk, val, label) 		\
	FTR_SECTION_ELSE_NESTED(label)				\
	MAKE_FTR_SECTION_ENTRY(msk, val, label, __fw_ftr_fixup)

#define END_FW_FTR_SECTION(msk, val)		\
	END_FW_FTR_SECTION_NESTED(msk, val, 97)

#define END_FW_FTR_SECTION_IFSET(msk)	END_FW_FTR_SECTION((msk), (msk))
#define END_FW_FTR_SECTION_IFCLR(msk)	END_FW_FTR_SECTION((msk), 0)

/* Firmware feature sections with alternatives */
#define FW_FTR_SECTION_ELSE_NESTED(label)	FTR_SECTION_ELSE_NESTED(label)
#define FW_FTR_SECTION_ELSE	FTR_SECTION_ELSE_NESTED(97)
#define ALT_FW_FTR_SECTION_END_NESTED(msk, val, label)	\
	MAKE_FTR_SECTION_ENTRY(msk, val, label, __fw_ftr_fixup)
#define ALT_FW_FTR_SECTION_END_NESTED_IFSET(msk, label)	\
	ALT_FW_FTR_SECTION_END_NESTED(msk, msk, label)
#define ALT_FW_FTR_SECTION_END_NESTED_IFCLR(msk, label)	\
	ALT_FW_FTR_SECTION_END_NESTED(msk, 0, label)
#define ALT_FW_FTR_SECTION_END(msk, val)	\
	ALT_FW_FTR_SECTION_END_NESTED(msk, val, 97)
#define ALT_FW_FTR_SECTION_END_IFSET(msk)	\
	ALT_FW_FTR_SECTION_END_NESTED_IFSET(msk, 97)
#define ALT_FW_FTR_SECTION_END_IFCLR(msk)	\
	ALT_FW_FTR_SECTION_END_NESTED_IFCLR(msk, 97)

#endif /* __ASSEMBLY__ */

/* LWSYNC feature sections */
#define START_LWSYNC_SECTION(label)	label##1:
#define MAKE_LWSYNC_SECTION_ENTRY(label, sect)		\
label##2:						\
	.pushsection sect,"a";				\
	.align 2;					\
label##3:					       	\
	.long label##1b-label##3b;			\
	.popsection;

#endif /* __ASM_POWERPC_FEATURE_FIXUPS_H */
