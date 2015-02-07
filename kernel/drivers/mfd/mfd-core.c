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
 * drivers/mfd/mfd-core.c
 *
 * core MFD support
 * Copyright (c) 2006 Ian Molton
 * Copyright (c) 2007,2008 Dmitry Baryshkov
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/mfd/core.h>

static int mfd_add_device(struct device *parent, int id,
			  const struct mfd_cell *cell,
			  struct resource *mem_base,
			  int irq_base)
{
	struct resource *res;
	struct platform_device *pdev;
	int ret = -ENOMEM;
	int r;

	pdev = platform_device_alloc(cell->name, id);
	if (!pdev)
		goto fail_alloc;

	res = kzalloc(sizeof(*res) * cell->num_resources, GFP_KERNEL);
	if (!res)
		goto fail_device;

	pdev->dev.parent = parent;
	platform_set_drvdata(pdev, cell->driver_data);

	ret = platform_device_add_data(pdev,
			cell->platform_data, cell->data_size);
	if (ret)
		goto fail_res;

	for (r = 0; r < cell->num_resources; r++) {
		res[r].name = cell->resources[r].name;
		res[r].flags = cell->resources[r].flags;

		/* Find out base to use */
		if (cell->resources[r].flags & IORESOURCE_MEM) {
			res[r].parent = mem_base;
			res[r].start = mem_base->start +
				cell->resources[r].start;
			res[r].end = mem_base->start +
				cell->resources[r].end;
		} else if (cell->resources[r].flags & IORESOURCE_IRQ) {
			res[r].start = irq_base +
				cell->resources[r].start;
			res[r].end   = irq_base +
				cell->resources[r].end;
		} else {
			res[r].parent = cell->resources[r].parent;
			res[r].start = cell->resources[r].start;
			res[r].end   = cell->resources[r].end;
		}
	}

	platform_device_add_resources(pdev, res, cell->num_resources);

	ret = platform_device_add(pdev);
	if (ret)
		goto fail_res;

	kfree(res);

	return 0;

/*	platform_device_del(pdev); */
fail_res:
	kfree(res);
fail_device:
	platform_device_put(pdev);
fail_alloc:
	return ret;
}

int mfd_add_devices(struct device *parent, int id,
		    const struct mfd_cell *cells, int n_devs,
		    struct resource *mem_base,
		    int irq_base)
{
	int i;
	int ret = 0;

	for (i = 0; i < n_devs; i++) {
		ret = mfd_add_device(parent, id, cells + i, mem_base, irq_base);
		if (ret)
			break;
	}

	if (ret)
		mfd_remove_devices(parent);

	return ret;
}
EXPORT_SYMBOL(mfd_add_devices);

static int mfd_remove_devices_fn(struct device *dev, void *unused)
{
	platform_device_unregister(to_platform_device(dev));
	return 0;
}

void mfd_remove_devices(struct device *parent)
{
	device_for_each_child(parent, NULL, mfd_remove_devices_fn);
}
EXPORT_SYMBOL(mfd_remove_devices);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ian Molton, Dmitry Baryshkov");
