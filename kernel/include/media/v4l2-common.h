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
    v4l2 common internal API header

    This header contains internal shared ioctl definitions for use by the
    internal low-level v4l2 drivers.
    Each ioctl begins with VIDIOC_INT_ to clearly mark that it is an internal
    define,

    Copyright (C) 2005  Hans Verkuil <hverkuil@xs4all.nl>

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef V4L2_COMMON_H_
#define V4L2_COMMON_H_

#include <media/v4l2-dev.h>

/* Common printk constucts for v4l-i2c drivers. These macros create a unique
   prefix consisting of the driver name, the adapter number and the i2c
   address. */
#define v4l_printk(level, name, adapter, addr, fmt, arg...) \
	printk(level "%s %d-%04x: " fmt, name, i2c_adapter_id(adapter), addr , ## arg)

#define v4l_client_printk(level, client, fmt, arg...)			    \
	v4l_printk(level, (client)->driver->driver.name, (client)->adapter, \
		   (client)->addr, fmt , ## arg)

#define v4l_err(client, fmt, arg...) \
	v4l_client_printk(KERN_ERR, client, fmt , ## arg)

#define v4l_warn(client, fmt, arg...) \
	v4l_client_printk(KERN_WARNING, client, fmt , ## arg)

#define v4l_info(client, fmt, arg...) \
	v4l_client_printk(KERN_INFO, client, fmt , ## arg)

/* These three macros assume that the debug level is set with a module
   parameter called 'debug'. */
#define v4l_dbg(level, debug, client, fmt, arg...)			     \
	do { 								     \
		if (debug >= (level))					     \
			v4l_client_printk(KERN_DEBUG, client, fmt , ## arg); \
	} while (0)

/* ------------------------------------------------------------------------- */

/* These printk constructs can be used with v4l2_device and v4l2_subdev */
#define v4l2_printk(level, dev, fmt, arg...) \
	printk(level "%s: " fmt, (dev)->name , ## arg)

#define v4l2_err(dev, fmt, arg...) \
	v4l2_printk(KERN_ERR, dev, fmt , ## arg)

#define v4l2_warn(dev, fmt, arg...) \
	v4l2_printk(KERN_WARNING, dev, fmt , ## arg)

#define v4l2_info(dev, fmt, arg...) \
	v4l2_printk(KERN_INFO, dev, fmt , ## arg)

/* These three macros assume that the debug level is set with a module
   parameter called 'debug'. */
#define v4l2_dbg(level, debug, dev, fmt, arg...)			\
	do { 								\
		if (debug >= (level))					\
			v4l2_printk(KERN_DEBUG, dev, fmt , ## arg); 	\
	} while (0)

/* ------------------------------------------------------------------------- */

/* Priority helper functions */

struct v4l2_prio_state {
	atomic_t prios[4];
};
int v4l2_prio_init(struct v4l2_prio_state *global);
int v4l2_prio_change(struct v4l2_prio_state *global, enum v4l2_priority *local,
		     enum v4l2_priority new);
int v4l2_prio_open(struct v4l2_prio_state *global, enum v4l2_priority *local);
int v4l2_prio_close(struct v4l2_prio_state *global, enum v4l2_priority *local);
enum v4l2_priority v4l2_prio_max(struct v4l2_prio_state *global);
int v4l2_prio_check(struct v4l2_prio_state *global, enum v4l2_priority *local);

/* ------------------------------------------------------------------------- */

/* Control helper functions */

int v4l2_ctrl_check(struct v4l2_ext_control *ctrl, struct v4l2_queryctrl *qctrl,
		const char **menu_items);
const char *v4l2_ctrl_get_name(u32 id);
const char **v4l2_ctrl_get_menu(u32 id);
int v4l2_ctrl_query_fill(struct v4l2_queryctrl *qctrl, s32 min, s32 max, s32 step, s32 def);
int v4l2_ctrl_query_fill_std(struct v4l2_queryctrl *qctrl);
int v4l2_ctrl_query_menu(struct v4l2_querymenu *qmenu,
		struct v4l2_queryctrl *qctrl, const char **menu_items);
#define V4L2_CTRL_MENU_IDS_END (0xffffffff)
int v4l2_ctrl_query_menu_valid_items(struct v4l2_querymenu *qmenu, const u32 *ids);
u32 v4l2_ctrl_next(const u32 * const *ctrl_classes, u32 id);

/* ------------------------------------------------------------------------- */

/* Register/chip ident helper function */

struct i2c_client; /* forward reference */
int v4l2_chip_match_i2c_client(struct i2c_client *c, const struct v4l2_dbg_match *match);
int v4l2_chip_ident_i2c_client(struct i2c_client *c, struct v4l2_dbg_chip_ident *chip,
		u32 ident, u32 revision);
int v4l2_chip_match_host(const struct v4l2_dbg_match *match);

/* ------------------------------------------------------------------------- */

/* Helper function for I2C legacy drivers */

struct i2c_driver;
struct i2c_adapter;
struct i2c_client;
struct i2c_device_id;
struct v4l2_device;
struct v4l2_subdev;
struct v4l2_subdev_ops;

int v4l2_i2c_attach(struct i2c_adapter *adapter, int address, struct i2c_driver *driver,
		const char *name,
		int (*probe)(struct i2c_client *, const struct i2c_device_id *));

/* Load an i2c module and return an initialized v4l2_subdev struct.
   Only call request_module if module_name != NULL.
   The client_type argument is the name of the chip that's on the adapter. */
struct v4l2_subdev *v4l2_i2c_new_subdev(struct i2c_adapter *adapter,
		const char *module_name, const char *client_type, u8 addr);
/* Probe and load an i2c module and return an initialized v4l2_subdev struct.
   Only call request_module if module_name != NULL.
   The client_type argument is the name of the chip that's on the adapter. */
struct v4l2_subdev *v4l2_i2c_new_probed_subdev(struct i2c_adapter *adapter,
		const char *module_name, const char *client_type,
		const unsigned short *addrs);
/* Initialize an v4l2_subdev with data from an i2c_client struct */
void v4l2_i2c_subdev_init(struct v4l2_subdev *sd, struct i2c_client *client,
		const struct v4l2_subdev_ops *ops);

/* ------------------------------------------------------------------------- */

/* Internal ioctls */

/* VIDIOC_INT_DECODE_VBI_LINE */
struct v4l2_decode_vbi_line {
	u32 is_second_field;	/* Set to 0 for the first (odd) field,
				   set to 1 for the second (even) field. */
	u8 *p; 			/* Pointer to the sliced VBI data from the decoder.
				   On exit points to the start of the payload. */
	u32 line;		/* Line number of the sliced VBI data (1-23) */
	u32 type;		/* VBI service type (V4L2_SLICED_*). 0 if no service found */
};

struct v4l2_priv_tun_config {
	int tuner;
	void *priv;
};

/* audio ioctls */

/* v4l device was opened in Radio mode, to be replaced by VIDIOC_INT_S_TUNER_MODE */
#define AUDC_SET_RADIO        _IO('d',88)

/* tuner ioctls */

/* Sets tuner type and its I2C addr */
#define TUNER_SET_TYPE_ADDR          _IOW('d', 90, int)

/* Puts tuner on powersaving state, disabling it, except for i2c. To be replaced
   by VIDIOC_INT_S_STANDBY. */
#define TUNER_SET_STANDBY            _IOW('d', 91, int)

/* Sets tda9887 specific stuff, like port1, port2 and qss */
#define TUNER_SET_CONFIG           _IOW('d', 92, struct v4l2_priv_tun_config)

/* Switch the tuner to a specific tuner mode. Replacement of AUDC_SET_RADIO */
#define VIDIOC_INT_S_TUNER_MODE	     _IOW('d', 93, enum v4l2_tuner_type)

/* Generic standby command. Passing -1 (all bits set to 1) will put the whole
   chip into standby mode, value 0 will make the chip fully active. Specific
   bits can be used by certain chips to enable/disable specific subsystems.
   Replacement of TUNER_SET_STANDBY. */
#define VIDIOC_INT_S_STANDBY 	     _IOW('d', 94, u32)

/* 100, 101 used by  VIDIOC_DBG_[SG]_REGISTER */

/* Generic reset command. The argument selects which subsystems to reset.
   Passing 0 will always reset the whole chip. */
#define VIDIOC_INT_RESET            	_IOW ('d', 102, u32)

/* Set the frequency (in Hz) of the audio clock output.
   Used to slave an audio processor to the video decoder, ensuring that audio
   and video remain synchronized.
   Usual values for the frequency are 48000, 44100 or 32000 Hz.
   If the frequency is not supported, then -EINVAL is returned. */
#define VIDIOC_INT_AUDIO_CLOCK_FREQ 	_IOW ('d', 103, u32)

/* Video decoders that support sliced VBI need to implement this ioctl.
   Field p of the v4l2_sliced_vbi_line struct is set to the start of the VBI
   data that was generated by the decoder. The driver then parses the sliced
   VBI data and sets the other fields in the struct accordingly. The pointer p
   is updated to point to the start of the payload which can be copied
   verbatim into the data field of the v4l2_sliced_vbi_data struct. If no
   valid VBI data was found, then the type field is set to 0 on return. */
#define VIDIOC_INT_DECODE_VBI_LINE  	_IOWR('d', 104, struct v4l2_decode_vbi_line)

/* Used to generate VBI signals on a video signal. v4l2_sliced_vbi_data is
   filled with the data packets that should be output. Note that if you set
   the line field to 0, then that VBI signal is disabled. If no
   valid VBI data was found, then the type field is set to 0 on return. */
#define VIDIOC_INT_S_VBI_DATA 		_IOW ('d', 105, struct v4l2_sliced_vbi_data)

/* Used to obtain the sliced VBI packet from a readback register. Not all
   video decoders support this. If no data is available because the readback
   register contains invalid or erroneous data -EIO is returned. Note that
   you must fill in the 'id' member and the 'field' member (to determine
   whether CC data from the first or second field should be obtained). */
#define VIDIOC_INT_G_VBI_DATA 		_IOWR('d', 106, struct v4l2_sliced_vbi_data)

/* Sets I2S speed in bps. This is used to provide a standard way to select I2S
   clock used by driving digital audio streams at some board designs.
   Usual values for the frequency are 1024000 and 2048000.
   If the frequency is not supported, then -EINVAL is returned. */
#define VIDIOC_INT_I2S_CLOCK_FREQ 	_IOW ('d', 108, u32)

/* Routing definition, device dependent. It specifies which inputs (if any)
   should be routed to which outputs (if any). */
struct v4l2_routing {
	u32 input;
	u32 output;
};

/* These internal commands should be used to define the inputs and outputs
   of an audio/video chip. They will replace the v4l2 API commands
   VIDIOC_S/G_INPUT, VIDIOC_S/G_OUTPUT, VIDIOC_S/G_AUDIO and VIDIOC_S/G_AUDOUT
   that are meant to be used by the user.
   The internal commands should be used to switch inputs/outputs
   because only the driver knows how to map a 'Television' input to the precise
   input/output routing of an A/D converter, or a DSP, or a video digitizer.
   These four commands should only be sent directly to an i2c device, they
   should not be broadcast as the routing is very device specific. */
#define	VIDIOC_INT_S_AUDIO_ROUTING	_IOW ('d', 109, struct v4l2_routing)
#define	VIDIOC_INT_G_AUDIO_ROUTING	_IOR ('d', 110, struct v4l2_routing)
#define	VIDIOC_INT_S_VIDEO_ROUTING	_IOW ('d', 111, struct v4l2_routing)
#define	VIDIOC_INT_G_VIDEO_ROUTING	_IOR ('d', 112, struct v4l2_routing)

struct v4l2_crystal_freq {
	u32 freq;	/* frequency in Hz of the crystal */
	u32 flags; 	/* device specific flags */
};

/* Sets the frequency of the crystal used to generate the clocks.
   An extra flags field allows device specific configuration regarding
   clock frequency dividers, etc. If not used, then set flags to 0.
   If the frequency is not supported, then -EINVAL is returned. */
#define VIDIOC_INT_S_CRYSTAL_FREQ 	_IOW('d', 113, struct v4l2_crystal_freq)

/* Initialize the sensor registors to some sort of reasonable
   default values. */
#define VIDIOC_INT_INIT			_IOW('d', 114, u32)

/* Set v4l2_std_id for video OUTPUT devices. This is ignored by
   video input devices. */
#define VIDIOC_INT_S_STD_OUTPUT		_IOW('d', 115, v4l2_std_id)

/* Get v4l2_std_id for video OUTPUT devices. This is ignored by
   video input devices. */
#define VIDIOC_INT_G_STD_OUTPUT		_IOW('d', 116, v4l2_std_id)

/* Set GPIO pins. Very simple right now, might need to be extended with
   a v4l2_gpio struct if a direction is also needed. */
#define VIDIOC_INT_S_GPIO		_IOW('d', 117, u32)

#endif /* V4L2_COMMON_H_ */
