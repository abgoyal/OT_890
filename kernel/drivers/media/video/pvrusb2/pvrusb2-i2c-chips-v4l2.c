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
 *
 *
 *  Copyright (C) 2005 Mike Isely <isely@pobox.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <linux/kernel.h>
#include "pvrusb2-i2c-core.h"
#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-debug.h"
#include "pvrusb2-i2c-cmd-v4l2.h"
#include "pvrusb2-audio.h"
#include "pvrusb2-tuner.h"
#include "pvrusb2-video-v4l.h"
#include "pvrusb2-cx2584x-v4l.h"
#include "pvrusb2-wm8775.h"

#define trace_i2c(...) pvr2_trace(PVR2_TRACE_I2C,__VA_ARGS__)

#define OP_STANDARD 0
#define OP_AUDIOMODE 1
#define OP_BCSH 2
#define OP_VOLUME 3
#define OP_FREQ 4
#define OP_AUDIORATE 5
#define OP_CROP 6
#define OP_SIZE 7
#define OP_LOG 8

static const struct pvr2_i2c_op * const ops[] = {
	[OP_STANDARD] = &pvr2_i2c_op_v4l2_standard,
	[OP_AUDIOMODE] = &pvr2_i2c_op_v4l2_audiomode,
	[OP_BCSH] = &pvr2_i2c_op_v4l2_bcsh,
	[OP_VOLUME] = &pvr2_i2c_op_v4l2_volume,
	[OP_FREQ] = &pvr2_i2c_op_v4l2_frequency,
	[OP_CROP] = &pvr2_i2c_op_v4l2_crop,
	[OP_SIZE] = &pvr2_i2c_op_v4l2_size,
	[OP_LOG] = &pvr2_i2c_op_v4l2_log,
};

void pvr2_i2c_probe(struct pvr2_hdw *hdw,struct pvr2_i2c_client *cp)
{
	int id;
	id = cp->client->driver->id;
	cp->ctl_mask = ((1 << OP_STANDARD) |
			(1 << OP_AUDIOMODE) |
			(1 << OP_BCSH) |
			(1 << OP_VOLUME) |
			(1 << OP_FREQ) |
			(1 << OP_CROP) |
			(1 << OP_SIZE) |
			(1 << OP_LOG));
	cp->status_poll = pvr2_v4l2_cmd_status_poll;

	if (id == I2C_DRIVERID_MSP3400) {
		if (pvr2_i2c_msp3400_setup(hdw,cp)) {
			return;
		}
	}
	if (id == I2C_DRIVERID_TUNER) {
		if (pvr2_i2c_tuner_setup(hdw,cp)) {
			return;
		}
	}
	if (id == I2C_DRIVERID_CX25840) {
		if (pvr2_i2c_cx2584x_v4l_setup(hdw,cp)) {
			return;
		}
	}
	if (id == I2C_DRIVERID_WM8775) {
		if (pvr2_i2c_wm8775_setup(hdw,cp)) {
			return;
		}
	}
	if (id == I2C_DRIVERID_SAA711X) {
		if (pvr2_i2c_decoder_v4l_setup(hdw,cp)) {
			return;
		}
	}
}


const struct pvr2_i2c_op *pvr2_i2c_get_op(unsigned int idx)
{
	if (idx >= ARRAY_SIZE(ops))
		return NULL;
	return ops[idx];
}


/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 75 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
