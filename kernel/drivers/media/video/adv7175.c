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
 *  adv7175 - adv7175a video encoder driver version 0.0.3
 *
 * Copyright (C) 1998 Dave Perks <dperks@ibm.net>
 * Copyright (C) 1999 Wolfgang Scherr <scherr@net4you.net>
 * Copyright (C) 2000 Serguei Miridonov <mirsev@cicese.mx>
 *    - some corrections for Pinnacle Systems Inc. DC10plus card.
 *
 * Changes by Ronald Bultje <rbultje@ronald.bitfreak.net>
 *    - moved over to linux>=2.4.x i2c protocol (9/9/2002)
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

#include <linux/module.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <asm/uaccess.h>
#include <linux/i2c.h>
#include <linux/i2c-id.h>
#include <linux/videodev.h>
#include <linux/video_encoder.h>
#include <media/v4l2-common.h>
#include <media/v4l2-i2c-drv-legacy.h>

MODULE_DESCRIPTION("Analog Devices ADV7175 video encoder driver");
MODULE_AUTHOR("Dave Perks");
MODULE_LICENSE("GPL");

static int debug;
module_param(debug, int, 0);
MODULE_PARM_DESC(debug, "Debug level (0-1)");

/* ----------------------------------------------------------------------- */

struct adv7175 {
	int norm;
	int input;
	int enable;
	int bright;
	int contrast;
	int hue;
	int sat;
};

#define   I2C_ADV7175        0xd4
#define   I2C_ADV7176        0x54

static char *inputs[] = { "pass_through", "play_back", "color_bar" };
static char *norms[] = { "PAL", "NTSC", "SECAM->PAL (may not work!)" };

/* ----------------------------------------------------------------------- */

static inline int adv7175_write(struct i2c_client *client, u8 reg, u8 value)
{
	return i2c_smbus_write_byte_data(client, reg, value);
}

static inline int adv7175_read(struct i2c_client *client, u8 reg)
{
	return i2c_smbus_read_byte_data(client, reg);
}

static int adv7175_write_block(struct i2c_client *client,
		     const u8 *data, unsigned int len)
{
	int ret = -1;
	u8 reg;

	/* the adv7175 has an autoincrement function, use it if
	 * the adapter understands raw I2C */
	if (i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		/* do raw I2C, not smbus compatible */
		u8 block_data[32];
		int block_len;

		while (len >= 2) {
			block_len = 0;
			block_data[block_len++] = reg = data[0];
			do {
				block_data[block_len++] = data[1];
				reg++;
				len -= 2;
				data += 2;
			} while (len >= 2 && data[0] == reg && block_len < 32);
			ret = i2c_master_send(client, block_data, block_len);
			if (ret < 0)
				break;
		}
	} else {
		/* do some slow I2C emulation kind of thing */
		while (len >= 2) {
			reg = *data++;
			ret = adv7175_write(client, reg, *data++);
			if (ret < 0)
				break;
			len -= 2;
		}
	}

	return ret;
}

static void set_subcarrier_freq(struct i2c_client *client, int pass_through)
{
	/* for some reason pass_through NTSC needs
	 * a different sub-carrier freq to remain stable. */
	if (pass_through)
		adv7175_write(client, 0x02, 0x00);
	else
		adv7175_write(client, 0x02, 0x55);

	adv7175_write(client, 0x03, 0x55);
	adv7175_write(client, 0x04, 0x55);
	adv7175_write(client, 0x05, 0x25);
}

/* ----------------------------------------------------------------------- */
/* Output filter:  S-Video  Composite */

#define MR050       0x11	/* 0x09 */
#define MR060       0x14	/* 0x0c */

/* ----------------------------------------------------------------------- */

#define TR0MODE     0x46
#define TR0RST	    0x80

#define TR1CAPT	    0x80
#define TR1PLAY	    0x00

