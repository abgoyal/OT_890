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
 *  Route Plug-In
 *  Copyright (c) 2000 by Abramo Bagnara <abramo@alsa-project.org>
 *
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#include <linux/slab.h>
#include <linux/time.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include "pcm_plugin.h"

static void zero_areas(struct snd_pcm_plugin_channel *dvp, int ndsts,
		       snd_pcm_uframes_t frames, int format)
{
	int dst = 0;
	for (; dst < ndsts; ++dst) {
		if (dvp->wanted)
			snd_pcm_area_silence(&dvp->area, 0, frames, format);
		dvp->enabled = 0;
		dvp++;
	}
}

static inline void copy_area(const struct snd_pcm_plugin_channel *src_channel,
			     struct snd_pcm_plugin_channel *dst_channel,
			     snd_pcm_uframes_t frames, int format)
{
	dst_channel->enabled = 1;
	snd_pcm_area_copy(&src_channel->area, 0, &dst_channel->area, 0, frames, format);
}

static snd_pcm_sframes_t route_transfer(struct snd_pcm_plugin *plugin,
					const struct snd_pcm_plugin_channel *src_channels,
					struct snd_pcm_plugin_channel *dst_channels,
					snd_pcm_uframes_t frames)
{
	int nsrcs, ndsts, dst;
	struct snd_pcm_plugin_channel *dvp;
	int format;

	if (snd_BUG_ON(!plugin || !src_channels || !dst_channels))
		return -ENXIO;
	if (frames == 0)
		return 0;

	nsrcs = plugin->src_format.channels;
	ndsts = plugin->dst_format.channels;

	format = plugin->dst_format.format;
	dvp = dst_channels;
	if (nsrcs <= 1) {
		/* expand to all channels */
		for (dst = 0; dst < ndsts; ++dst) {
			copy_area(src_channels, dvp, frames, format);
			dvp++;
		}
		return frames;
	}

	for (dst = 0; dst < ndsts && dst < nsrcs; ++dst) {
		copy_area(src_channels, dvp, frames, format);
		dvp++;
		src_channels++;
	}
	if (dst < ndsts)
		zero_areas(dvp, ndsts - dst, frames, format);
	return frames;
}

int snd_pcm_plugin_build_route(struct snd_pcm_substream *plug,
			       struct snd_pcm_plugin_format *src_format,
			       struct snd_pcm_plugin_format *dst_format,
			       struct snd_pcm_plugin **r_plugin)
{
	struct snd_pcm_plugin *plugin;
	int err;

	if (snd_BUG_ON(!r_plugin))
		return -ENXIO;
	*r_plugin = NULL;
	if (snd_BUG_ON(src_format->rate != dst_format->rate))
		return -ENXIO;
	if (snd_BUG_ON(src_format->format != dst_format->format))
		return -ENXIO;

	err = snd_pcm_plugin_build(plug, "route conversion",
				   src_format, dst_format, 0, &plugin);
	if (err < 0)
		return err;

	plugin->transfer = route_transfer;
	*r_plugin = plugin;
	return 0;
}
