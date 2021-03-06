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
 * Copyright (c) 2001 Jean-Fredric Clere, Nikolas Zimmermann, Georg Acher
 *		      Mark Cave-Ayland, Carlo E Prelz, Dick Streefland
 * Copyright (c) 2002, 2003 Tuukka Toivonen
 * Copyright (c) 2008 Erik Andrén
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * P/N 861037:      Sensor HDCS1000        ASIC STV0600
 * P/N 861050-0010: Sensor HDCS1000        ASIC STV0600
 * P/N 861050-0020: Sensor Photobit PB100  ASIC STV0600-1 - QuickCam Express
 * P/N 861055:      Sensor ST VV6410       ASIC STV0610   - LEGO cam
 * P/N 861075-0040: Sensor HDCS1000        ASIC
 * P/N 961179-0700: Sensor ST VV6410       ASIC STV0602   - Dexxa WebCam USB
 * P/N 861040-0000: Sensor ST VV6410       ASIC STV0610   - QuickCam Web
 */

/*
 * The spec file for the PB-0100 suggests the following for best quality
 * images after the sensor has been reset :
 *
 * PB_ADCGAINL      = R60 = 0x03 (3 dec)      : sets low reference of ADC
						to produce good black level
 * PB_PREADCTRL     = R32 = 0x1400 (5120 dec) : Enables global gain changes
						through R53
 * PB_ADCMINGAIN    = R52 = 0x10 (16 dec)     : Sets the minimum gain for
						auto-exposure
 * PB_ADCGLOBALGAIN = R53 = 0x10 (16 dec)     : Sets the global gain
 * PB_EXPGAIN       = R14 = 0x11 (17 dec)     : Sets the auto-exposure value
 * PB_UPDATEINT     = R23 = 0x02 (2 dec)      : Sets the speed on
						auto-exposure routine
 * PB_CFILLIN       = R5  = 0x0E (14 dec)     : Sets the frame rate
 */

#include "stv06xx_pb0100.h"

static int pb0100_probe(struct sd *sd)
{
	u16 sensor;
	int i, err;
	s32 *sensor_settings;

	err = stv06xx_read_sensor(sd, PB_IDENT, &sensor);

	if (err < 0)
		return -ENODEV;

	if ((sensor >> 8) == 0x64) {
		sensor_settings = kmalloc(
				stv06xx_sensor_pb0100.nctrls * sizeof(s32),
				GFP_KERNEL);
		if (!sensor_settings)
			return -ENOMEM;

		info("Photobit pb0100 sensor detected");

		sd->gspca_dev.cam.cam_mode = stv06xx_sensor_pb0100.modes;
		sd->gspca_dev.cam.nmodes = stv06xx_sensor_pb0100.nmodes;
		sd->desc.ctrls = stv06xx_sensor_pb0100.ctrls;
		sd->desc.nctrls = stv06xx_sensor_pb0100.nctrls;
		for (i = 0; i < stv06xx_sensor_pb0100.nctrls; i++)
			sensor_settings[i] = stv06xx_sensor_pb0100.
					     ctrls[i].qctrl.default_value;
		sd->sensor_priv = sensor_settings;

		return 0;
	}

	return -ENODEV;
}

