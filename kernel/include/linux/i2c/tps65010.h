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

/* linux/i2c/tps65010.h
 *
 * Functions to access TPS65010 power management device.
 *
 * Copyright (C) 2004 Dirk Behme <dirk.behme@de.bosch.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __LINUX_I2C_TPS65010_H
#define __LINUX_I2C_TPS65010_H

/*
 * ----------------------------------------------------------------------------
 * Registers, all 8 bits
 * ----------------------------------------------------------------------------
 */

#define	TPS_CHGSTATUS		0x01
#	define	TPS_CHG_USB		(1 << 7)
#	define	TPS_CHG_AC		(1 << 6)
#	define	TPS_CHG_THERM		(1 << 5)
#	define	TPS_CHG_TERM		(1 << 4)
#	define	TPS_CHG_TAPER_TMO	(1 << 3)
#	define	TPS_CHG_CHG_TMO		(1 << 2)
#	define	TPS_CHG_PRECHG_TMO	(1 << 1)
#	define	TPS_CHG_TEMP_ERR	(1 << 0)
#define	TPS_REGSTATUS		0x02
#	define	TPS_REG_ONOFF		(1 << 7)
#	define	TPS_REG_COVER		(1 << 6)
#	define	TPS_REG_UVLO		(1 << 5)
#	define	TPS_REG_NO_CHG		(1 << 4)	/* tps65013 */
#	define	TPS_REG_PG_LD02		(1 << 3)
#	define	TPS_REG_PG_LD01		(1 << 2)
#	define	TPS_REG_PG_MAIN		(1 << 1)
#	define	TPS_REG_PG_CORE		(1 << 0)
#define	TPS_MASK1		0x03
#define	TPS_MASK2		0x04
#define	TPS_ACKINT1		0x05
#define	TPS_ACKINT2		0x06
#define	TPS_CHGCONFIG		0x07
#	define	TPS_CHARGE_POR		(1 << 7)	/* 65010/65012 */
#	define	TPS65013_AUA		(1 << 7)	/* 65011/65013 */
#	define	TPS_CHARGE_RESET	(1 << 6)
#	define	TPS_CHARGE_FAST		(1 << 5)
#	define	TPS_CHARGE_CURRENT	(3 << 3)
#	define	TPS_VBUS_500MA		(1 << 2)
#	define	TPS_VBUS_CHARGING	(1 << 1)
#	define	TPS_CHARGE_ENABLE	(1 << 0)
#define	TPS_LED1_ON		0x08
#define	TPS_LED1_PER		0x09
#define	TPS_LED2_ON		0x0a
#define	TPS_LED2_PER		0x0b
#define	TPS_VDCDC1		0x0c
#	define	TPS_ENABLE_LP		(1 << 3)
#define	TPS_VDCDC2		0x0d
#define	TPS_VREGS1		0x0e
#	define	TPS_LDO2_ENABLE	(1 << 7)
#	define	TPS_LDO2_OFF	(1 << 6)
#	define	TPS_VLDO2_3_0V	(3 << 4)
#	define	TPS_VLDO2_2_75V	(2 << 4)
#	define	TPS_VLDO2_2_5V	(1 << 4)
#	define	TPS_VLDO2_1_8V	(0 << 4)
#	define	TPS_LDO1_ENABLE	(1 << 3)
#	define	TPS_LDO1_OFF	(1 << 2)
#	define	TPS_VLDO1_3_0V	(3 << 0)
#	define	TPS_VLDO1_2_75V	(2 << 0)
#	define	TPS_VLDO1_2_5V	(1 << 0)
#	define	TPS_VLDO1_ADJ	(0 << 0)
#define	TPS_MASK3		0x0f
#define	TPS_DEFGPIO		0x10

/*
 * ----------------------------------------------------------------------------
 * Macros used by exported functions
 * ----------------------------------------------------------------------------
 */

#define LED1  1
#define LED2  2
#define OFF   0
#define ON    1
#define BLINK 2
#define GPIO1 1
#define GPIO2 2
#define GPIO3 3
#define GPIO4 4
#define LOW   0
#define HIGH  1

/*
 * ----------------------------------------------------------------------------
 * Exported functions
 * ----------------------------------------------------------------------------
 */

/* Draw from VBUS:
 *   0 mA -- DON'T DRAW (might supply power instead)
 * 100 mA -- usb unit load (slowest charge rate)
 * 500 mA -- usb high power (fast battery charge)
 */
extern int tps65010_set_vbus_draw(unsigned mA);

/* tps65010_set_gpio_out_value parameter:
 * gpio:  GPIO1, GPIO2, GPIO3 or GPIO4
 * value: LOW or HIGH
 */
extern int tps65010_set_gpio_out_value(unsigned gpio, unsigned value);

/* tps65010_set_led parameter:
 * led:  LED1 or LED2
 * mode: ON, OFF or BLINK
 */
extern int tps65010_set_led(unsigned led, unsigned mode);

/* tps65010_set_vib parameter:
 * value: ON or OFF
 */
extern int tps65010_set_vib(unsigned value);

/* tps65010_set_low_pwr parameter:
 * mode: ON or OFF
 */
extern int tps65010_set_low_pwr(unsigned mode);

/* tps65010_config_vregs1 parameter:
 * value to be written to VREGS1 register
 * Note: The complete register is written, set all bits you need
 */
extern int tps65010_config_vregs1(unsigned value);

/* tps65013_set_low_pwr parameter:
 * mode: ON or OFF
 */
extern int tps65013_set_low_pwr(unsigned mode);


struct i2c_client;

/**
 * struct tps65010_board - packages GPIO and LED lines
 * @base: the GPIO number to assign to GPIO-1
 * @outmask: bit (N-1) is set to allow GPIO-N to be used as an
 *	(open drain) output
 * @setup: optional callback issued once the GPIOs are valid
 * @teardown: optional callback issued before the GPIOs are invalidated
 * @context: optional parameter passed to setup() and teardown()
 *
 * Board data may be used to package the GPIO (and LED) lines for use
 * in by the generic GPIO and LED frameworks.  The first four GPIOs
 * starting at gpio_base are GPIO1..GPIO4.  The next two are LED1/nPG
 * and LED2 (with hardware blinking capability, not currently exposed).
 *
 * The @setup callback may be used with the kind of board-specific glue
 * which hands the (now-valid) GPIOs to other drivers, or which puts
 * devices in their initial states using these GPIOs.
 */
struct tps65010_board {
	int				base;
	unsigned			outmask;

	int		(*setup)(struct i2c_client *client, void *context);
	int		(*teardown)(struct i2c_client *client, void *context);
	void		*context;
};

#endif /*  __LINUX_I2C_TPS65010_H */

