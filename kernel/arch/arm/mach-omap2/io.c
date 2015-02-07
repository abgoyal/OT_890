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
 * linux/arch/arm/mach-omap2/io.c
 *
 * OMAP2 I/O mapping code
 *
 * Copyright (C) 2005 Nokia Corporation
 * Copyright (C) 2007 Texas Instruments
 *
 * Author:
 *	Juha Yrjola <juha.yrjola@nokia.com>
 *	Syed Khasim <x0khasim@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>

#include <asm/tlb.h>

#include <asm/mach/map.h>

#include <mach/mux.h>
#include <mach/omapfb.h>
#include <mach/sram.h>

#include "memory.h"

#include "clock.h"

#include <mach/powerdomain.h>

#include "powerdomains.h"

#include <mach/clockdomain.h>
#include "clockdomains.h"

/*
 * The machine specific code may provide the extra mapping besides the
 * default mapping provided here.
 */

#ifdef CONFIG_ARCH_OMAP24XX
static struct map_desc omap24xx_io_desc[] __initdata = {
	{
		.virtual	= L3_24XX_VIRT,
		.pfn		= __phys_to_pfn(L3_24XX_PHYS),
		.length		= L3_24XX_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= L4_24XX_VIRT,
		.pfn		= __phys_to_pfn(L4_24XX_PHYS),
		.length		= L4_24XX_SIZE,
		.type		= MT_DEVICE
	},
};

#ifdef CONFIG_ARCH_OMAP2420
static struct map_desc omap242x_io_desc[] __initdata = {
	{
		.virtual	= DSP_MEM_24XX_VIRT,
		.pfn		= __phys_to_pfn(DSP_MEM_24XX_PHYS),
		.length		= DSP_MEM_24XX_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= DSP_IPI_24XX_VIRT,
		.pfn		= __phys_to_pfn(DSP_IPI_24XX_PHYS),
		.length		= DSP_IPI_24XX_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= DSP_MMU_24XX_VIRT,
		.pfn		= __phys_to_pfn(DSP_MMU_24XX_PHYS),
		.length		= DSP_MMU_24XX_SIZE,
		.type		= MT_DEVICE
	},
};

#endif

#ifdef CONFIG_ARCH_OMAP2430
static struct map_desc omap243x_io_desc[] __initdata = {
	{
		.virtual	= L4_WK_243X_VIRT,
		.pfn		= __phys_to_pfn(L4_WK_243X_PHYS),
		.length		= L4_WK_243X_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= OMAP243X_GPMC_VIRT,
		.pfn		= __phys_to_pfn(OMAP243X_GPMC_PHYS),
		.length		= OMAP243X_GPMC_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= OMAP243X_SDRC_VIRT,
		.pfn		= __phys_to_pfn(OMAP243X_SDRC_PHYS),
		.length		= OMAP243X_SDRC_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= OMAP243X_SMS_VIRT,
		.pfn		= __phys_to_pfn(OMAP243X_SMS_PHYS),
		.length		= OMAP243X_SMS_SIZE,
		.type		= MT_DEVICE
	},
};
#endif
#endif

#ifdef	CONFIG_ARCH_OMAP34XX
static struct map_desc omap34xx_io_desc[] __initdata = {
	{
		.virtual	= L3_34XX_VIRT,
		.pfn		= __phys_to_pfn(L3_34XX_PHYS),
		.length		= L3_34XX_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= L4_34XX_VIRT,
		.pfn		= __phys_to_pfn(L4_34XX_PHYS),
		.length		= L4_34XX_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= L4_WK_34XX_VIRT,
		.pfn		= __phys_to_pfn(L4_WK_34XX_PHYS),
		.length		= L4_WK_34XX_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= OMAP34XX_GPMC_VIRT,
		.pfn		= __phys_to_pfn(OMAP34XX_GPMC_PHYS),
		.length		= OMAP34XX_GPMC_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= OMAP343X_SMS_VIRT,
		.pfn		= __phys_to_pfn(OMAP343X_SMS_PHYS),
		.length		= OMAP343X_SMS_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= OMAP343X_SDRC_VIRT,
		.pfn		= __phys_to_pfn(OMAP343X_SDRC_PHYS),
		.length		= OMAP343X_SDRC_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= L4_PER_34XX_VIRT,
		.pfn		= __phys_to_pfn(L4_PER_34XX_PHYS),
		.length		= L4_PER_34XX_SIZE,
		.type		= MT_DEVICE
	},
	{
		.virtual	= L4_EMU_34XX_VIRT,
		.pfn		= __phys_to_pfn(L4_EMU_34XX_PHYS),
		.length		= L4_EMU_34XX_SIZE,
		.type		= MT_DEVICE
	},
};
#endif

void __init omap2_map_common_io(void)
{
#if defined(CONFIG_ARCH_OMAP2420)
	iotable_init(omap24xx_io_desc, ARRAY_SIZE(omap24xx_io_desc));
	iotable_init(omap242x_io_desc, ARRAY_SIZE(omap242x_io_desc));
#endif

#if defined(CONFIG_ARCH_OMAP2430)
	iotable_init(omap24xx_io_desc, ARRAY_SIZE(omap24xx_io_desc));
	iotable_init(omap243x_io_desc, ARRAY_SIZE(omap243x_io_desc));
#endif

#if defined(CONFIG_ARCH_OMAP34XX)
	iotable_init(omap34xx_io_desc, ARRAY_SIZE(omap34xx_io_desc));
#endif

	/* Normally devicemaps_init() would flush caches and tlb after
	 * mdesc->map_io(), but we must also do it here because of the CPU
	 * revision check below.
	 */
	local_flush_tlb_all();
	flush_cache_all();

	omap2_check_revision();
	omap_sram_init();
	omapfb_reserve_sdram();
}

void __init omap2_init_common_hw(void)
{
	omap2_mux_init();
	pwrdm_init(powerdomains_omap);
	clkdm_init(clockdomains_omap, clkdm_pwrdm_autodeps);
	omap2_clk_init();
	omap2_init_memory();
	gpmc_init();
}