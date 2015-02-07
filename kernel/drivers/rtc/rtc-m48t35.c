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
 * Driver for the SGS-Thomson M48T35 Timekeeper RAM chip
 *
 * Copyright (C) 2000 Silicon Graphics, Inc.
 * Written by Ulf Carlsson (ulfc@engr.sgi.com)
 *
 * Copyright (C) 2008 Thomas Bogendoerfer
 *
 * Based on code written by Paul Gortmaker.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */

#include <linux/module.h>
#include <linux/rtc.h>
#include <linux/platform_device.h>
#include <linux/bcd.h>
#include <linux/io.h>

#define DRV_VERSION		"1.0"

struct m48t35_rtc {
	u8	pad[0x7ff8];    /* starts at 0x7ff8 */
	u8	control;
	u8	sec;
	u8	min;
	u8	hour;
	u8	day;
	u8	date;
	u8	month;
	u8	year;
};

#define M48T35_RTC_SET		0x80
#define M48T35_RTC_READ		0x40

struct m48t35_priv {
	struct rtc_device *rtc;
	struct m48t35_rtc __iomem *reg;
	size_t size;
	unsigned long baseaddr;
	spinlock_t lock;
};

static int m48t35_read_time(struct device *dev, struct rtc_time *tm)
{
	struct m48t35_priv *priv = dev_get_drvdata(dev);
	u8 control;

	/*
	 * Only the values that we read from the RTC are set. We leave
	 * tm_wday, tm_yday and tm_isdst untouched. Even though the
	 * RTC has RTC_DAY_OF_WEEK, we ignore it, as it is only updated
	 * by the RTC when initially set to a non-zero value.
	 */
	spin_lock_irq(&priv->lock);
	control = readb(&priv->reg->control);
	writeb(control | M48T35_RTC_READ, &priv->reg->control);
	tm->tm_sec = readb(&priv->reg->sec);
	tm->tm_min = readb(&priv->reg->min);
	tm->tm_hour = readb(&priv->reg->hour);
	tm->tm_mday = readb(&priv->reg->date);
	tm->tm_mon = readb(&priv->reg->month);
	tm->tm_year = readb(&priv->reg->year);
	writeb(control, &priv->reg->control);
	spin_unlock_irq(&priv->lock);

	tm->tm_sec = bcd2bin(tm->tm_sec);
	tm->tm_min = bcd2bin(tm->tm_min);
	tm->tm_hour = bcd2bin(tm->tm_hour);
	tm->tm_mday = bcd2bin(tm->tm_mday);
	tm->tm_mon = bcd2bin(tm->tm_mon);
	tm->tm_year = bcd2bin(tm->tm_year);

	/*
	 * Account for differences between how the RTC uses the values
	 * and how they are defined in a struct rtc_time;
	 */
	tm->tm_year += 70;
	if (tm->tm_year <= 69)
		tm->tm_year += 100;

	tm->tm_mon--;
	return rtc_valid_tm(tm);
}

static int m48t35_set_time(struct device *dev, struct rtc_time *tm)
{
	struct m48t35_priv *priv = dev_get_drvdata(dev);
	unsigned char mon, day, hrs, min, sec;
	unsigned int yrs;
	u8 control;

	yrs = tm->tm_year + 1900;
	mon = tm->tm_mon + 1;   /* tm_mon starts at zero */
	day = tm->tm_mday;
	hrs = tm->tm_hour;
	min = tm->tm_min;
	sec = tm->tm_sec;

	if (yrs < 1970)
		return -EINVAL;

	yrs -= 1970;
	if (yrs > 255)    /* They are unsigned */
		return -EINVAL;

	if (yrs > 169)
		return -EINVAL;

	if (yrs >= 100)
		yrs -= 100;

	sec = bin2bcd(sec);
	min = bin2bcd(min);
	hrs = bin2bcd(hrs);
	day = bin2bcd(day);
	mon = bin2bcd(mon);
	yrs = bin2bcd(yrs);

	spin_lock_irq(&priv->lock);
	control = readb(&priv->reg->control);
	writeb(control | M48T35_RTC_SET, &priv->reg->control);
	writeb(yrs, &priv->reg->year);
	writeb(mon, &priv->reg->month);
	writeb(day, &priv->reg->date);
	writeb(hrs, &priv->reg->hour);
	writeb(min, &priv->reg->min);
	writeb(sec, &priv->reg->sec);
	writeb(control, &priv->reg->control);
	spin_unlock_irq(&priv->lock);
	return 0;
}

static const struct rtc_class_ops m48t35_ops = {
	.read_time	= m48t35_read_time,
	.set_time	= m48t35_set_time,
};

static int __devinit m48t35_probe(struct platform_device *pdev)
{
	struct rtc_device *rtc;
	struct resource *res;
	struct m48t35_priv *priv;
	int ret = 0;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENODEV;
	priv = kzalloc(sizeof(struct m48t35_priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->size = res->end - res->start + 1;
	/*
	 * kludge: remove the #ifndef after ioc3 resource
	 * conflicts are resolved
	 */
#ifndef CONFIG_SGI_IP27
	if (!request_mem_region(res->start, priv->size, pdev->name)) {
		ret = -EBUSY;
		goto out;
	}
#endif
	priv->baseaddr = res->start;
	priv->reg = ioremap(priv->baseaddr, priv->size);
	if (!priv->reg) {
		ret = -ENOMEM;
		goto out;
	}
	spin_lock_init(&priv->lock);
	rtc = rtc_device_register("m48t35", &pdev->dev,
				  &m48t35_ops, THIS_MODULE);
	if (IS_ERR(rtc)) {
		ret = PTR_ERR(rtc);
		goto out;
	}
	priv->rtc = rtc;
	platform_set_drvdata(pdev, priv);
	return 0;

out:
	if (priv->rtc)
		rtc_device_unregister(priv->rtc);
	if (priv->reg)
		iounmap(priv->reg);
	if (priv->baseaddr)
		release_mem_region(priv->baseaddr, priv->size);
	kfree(priv);
	return ret;
}

static int __devexit m48t35_remove(struct platform_device *pdev)
{
	struct m48t35_priv *priv = platform_get_drvdata(pdev);

	rtc_device_unregister(priv->rtc);
	iounmap(priv->reg);
#ifndef CONFIG_SGI_IP27
	release_mem_region(priv->baseaddr, priv->size);
#endif
	kfree(priv);
	return 0;
}

static struct platform_driver m48t35_platform_driver = {
	.driver		= {
		.name	= "rtc-m48t35",
		.owner	= THIS_MODULE,
	},
	.probe		= m48t35_probe,
	.remove		= __devexit_p(m48t35_remove),
};

static int __init m48t35_init(void)
{
	return platform_driver_register(&m48t35_platform_driver);
}

static void __exit m48t35_exit(void)
{
	platform_driver_unregister(&m48t35_platform_driver);
}

MODULE_AUTHOR("Thomas Bogendoerfer <tsbogend@alpha.franken.de>");
MODULE_DESCRIPTION("M48T35 RTC driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
MODULE_ALIAS("platform:rtc-m48t35");

module_init(m48t35_init);
module_exit(m48t35_exit);
