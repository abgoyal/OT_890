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
 * lm70.c
 *
 * The LM70 is a temperature sensor chip from National Semiconductor (NS).
 * Copyright (C) 2006 Kaiwan N Billimoria <kaiwan@designergraphix.com>
 *
 * The LM70 communicates with a host processor via an SPI/Microwire Bus
 * interface. The complete datasheet is available at National's website
 * here:
 * http://www.national.com/pf/LM/LM70.html
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/sysfs.h>
#include <linux/hwmon.h>
#include <linux/mutex.h>
#include <linux/spi/spi.h>


#define DRVNAME		"lm70"

#define LM70_CHIP_LM70		0	/* original NS LM70 */
#define LM70_CHIP_TMP121	1	/* TI TMP121/TMP123 */

struct lm70 {
	struct device *hwmon_dev;
	struct mutex lock;
	unsigned int chip;
};

/* sysfs hook function */
static ssize_t lm70_sense_temp(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct spi_device *spi = to_spi_device(dev);
	int status, val = 0;
	u8 rxbuf[2];
	s16 raw=0;
	struct lm70 *p_lm70 = dev_get_drvdata(&spi->dev);

	if (mutex_lock_interruptible(&p_lm70->lock))
		return -ERESTARTSYS;

	/*
	 * spi_read() requires a DMA-safe buffer; so we use
	 * spi_write_then_read(), transmitting 0 bytes.
	 */
	status = spi_write_then_read(spi, NULL, 0, &rxbuf[0], 2);
	if (status < 0) {
		printk(KERN_WARNING
		"spi_write_then_read failed with status %d\n", status);
		goto out;
	}
	raw = (rxbuf[0] << 8) + rxbuf[1];
	dev_dbg(dev, "rxbuf[0] : 0x%02x rxbuf[1] : 0x%02x raw=0x%04x\n",
		rxbuf[0], rxbuf[1], raw);

	/*
	 * LM70:
	 * The "raw" temperature read into rxbuf[] is a 16-bit signed 2's
	 * complement value. Only the MSB 11 bits (1 sign + 10 temperature
	 * bits) are meaningful; the LSB 5 bits are to be discarded.
	 * See the datasheet.
	 *
	 * Further, each bit represents 0.25 degrees Celsius; so, multiply
	 * by 0.25. Also multiply by 1000 to represent in millidegrees
	 * Celsius.
	 * So it's equivalent to multiplying by 0.25 * 1000 = 250.
	 *
	 * TMP121/TMP123:
	 * 13 bits of 2's complement data, discard LSB 3 bits,
	 * resolution 0.0625 degrees celsius.
	 */
	switch (p_lm70->chip) {
	case LM70_CHIP_LM70:
		val = ((int)raw / 32) * 250;
		break;

	case LM70_CHIP_TMP121:
		val = ((int)raw / 8) * 625 / 10;
		break;
	}

	status = sprintf(buf, "%d\n", val); /* millidegrees Celsius */
out:
	mutex_unlock(&p_lm70->lock);
	return status;
}

static DEVICE_ATTR(temp1_input, S_IRUGO, lm70_sense_temp, NULL);

static ssize_t lm70_show_name(struct device *dev, struct device_attribute
			      *devattr, char *buf)
{
	struct lm70 *p_lm70 = dev_get_drvdata(dev);
	int ret;

	switch (p_lm70->chip) {
	case LM70_CHIP_LM70:
		ret = sprintf(buf, "lm70\n");
		break;
	case LM70_CHIP_TMP121:
		ret = sprintf(buf, "tmp121\n");
		break;
	default:
		ret = -EINVAL;
	}
	return ret;
}

static DEVICE_ATTR(name, S_IRUGO, lm70_show_name, NULL);

/*----------------------------------------------------------------------*/

static int __devinit common_probe(struct spi_device *spi, int chip)
{
	struct lm70 *p_lm70;
	int status;

	/* NOTE:  we assume 8-bit words, and convert to 16 bits manually */

	p_lm70 = kzalloc(sizeof *p_lm70, GFP_KERNEL);
	if (!p_lm70)
		return -ENOMEM;

	mutex_init(&p_lm70->lock);
	p_lm70->chip = chip;

	/* sysfs hook */
	p_lm70->hwmon_dev = hwmon_device_register(&spi->dev);
	if (IS_ERR(p_lm70->hwmon_dev)) {
		dev_dbg(&spi->dev, "hwmon_device_register failed.\n");
		status = PTR_ERR(p_lm70->hwmon_dev);
		goto out_dev_reg_failed;
	}
	dev_set_drvdata(&spi->dev, p_lm70);

	if ((status = device_create_file(&spi->dev, &dev_attr_temp1_input))
	 || (status = device_create_file(&spi->dev, &dev_attr_name))) {
		dev_dbg(&spi->dev, "device_create_file failure.\n");
		goto out_dev_create_file_failed;
	}

	return 0;

out_dev_create_file_failed:
	device_remove_file(&spi->dev, &dev_attr_temp1_input);
	hwmon_device_unregister(p_lm70->hwmon_dev);
out_dev_reg_failed:
	dev_set_drvdata(&spi->dev, NULL);
	kfree(p_lm70);
	return status;
}

static int __devinit lm70_probe(struct spi_device *spi)
{
	/* signaling is SPI_MODE_0 on a 3-wire link (shared SI/SO) */
	if ((spi->mode & (SPI_CPOL | SPI_CPHA)) || !(spi->mode & SPI_3WIRE))
		return -EINVAL;

	return common_probe(spi, LM70_CHIP_LM70);
}

static int __devinit tmp121_probe(struct spi_device *spi)
{
	/* signaling is SPI_MODE_0 with only MISO connected */
	if (spi->mode & (SPI_CPOL | SPI_CPHA))
		return -EINVAL;

	return common_probe(spi, LM70_CHIP_TMP121);
}

static int __devexit lm70_remove(struct spi_device *spi)
{
	struct lm70 *p_lm70 = dev_get_drvdata(&spi->dev);

	device_remove_file(&spi->dev, &dev_attr_temp1_input);
	device_remove_file(&spi->dev, &dev_attr_name);
	hwmon_device_unregister(p_lm70->hwmon_dev);
	dev_set_drvdata(&spi->dev, NULL);
	kfree(p_lm70);

	return 0;
}

static struct spi_driver tmp121_driver = {
	.driver = {
		.name	= "tmp121",
		.owner	= THIS_MODULE,
	},
	.probe	= tmp121_probe,
	.remove	= __devexit_p(lm70_remove),
};

static struct spi_driver lm70_driver = {
	.driver = {
		.name	= "lm70",
		.owner	= THIS_MODULE,
	},
	.probe	= lm70_probe,
	.remove	= __devexit_p(lm70_remove),
};

static int __init init_lm70(void)
{
	int ret = spi_register_driver(&lm70_driver);
	if (ret)
		return ret;

	ret = spi_register_driver(&tmp121_driver);
	if (ret)
		spi_unregister_driver(&lm70_driver);

	return ret;
}

static void __exit cleanup_lm70(void)
{
	spi_unregister_driver(&lm70_driver);
	spi_unregister_driver(&tmp121_driver);
}

module_init(init_lm70);
module_exit(cleanup_lm70);

MODULE_AUTHOR("Kaiwan N Billimoria");
MODULE_DESCRIPTION("NS LM70 / TI TMP121/TMP123 Linux driver");
MODULE_LICENSE("GPL");