static const unsigned char init_common[] = {

	0x00, MR050,		/* MR0, PAL enabled */
	0x01, 0x00,		/* MR1 */
	0x02, 0x0c,		/* subc. freq. */
	0x03, 0x8c,		/* subc. freq. */
	0x04, 0x79,		/* subc. freq. */
	0x05, 0x26,		/* subc. freq. */
	0x06, 0x40,		/* subc. phase */

	0x07, TR0MODE,		/* TR0, 16bit */
	0x08, 0x21,		/*  */
	0x09, 0x00,		/*  */
	0x0a, 0x00,		/*  */
	0x0b, 0x00,		/*  */
	0x0c, TR1CAPT,		/* TR1 */
	0x0d, 0x4f,		/* MR2 */
	0x0e, 0x00,		/*  */
	0x0f, 0x00,		/*  */
	0x10, 0x00,		/*  */
	0x11, 0x00,		/*  */
};

static const unsigned char init_pal[] = {
	0x00, MR050,		/* MR0, PAL enabled */
	0x01, 0x00,		/* MR1 */
	0x02, 0x0c,		/* subc. freq. */
	0x03, 0x8c,		/* subc. freq. */
	0x04, 0x79,		/* subc. freq. */
	0x05, 0x26,		/* subc. freq. */
	0x06, 0x40,		/* subc. phase */
};

static const unsigned char init_ntsc[] = {
	0x00, MR060,		/* MR0, NTSC enabled */
	0x01, 0x00,		/* MR1 */
	0x02, 0x55,		/* subc. freq. */
	0x03, 0x55,		/* subc. freq. */
	0x04, 0x55,		/* subc. freq. */
	0x05, 0x25,		/* subc. freq. */
	0x06, 0x1a,		/* subc. phase */
};

static int adv7175_command(struct i2c_client *client, unsigned cmd, void *arg)
{
	struct adv7175 *encoder = i2c_get_clientdata(client);

	switch (cmd) {
	case 0:
		/* This is just for testing!!! */
		adv7175_write_block(client, init_common,
				    sizeof(init_common));
		adv7175_write(client, 0x07, TR0MODE | TR0RST);
		adv7175_write(client, 0x07, TR0MODE);
		break;

	case ENCODER_GET_CAPABILITIES:
	{
		struct video_encoder_capability *cap = arg;

		cap->flags = VIDEO_ENCODER_PAL |
			     VIDEO_ENCODER_NTSC |
			     VIDEO_ENCODER_SECAM; /* well, hacky */
		cap->inputs = 2;
		cap->outputs = 1;
		break;
	}

	case ENCODER_SET_NORM:
	{
		int iarg = *(int *) arg;

		switch (iarg) {
		case VIDEO_MODE_NTSC:
			adv7175_write_block(client, init_ntsc,
					    sizeof(init_ntsc));
			if (encoder->input == 0)
				adv7175_write(client, 0x0d, 0x4f);	// Enable genlock
			adv7175_write(client, 0x07, TR0MODE | TR0RST);
			adv7175_write(client, 0x07, TR0MODE);
			break;

		case VIDEO_MODE_PAL:
			adv7175_write_block(client, init_pal,
					    sizeof(init_pal));
			if (encoder->input == 0)
				adv7175_write(client, 0x0d, 0x4f);	// Enable genlock
			adv7175_write(client, 0x07, TR0MODE | TR0RST);
			adv7175_write(client, 0x07, TR0MODE);
			break;

		case VIDEO_MODE_SECAM:	// WARNING! ADV7176 does not support SECAM.
			/* This is an attempt to convert
			 * SECAM->PAL (typically it does not work
			 * due to genlock: when decoder is in SECAM
			 * and encoder in in PAL the subcarrier can
			 * not be syncronized with horizontal
			 * quency) */
			adv7175_write_block(client, init_pal,
					    sizeof(init_pal));
			if (encoder->input == 0)
				adv7175_write(client, 0x0d, 0x49);	// Disable genlock
			adv7175_write(client, 0x07, TR0MODE | TR0RST);
			adv7175_write(client, 0x07, TR0MODE);
			break;
		default:
			v4l_dbg(1, debug, client, "illegal norm: %d\n", iarg);
			return -EINVAL;
		}
		v4l_dbg(1, debug, client, "switched to %s\n", norms[iarg]);
		encoder->norm = iarg;
		break;
	}

	case ENCODER_SET_INPUT:
	{
		int iarg = *(int *) arg;

		/* RJ: *iarg = 0: input is from SAA7110
		 *iarg = 1: input is from ZR36060
		 *iarg = 2: color bar */

		switch (iarg) {
		case 0:
			adv7175_write(client, 0x01, 0x00);

			if (encoder->norm == VIDEO_MODE_NTSC)
				set_subcarrier_freq(client, 1);

			adv7175_write(client, 0x0c, TR1CAPT);	/* TR1 */
			if (encoder->norm == VIDEO_MODE_SECAM)
				adv7175_write(client, 0x0d, 0x49);	// Disable genlock
			else
				adv7175_write(client, 0x0d, 0x4f);	// Enable genlock
			adv7175_write(client, 0x07, TR0MODE | TR0RST);
			adv7175_write(client, 0x07, TR0MODE);
			//udelay(10);
			break;

		case 1:
			adv7175_write(client, 0x01, 0x00);

			if (encoder->norm == VIDEO_MODE_NTSC)
				set_subcarrier_freq(client, 0);

			adv7175_write(client, 0x0c, TR1PLAY);	/* TR1 */
			adv7175_write(client, 0x0d, 0x49);
			adv7175_write(client, 0x07, TR0MODE | TR0RST);
			adv7175_write(client, 0x07, TR0MODE);
			/* udelay(10); */
			break;

		case 2:
			adv7175_write(client, 0x01, 0x80);

			if (encoder->norm == VIDEO_MODE_NTSC)
				set_subcarrier_freq(client, 0);

			adv7175_write(client, 0x0d, 0x49);
			adv7175_write(client, 0x07, TR0MODE | TR0RST);
			adv7175_write(client, 0x07, TR0MODE);
			/* udelay(10); */
			break;

		default:
			v4l_dbg(1, debug, client, "illegal input: %d\n", iarg);
			return -EINVAL;
		}
		v4l_dbg(1, debug, client, "switched to %s\n", inputs[iarg]);
		encoder->input = iarg;
		break;
	}

	case ENCODER_SET_OUTPUT:
	{
		int *iarg = arg;

		/* not much choice of outputs */
		if (*iarg != 0)
			return -EINVAL;
		break;
	}

	case ENCODER_ENABLE_OUTPUT:
	{
		int *iarg = arg;

		encoder->enable = !!*iarg;
		break;
	}

	default:
		return -EINVAL;
	}

	return 0;
}

