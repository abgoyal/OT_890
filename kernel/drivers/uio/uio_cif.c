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
 * UIO Hilscher CIF card driver
 *
 * (C) 2007 Hans J. Koch <hjk@linutronix.de>
 * Original code (C) 2005 Benedikt Spranger <b.spranger@linutronix.de>
 *
 * Licensed under GPL version 2 only.
 *
 */

#include <linux/device.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/uio_driver.h>

#include <asm/io.h>

#define PLX9030_INTCSR		0x4C
#define INTSCR_INT1_ENABLE	0x01
#define INTSCR_INT1_STATUS	0x04
#define INT1_ENABLED_AND_ACTIVE	(INTSCR_INT1_ENABLE | INTSCR_INT1_STATUS)

#define PCI_SUBVENDOR_ID_PEP	0x1518
#define CIF_SUBDEVICE_PROFIBUS	0x430
#define CIF_SUBDEVICE_DEVICENET	0x432


static irqreturn_t hilscher_handler(int irq, struct uio_info *dev_info)
{
	void __iomem *plx_intscr = dev_info->mem[0].internal_addr
					+ PLX9030_INTCSR;

	if ((ioread8(plx_intscr) & INT1_ENABLED_AND_ACTIVE)
	    != INT1_ENABLED_AND_ACTIVE)
		return IRQ_NONE;

	/* Disable interrupt */
	iowrite8(ioread8(plx_intscr) & ~INTSCR_INT1_ENABLE, plx_intscr);
	return IRQ_HANDLED;
}

static int __devinit hilscher_pci_probe(struct pci_dev *dev,
					const struct pci_device_id *id)
{
	struct uio_info *info;

	info = kzalloc(sizeof(struct uio_info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	if (pci_enable_device(dev))
		goto out_free;

	if (pci_request_regions(dev, "hilscher"))
		goto out_disable;

	info->mem[0].addr = pci_resource_start(dev, 0);
	if (!info->mem[0].addr)
		goto out_release;
	info->mem[0].internal_addr = pci_ioremap_bar(dev, 0);
	if (!info->mem[0].internal_addr)
		goto out_release;

	info->mem[0].size = pci_resource_len(dev, 0);
	info->mem[0].memtype = UIO_MEM_PHYS;
	info->mem[1].addr = pci_resource_start(dev, 2);
	info->mem[1].size = pci_resource_len(dev, 2);
	info->mem[1].memtype = UIO_MEM_PHYS;
	switch (id->subdevice) {
		case CIF_SUBDEVICE_PROFIBUS:
			info->name = "CIF_Profibus";
			break;
		case CIF_SUBDEVICE_DEVICENET:
			info->name = "CIF_Devicenet";
			break;
		default:
			info->name = "CIF_???";
	}
	info->version = "0.0.1";
	info->irq = dev->irq;
	info->irq_flags = IRQF_DISABLED | IRQF_SHARED;
	info->handler = hilscher_handler;

	if (uio_register_device(&dev->dev, info))
		goto out_unmap;

	pci_set_drvdata(dev, info);

	return 0;
out_unmap:
	iounmap(info->mem[0].internal_addr);
out_release:
	pci_release_regions(dev);
out_disable:
	pci_disable_device(dev);
out_free:
	kfree (info);
	return -ENODEV;
}

static void hilscher_pci_remove(struct pci_dev *dev)
{
	struct uio_info *info = pci_get_drvdata(dev);

	uio_unregister_device(info);
	pci_release_regions(dev);
	pci_disable_device(dev);
	pci_set_drvdata(dev, NULL);
	iounmap(info->mem[0].internal_addr);

	kfree (info);
}

static struct pci_device_id hilscher_pci_ids[] __devinitdata = {
	{
		.vendor =	PCI_VENDOR_ID_PLX,
		.device =	PCI_DEVICE_ID_PLX_9030,
		.subvendor =	PCI_SUBVENDOR_ID_PEP,
		.subdevice =	CIF_SUBDEVICE_PROFIBUS,
	},
	{
		.vendor =	PCI_VENDOR_ID_PLX,
		.device =	PCI_DEVICE_ID_PLX_9030,
		.subvendor =	PCI_SUBVENDOR_ID_PEP,
		.subdevice =	CIF_SUBDEVICE_DEVICENET,
	},
	{ 0, }
};

static struct pci_driver hilscher_pci_driver = {
	.name = "hilscher",
	.id_table = hilscher_pci_ids,
	.probe = hilscher_pci_probe,
	.remove = hilscher_pci_remove,
};

static int __init hilscher_init_module(void)
{
	return pci_register_driver(&hilscher_pci_driver);
}

static void __exit hilscher_exit_module(void)
{
	pci_unregister_driver(&hilscher_pci_driver);
}

module_init(hilscher_init_module);
module_exit(hilscher_exit_module);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Hans J. Koch, Benedikt Spranger");
