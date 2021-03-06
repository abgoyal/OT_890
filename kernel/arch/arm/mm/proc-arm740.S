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
 *  linux/arch/arm/mm/arm740.S: utility functions for ARM740
 *
 *  Copyright (C) 2004-2006 Hyok S. Choi (hyok.choi@samsung.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <linux/linkage.h>
#include <linux/init.h>
#include <asm/assembler.h>
#include <asm/asm-offsets.h>
#include <asm/hwcap.h>
#include <asm/pgtable-hwdef.h>
#include <asm/pgtable.h>
#include <asm/ptrace.h>

	.text
/*
 * cpu_arm740_proc_init()
 * cpu_arm740_do_idle()
 * cpu_arm740_dcache_clean_area()
 * cpu_arm740_switch_mm()
 *
 * These are not required.
 */
ENTRY(cpu_arm740_proc_init)
ENTRY(cpu_arm740_do_idle)
ENTRY(cpu_arm740_dcache_clean_area)
ENTRY(cpu_arm740_switch_mm)
	mov	pc, lr

/*
 * cpu_arm740_proc_fin()
 */
ENTRY(cpu_arm740_proc_fin)
	stmfd	sp!, {lr}
	mov	ip, #PSR_F_BIT | PSR_I_BIT | SVC_MODE
	msr	cpsr_c, ip
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x3f000000		@ bank/f/lock/s
	bic	r0, r0, #0x0000000c		@ w-buffer/cache
	mcr	p15, 0, r0, c1, c0, 0		@ disable caches
	mcr	p15, 0, r0, c7, c0, 0		@ invalidate cache
	ldmfd	sp!, {pc}

/*
 * cpu_arm740_reset(loc)
 * Params  : r0 = address to jump to
 * Notes   : This sets up everything for a reset
 */
ENTRY(cpu_arm740_reset)
	mov	ip, #0
	mcr	p15, 0, ip, c7, c0, 0		@ invalidate cache
	mrc	p15, 0, ip, c1, c0, 0		@ get ctrl register
	bic	ip, ip, #0x0000000c		@ ............wc..
	mcr	p15, 0, ip, c1, c0, 0		@ ctrl register
	mov	pc, r0

	__INIT

	.type	__arm740_setup, #function
__arm740_setup:
	mov	r0, #0
	mcr	p15, 0, r0, c7, c0, 0		@ invalidate caches

	mcr	p15, 0, r0, c6, c3		@ disable area 3~7
	mcr	p15, 0, r0, c6, c4
	mcr	p15, 0, r0, c6, c5
	mcr	p15, 0, r0, c6, c6
	mcr	p15, 0, r0, c6, c7

	mov	r0, #0x0000003F			@ base = 0, size = 4GB
	mcr	p15, 0, r0, c6,	c0		@ set area 0, default

	ldr	r0, =(CONFIG_DRAM_BASE & 0xFFFFF000) @ base[31:12] of RAM
	ldr	r1, =(CONFIG_DRAM_SIZE >> 12)	@ size of RAM (must be >= 4KB)
	mov	r2, #10				@ 11 is the minimum (4KB)
1:	add	r2, r2, #1			@ area size *= 2
	mov	r1, r1, lsr #1
	bne	1b				@ count not zero r-shift
	orr	r0, r0, r2, lsl #1		@ the area register value
	orr	r0, r0, #1			@ set enable bit
	mcr	p15, 0, r0, c6,	c1		@ set area 1, RAM

	ldr	r0, =(CONFIG_FLASH_MEM_BASE & 0xFFFFF000) @ base[31:12] of FLASH
	ldr	r1, =(CONFIG_FLASH_SIZE >> 12)	@ size of FLASH (must be >= 4KB)
	mov	r2, #10				@ 11 is the minimum (4KB)
1:	add	r2, r2, #1			@ area size *= 2
	mov	r1, r1, lsr #1
	bne	1b				@ count not zero r-shift
	orr	r0, r0, r2, lsl #1		@ the area register value
	orr	r0, r0, #1			@ set enable bit
	mcr	p15, 0, r0, c6,	c2		@ set area 2, ROM/FLASH

	mov	r0, #0x06
	mcr	p15, 0, r0, c2, c0		@ Region 1&2 cacheable
#ifdef CONFIG_CPU_DCACHE_WRITETHROUGH
	mov	r0, #0x00			@ disable whole write buffer
#else
	mov	r0, #0x02			@ Region 1 write bufferred
#endif
	mcr	p15, 0, r0, c3, c0

	mov	r0, #0x10000
	sub	r0, r0, #1			@ r0 = 0xffff
	mcr	p15, 0, r0, c5, c0		@ all read/write access

	mrc	p15, 0, r0, c1, c0		@ get control register
	bic	r0, r0, #0x3F000000		@ set to standard caching mode
						@ need some benchmark
	orr	r0, r0, #0x0000000d		@ MPU/Cache/WB

	mov	pc, lr

	.size	__arm740_setup, . - __arm740_setup

	__INITDATA

/*
 * Purpose : Function pointers used to access above functions - all calls
 *	     come through these
 */
	.type	arm740_processor_functions, #object
ENTRY(arm740_processor_functions)
	.word	v4t_late_abort
	.word	pabort_noifar
	.word	cpu_arm740_proc_init
	.word	cpu_arm740_proc_fin
	.word	cpu_arm740_reset
	.word   cpu_arm740_do_idle
	.word	cpu_arm740_dcache_clean_area
	.word	cpu_arm740_switch_mm
	.word	0			@ cpu_*_set_pte
	.size	arm740_processor_functions, . - arm740_processor_functions

	.section ".rodata"

	.type	cpu_arch_name, #object
cpu_arch_name:
	.asciz	"armv4"
	.size	cpu_arch_name, . - cpu_arch_name

	.type	cpu_elf_name, #object
cpu_elf_name:
	.asciz	"v4"
	.size	cpu_elf_name, . - cpu_elf_name

	.type	cpu_arm740_name, #object
cpu_arm740_name:
	.ascii	"ARM740T"
	.size	cpu_arm740_name, . - cpu_arm740_name

	.align

	.section ".proc.info.init", #alloc, #execinstr
	.type	__arm740_proc_info,#object
__arm740_proc_info:
	.long	0x41807400
	.long	0xfffffff0
	.long	0
	b	__arm740_setup
	.long	cpu_arch_name
	.long	cpu_elf_name
	.long	HWCAP_SWP | HWCAP_HALF | HWCAP_26BIT
	.long	cpu_arm740_name
	.long	arm740_processor_functions
	.long	0
	.long	0
	.long	v3_cache_fns			@ cache model
	.size	__arm740_proc_info, . - __arm740_proc_info


