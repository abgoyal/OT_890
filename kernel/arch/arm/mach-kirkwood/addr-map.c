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
 * arch/arm/mach-kirkwood/addr-map.c
 *
 * Address map functions for Marvell Kirkwood SoCs
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
#define TARGET_PCIE		4
#define ATTR_DEV_SPI_ROM	0x1e
#define ATTR_DEV_BOOT		0x1d
#define ATTR_DEV_NAND		0x2f
#define ATTR_DEV_CS3		0x37
#define ATTR_DEV_CS2		0x3b
#define ATTR_DEV_CS1		0x3d
#define ATTR_DEV_CS0		0x3e
#define ATTR_PCIE_IO		0xe0
#define ATTR_PCIE_MEM		0xe8

/*
 * Helpers to get DDR bank info
 */
#define DDR_BASE_CS_OFF(n)	(0x0000 + ((n) << 3))
#define DDR_SIZE_CS_OFF(n)	(0x0004 + ((n) << 3))

/*
 * CPU Address Decode Windows registers
 */
#define WIN_OFF(n)		(BRIDGE_VIRT_BASE + 0x0000 + ((n) << 4))
#define WIN_CTRL_OFF		0x0000
#define WIN_BASE_OFF		0x0004
#define WIN_REMAP_LO_OFF	0x0008
#define WIN_REMAP_HI_OFF	0x000c


struct mbus_dram_target_info kirkwood_mbus_dram_info;
static int __initdata win_alloc_count;

static int __init cpu_win_can_remap(int win)
{
	if (win < 4)
		return 1;

	return 0;
}

static void __init setup_cpu_win(int win, u32 base, u32 size,
				 u8 target, u8 attr, int remap)
{
	void __iomem *addr = (void __iomem *)WIN_OFF(win);
	u32 ctrl;

	base &= 0xffff0000;
	ctrl = ((size - 1) & 0xffff0000) | (attr << 8) | (target << 4) | 1;

	writel(base, addr + WIN_BASE_OFF);
	writel(ctrl, addr + WIN_CTRL_OFF);
	if (cpu_win_can_remap(win)) {
		if (remap < 0)
			remap = base;

		writel(remap & 0xffff0000, addr + WIN_REMAP_LO_OFF);
		writel(0, addr + WIN_REMAP_HI_OFF);
	}
}

void __init kirkwood_setup_cpu_mbus(void)
{
	void __iomem *addr;
	int i;
	int cs;

	/*
	 * First, disable and clear windows.
	 */
	for (i = 0; i < 8; i++) {
		addr = (void __iomem *)WIN_OFF(i);

		writel(0, addr + WIN_BASE_OFF);
		writel(0, addr + WIN_CTRL_OFF);
		if (cpu_win_can_remap(i)) {
			writel(0, addr + WIN_REMAP_LO_OFF);
			writel(0, addr + WIN_REMAP_HI_OFF);
		}
	}

	/*
	 * Setup windows for PCIe IO+MEM space.
	 */
	setup_cpu_win(0, KIRKWOOD_PCIE_IO_PHYS_BASE, KIRKWOOD_PCIE_IO_SIZE,
		      TARGET_PCIE, ATTR_PCIE_IO, KIRKWOOD_PCIE_IO_BUS_BASE);
	setup_cpu_win(1, KIRKWOOD_PCIE_MEM_PHYS_BASE, KIRKWOOD_PCIE_MEM_SIZE,
		      TARGET_PCIE, ATTR_PCIE_MEM, -1);

	/*
	 * Setup window for NAND controller.
	 */
	setup_cpu_win(2, KIRKWOOD_NAND_MEM_PHYS_BASE, KIRKWOOD_NAND_MEM_SIZE,
		      TARGET_DEV_BUS, ATTR_DEV_NAND, -1);

	win_alloc_count = 3;

	/*
	 * Setup MBUS dram target info.
	 */
	kirkwood_mbus_dram_info.mbus_dram_target_id = TARGET_DDR;

	addr = (void __iomem *)DDR_WINDOW_CPU_BASE;

	for (i = 0, cs = 0; i < 4; i++) {
		u32 base = readl(addr + DDR_BASE_CS_OFF(i));
		u32 size = readl(addr + DDR_SIZE_CS_OFF(i));

		/*
		 * Chip select enabled?
		 */
		if (size & 1) {
			struct mbus_dram_window *w;

			w = &kirkwood_mbus_dram_info.cs[cs++];
			w->cs_index = i;
			w->mbus_attr = 0xf & ~(1 << i);
			w->base = base & 0xffff0000;
			w->size = (size | 0x0000ffff) + 1;
		}
	}
	kirkwood_mbus_dram_info.num_cs = cs;
}

void __init kirkwood_setup_sram_win(u32 base, u32 size)
{
	setup_cpu_win(win_alloc_count++, base, size, 0x03, 0x00, -1);
}
