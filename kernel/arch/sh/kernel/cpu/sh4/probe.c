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
 * arch/sh/kernel/cpu/sh4/probe.c
 *
 * CPU Subtype Probing for SH-4.
 *
 * Copyright (C) 2001 - 2007  Paul Mundt
 * Copyright (C) 2003  Richard Curnow
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */
#include <linux/init.h>
#include <linux/io.h>
#include <asm/processor.h>
#include <asm/cache.h>

int __init detect_cpu_and_cache_system(void)
{
	unsigned long pvr, prr, cvr;
	unsigned long size;

	static unsigned long sizes[16] = {
		[1] = (1 << 12),
		[2] = (1 << 13),
		[4] = (1 << 14),
		[8] = (1 << 15),
		[9] = (1 << 16)
	};

	pvr = (ctrl_inl(CCN_PVR) >> 8) & 0xffffff;
	prr = (ctrl_inl(CCN_PRR) >> 4) & 0xff;
	cvr = (ctrl_inl(CCN_CVR));

	/*
	 * Setup some sane SH-4 defaults for the icache
	 */
	boot_cpu_data.icache.way_incr		= (1 << 13);
	boot_cpu_data.icache.entry_shift	= 5;
	boot_cpu_data.icache.sets		= 256;
	boot_cpu_data.icache.ways		= 1;
	boot_cpu_data.icache.linesz		= L1_CACHE_BYTES;

	/*
	 * And again for the dcache ..
	 */
	boot_cpu_data.dcache.way_incr		= (1 << 14);
	boot_cpu_data.dcache.entry_shift	= 5;
	boot_cpu_data.dcache.sets		= 512;
	boot_cpu_data.dcache.ways		= 1;
	boot_cpu_data.dcache.linesz		= L1_CACHE_BYTES;

	/* We don't know the chip cut */
	boot_cpu_data.cut_major = boot_cpu_data.cut_minor = -1;

	/*
	 * Setup some generic flags we can probe on SH-4A parts
	 */
	if (((pvr >> 16) & 0xff) == 0x10) {
		if ((cvr & 0x10000000) == 0)
			boot_cpu_data.flags |= CPU_HAS_DSP;

		boot_cpu_data.flags |= CPU_HAS_LLSC;
		boot_cpu_data.cut_major = pvr & 0x7f;
	}

	/* FPU detection works for everyone */
	if ((cvr & 0x20000000) == 1)
		boot_cpu_data.flags |= CPU_HAS_FPU;

	/* Mask off the upper chip ID */
	pvr &= 0xffff;

	/*
	 * Probe the underlying processor version/revision and
	 * adjust cpu_data setup accordingly.
	 */
	switch (pvr) {
	case 0x205:
		boot_cpu_data.type = CPU_SH7750;
		boot_cpu_data.flags |= CPU_HAS_P2_FLUSH_BUG | CPU_HAS_FPU |
				   CPU_HAS_PERF_COUNTER;
		break;
	case 0x206:
		boot_cpu_data.type = CPU_SH7750S;
		boot_cpu_data.flags |= CPU_HAS_P2_FLUSH_BUG | CPU_HAS_FPU |
				   CPU_HAS_PERF_COUNTER;
		break;
	case 0x1100:
		boot_cpu_data.type = CPU_SH7751;
		boot_cpu_data.flags |= CPU_HAS_FPU;
		break;
	case 0x2001:
	case 0x2004:
		boot_cpu_data.type = CPU_SH7770;
		boot_cpu_data.icache.ways = 4;
		boot_cpu_data.dcache.ways = 4;

		boot_cpu_data.flags |= CPU_HAS_FPU | CPU_HAS_LLSC;
		break;
	case 0x2006:
	case 0x200A:
		if (prr == 0x61)
			boot_cpu_data.type = CPU_SH7781;
		else if (prr == 0xa1)
			boot_cpu_data.type = CPU_SH7763;
		else
			boot_cpu_data.type = CPU_SH7780;

		boot_cpu_data.icache.ways = 4;
		boot_cpu_data.dcache.ways = 4;

		boot_cpu_data.flags |= CPU_HAS_FPU | CPU_HAS_PERF_COUNTER |
				   CPU_HAS_LLSC;
		break;
	case 0x3000:
	case 0x3003:
	case 0x3009:
		boot_cpu_data.type = CPU_SH7343;
		boot_cpu_data.icache.ways = 4;
		boot_cpu_data.dcache.ways = 4;
		boot_cpu_data.flags |= CPU_HAS_LLSC;
		break;
	case 0x3004:
	case 0x3007:
		boot_cpu_data.type = CPU_SH7785;
		boot_cpu_data.icache.ways = 4;
		boot_cpu_data.dcache.ways = 4;
		boot_cpu_data.flags |= CPU_HAS_FPU | CPU_HAS_PERF_COUNTER |
					  CPU_HAS_LLSC;
		break;
	case 0x3008:
		boot_cpu_data.icache.ways = 4;
		boot_cpu_data.dcache.ways = 4;
		boot_cpu_data.flags |= CPU_HAS_LLSC;

		switch (prr) {
		case 0x50:
		case 0x51:
			boot_cpu_data.type = CPU_SH7723;
			boot_cpu_data.flags |= CPU_HAS_FPU | CPU_HAS_L2_CACHE;
			break;
		case 0x70:
			boot_cpu_data.type = CPU_SH7366;
			break;
		case 0xa0:
		case 0xa1:
			boot_cpu_data.type = CPU_SH7722;
			break;
		}
		break;
	case 0x4000:	/* 1st cut */
	case 0x4001:	/* 2nd cut */
		boot_cpu_data.type = CPU_SHX3;
		boot_cpu_data.icache.ways = 4;
		boot_cpu_data.dcache.ways = 4;
		boot_cpu_data.flags |= CPU_HAS_FPU | CPU_HAS_PERF_COUNTER |
					  CPU_HAS_LLSC;
		break;
	case 0x700:
		boot_cpu_data.type = CPU_SH4_501;
		boot_cpu_data.icache.ways = 2;
		boot_cpu_data.dcache.ways = 2;
		break;
	case 0x600:
		boot_cpu_data.type = CPU_SH4_202;
		boot_cpu_data.icache.ways = 2;
		boot_cpu_data.dcache.ways = 2;
		boot_cpu_data.flags |= CPU_HAS_FPU;
		break;
	case 0x500 ... 0x501:
		switch (prr) {
		case 0x10:
			boot_cpu_data.type = CPU_SH7750R;
			break;
		case 0x11:
			boot_cpu_data.type = CPU_SH7751R;
			break;
		case 0x50 ... 0x5f:
			boot_cpu_data.type = CPU_SH7760;
			break;
		}

		boot_cpu_data.icache.ways = 2;
		boot_cpu_data.dcache.ways = 2;

		boot_cpu_data.flags |= CPU_HAS_FPU;

		break;
	default:
		boot_cpu_data.type = CPU_SH_NONE;
		break;
	}

#ifdef CONFIG_SH_DIRECT_MAPPED
	boot_cpu_data.icache.ways = 1;
	boot_cpu_data.dcache.ways = 1;
#endif

#ifdef CONFIG_CPU_HAS_PTEA
	boot_cpu_data.flags |= CPU_HAS_PTEA;
#endif

	/*
	 * On anything that's not a direct-mapped cache, look to the CVR
	 * for I/D-cache specifics.
	 */
	if (boot_cpu_data.icache.ways > 1) {
		size = sizes[(cvr >> 20) & 0xf];
		boot_cpu_data.icache.way_incr	= (size >> 1);
		boot_cpu_data.icache.sets	= (size >> 6);

	}

	/* And the rest of the D-cache */
	if (boot_cpu_data.dcache.ways > 1) {
		size = sizes[(cvr >> 16) & 0xf];
		boot_cpu_data.dcache.way_incr	= (size >> 1);
		boot_cpu_data.dcache.sets	= (size >> 6);
	}

	/*
	 * Setup the L2 cache desc
	 *
	 * SH-4A's have an optional PIPT L2.
	 */
	if (boot_cpu_data.flags & CPU_HAS_L2_CACHE) {
		/* Bug if we can't decode the L2 info */
		BUG_ON(!(cvr & 0xf));

		/* Silicon and specifications have clearly never met.. */
		cvr ^= 0xf;

		/*
		 * Size calculation is much more sensible
		 * than it is for the L1.
		 *
		 * Sizes are 128KB, 258KB, 512KB, and 1MB.
		 */
		size = (cvr & 0xf) << 17;

		BUG_ON(!size);

		boot_cpu_data.scache.way_incr		= (1 << 16);
		boot_cpu_data.scache.entry_shift	= 5;
		boot_cpu_data.scache.ways		= 4;
		boot_cpu_data.scache.linesz		= L1_CACHE_BYTES;

		boot_cpu_data.scache.entry_mask	=
			(boot_cpu_data.scache.way_incr -
			 boot_cpu_data.scache.linesz);

		boot_cpu_data.scache.sets	= size /
			(boot_cpu_data.scache.linesz *
			 boot_cpu_data.scache.ways);

		boot_cpu_data.scache.way_size	=
			(boot_cpu_data.scache.sets *
			 boot_cpu_data.scache.linesz);
	}

	return 0;
}
