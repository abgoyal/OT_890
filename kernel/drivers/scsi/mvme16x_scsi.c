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
 * Detection routine for the NCR53c710 based MVME16x SCSI Controllers for Linux.
 *
 * Based on work by Alan Hourihane
 *
 * Rewritten to use 53c700.c by Kars de Jong <jongk@linux-m68k.org>
 */

#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/mvme16xhw.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_transport.h>
#include <scsi/scsi_transport_spi.h>

#include "53c700.h"

MODULE_AUTHOR("Kars de Jong <jongk@linux-m68k.org>");
MODULE_DESCRIPTION("MVME16x NCR53C710 driver");
MODULE_LICENSE("GPL");

static struct scsi_host_template mvme16x_scsi_driver_template = {
	.name			= "MVME16x NCR53c710 SCSI",
	.proc_name		= "MVME16x",
	.this_id		= 7,
	.module			= THIS_MODULE,
};

static struct platform_device *mvme16x_scsi_device;

static __devinit int
mvme16x_probe(struct device *dev)
{
	struct Scsi_Host * host = NULL;
	struct NCR_700_Host_Parameters *hostdata;

	if (!MACH_IS_MVME16x)
		goto out;

	if (mvme16x_config & MVME16x_CONFIG_NO_SCSICHIP) {
		printk(KERN_INFO "mvme16x-scsi: detection disabled, "
				 "SCSI chip not present\n");
		goto out;
	}

	hostdata = kzalloc(sizeof(struct NCR_700_Host_Parameters), GFP_KERNEL);
	if (hostdata == NULL) {
		printk(KERN_ERR "mvme16x-scsi: "
				"Failed to allocate host data\n");
		goto out;
	}

	/* Fill in the required pieces of hostdata */
	hostdata->base = (void __iomem *)0xfff47000UL;
	hostdata->clock = 50;	/* XXX - depends on the CPU clock! */
	hostdata->chip710 = 1;
	hostdata->dmode_extra = DMODE_FC2;
	hostdata->dcntl_extra = EA_710;
	hostdata->ctest7_extra = CTEST7_TT1;

	/* and register the chip */
	host = NCR_700_detect(&mvme16x_scsi_driver_template, hostdata, dev);
	if (!host) {
		printk(KERN_ERR "mvme16x-scsi: No host detected; "
				"board configuration problem?\n");
		goto out_free;
	}
	host->this_id = 7;
	host->base = 0xfff47000UL;
	host->irq = MVME16x_IRQ_SCSI;
	if (request_irq(host->irq, NCR_700_intr, 0, "mvme16x-scsi", host)) {
		printk(KERN_ERR "mvme16x-scsi: request_irq failed\n");
		goto out_put_host;
	}

	/* Enable scsi chip ints */
	{
		volatile unsigned long v;

		/* Enable scsi interrupts at level 4 in PCCchip2 */
		v = in_be32(0xfff4202c);
		v = (v & ~0xff) | 0x10 | 4;
		out_be32(0xfff4202c, v);
	}

	dev_set_drvdata(dev, host);
	scsi_scan_host(host);

	return 0;

 out_put_host:
	scsi_host_put(host);
 out_free:
	kfree(hostdata);
 out:
	return -ENODEV;
}

static __devexit int
mvme16x_device_remove(struct device *dev)
{
	struct Scsi_Host *host = dev_get_drvdata(dev);
	struct NCR_700_Host_Parameters *hostdata = shost_priv(host);

	/* Disable scsi chip ints */
	{
		volatile unsigned long v;

		v = in_be32(0xfff4202c);
		v &= ~0x10;
		out_be32(0xfff4202c, v);
	}
	scsi_remove_host(host);
	NCR_700_release(host);
	kfree(hostdata);
	free_irq(host->irq, host);

	return 0;
}

static struct device_driver mvme16x_scsi_driver = {
	.name	= "mvme16x-scsi",
	.bus	= &platform_bus_type,
	.probe	= mvme16x_probe,
	.remove	= __devexit_p(mvme16x_device_remove),
};

static int __init mvme16x_scsi_init(void)
{
	int err;

	err = driver_register(&mvme16x_scsi_driver);
	if (err)
		return err;

	mvme16x_scsi_device = platform_device_register_simple("mvme16x-scsi",
							      -1, NULL, 0);
	if (IS_ERR(mvme16x_scsi_device)) {
		driver_unregister(&mvme16x_scsi_driver);
		return PTR_ERR(mvme16x_scsi_device);
	}

	return 0;
}

static void __exit mvme16x_scsi_exit(void)
{
	platform_device_unregister(mvme16x_scsi_device);
	driver_unregister(&mvme16x_scsi_driver);
}

module_init(mvme16x_scsi_init);
module_exit(mvme16x_scsi_exit);