/* ----------------------------------------------------------------------- */

/*
 * Generic i2c probe
 * concerning the addresses: i2c wants 7 bit (without the r/w bit), so '>>1'
 */
static unsigned short normal_i2c[] = {
	I2C_ADV7175 >> 1, (I2C_ADV7175 >> 1) + 1,
	I2C_ADV7176 >> 1, (I2C_ADV7176 >> 1) + 1,
	I2C_CLIENT_END
};

I2C_CLIENT_INSMOD;

static int adv7175_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	int i;
	struct adv7175 *encoder;

	/* Check if the adapter supports the needed features */
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return -ENODEV;

	v4l_info(client, "chip found @ 0x%x (%s)\n",
			client->addr << 1, client->adapter->name);

	encoder = kzalloc(sizeof(struct adv7175), GFP_KERNEL);
	if (encoder == NULL)
		return -ENOMEM;
	encoder->norm = VIDEO_MODE_PAL;
	encoder->input = 0;
	encoder->enable = 1;
	i2c_set_clientdata(client, encoder);

	i = adv7175_write_block(client, init_common, sizeof(init_common));
	if (i >= 0) {
		i = adv7175_write(client, 0x07, TR0MODE | TR0RST);
		i = adv7175_write(client, 0x07, TR0MODE);
		i = adv7175_read(client, 0x12);
		v4l_dbg(1, debug, client, "revision %d\n", i & 1);
	}
	if (i < 0)
		v4l_dbg(1, debug, client, "init error 0x%x\n", i);
	return 0;
}

static int adv7175_remove(struct i2c_client *client)
{
	kfree(i2c_get_clientdata(client));
	return 0;
}

/* ----------------------------------------------------------------------- */

static const struct i2c_device_id adv7175_id[] = {
	{ "adv7175", 0 },
	{ "adv7176", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, adv7175_id);

static struct v4l2_i2c_driver_data v4l2_i2c_data = {
	.name = "adv7175",
	.driverid = I2C_DRIVERID_ADV7175,
	.command = adv7175_command,
	.probe = adv7175_probe,
	.remove = adv7175_remove,
	.id_table = adv7175_id,
};
