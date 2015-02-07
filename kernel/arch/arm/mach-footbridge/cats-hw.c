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
 * linux/arch/arm/mach-footbridge/cats-hw.c
 *
 * CATS machine fixup
 *
 * Copyright (C) 1998, 1999 Russell King, Phil Blundell
 */
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/screen_info.h>
#include <linux/io.h>
#include <linux/spinlock.h>

#include <asm/hardware/dec21285.h>
#include <asm/mach-types.h>
#include <asm/setup.h>

#include <asm/mach/arch.h>

#include "common.h"

#define CFG_PORT	0x370
#define INDEX_PORT	(CFG_PORT)
#define DATA_PORT	(CFG_PORT + 1)

static int __init cats_hw_init(void)
{
	if (machine_is_cats()) {
		/* Set Aladdin to CONFIGURE mode */
		outb(0x51, CFG_PORT);
		outb(0x23, CFG_PORT);

		/* Select logical device 3 */
		outb(0x07, INDEX_PORT);
		outb(0x03, DATA_PORT);

		/* Set parallel port to DMA channel 3, ECP+EPP1.9, 
		   enable EPP timeout */
		outb(0x74, INDEX_PORT);
		outb(0x03, DATA_PORT);
	
		outb(0xf0, INDEX_PORT);
		outb(0x0f, DATA_PORT);

		outb(0xf1, INDEX_PORT);
		outb(0x07, DATA_PORT);

		/* Select logical device 4 */
		outb(0x07, INDEX_PORT);
		outb(0x04, DATA_PORT);

		/* UART1 high speed mode */
		outb(0xf0, INDEX_PORT);
		outb(0x02, DATA_PORT);

		/* Select logical device 5 */
		outb(0x07, INDEX_PORT);
		outb(0x05, DATA_PORT);

		/* UART2 high speed mode */
		outb(0xf0, INDEX_PORT);
		outb(0x02, DATA_PORT);

		/* Set Aladdin to RUN mode */
		outb(0xbb, CFG_PORT);
	}

	return 0;
}

__initcall(cats_hw_init);

/*
 * CATS uses soft-reboot by default, since
 * hard reboots fail on early boards.
 */
static void __init
fixup_cats(struct machine_desc *desc, struct tag *tags,
	   char **cmdline, struct meminfo *mi)
{
	screen_info.orig_video_lines  = 25;
	screen_info.orig_video_points = 16;
	screen_info.orig_y = 24;
}

MACHINE_START(CATS, "Chalice-CATS")
	/* Maintainer: Philip Blundell */
	.phys_io	= DC21285_ARMCSR_BASE,
	.io_pg_offst	= ((0xfe000000) >> 18) & 0xfffc,
	.boot_params	= 0x00000100,
	.soft_reboot	= 1,
	.fixup		= fixup_cats,
	.map_io		= footbridge_map_io,
	.init_irq	= footbridge_init_irq,
	.timer		= &isa_timer,
MACHINE_END
