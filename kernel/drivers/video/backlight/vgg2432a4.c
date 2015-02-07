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

/* drivers/video/backlight/vgg2432a4.c
 *
 * VGG2432A4 (ILI9320) LCD controller driver.
 *
 * Copyright 2007 Simtec Electronics
 *	http://armlinux.simtec.co.uk/
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/lcd.h>
#include <linux/module.h>

#include <linux/spi/spi.h>

#include <video/ili9320.h>

#include "ili9320.h"

/* Device initialisation sequences */

static struct ili9320_reg vgg_init1[] = {
	{
		.address = ILI9320_POWER1,
		.value	 = ILI9320_POWER1_AP(0) | ILI9320_POWER1_BT(0),
	}, {
		.address = ILI9320_POWER2,
		.value	 = (ILI9320_POWER2_VC(7) |
			    ILI9320_POWER2_DC0(0) | ILI9320_POWER2_DC1(0)),
	}, {
		.address = ILI9320_POWER3,
		.value	 = ILI9320_POWER3_VRH(0),
	}, {
		.address = ILI9320_POWER4,
		.value	 = ILI9320_POWER4_VREOUT(0),
	},
};

static struct ili9320_reg vgg_init2[] = {
	{
		.address = ILI9320_POWER1,
		.value   = (ILI9320_POWER1_AP(3) | ILI9320_POWER1_APE |
			    ILI9320_POWER1_BT(7) | ILI9320_POWER1_SAP),
	}, {
		.address = ILI9320_POWER2,
		.value   = ILI9320_POWER2_VC(7) | ILI9320_POWER2_DC0(3),
	}
};

static struct ili9320_reg vgg_gamma[] = {
	{
		.address = ILI9320_GAMMA1,
		.value	 = 0x0000,
	}, {
		.address = ILI9320_GAMMA2,
		.value   = 0x0505,
	}, {
		.address = ILI9320_GAMMA3,
		.value	 = 0x0004,
	}, {
		.address = ILI9320_GAMMA4,
		.value	 = 0x0006,
	}, {
		.address = ILI9320_GAMMA5,
		.value	 = 0x0707,
	}, {
		.address = ILI9320_GAMMA6,
		.value	 = 0x0105,
	}, {
		.address = ILI9320_GAMMA7,
		.value	 = 0x0002,
	}, {
		.address = ILI9320_GAMMA8,
		.value	 = 0x0707,
	}, {
		.address = ILI9320_GAMMA9,
		.value	 = 0x0704,
	}, {
		.address = ILI9320_GAMMA10,
		.value	 = 0x807,
	}

};

static struct ili9320_reg vgg_init0[] = {
	[0]	= {
		/* set direction and scan mode gate */
		.address = ILI9320_DRIVER,
		.value	 = ILI9320_DRIVER_SS,
	}, {
		.address = ILI9320_DRIVEWAVE,
		.value	 = (ILI9320_DRIVEWAVE_MUSTSET |
			    ILI9320_DRIVEWAVE_EOR | ILI9320_DRIVEWAVE_BC),
	}, {
		.address = ILI9320_ENTRYMODE,
		.value	 = ILI9320_ENTRYMODE_ID(3) | ILI9320_ENTRYMODE_BGR,
	}, {
		.address = ILI9320_RESIZING,
		.value	 = 0x0,
	},
};


static int vgg2432a4_lcd_init(struct ili9320 *lcd,
			      struct ili9320_platdata *cfg)
{
	unsigned int addr;
	int ret;

	/* Set VCore before anything else (VGG243237-6UFLWA) */
	ret = ili9320_write(lcd, 0x00e5, 0x8000);
	if (ret)
		goto err_initial;

	/* Start the oscillator up before we can do anything else. */
	ret = ili9320_write(lcd, ILI9320_OSCILATION, ILI9320_OSCILATION_OSC);
	if (ret)
		goto err_initial;

	/* must wait at-lesat 10ms after starting */
	mdelay(15);

	ret = ili9320_write_regs(lcd, vgg_init0, ARRAY_SIZE(vgg_init0));
	if (ret != 0)
		goto err_initial;

	ili9320_write(lcd, ILI9320_DISPLAY2, cfg->display2);
	ili9320_write(lcd, ILI9320_DISPLAY3, cfg->display3);
	ili9320_write(lcd, ILI9320_DISPLAY4, cfg->display4);

	ili9320_write(lcd, ILI9320_RGB_IF1, cfg->rgb_if1);
	ili9320_write(lcd, ILI9320_FRAMEMAKER, 0x0);
	ili9320_write(lcd, ILI9320_RGB_IF2, cfg->rgb_if2);

	ret = ili9320_write_regs(lcd, vgg_init1, ARRAY_SIZE(vgg_init1));
	if (ret != 0)
		goto err_vgg;

	mdelay(300);

