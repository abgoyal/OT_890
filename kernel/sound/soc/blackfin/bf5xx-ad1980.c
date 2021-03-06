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
 * File:         sound/soc/blackfin/bf5xx-ad1980.c
 * Author:       Cliff Cai <Cliff.Cai@analog.com>
 *
 * Created:      Tue June 06 2008
 * Description:  Board driver for AD1980/1 audio codec
 *
 * Modified:
 *               Copyright 2008 Analog Devices Inc.
 *
 * Bugs:         Enter bugs at http://blackfin.uclinux.org/
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
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/device.h>
#include <asm/dma.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>

#include <linux/gpio.h>
#include <asm/portmux.h>

#include "../codecs/ad1980.h"
#include "bf5xx-sport.h"
#include "bf5xx-ac97-pcm.h"
#include "bf5xx-ac97.h"

static struct snd_soc_card bf5xx_board;

static int bf5xx_board_startup(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;

	pr_debug("%s enter\n", __func__);
	cpu_dai->private_data = sport_handle;
	return 0;
}

static struct snd_soc_ops bf5xx_board_ops = {
	.startup = bf5xx_board_startup,
};

static struct snd_soc_dai_link bf5xx_board_dai = {
	.name = "AC97",
	.stream_name = "AC97 HiFi",
	.cpu_dai = &bfin_ac97_dai,
	.codec_dai = &ad1980_dai,
	.ops = &bf5xx_board_ops,
};

static struct snd_soc_card bf5xx_board = {
	.name = "bf5xx-board",
	.platform = &bf5xx_ac97_soc_platform,
	.dai_link = &bf5xx_board_dai,
	.num_links = 1,
};

static struct snd_soc_device bf5xx_board_snd_devdata = {
	.card = &bf5xx_board,
	.codec_dev = &soc_codec_dev_ad1980,
};

static struct platform_device *bf5xx_board_snd_device;

static int __init bf5xx_board_init(void)
{
	int ret;

	bf5xx_board_snd_device = platform_device_alloc("soc-audio", -1);
	if (!bf5xx_board_snd_device)
		return -ENOMEM;

	platform_set_drvdata(bf5xx_board_snd_device, &bf5xx_board_snd_devdata);
	bf5xx_board_snd_devdata.dev = &bf5xx_board_snd_device->dev;
	ret = platform_device_add(bf5xx_board_snd_device);

	if (ret)
		platform_device_put(bf5xx_board_snd_device);

	return ret;
}

static void __exit bf5xx_board_exit(void)
{
	platform_device_unregister(bf5xx_board_snd_device);
}

module_init(bf5xx_board_init);
module_exit(bf5xx_board_exit);

/* Module information */
MODULE_AUTHOR("Cliff Cai");
MODULE_DESCRIPTION("ALSA SoC AD1980/1 BF5xx board");
MODULE_LICENSE("GPL");
