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
    i2c-stub.c - I2C/SMBus chip emulator

    Copyright (c) 2004 Mark M. Hoffman <mhoffman@lightlink.com>
    Copyright (C) 2007 Jean Delvare <khali@linux-fr.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#define DEBUG 1

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/i2c.h>

#define MAX_CHIPS 10

static unsigned short chip_addr[MAX_CHIPS];
module_param_array(chip_addr, ushort, NULL, S_IRUGO);
MODULE_PARM_DESC(chip_addr,
		 "Chip addresses (up to 10, between 0x03 and 0x77)");

struct stub_chip {
	u8 pointer;
	u16 words[256];		/* Byte operations use the LSB as per SMBus
				   specification */
};

static struct stub_chip *stub_chips;

/* Return negative errno on error. */
static s32 stub_xfer(struct i2c_adapter * adap, u16 addr, unsigned short flags,
	char read_write, u8 command, int size, union i2c_smbus_data * data)
{
	s32 ret;
	int i;
	struct stub_chip *chip = NULL;

	/* Search for the right chip */
	for (i = 0; i < MAX_CHIPS && chip_addr[i]; i++) {
		if (addr == chip_addr[i]) {
			chip = stub_chips + i;
			break;
		}
	}
	if (!chip)
		return -ENODEV;

	switch (size) {

	case I2C_SMBUS_QUICK:
		dev_dbg(&adap->dev, "smbus quick - addr 0x%02x\n", addr);
		ret = 0;
		break;

	case I2C_SMBUS_BYTE:
		if (read_write == I2C_SMBUS_WRITE) {
			chip->pointer = command;
			dev_dbg(&adap->dev, "smbus byte - addr 0x%02x, "
					"wrote 0x%02x.\n",
					addr, command);
		} else {
			data->byte = chip->words[chip->pointer++] & 0xff;
			dev_dbg(&adap->dev, "smbus byte - addr 0x%02x, "
					"read  0x%02x.\n",
					addr, data->byte);
		}

		ret = 0;
		break;

	case I2C_SMBUS_BYTE_DATA:
		if (read_write == I2C_SMBUS_WRITE) {
			chip->words[command] &= 0xff00;
			chip->words[command] |= data->byte;
			dev_dbg(&adap->dev, "smbus byte data - addr 0x%02x, "
					"wrote 0x%02x at 0x%02x.\n",
					addr, data->byte, command);
		} else {
			data->byte = chip->words[command] & 0xff;
			dev_dbg(&adap->dev, "smbus byte data - addr 0x%02x, "
					"read  0x%02x at 0x%02x.\n",
					addr, data->byte, command);
		}
		chip->pointer = command + 1;

		ret = 0;
		break;

	case I2C_SMBUS_WORD_DATA:
		if (read_write == I2C_SMBUS_WRITE) {
			chip->words[command] = data->word;
			dev_dbg(&adap->dev, "smbus word data - addr 0x%02x, "
					"wrote 0x%04x at 0x%02x.\n",
					addr, data->word, command);
		} else {
			data->word = chip->words[command];
			dev_dbg(&adap->dev, "smbus word data - addr 0x%02x, "
					"read  0x%04x at 0x%02x.\n",
					addr, data->word, command);
		}

		ret = 0;
		break;

	default:
		dev_dbg(&adap->dev, "Unsupported I2C/SMBus command\n");
		ret = -EOPNOTSUPP;
		break;
	} /* switch (size) */

	return ret;
}

static u32 stub_func(struct i2c_adapter *adapter)
{
	return I2C_FUNC_SMBUS_QUICK | I2C_FUNC_SMBUS_BYTE |
		I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA;
}

static const struct i2c_algorithm smbus_algorithm = {
	.functionality	= stub_func,
	.smbus_xfer	= stub_xfer,
};

static struct i2c_adapter stub_adapter = {
	.owner		= THIS_MODULE,
	.class		= I2C_CLASS_HWMON | I2C_CLASS_SPD,
	.algo		= &smbus_algorithm,
	.name		= "SMBus stub driver",
};

static int __init i2c_stub_init(void)
{
	int i, ret;

	if (!chip_addr[0]) {
		printk(KERN_ERR "i2c-stub: Please specify a chip address\n");
		return -ENODEV;
	}

	for (i = 0; i < MAX_CHIPS && chip_addr[i]; i++) {
		if (chip_addr[i] < 0x03 || chip_addr[i] > 0x77) {
			printk(KERN_ERR "i2c-stub: Invalid chip address "
			       "0x%02x\n", chip_addr[i]);
			return -EINVAL;
		}

		printk(KERN_INFO "i2c-stub: Virtual chip at 0x%02x\n",
		       chip_addr[i]);
	}

	/* Allocate memory for all chips at once */
	stub_chips = kzalloc(i * sizeof(struct stub_chip), GFP_KERNEL);
	if (!stub_chips) {
		printk(KERN_ERR "i2c-stub: Out of memory\n");
		return -ENOMEM;
	}

	ret = i2c_add_adapter(&stub_adapter);
	if (ret)
		kfree(stub_chips);
	return ret;
}

static void __exit i2c_stub_exit(void)
{
	i2c_del_adapter(&stub_adapter);
	kfree(stub_chips);
}

MODULE_AUTHOR("Mark M. Hoffman <mhoffman@lightlink.com>");
MODULE_DESCRIPTION("I2C stub driver");
MODULE_LICENSE("GPL");

module_init(i2c_stub_init);
module_exit(i2c_stub_exit);