static int pb0100_start(struct sd *sd)
{
	int err;
	struct cam *cam = &sd->gspca_dev.cam;
	s32 *sensor_settings = sd->sensor_priv;
	u32 mode = cam->cam_mode[sd->gspca_dev.curr_mode].priv;

	/* Setup sensor window */
	if (mode & PB0100_CROP_TO_VGA) {
		stv06xx_write_sensor(sd, PB_RSTART, 30);
		stv06xx_write_sensor(sd, PB_CSTART, 20);
		stv06xx_write_sensor(sd, PB_RWSIZE, 240 - 1);
		stv06xx_write_sensor(sd, PB_CWSIZE, 320 - 1);
	} else {
		stv06xx_write_sensor(sd, PB_RSTART, 8);
		stv06xx_write_sensor(sd, PB_CSTART, 4);
		stv06xx_write_sensor(sd, PB_RWSIZE, 288 - 1);
		stv06xx_write_sensor(sd, PB_CWSIZE, 352 - 1);
	}

	if (mode & PB0100_SUBSAMPLE) {
		stv06xx_write_bridge(sd, STV_Y_CTRL, 0x02); /* Wrong, FIXME */
		stv06xx_write_bridge(sd, STV_X_CTRL, 0x06);

		stv06xx_write_bridge(sd, STV_SCAN_RATE, 0x10);
	} else {
		stv06xx_write_bridge(sd, STV_Y_CTRL, 0x01);
		stv06xx_write_bridge(sd, STV_X_CTRL, 0x0a);
		/* larger -> slower */
		stv06xx_write_bridge(sd, STV_SCAN_RATE, 0x20);
	}

	/* set_gain also sets red and blue balance */
	pb0100_set_gain(&sd->gspca_dev, sensor_settings[GAIN_IDX]);
	pb0100_set_exposure(&sd->gspca_dev, sensor_settings[EXPOSURE_IDX]);
	pb0100_set_autogain_target(&sd->gspca_dev,
				   sensor_settings[AUTOGAIN_TARGET_IDX]);
	pb0100_set_autogain(&sd->gspca_dev, sensor_settings[AUTOGAIN_IDX]);

	err = stv06xx_write_sensor(sd, PB_CONTROL, BIT(5)|BIT(3)|BIT(1));
	PDEBUG(D_STREAM, "Started stream, status: %d", err);

	return (err < 0) ? err : 0;
}

static int pb0100_stop(struct sd *sd)
{
	int err;

	err = stv06xx_write_sensor(sd, PB_ABORTFRAME, 1);

	if (err < 0)
		goto out;

	/* Set bit 1 to zero */
	err = stv06xx_write_sensor(sd, PB_CONTROL, BIT(5)|BIT(3));

	PDEBUG(D_STREAM, "Halting stream");
out:
	return (err < 0) ? err : 0;
}

/* FIXME: Sort the init commands out and put them into tables,
	  this is only for getting the camera to work */
/* FIXME: No error handling for now,
	  add this once the init has been converted to proper tables */
static int pb0100_init(struct sd *sd)
{
	stv06xx_write_bridge(sd, STV_REG00, 1);
	stv06xx_write_bridge(sd, STV_SCAN_RATE, 0);

	/* Reset sensor */
	stv06xx_write_sensor(sd, PB_RESET, 1);
	stv06xx_write_sensor(sd, PB_RESET, 0);

	/* Disable chip */
	stv06xx_write_sensor(sd, PB_CONTROL, BIT(5)|BIT(3));

	/* Gain stuff...*/
	stv06xx_write_sensor(sd, PB_PREADCTRL, BIT(12)|BIT(10)|BIT(6));
	stv06xx_write_sensor(sd, PB_ADCGLOBALGAIN, 12);

	/* Set up auto-exposure */
	/* ADC VREF_HI new setting for a transition
	  from the Expose1 to the Expose2 setting */
	stv06xx_write_sensor(sd, PB_R28, 12);
	/* gain max for autoexposure */
	stv06xx_write_sensor(sd, PB_ADCMAXGAIN, 180);
	/* gain min for autoexposure  */
	stv06xx_write_sensor(sd, PB_ADCMINGAIN, 12);
	/* Maximum frame integration time (programmed into R8)
	   allowed for auto-exposure routine */
	stv06xx_write_sensor(sd, PB_R54, 3);
	/* Minimum frame integration time (programmed into R8)
	   allowed for auto-exposure routine */
	stv06xx_write_sensor(sd, PB_R55, 0);
	stv06xx_write_sensor(sd, PB_UPDATEINT, 1);
	/* R15  Expose0 (maximum that auto-exposure may use) */
	stv06xx_write_sensor(sd, PB_R15, 800);
	/* R17  Expose2 (minimum that auto-exposure may use) */
	stv06xx_write_sensor(sd, PB_R17, 10);

	stv06xx_write_sensor(sd, PB_EXPGAIN, 0);

	/* 0x14 */
	stv06xx_write_sensor(sd, PB_VOFFSET, 0);
	/* 0x0D */
	stv06xx_write_sensor(sd, PB_ADCGAINH, 11);
	/* Set black level (important!) */
	stv06xx_write_sensor(sd, PB_ADCGAINL, 0);

	/* ??? */
	stv06xx_write_bridge(sd, STV_REG00, 0x11);
	stv06xx_write_bridge(sd, STV_REG03, 0x45);
	stv06xx_write_bridge(sd, STV_REG04, 0x07);

	/* ISO-Size (0x27b: 635... why? - HDCS uses 847) */
	stv06xx_write_bridge(sd, STV_ISO_SIZE_L, 847);

	/* Scan/timing for the sensor */
	stv06xx_write_sensor(sd, PB_ROWSPEED, BIT(4)|BIT(3)|BIT(1));
	stv06xx_write_sensor(sd, PB_CFILLIN, 14);
	stv06xx_write_sensor(sd, PB_VBL, 0);
	stv06xx_write_sensor(sd, PB_FINTTIME, 0);
	stv06xx_write_sensor(sd, PB_RINTTIME, 123);

	stv06xx_write_bridge(sd, STV_REG01, 0xc2);
	stv06xx_write_bridge(sd, STV_REG02, 0xb0);
	return 0;
}

