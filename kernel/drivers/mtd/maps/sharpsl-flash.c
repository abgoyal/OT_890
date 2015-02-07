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
 * sharpsl-flash.c
 *
 * Copyright (C) 2001 Lineo Japan, Inc.
 * Copyright (C) 2002  SHARP
 *
 * based on rpxlite.c,v 1.15 2001/10/02 15:05:14 dwmw2 Exp
 *          Handle mapping of the flash on the RPX Lite and CLLF boards
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <asm/io.h>
#include <asm/mach-types.h>

#define WINDOW_ADDR 0x00000000
#define WINDOW_SIZE 0x00800000
#define BANK_WIDTH 2

static struct mtd_info *mymtd;

struct map_info sharpsl_map = {
	.name = "sharpsl-flash",
	.size = WINDOW_SIZE,
	.bankwidth = BANK_WIDTH,
	.phys = WINDOW_ADDR
};

static struct mtd_partition sharpsl_partitions[1] = {
	{
		name:		"Boot PROM Filesystem",
	}
};

static int __init init_sharpsl(void)
{
	struct mtd_partition *parts;
	int nb_parts = 0;
	char *part_type = "static";

	printk(KERN_NOTICE "Sharp SL series flash device: %x at %x\n",
		WINDOW_SIZE, WINDOW_ADDR);
	sharpsl_map.virt = ioremap(WINDOW_ADDR, WINDOW_SIZE);
	if (!sharpsl_map.virt) {
		printk("Failed to ioremap\n");
		return -EIO;
	}

	simple_map_init(&sharpsl_map);

	mymtd = do_map_probe("map_rom", &sharpsl_map);
	if (!mymtd) {
		iounmap(sharpsl_map.virt);
		return -ENXIO;
	}

	mymtd->owner = THIS_MODULE;

	if (machine_is_corgi() || machine_is_shepherd() || machine_is_husky()
		|| machine_is_poodle()) {
		sharpsl_partitions[0].size=0x006d0000;
		sharpsl_partitions[0].offset=0x00120000;
	} else if (machine_is_tosa()) {
		sharpsl_partitions[0].size=0x006a0000;
		sharpsl_partitions[0].offset=0x00160000;
	} else if (machine_is_spitz() || machine_is_akita() || machine_is_borzoi()) {
		sharpsl_partitions[0].size=0x006b0000;
		sharpsl_partitions[0].offset=0x00140000;
	} else {
		map_destroy(mymtd);
		iounmap(sharpsl_map.virt);
		return -ENODEV;
	}

	parts = sharpsl_partitions;
	nb_parts = ARRAY_SIZE(sharpsl_partitions);

	printk(KERN_NOTICE "Using %s partition definition\n", part_type);
	add_mtd_partitions(mymtd, parts, nb_parts);

	return 0;
}

static void __exit cleanup_sharpsl(void)
{
	if (mymtd) {
		del_mtd_partitions(mymtd);
		map_destroy(mymtd);
	}
	if (sharpsl_map.virt) {
		iounmap(sharpsl_map.virt);
		sharpsl_map.virt = 0;
	}
}

module_init(init_sharpsl);
module_exit(cleanup_sharpsl);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SHARP (Original: Arnold Christensen <AKC@pel.dk>)");
MODULE_DESCRIPTION("MTD map driver for SHARP SL series");
