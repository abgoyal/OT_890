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

/* -*- linux-c -*- ------------------------------------------------------- *
 *
 *   Copyright (C) 1991, 1992 Linus Torvalds
 *   Copyright 2007 rPath, Inc. - All Rights Reserved
 *
 *   This file is part of the Linux kernel, and is made available under
 *   the terms of the GNU General Public License version 2.
 *
 * ----------------------------------------------------------------------- */

/*
 * Memory detection code
 */

#include "boot.h"

#define SMAP	0x534d4150	/* ASCII "SMAP" */

static int detect_memory_e820(void)
{
	int count = 0;
	u32 next = 0;
	u32 size, id;
	u8 err;
	struct e820entry *desc = boot_params.e820_map;

	do {
		size = sizeof(struct e820entry);

		/* Important: %edx is clobbered by some BIOSes,
		   so it must be either used for the error output
		   or explicitly marked clobbered. */
		asm("int $0x15; setc %0"
		    : "=d" (err), "+b" (next), "=a" (id), "+c" (size),
		      "=m" (*desc)
		    : "D" (desc), "d" (SMAP), "a" (0xe820));

		/* BIOSes which terminate the chain with CF = 1 as opposed
		   to %ebx = 0 don't always report the SMAP signature on
		   the final, failing, probe. */
		if (err)
			break;

		/* Some BIOSes stop returning SMAP in the middle of
		   the search loop.  We don't know exactly how the BIOS
		   screwed up the map at that point, we might have a
		   partial map, the full map, or complete garbage, so
		   just return failure. */
		if (id != SMAP) {
			count = 0;
			break;
		}

		count++;
		desc++;
	} while (next && count < ARRAY_SIZE(boot_params.e820_map));

	return boot_params.e820_entries = count;
}

static int detect_memory_e801(void)
{
	u16 ax, bx, cx, dx;
	u8 err;

	bx = cx = dx = 0;
	ax = 0xe801;
	asm("stc; int $0x15; setc %0"
	    : "=m" (err), "+a" (ax), "+b" (bx), "+c" (cx), "+d" (dx));

	if (err)
		return -1;

	/* Do we really need to do this? */
	if (cx || dx) {
		ax = cx;
		bx = dx;
	}

	if (ax > 15*1024)
		return -1;	/* Bogus! */

	/* This ignores memory above 16MB if we have a memory hole
	   there.  If someone actually finds a machine with a memory
	   hole at 16MB and no support for 0E820h they should probably
	   generate a fake e820 map. */
	boot_params.alt_mem_k = (ax == 15*1024) ? (dx << 6)+ax : ax;

	return 0;
}

static int detect_memory_88(void)
{
	u16 ax;
	u8 err;

	ax = 0x8800;
	asm("stc; int $0x15; setc %0" : "=bcdm" (err), "+a" (ax));

	boot_params.screen_info.ext_mem_k = ax;

	return -err;
}

int detect_memory(void)
{
	int err = -1;

	if (detect_memory_e820() > 0)
		err = 0;

	if (!detect_memory_e801())
		err = 0;

	if (!detect_memory_88())
		err = 0;

	return err;
}