static int pb0100_dump(struct sd *sd)
{
	return 0;
}

static int pb0100_get_gain(struct gspca_dev *gspca_dev, __s32 *val)
{
	struct sd *sd = (struct sd *) gspca_dev;
	s32 *sensor_settings = sd->sensor_priv;

	*val = sensor_settings[GAIN_IDX];

	return 0;
}

static int pb0100_set_gain(struct gspca_dev *gspca_dev, __s32 val)
{
	int err;
	struct sd *sd = (struct sd *) gspca_dev;
	s32 *sensor_settings = sd->sensor_priv;

	if (sensor_settings[AUTOGAIN_IDX])
		return -EBUSY;

	sensor_settings[GAIN_IDX] = val;
	err = stv06xx_write_sensor(sd, PB_G1GAIN, val);
	if (!err)
		err = stv06xx_write_sensor(sd, PB_G2GAIN, val);
	PDEBUG(D_V4L2, "Set green gain to %d, status: %d", val, err);

	if (!err)
		err = pb0100_set_red_balance(gspca_dev,
					     sensor_settings[RED_BALANCE_IDX]);
	if (!err)
		err = pb0100_set_blue_balance(gspca_dev,
					    sensor_settings[BLUE_BALANCE_IDX]);

	return err;
}

static int pb0100_get_red_balance(struct gspca_dev *gspca_dev, __s32 *val)
{
	struct sd *sd = (struct sd *) gspca_dev;
	s32 *sensor_settings = sd->sensor_priv;

	*val = sensor_settings[RED_BALANCE_IDX];

	return 0;
}

static int pb0100_set_red_balance(struct gspca_dev *gspca_dev, __s32 val)
{
	int err;
	struct sd *sd = (struct sd *) gspca_dev;
	s32 *sensor_settings = sd->sensor_priv;

	if (sensor_settings[AUTOGAIN_IDX])
		return -EBUSY;

	sensor_settings[RED_BALANCE_IDX] = val;
	val += sensor_settings[GAIN_IDX];
	if (val < 0)
		val = 0;
	else if (val > 255)
		val = 255;

	err = stv06xx_write_sensor(sd, PB_RGAIN, val);
	PDEBUG(D_V4L2, "Set red gain to %d, status: %d", val, err);

	return err;
}

static int pb0100_get_blue_balance(struct gspca_dev *gspca_dev, __s32 *val)
{
	struct sd *sd = (struct sd *) gspca_dev;
	s32 *sensor_settings = sd->sensor_priv;

	*val = sensor_settings[BLUE_BALANCE_IDX];

	return 0;
}

static int pb0100_set_blue_balance(struct gspca_dev *gspca_dev, __s32 val)
{
	int err;
	struct sd *sd = (struct sd *) gspca_dev;
	s32 *sensor_settings = sd->sensor_priv;

	if (sensor_settings[AUTOGAIN_IDX])
		return -EBUSY;

	sensor_settings[BLUE_BALANCE_IDX] = val;
	val += sensor_settings[GAIN_IDX];
	if (val < 0)
		val = 0;
	else if (val > 255)
		val = 255;

	err = stv06xx_write_sensor(sd, PB_BGAIN, val);
	PDEBUG(D_V4L2, "Set blue gain to %d, status: %d", val, err);

	return err;
}

