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
 * AVR32 linker script for the Linux kernel
 *
 * Copyright (C) 2004-2006 Atmel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#define LOAD_OFFSET 0x00000000
#include <asm-generic/vmlinux.lds.h>
#include <asm/cache.h>
#include <asm/thread_info.h>

OUTPUT_FORMAT("elf32-avr32", "elf32-avr32", "elf32-avr32")
OUTPUT_ARCH(avr32)
ENTRY(_start)

/* Big endian */
jiffies = jiffies_64 + 4;

SECTIONS
{
	. = CONFIG_ENTRY_ADDRESS;
	.init		: AT(ADDR(.init) - LOAD_OFFSET) {
		_stext = .;
		__init_begin = .;
			_sinittext = .;
			*(.text.reset)
			INIT_TEXT
			/*
			 * .exit.text is discarded at runtime, not
			 * link time, to deal with references from
			 * __bug_table
			 */
			EXIT_TEXT
			_einittext = .;
		. = ALIGN(4);
		__tagtable_begin = .;
			*(.taglist.init)
		__tagtable_end = .;
			INIT_DATA
		. = ALIGN(16);
		__setup_start = .;
			*(.init.setup)
		__setup_end = .;
		. = ALIGN(4);
		__initcall_start = .;
			INITCALLS
		__initcall_end = .;
		__con_initcall_start = .;
			*(.con_initcall.init)
		__con_initcall_end = .;
		__security_initcall_start = .;
			*(.security_initcall.init)
		__security_initcall_end = .;
#ifdef CONFIG_BLK_DEV_INITRD
		. = ALIGN(32);
		__initramfs_start = .;
			*(.init.ramfs)
		__initramfs_end = .;
#endif
		. = ALIGN(PAGE_SIZE);
		__init_end = .;
	}

	.text		: AT(ADDR(.text) - LOAD_OFFSET) {
		_evba = .;
		_text = .;
		*(.ex.text)
		*(.irq.text)
		KPROBES_TEXT
		TEXT_TEXT
		SCHED_TEXT
		LOCK_TEXT
		*(.fixup)
		*(.gnu.warning)
		_etext = .;
	} = 0xd703d703

	. = ALIGN(4);
	__ex_table	: AT(ADDR(__ex_table) - LOAD_OFFSET) {
		__start___ex_table = .;
		*(__ex_table)
		__stop___ex_table = .;
	}

	RODATA

	. = ALIGN(THREAD_SIZE);

	.data		: AT(ADDR(.data) - LOAD_OFFSET) {
		_data = .;
		_sdata = .;
		/*
		 * First, the init task union, aligned to an 8K boundary.
		 */
		*(.data.init_task)

		/* Then, the page-aligned data */
		. = ALIGN(PAGE_SIZE);
		*(.data.page_aligned)

		/* Then, the cacheline aligned data */
		. = ALIGN(L1_CACHE_BYTES);
		*(.data.cacheline_aligned)

		/* And the rest... */
		*(.data.rel*)
		DATA_DATA
		CONSTRUCTORS

		_edata = .;
	}


	. = ALIGN(8);
	.bss    	: AT(ADDR(.bss) - LOAD_OFFSET) {
		__bss_start = .;
		*(.bss)
		*(COMMON)
		. = ALIGN(8);
		__bss_stop = .;
		_end = .;
	}

	/* When something in the kernel is NOT compiled as a module, the module
	 * cleanup code and data are put into these segments. Both can then be
	 * thrown away, as cleanup code is never called unless it's a module.
	 */
	/DISCARD/       	: {
		EXIT_DATA
		*(.exitcall.exit)
	}

	DWARF_DEBUG
}
