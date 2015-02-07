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
 * Copyright 2003 PathScale, Inc.
 * Copyright (C) 2003 - 2007 Jeff Dike (jdike@{addtoit,linux.intel}.com)
 *
 * Licensed under the GPL
 */
#ifndef __UM_ELF_X86_64_H
#define __UM_ELF_X86_64_H

#include <asm/user.h>
#include "skas.h"

/* x86-64 relocation types, taken from asm-x86_64/elf.h */
#define R_X86_64_NONE		0	/* No reloc */
#define R_X86_64_64		1	/* Direct 64 bit  */
#define R_X86_64_PC32		2	/* PC relative 32 bit signed */
#define R_X86_64_GOT32		3	/* 32 bit GOT entry */
#define R_X86_64_PLT32		4	/* 32 bit PLT address */
#define R_X86_64_COPY		5	/* Copy symbol at runtime */
#define R_X86_64_GLOB_DAT	6	/* Create GOT entry */
#define R_X86_64_JUMP_SLOT	7	/* Create PLT entry */
#define R_X86_64_RELATIVE	8	/* Adjust by program base */
#define R_X86_64_GOTPCREL	9	/* 32 bit signed pc relative
					   offset to GOT */
#define R_X86_64_32		10	/* Direct 32 bit zero extended */
#define R_X86_64_32S		11	/* Direct 32 bit sign extended */
#define R_X86_64_16		12	/* Direct 16 bit zero extended */
#define R_X86_64_PC16		13	/* 16 bit sign extended pc relative */
#define R_X86_64_8		14	/* Direct 8 bit sign extended  */
#define R_X86_64_PC8		15	/* 8 bit sign extended pc relative */

#define R_X86_64_NUM		16

typedef unsigned long elf_greg_t;

#define ELF_NGREG (sizeof (struct user_regs_struct) / sizeof(elf_greg_t))
typedef elf_greg_t elf_gregset_t[ELF_NGREG];

typedef struct user_i387_struct elf_fpregset_t;

/*
 * This is used to ensure we don't load something for the wrong architecture.
 */
#define elf_check_arch(x) \
	((x)->e_machine == EM_X86_64)

#define ELF_CLASS	ELFCLASS64
#define ELF_DATA        ELFDATA2LSB
#define ELF_ARCH        EM_X86_64

#define ELF_PLAT_INIT(regs, load_addr)    do { \
	PT_REGS_RBX(regs) = 0; \
	PT_REGS_RCX(regs) = 0; \
	PT_REGS_RDX(regs) = 0; \
	PT_REGS_RSI(regs) = 0; \
	PT_REGS_RDI(regs) = 0; \
	PT_REGS_RBP(regs) = 0; \
	PT_REGS_RAX(regs) = 0; \
	PT_REGS_R8(regs) = 0; \
	PT_REGS_R9(regs) = 0; \
	PT_REGS_R10(regs) = 0; \
	PT_REGS_R11(regs) = 0; \
	PT_REGS_R12(regs) = 0; \
	PT_REGS_R13(regs) = 0; \
	PT_REGS_R14(regs) = 0; \
	PT_REGS_R15(regs) = 0; \
} while (0)

#define ELF_CORE_COPY_REGS(pr_reg, regs)		\
	(pr_reg)[0] = (regs)->regs.gp[0];			\
	(pr_reg)[1] = (regs)->regs.gp[1];			\
	(pr_reg)[2] = (regs)->regs.gp[2];			\
	(pr_reg)[3] = (regs)->regs.gp[3];			\
	(pr_reg)[4] = (regs)->regs.gp[4];			\
	(pr_reg)[5] = (regs)->regs.gp[5];			\
	(pr_reg)[6] = (regs)->regs.gp[6];			\
	(pr_reg)[7] = (regs)->regs.gp[7];			\
	(pr_reg)[8] = (regs)->regs.gp[8];			\
	(pr_reg)[9] = (regs)->regs.gp[9];			\
	(pr_reg)[10] = (regs)->regs.gp[10];			\
	(pr_reg)[11] = (regs)->regs.gp[11];			\
	(pr_reg)[12] = (regs)->regs.gp[12];			\
	(pr_reg)[13] = (regs)->regs.gp[13];			\
	(pr_reg)[14] = (regs)->regs.gp[14];			\
	(pr_reg)[15] = (regs)->regs.gp[15];			\
	(pr_reg)[16] = (regs)->regs.gp[16];			\
	(pr_reg)[17] = (regs)->regs.gp[17];			\
	(pr_reg)[18] = (regs)->regs.gp[18];			\
	(pr_reg)[19] = (regs)->regs.gp[19];			\
	(pr_reg)[20] = (regs)->regs.gp[20];			\
	(pr_reg)[21] = current->thread.arch.fs;			\
	(pr_reg)[22] = 0;					\
	(pr_reg)[23] = 0;					\
	(pr_reg)[24] = 0;					\
	(pr_reg)[25] = 0;					\
	(pr_reg)[26] = 0;

extern int elf_core_copy_fpregs(struct task_struct *t, elf_fpregset_t *fpu);

#define ELF_CORE_COPY_FPREGS(t, fpu) elf_core_copy_fpregs(t, fpu)

#ifdef TIF_IA32 /* XXX */
#error XXX, indeed
        clear_thread_flag(TIF_IA32);
#endif

#define USE_ELF_CORE_DUMP
#define ELF_EXEC_PAGESIZE 4096

#define ELF_ET_DYN_BASE (2 * TASK_SIZE / 3)

extern long elf_aux_hwcap;
#define ELF_HWCAP (elf_aux_hwcap)

#define ELF_PLATFORM "x86_64"

#define SET_PERSONALITY(ex) do ; while(0)

#endif
