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
 *  gpio.h: GPIO Support for PNX833X.
 *
 *  Copyright 2008 NXP Semiconductors
 *	  Chris Steel <chris.steel@nxp.com>
 *    Daniel Laird <daniel.j.laird@nxp.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef __ASM_MIPS_MACH_PNX833X_GPIO_H
#define __ASM_MIPS_MACH_PNX833X_GPIO_H

/* BIG FAT WARNING: races danger!
   No protections exist here. Current users are only early init code,
   when locking is not needed because no cuncurency yet exists there,
   and GPIO IRQ dispatcher, which does locking.
   However, if many uses will ever happen, proper locking will be needed
   - including locking between different uses
*/

#include "pnx833x.h"

#define SET_REG_BIT(reg, bit)		do { (reg |= (1 << (bit))); } while (0)
#define CLEAR_REG_BIT(reg, bit)		do { (reg &= ~(1 << (bit))); } while (0)

/* Initialize GPIO to a known state */
static inline void pnx833x_gpio_init(void)
{
	PNX833X_PIO_DIR = 0;
	PNX833X_PIO_DIR2 = 0;
	PNX833X_PIO_SEL = 0;
	PNX833X_PIO_SEL2 = 0;
	PNX833X_PIO_INT_EDGE = 0;
	PNX833X_PIO_INT_HI = 0;
	PNX833X_PIO_INT_LO = 0;

	/* clear any GPIO interrupt requests */
	PNX833X_PIO_INT_CLEAR = 0xffff;
	PNX833X_PIO_INT_CLEAR = 0;
	PNX833X_PIO_INT_ENABLE = 0;
}

/* Select GPIO direction for a pin */
static inline void pnx833x_gpio_select_input(unsigned int pin)
{
	if (pin < 32)
		CLEAR_REG_BIT(PNX833X_PIO_DIR, pin);
	else
		CLEAR_REG_BIT(PNX833X_PIO_DIR2, pin & 31);
}
static inline void pnx833x_gpio_select_output(unsigned int pin)
{
	if (pin < 32)
		SET_REG_BIT(PNX833X_PIO_DIR, pin);
	else
		SET_REG_BIT(PNX833X_PIO_DIR2, pin & 31);
}

/* Select GPIO or alternate function for a pin */
static inline void pnx833x_gpio_select_function_io(unsigned int pin)
{
	if (pin < 32)
		CLEAR_REG_BIT(PNX833X_PIO_SEL, pin);
	else
		CLEAR_REG_BIT(PNX833X_PIO_SEL2, pin & 31);
}
static inline void pnx833x_gpio_select_function_alt(unsigned int pin)
{
	if (pin < 32)
		SET_REG_BIT(PNX833X_PIO_SEL, pin);
	else
		SET_REG_BIT(PNX833X_PIO_SEL2, pin & 31);
}

/* Read GPIO pin */
static inline int pnx833x_gpio_read(unsigned int pin)
{
	if (pin < 32)
		return (PNX833X_PIO_IN >> pin) & 1;
	else
		return (PNX833X_PIO_IN2 >> (pin & 31)) & 1;
}

/* Write GPIO pin */
static inline void pnx833x_gpio_write(unsigned int val, unsigned int pin)
{
	if (pin < 32) {
		if (val)
			SET_REG_BIT(PNX833X_PIO_OUT, pin);
		else
			CLEAR_REG_BIT(PNX833X_PIO_OUT, pin);
	} else {
		if (val)
			SET_REG_BIT(PNX833X_PIO_OUT2, pin & 31);
		else
			CLEAR_REG_BIT(PNX833X_PIO_OUT2, pin & 31);
	}
}

/* Configure GPIO interrupt */
#define GPIO_INT_NONE		0
#define GPIO_INT_LEVEL_LOW	1
#define GPIO_INT_LEVEL_HIGH	2
#define GPIO_INT_EDGE_RISING	3
#define GPIO_INT_EDGE_FALLING	4
#define GPIO_INT_EDGE_BOTH	5
static inline void pnx833x_gpio_setup_irq(int when, unsigned int pin)
{
	switch (when) {
	case GPIO_INT_LEVEL_LOW:
		CLEAR_REG_BIT(PNX833X_PIO_INT_EDGE, pin);
		CLEAR_REG_BIT(PNX833X_PIO_INT_HI, pin);
		SET_REG_BIT(PNX833X_PIO_INT_LO, pin);
		break;
	case GPIO_INT_LEVEL_HIGH:
		CLEAR_REG_BIT(PNX833X_PIO_INT_EDGE, pin);
		SET_REG_BIT(PNX833X_PIO_INT_HI, pin);
		CLEAR_REG_BIT(PNX833X_PIO_INT_LO, pin);
		break;
	case GPIO_INT_EDGE_RISING:
		SET_REG_BIT(PNX833X_PIO_INT_EDGE, pin);
		SET_REG_BIT(PNX833X_PIO_INT_HI, pin);
		CLEAR_REG_BIT(PNX833X_PIO_INT_LO, pin);
		break;
	case GPIO_INT_EDGE_FALLING:
		SET_REG_BIT(PNX833X_PIO_INT_EDGE, pin);
		CLEAR_REG_BIT(PNX833X_PIO_INT_HI, pin);
		SET_REG_BIT(PNX833X_PIO_INT_LO, pin);
		break;
	case GPIO_INT_EDGE_BOTH:
		SET_REG_BIT(PNX833X_PIO_INT_EDGE, pin);
		SET_REG_BIT(PNX833X_PIO_INT_HI, pin);
		SET_REG_BIT(PNX833X_PIO_INT_LO, pin);
		break;
	default:
		CLEAR_REG_BIT(PNX833X_PIO_INT_EDGE, pin);
		CLEAR_REG_BIT(PNX833X_PIO_INT_HI, pin);
		CLEAR_REG_BIT(PNX833X_PIO_INT_LO, pin);
		break;
	}
}

/* Enable/disable GPIO interrupt */
static inline void pnx833x_gpio_enable_irq(unsigned int pin)
{
	SET_REG_BIT(PNX833X_PIO_INT_ENABLE, pin);
}
static inline void pnx833x_gpio_disable_irq(unsigned int pin)
{
	CLEAR_REG_BIT(PNX833X_PIO_INT_ENABLE, pin);
}

/* Clear GPIO interrupt request */
static inline void pnx833x_gpio_clear_irq(unsigned int pin)
{
	SET_REG_BIT(PNX833X_PIO_INT_CLEAR, pin);
	CLEAR_REG_BIT(PNX833X_PIO_INT_CLEAR, pin);
}

#endif
