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
 * arch/sh/boards/renesas/x3proto/ilsel.c
 *
 * Helper routines for SH-X3 proto board ILSEL.
 *
 * Copyright (C) 2007 Paul Mundt
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/bitmap.h>
#include <linux/io.h>
#include <asm/ilsel.h>

/*
 * ILSEL is split across:
 *
 *	ILSEL0 - 0xb8100004 [ Levels  1 -  4 ]
 *	ILSEL1 - 0xb8100006 [ Levels  5 -  8 ]
 *	ILSEL2 - 0xb8100008 [ Levels  9 - 12 ]
 *	ILSEL3 - 0xb810000a [ Levels 13 - 15 ]
 *
 * With each level being relative to an ilsel_source_t.
 */
#define ILSEL_BASE	0xb8100004
#define ILSEL_LEVELS	15

/*
 * ILSEL level map, in descending order from the highest level down.
 *
 * Supported levels are 1 - 15 spread across ILSEL0 - ILSEL4, mapping
 * directly to IRLs. As the IRQs are numbered in reverse order relative
 * to the interrupt level, the level map is carefully managed to ensure a
 * 1:1 mapping between the bit position and the IRQ number.
 *
 * This careful constructions allows ilsel_enable*() to be referenced
 * directly for hooking up an ILSEL set and getting back an IRQ which can
 * subsequently be used for internal accounting in the (optional) disable
 * path.
 */
static unsigned long ilsel_level_map;

static inline unsigned int ilsel_offset(unsigned int bit)
{
	return ILSEL_LEVELS - bit - 1;
}

static inline unsigned long mk_ilsel_addr(unsigned int bit)
{
	return ILSEL_BASE + ((ilsel_offset(bit) >> 1) & ~0x1);
}

static inline unsigned int mk_ilsel_shift(unsigned int bit)
{
	return (ilsel_offset(bit) & 0x3) << 2;
}

static void __ilsel_enable(ilsel_source_t set, unsigned int bit)
{
	unsigned int tmp, shift;
	unsigned long addr;

	addr = mk_ilsel_addr(bit);
	shift = mk_ilsel_shift(bit);

	pr_debug("%s: bit#%d: addr - 0x%08lx (shift %d, set %d)\n",
		 __func__, bit, addr, shift, set);

	tmp = ctrl_inw(addr);
	tmp &= ~(0xf << shift);
	tmp |= set << shift;
	ctrl_outw(tmp, addr);
}

/**
 * ilsel_enable - Enable an ILSEL set.
 * @set: ILSEL source (see ilsel_source_t enum in include/asm-sh/ilsel.h).
 *
 * Enables a given non-aliased ILSEL source (<= ILSEL_KEY) at the highest
 * available interrupt level. Callers should take care to order callsites
 * noting descending interrupt levels. Aliasing FPGA and external board
 * IRQs need to use ilsel_enable_fixed().
 *
 * The return value is an IRQ number that can later be taken down with
 * ilsel_disable().
 */
int ilsel_enable(ilsel_source_t set)
{
	unsigned int bit;

	/* Aliased sources must use ilsel_enable_fixed() */
	BUG_ON(set > ILSEL_KEY);

	do {
		bit = find_first_zero_bit(&ilsel_level_map, ILSEL_LEVELS);
	} while (test_and_set_bit(bit, &ilsel_level_map));

	__ilsel_enable(set, bit);

	return bit;
}
EXPORT_SYMBOL_GPL(ilsel_enable);

/**
 * ilsel_enable_fixed - Enable an ILSEL set at a fixed interrupt level
 * @set: ILSEL source (see ilsel_source_t enum in include/asm-sh/ilsel.h).
 * @level: Interrupt level (1 - 15)
 *
 * Enables a given ILSEL source at a fixed interrupt level. Necessary
 * both for level reservation as well as for aliased sources that only
 * exist on special ILSEL#s.
 *
 * Returns an IRQ number (as ilsel_enable()).
 */
int ilsel_enable_fixed(ilsel_source_t set, unsigned int level)
{
	unsigned int bit = ilsel_offset(level - 1);

	if (test_and_set_bit(bit, &ilsel_level_map))
		return -EBUSY;

	__ilsel_enable(set, bit);

	return bit;
}
EXPORT_SYMBOL_GPL(ilsel_enable_fixed);

/**
 * ilsel_disable - Disable an ILSEL set
 * @irq: Bit position for ILSEL set value (retval from enable routines)
 *
 * Disable a previously enabled ILSEL set.
 */
void ilsel_disable(unsigned int irq)
{
	unsigned long addr;
	unsigned int tmp;

	addr = mk_ilsel_addr(irq);

	tmp = ctrl_inw(addr);
	tmp &= ~(0xf << mk_ilsel_shift(irq));
	ctrl_outw(tmp, addr);

	clear_bit(irq, &ilsel_level_map);
}
EXPORT_SYMBOL_GPL(ilsel_disable);
