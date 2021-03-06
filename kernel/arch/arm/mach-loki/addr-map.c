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
 * arch/arm/mach-loki/addr-map.c
 *
 * Address map functions for Marvell Loki (88RC8480) SoCs
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mbus.h>
#include <linux/io.h>
#include <mach/hardware.h>
#include "common.h"

/*
 * Generic Address Decode Windows bit settings
 */
#define TARGET_DDR		0
#define TARGET_DEV_BUS		1
#define TARGET_PCIE0		3
#define TARGET_PCIE1		4
#define ATTR_DEV_BOOT		0x0f
#define ATTR_DEV_CS2		0x1b
#define ATTR_DEV_CS1		0x1d
#define ATTR_DEV_CS0		0x1e
#define ATTR_PCIE_IO		0x51
#define ATTR_PCIE_MEM		0x59

/*
 * Helpers to get DDR bank info
 */
#define DDR_SIZE_CS(n)		DDR_REG(0x1500 + ((n) << 3))
#define DDR_BASE_CS(n)		DDR_REG(0x1504 + ((n) << 3))

/*
 * CPU Address Decode Windows registers
 */
#define CPU_WIN_CTRL(n)		BRIDGE_REG(0x000 | ((n) << 4))
#define CPU_WIN_BASE(n)		BRIDGE_REG(0x004 | ((n) << 4))
#define CPU_WIN_REMAP_LO(n)	BRIDGE_REG(0x008 | ((n) << 4))
#define CPU_WIN_REMAP_HI(n)	BRIDGE_REG(0x00c | ((n) << 4))


struct mbus_dram_target_info loki_mbus_dram_info;

static void __init setup_cpu_win(int win, u32 base, u32 size,
				 u8 target, u8 attr, int remap)
{
	u32 ctrl;

	base &= 0xffff0000;
	ctrl = ((size - 1) & 0xffff0000) | (attr << 8) | (1 << 5) | target;

	writel(base, CPU_WIN_BASE(win));
	writel(ctrl, CPU_WIN_CTRL(win));
	if (win < 2) {
		if (remap < 0)
			remap = base;

		writel(remap & 0xffff0000, CPU_WIN_REMAP_LO(win));
		writel(0, CPU_WIN_REMAP_HI(win));
	}
}

void __init loki_setup_cpu_mbus(void)
{
	int i;
	int cs;

	/*
	 * First, disable and clear windows.
	 */
	for (i = 0; i < 8; i++) {
		writel(0, CPU_WIN_BASE(i));
		writel(0, CPU_WIN_CTRL(i));
		if (i < 2) {
			writel(0, CPU_WIN_REMAP_LO(i));
			writel(0, CPU_WIN_REMAP_HI(i));
		}
	}

	/*
	 * Setup windows for PCIe IO+MEM space.
	 */
	setup_cpu_win(2, LOKI_PCIE0_MEM_PHYS_BASE, LOKI_PCIE0_MEM_SIZE,
		      TARGET_PCIE0, ATTR_PCIE_MEM, -1);
	setup_cpu_win(3, LOKI_PCIE1_MEM_PHYS_BASE, LOKI_PCIE1_MEM_SIZE,
		      TARGET_PCIE1, ATTR_PCIE_MEM, -1);

	/*
	 * Setup MBUS dram target info.
	 */
	loki_mbus_dram_info.mbus_dram_target_id = TARGET_DDR;

	for (i = 0, cs = 0; i < 4; i++) {
		u32 base = readl(DDR_BASE_CS(i));
		u32 size = readl(DDR_SIZE_CS(i));

		/*
		 * Chip select enabled?
		 */
		if (size & 1) {
			struct mbus_dram_window *w;

			w = &loki_mbus_dram_info.cs[cs++];
			w->cs_index = i;
			w->mbus_attr = 0xf & ~(1 << i);
			w->base = base & 0xffff0000;
			w->size = (size | 0x0000ffff) + 1;
		}
	}
	loki_mbus_dram_info.num_cs = cs;
}

void __init loki_setup_dev_boot_win(u32 base, u32 size)
{
	setup_cpu_win(4, base, size, TARGET_DEV_BUS, ATTR_DEV_BOOT, -1);
}
