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
 * arch/sh/kernel/cpu/sh4/clock-shx3.c
 *
 * SH-X3 support for the clock framework
 *
 *  Copyright (C) 2006-2007  Renesas Technology Corp.
 *  Copyright (C) 2006-2007  Renesas Solutions Corp.
 *  Copyright (C) 2006-2007  Paul Mundt
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/clock.h>
#include <asm/freq.h>
#include <asm/io.h>

static int ifc_divisors[] = { 1, 2, 4 ,6 };
static int bfc_divisors[] = { 1, 1, 1, 1, 1, 12, 16, 18, 24, 32, 36, 48 };
static int pfc_divisors[] = { 1, 1, 1, 1, 1, 1, 1, 18, 24, 32, 36, 48 };
static int cfc_divisors[] = { 1, 1, 4, 6 };

#define IFC_POS		28
#define IFC_MSK		0x0003
#define BFC_MSK		0x000f
#define PFC_MSK		0x000f
#define CFC_MSK		0x0003
#define BFC_POS		16
#define PFC_POS		0
#define CFC_POS		20

static void master_clk_init(struct clk *clk)
{
	clk->rate *= pfc_divisors[(ctrl_inl(FRQCR) >> PFC_POS) & PFC_MSK];
}

static struct clk_ops shx3_master_clk_ops = {
	.init		= master_clk_init,
};

static void module_clk_recalc(struct clk *clk)
{
	int idx = ((ctrl_inl(FRQCR) >> PFC_POS) & PFC_MSK);
	clk->rate = clk->parent->rate / pfc_divisors[idx];
}

static struct clk_ops shx3_module_clk_ops = {
	.recalc		= module_clk_recalc,
};

static void bus_clk_recalc(struct clk *clk)
{
	int idx = ((ctrl_inl(FRQCR) >> BFC_POS) & BFC_MSK);
	clk->rate = clk->parent->rate / bfc_divisors[idx];
}

static struct clk_ops shx3_bus_clk_ops = {
	.recalc		= bus_clk_recalc,
};

static void cpu_clk_recalc(struct clk *clk)
{
	int idx = ((ctrl_inl(FRQCR) >> IFC_POS) & IFC_MSK);
	clk->rate = clk->parent->rate / ifc_divisors[idx];
}

static struct clk_ops shx3_cpu_clk_ops = {
	.recalc		= cpu_clk_recalc,
};

static struct clk_ops *shx3_clk_ops[] = {
	&shx3_master_clk_ops,
	&shx3_module_clk_ops,
	&shx3_bus_clk_ops,
	&shx3_cpu_clk_ops,
};

void __init arch_init_clk_ops(struct clk_ops **ops, int idx)
{
	if (idx < ARRAY_SIZE(shx3_clk_ops))
		*ops = shx3_clk_ops[idx];
}

static void shyway_clk_recalc(struct clk *clk)
{
	int idx = ((ctrl_inl(FRQCR) >> CFC_POS) & CFC_MSK);
	clk->rate = clk->parent->rate / cfc_divisors[idx];
}

static struct clk_ops shx3_shyway_clk_ops = {
	.recalc		= shyway_clk_recalc,
};

static struct clk shx3_shyway_clk = {
	.name		= "shyway_clk",
	.flags		= CLK_ALWAYS_ENABLED,
	.ops		= &shx3_shyway_clk_ops,
};

/*
 * Additional SHx3-specific on-chip clocks that aren't already part of the
 * clock framework
 */
static struct clk *shx3_onchip_clocks[] = {
	&shx3_shyway_clk,
};

static int __init shx3_clk_init(void)
{
	struct clk *clk = clk_get(NULL, "master_clk");
	int i;

	for (i = 0; i < ARRAY_SIZE(shx3_onchip_clocks); i++) {
		struct clk *clkp = shx3_onchip_clocks[i];

		clkp->parent = clk;
		clk_register(clkp);
		clk_enable(clkp);
	}

	/*
	 * Now that we have the rest of the clocks registered, we need to
	 * force the parent clock to propagate so that these clocks will
	 * automatically figure out their rate. We cheat by handing the
	 * parent clock its current rate and forcing child propagation.
	 */
	clk_set_rate(clk, clk_get_rate(clk));

	clk_put(clk);

	return 0;
}
arch_initcall(shx3_clk_init);
