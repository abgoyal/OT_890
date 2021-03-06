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

/* linux/arch/arm/plat-s3c/include/plat/gpio-core.h
 *
 * Copyright 2008 Simtec Electronics
 *	http://armlinux.simtec.co.uk/
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * S3C Platform - GPIO core
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

/* Define the core gpiolib support functions that the s3c platforms may
 * need to extend or change depending on the hardware and the s3c chip
 * selected at build or found at run time.
 *
 * These definitions are not intended for driver inclusion, there is
 * nothing here that should not live outside the platform and core
 * specific code.
*/

struct s3c_gpio_cfg;

/**
 * struct s3c_gpio_chip - wrapper for specific implementation of gpio
 * @chip: The chip structure to be exported via gpiolib.
 * @base: The base pointer to the gpio configuration registers.
 * @config: special function and pull-resistor control information.
 *
 * This wrapper provides the necessary information for the Samsung
 * specific gpios being registered with gpiolib.
 */
struct s3c_gpio_chip {
	struct gpio_chip	chip;
	struct s3c_gpio_cfg	*config;
	void __iomem		*base;
};

static inline struct s3c_gpio_chip *to_s3c_gpio(struct gpio_chip *gpc)
{
	return container_of(gpc, struct s3c_gpio_chip, chip);
}

/** s3c_gpiolib_add() - add the s3c specific version of a gpio_chip.
 * @chip: The chip to register
 *
 * This is a wrapper to gpiochip_add() that takes our specific gpio chip
 * information and makes the necessary alterations for the platform and
 * notes the information for use with the configuration systems and any
 * other parts of the system.
 */
extern void s3c_gpiolib_add(struct s3c_gpio_chip *chip);

/* CONFIG_S3C_GPIO_TRACK enables the tracking of the s3c specific gpios
 * for use with the configuration calls, and other parts of the s3c gpiolib
 * support code.
 *
 * Not all s3c support code will need this, as some configurations of cpu
 * may only support one or two different configuration options and have an
 * easy gpio to s3c_gpio_chip mapping function. If this is the case, then
 * the machine support file should provide its own s3c_gpiolib_getchip()
 * and any other necessary functions.
 */

#ifdef CONFIG_S3C_GPIO_TRACK
extern struct s3c_gpio_chip *s3c_gpios[S3C_GPIO_END];

static inline struct s3c_gpio_chip *s3c_gpiolib_getchip(unsigned int chip)
{
	return (chip < S3C_GPIO_END) ? s3c_gpios[chip] : NULL;
}
#else
/* machine specific code should provide s3c_gpiolib_getchip */

static inline void s3c_gpiolib_track(struct s3c_gpio_chip *chip) { }
#endif
