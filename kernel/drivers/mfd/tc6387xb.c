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
 * Toshiba TC6387XB support
 * Copyright (c) 2005 Ian Molton
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This file contains TC6387XB base support.
 *
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/mfd/core.h>
#include <linux/mfd/tmio.h>
#include <linux/mfd/tc6387xb.h>

enum {
	TC6387XB_CELL_MMC,
};

#ifdef CONFIG_PM
static int tc6387xb_suspend(struct platform_device *dev, pm_message_t state)
{
	struct clk *clk32k = platform_get_drvdata(dev);
	struct tc6387xb_platform_data *pdata = dev->dev.platform_data;

	if (pdata && pdata->suspend)
		pdata->suspend(dev);
	clk_disable(clk32k);

	return 0;
}

static int tc6387xb_resume(struct platform_device *dev)
{
	struct clk *clk32k = platform_get_drvdata(dev);
	struct tc6387xb_platform_data *pdata = dev->dev.platform_data;

	clk_enable(clk32k);
	if (pdata && pdata->resume)
		pdata->resume(dev);

	return 0;
}
#else
#define tc6387xb_suspend  NULL
#define tc6387xb_resume   NULL
#endif

/*--------------------------------------------------------------------------*/

static int tc6387xb_mmc_enable(struct platform_device *mmc)
{
	struct platform_device *dev      = to_platform_device(mmc->dev.parent);
	struct clk *clk32k = platform_get_drvdata(dev);

	clk_enable(clk32k);

	return 0;
}

static int tc6387xb_mmc_disable(struct platform_device *mmc)
{
	struct platform_device *dev      = to_platform_device(mmc->dev.parent);
	struct clk *clk32k = platform_get_drvdata(dev);

	clk_disable(clk32k);

	return 0;
}

/*--------------------------------------------------------------------------*/

static struct resource tc6387xb_mmc_resources[] = {
	{
		.start = 0x800,
		.end   = 0x9ff,
		.flags = IORESOURCE_MEM,
	},
	{
		.start = 0x200,
		.end   = 0x2ff,
		.flags = IORESOURCE_MEM,
	},
	{
		.start = 0,
		.end   = 0,
		.flags = IORESOURCE_IRQ,
	},
};

static struct mfd_cell tc6387xb_cells[] = {
	[TC6387XB_CELL_MMC] = {
		.name = "tmio-mmc",
		.enable = tc6387xb_mmc_enable,
		.disable = tc6387xb_mmc_disable,
		.num_resources = ARRAY_SIZE(tc6387xb_mmc_resources),
		.resources = tc6387xb_mmc_resources,
	},
};

static int tc6387xb_probe(struct platform_device *dev)
{
	struct tc6387xb_platform_data *pdata = dev->dev.platform_data;
	struct resource *iomem;
	struct clk *clk32k;
	int irq, ret;

	iomem = platform_get_resource(dev, IORESOURCE_MEM, 0);
	if (!iomem) {
		return -EINVAL;
	}

	ret  = platform_get_irq(dev, 0);
	if (ret >= 0)
		irq = ret;
	else
		goto err_resource;

	clk32k = clk_get(&dev->dev, "CLK_CK32K");
	if (IS_ERR(clk32k)) {
		ret = PTR_ERR(clk32k);
		goto err_resource;
	}
	platform_set_drvdata(dev, clk32k);

	if (pdata && pdata->enable)
		pdata->enable(dev);

	printk(KERN_INFO "Toshiba tc6387xb initialised\n");

	tc6387xb_cells[TC6387XB_CELL_MMC].platform_data =
		&tc6387xb_cells[TC6387XB_CELL_MMC];
	tc6387xb_cells[TC6387XB_CELL_MMC].data_size =
		sizeof(tc6387xb_cells[TC6387XB_CELL_MMC]);

	ret = mfd_add_devices(&dev->dev, dev->id, tc6387xb_cells,
			      ARRAY_SIZE(tc6387xb_cells), iomem, irq);

	if (!ret)
		return 0;

	clk_put(clk32k);
err_resource:
	return ret;
}

static int tc6387xb_remove(struct platform_device *dev)
{
	struct clk *clk32k = platform_get_drvdata(dev);

	mfd_remove_devices(&dev->dev);
	clk_disable(clk32k);
	clk_put(clk32k);
	platform_set_drvdata(dev, NULL);

	return 0;
}


static struct platform_driver tc6387xb_platform_driver = {
	.driver = {
		.name		= "tc6387xb",
	},
	.probe		= tc6387xb_probe,
	.remove		= tc6387xb_remove,
	.suspend        = tc6387xb_suspend,
	.resume         = tc6387xb_resume,
};


static int __init tc6387xb_init(void)
{
	return platform_driver_register(&tc6387xb_platform_driver);
}

static void __exit tc6387xb_exit(void)
{
	platform_driver_unregister(&tc6387xb_platform_driver);
}

module_init(tc6387xb_init);
module_exit(tc6387xb_exit);

MODULE_DESCRIPTION("Toshiba TC6387XB core driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Ian Molton");
MODULE_ALIAS("platform:tc6387xb");