	ret = ili9320_write_regs(lcd, vgg_init2, ARRAY_SIZE(vgg_init2));
	if (ret != 0)
		goto err_vgg2;

	mdelay(100);

	ili9320_write(lcd, ILI9320_POWER3, 0x13c);

	mdelay(100);

	ili9320_write(lcd, ILI9320_POWER4, 0x1c00);
	ili9320_write(lcd, ILI9320_POWER7, 0x000e);

	mdelay(100);

	ili9320_write(lcd, ILI9320_GRAM_HORIZ_ADDR, 0x00);
	ili9320_write(lcd, ILI9320_GRAM_VERT_ADD, 0x00);

	ret = ili9320_write_regs(lcd, vgg_gamma, ARRAY_SIZE(vgg_gamma));
	if (ret != 0)
		goto err_vgg3;

	ili9320_write(lcd, ILI9320_HORIZ_START, 0x0);
	ili9320_write(lcd, ILI9320_HORIZ_END, cfg->hsize - 1);
	ili9320_write(lcd, ILI9320_VERT_START, 0x0);
	ili9320_write(lcd, ILI9320_VERT_END, cfg->vsize - 1);

	ili9320_write(lcd, ILI9320_DRIVER2,
		      ILI9320_DRIVER2_NL(((cfg->vsize - 240) / 8) + 0x1D));

	ili9320_write(lcd, ILI9320_BASE_IMAGE, 0x1);
	ili9320_write(lcd, ILI9320_VERT_SCROLL, 0x00);

	for (addr = ILI9320_PARTIAL1_POSITION; addr <= ILI9320_PARTIAL2_END;
	     addr++) {
		ili9320_write(lcd, addr, 0x0);
	}

	ili9320_write(lcd, ILI9320_INTERFACE1, 0x10);
	ili9320_write(lcd, ILI9320_INTERFACE2, cfg->interface2);
	ili9320_write(lcd, ILI9320_INTERFACE3, cfg->interface3);
	ili9320_write(lcd, ILI9320_INTERFACE4, cfg->interface4);
	ili9320_write(lcd, ILI9320_INTERFACE5, cfg->interface5);
	ili9320_write(lcd, ILI9320_INTERFACE6, cfg->interface6);

	lcd->display1 = (ILI9320_DISPLAY1_D(3) | ILI9320_DISPLAY1_DTE |
			 ILI9320_DISPLAY1_GON | ILI9320_DISPLAY1_BASEE |
			 0x40);

	ili9320_write(lcd, ILI9320_DISPLAY1, lcd->display1);

	return 0;

 err_vgg3:
 err_vgg2:
 err_vgg:
 err_initial:
	return ret;
}

#ifdef CONFIG_PM
static int vgg2432a4_suspend(struct spi_device *spi, pm_message_t state)
{
	return ili9320_suspend(dev_get_drvdata(&spi->dev), state);
}

static int vgg2432a4_resume(struct spi_device *spi)
{
	return ili9320_resume(dev_get_drvdata(&spi->dev));
}
#else
#define vgg2432a4_suspend	NULL
#define vgg2432a4_resume 	NULL
#endif

static struct ili9320_client vgg2432a4_client = {
	.name	= "VGG2432A4",
	.init	= vgg2432a4_lcd_init,
};

/* Device probe */

static int __devinit vgg2432a4_probe(struct spi_device *spi)
{
	int ret;

	ret = ili9320_probe_spi(spi, &vgg2432a4_client);
	if (ret != 0) {
		dev_err(&spi->dev, "failed to initialise ili9320\n");
		return ret;
	}

	return 0;
}

static int __devexit vgg2432a4_remove(struct spi_device *spi)
{
	return ili9320_remove(dev_get_drvdata(&spi->dev));
}

static void vgg2432a4_shutdown(struct spi_device *spi)
{
	ili9320_shutdown(dev_get_drvdata(&spi->dev));
}

static struct spi_driver vgg2432a4_driver = {
	.driver = {
		.name		= "VGG2432A4",
		.owner		= THIS_MODULE,
	},
	.probe		= vgg2432a4_probe,
	.remove		= __devexit_p(vgg2432a4_remove),
	.shutdown	= vgg2432a4_shutdown,
	.suspend	= vgg2432a4_suspend,
	.resume		= vgg2432a4_resume,
};

/* Device driver initialisation */

static int __init vgg2432a4_init(void)
{
	return spi_register_driver(&vgg2432a4_driver);
}

static void __exit vgg2432a4_exit(void)
{
	spi_unregister_driver(&vgg2432a4_driver);
}

module_init(vgg2432a4_init);
module_exit(vgg2432a4_exit);

MODULE_AUTHOR("Ben Dooks <ben-linux@fluff.org>");
MODULE_DESCRIPTION("VGG2432A4 LCD Driver");
MODULE_LICENSE("GPL v2");