static int pb0100_get_exposure(struct gspca_dev *gspca_dev, __s32 *val)
{
	struct sd *sd = (struct sd *) gspca_dev;
	s32 *sensor_settings = sd->sensor_priv;

	*val = sensor_settings[EXPOSURE_IDX];

	return 0;
}

static int pb0100_set_exposure(struct gspca_dev *gspca_dev, __s32 val)
{
	int err;
	struct sd *sd = (struct sd *) gspca_dev;
	s32 *sensor_settings = sd->sensor_priv;

	if (sensor_settings[AUTOGAIN_IDX])
		return -EBUSY;

	sensor_settings[EXPOSURE_IDX] = val;
	err = stv06xx_write_sensor(sd, PB_RINTTIME, val);
	PDEBUG(D_V4L2, "Set exposure to %d, status: %d", val, err);

	return err;
}

static int pb0100_get_autogain(struct gspca_dev *gspca_dev, __s32 *val)
{
	struct sd *sd = (struct sd *) gspca_dev;
	s32 *sensor_settings = sd->sensor_priv;

	*val = sensor_settings[AUTOGAIN_IDX];

	return 0;
}

static int pb0100_set_autogain(struct gspca_dev *gspca_dev, __s32 val)
{
	int err;
	struct sd *sd = (struct sd *) gspca_dev;
	s32 *sensor_settings = sd->sensor_priv;

	sensor_settings[AUTOGAIN_IDX] = val;
	if (sensor_settings[AUTOGAIN_IDX]) {
		if (sensor_settings[NATURAL_IDX])
			val = BIT(6)|BIT(4)|BIT(0);
		else
			val = BIT(4)|BIT(0);
	} else
		val = 0;

	err = stv06xx_write_sensor(sd, PB_EXPGAIN, val);
	PDEBUG(D_V4L2, "Set autogain to %d (natural: %d), status: %d",
	       sensor_settings[AUTOGAIN_IDX], sensor_settings[NATURAL_IDX],
	       err);

	return err;
}

static int pb0100_get_autogain_target(struct gspca_dev *gspca_dev, __s32 *val)
{
	struct sd *sd = (struct sd *) gspca_dev;
	s32 *sensor_settings = sd->sensor_priv;

	*val = sensor_settings[AUTOGAIN_TARGET_IDX];

	return 0;
}

static int pb0100_set_autogain_target(struct gspca_dev *gspca_dev, __s32 val)
{
	int err, totalpixels, brightpixels, darkpixels;
	struct sd *sd = (struct sd *) gspca_dev;
	s32 *sensor_settings = sd->sensor_priv;

	sensor_settings[AUTOGAIN_TARGET_IDX] = val;

	/* Number of pixels counted by the sensor when subsampling the pixels.
	 * Slightly larger than the real value to avoid oscillation */
	totalpixels = gspca_dev->width * gspca_dev->height;
	totalpixels = totalpixels/(8*8) + totalpixels/(64*64);

	brightpixels = (totalpixels * val) >> 8;
	darkpixels   = totalpixels - brightpixels;
	err = stv06xx_write_sensor(sd, PB_R21, brightpixels);
	if (!err)
		err = stv06xx_write_sensor(sd, PB_R22, darkpixels);

	PDEBUG(D_V4L2, "Set autogain target to %d, status: %d", val, err);

	return err;
}

static int pb0100_get_natural(struct gspca_dev *gspca_dev, __s32 *val)
{
	struct sd *sd = (struct sd *) gspca_dev;
	s32 *sensor_settings = sd->sensor_priv;

	*val = sensor_settings[NATURAL_IDX];

	return 0;
}

static int pb0100_set_natural(struct gspca_dev *gspca_dev, __s32 val)
{
	struct sd *sd = (struct sd *) gspca_dev;
	s32 *sensor_settings = sd->sensor_priv;

	sensor_settings[NATURAL_IDX] = val;

	return pb0100_set_autogain(gspca_dev, sensor_settings[AUTOGAIN_IDX]);
}
