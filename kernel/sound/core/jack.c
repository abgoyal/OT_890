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
 *  Jack abstraction layer
 *
 *  Copyright 2008 Wolfson Microelectronics
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#include <linux/input.h>
#include <sound/jack.h>
#include <sound/core.h>

static int snd_jack_dev_free(struct snd_device *device)
{
	struct snd_jack *jack = device->device_data;

	/* If the input device is registered with the input subsystem
	 * then we need to use a different deallocator. */
	if (jack->registered)
		input_unregister_device(jack->input_dev);
	else
		input_free_device(jack->input_dev);

	kfree(jack->id);
	kfree(jack);

	return 0;
}

static int snd_jack_dev_register(struct snd_device *device)
{
	struct snd_jack *jack = device->device_data;
	struct snd_card *card = device->card;
	int err;

	snprintf(jack->name, sizeof(jack->name), "%s %s",
		 card->shortname, jack->id);
	jack->input_dev->name = jack->name;

	/* Default to the sound card device. */
	if (!jack->input_dev->dev.parent)
		jack->input_dev->dev.parent = card->dev;

	err = input_register_device(jack->input_dev);
	if (err == 0)
		jack->registered = 1;

	return err;
}

/**
 * snd_jack_new - Create a new jack
 * @card:  the card instance
 * @id:    an identifying string for this jack
 * @type:  a bitmask of enum snd_jack_type values that can be detected by
 *         this jack
 * @jjack: Used to provide the allocated jack object to the caller.
 *
 * Creates a new jack object.
 *
 * Returns zero if successful, or a negative error code on failure.
 * On success jjack will be initialised.
 */
int snd_jack_new(struct snd_card *card, const char *id, int type,
		 struct snd_jack **jjack)
{
	struct snd_jack *jack;
	int err;
	static struct snd_device_ops ops = {
		.dev_free = snd_jack_dev_free,
		.dev_register = snd_jack_dev_register,
	};

	jack = kzalloc(sizeof(struct snd_jack), GFP_KERNEL);
	if (jack == NULL)
		return -ENOMEM;

	jack->id = kstrdup(id, GFP_KERNEL);

	jack->input_dev = input_allocate_device();
	if (jack->input_dev == NULL) {
		err = -ENOMEM;
		goto fail_input;
	}

	jack->input_dev->phys = "ALSA";

	jack->type = type;

	if (type & SND_JACK_HEADPHONE)
		input_set_capability(jack->input_dev, EV_SW,
				     SW_HEADPHONE_INSERT);
	if (type & SND_JACK_LINEOUT)
		input_set_capability(jack->input_dev, EV_SW,
				     SW_LINEOUT_INSERT);
	if (type & SND_JACK_MICROPHONE)
		input_set_capability(jack->input_dev, EV_SW,
				     SW_MICROPHONE_INSERT);
	if (type & SND_JACK_MECHANICAL)
		input_set_capability(jack->input_dev, EV_SW,
				     SW_JACK_PHYSICAL_INSERT);

	err = snd_device_new(card, SNDRV_DEV_JACK, jack, &ops);
	if (err < 0)
		goto fail_input;

	*jjack = jack;

	return 0;

fail_input:
	input_free_device(jack->input_dev);
	kfree(jack);
	return err;
}
EXPORT_SYMBOL(snd_jack_new);

/**
 * snd_jack_set_parent - Set the parent device for a jack
 *
 * @jack:   The jack to configure
 * @parent: The device to set as parent for the jack.
 *
 * Set the parent for the jack input device in the device tree.  This
 * function is only valid prior to registration of the jack.  If no
 * parent is configured then the parent device will be the sound card.
 */
void snd_jack_set_parent(struct snd_jack *jack, struct device *parent)
{
	WARN_ON(jack->registered);

	jack->input_dev->dev.parent = parent;
}
EXPORT_SYMBOL(snd_jack_set_parent);

/**
 * snd_jack_report - Report the current status of a jack
 *
 * @jack:   The jack to report status for
 * @status: The current status of the jack
 */
void snd_jack_report(struct snd_jack *jack, int status)
{
	if (!jack)
		return;

	if (jack->type & SND_JACK_HEADPHONE)
		input_report_switch(jack->input_dev, SW_HEADPHONE_INSERT,
				    status & SND_JACK_HEADPHONE);
	if (jack->type & SND_JACK_LINEOUT)
		input_report_switch(jack->input_dev, SW_LINEOUT_INSERT,
				    status & SND_JACK_LINEOUT);
	if (jack->type & SND_JACK_MICROPHONE)
		input_report_switch(jack->input_dev, SW_MICROPHONE_INSERT,
				    status & SND_JACK_MICROPHONE);
	if (jack->type & SND_JACK_MECHANICAL)
		input_report_switch(jack->input_dev, SW_JACK_PHYSICAL_INSERT,
				    status & SND_JACK_MECHANICAL);

	input_sync(jack->input_dev);
}
EXPORT_SYMBOL(snd_jack_report);

MODULE_AUTHOR("Mark Brown <broonie@opensource.wolfsonmicro.com>");
MODULE_DESCRIPTION("Jack detection support for ALSA");
MODULE_LICENSE("GPL");
