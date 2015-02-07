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
	Copyright (C) 2004 - 2008 rt2x00 SourceForge Project
	<http://rt2x00.serialmonkey.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the
	Free Software Foundation, Inc.,
	59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	Module: rt2x00lib
	Abstract: rt2x00 led specific routines.
 */

#include <linux/kernel.h>
#include <linux/module.h>

#include "rt2x00.h"
#include "rt2x00lib.h"

void rt2x00leds_led_quality(struct rt2x00_dev *rt2x00dev, int rssi)
{
	struct rt2x00_led *led = &rt2x00dev->led_qual;
	unsigned int brightness;

	if ((led->type != LED_TYPE_QUALITY) || !(led->flags & LED_REGISTERED))
		return;

	/*
	 * Led handling requires a positive value for the rssi,
	 * to do that correctly we need to add the correction.
	 */
	rssi += rt2x00dev->rssi_offset;

	/*
	 * Get the rssi level, this is used to convert the rssi
	 * to a LED value inside the range LED_OFF - LED_FULL.
	 */
	if (rssi <= 30)
		rssi = 0;
	else if (rssi <= 39)
		rssi = 1;
	else if (rssi <= 49)
		rssi = 2;
	else if (rssi <= 53)
		rssi = 3;
	else if (rssi <= 63)
		rssi = 4;
	else
		rssi = 5;

	/*
	 * Note that we must _not_ send LED_OFF since the driver
	 * is going to calculate the value and might use it in a
	 * division.
	 */
	brightness = ((LED_FULL / 6) * rssi) + 1;
	if (brightness != led->led_dev.brightness) {
		led->led_dev.brightness_set(&led->led_dev, brightness);
		led->led_dev.brightness = brightness;
	}
}

static void rt2x00led_led_simple(struct rt2x00_led *led, bool enabled)
{
	unsigned int brightness = enabled ? LED_FULL : LED_OFF;

	if (!(led->flags & LED_REGISTERED))
		return;

	led->led_dev.brightness_set(&led->led_dev, brightness);
	led->led_dev.brightness = brightness;
}

void rt2x00led_led_activity(struct rt2x00_dev *rt2x00dev, bool enabled)
{
	if (rt2x00dev->led_qual.type == LED_TYPE_ACTIVITY)
		rt2x00led_led_simple(&rt2x00dev->led_qual, enabled);
}

void rt2x00leds_led_assoc(struct rt2x00_dev *rt2x00dev, bool enabled)
{
	if (rt2x00dev->led_assoc.type == LED_TYPE_ASSOC)
		rt2x00led_led_simple(&rt2x00dev->led_assoc, enabled);
}

void rt2x00leds_led_radio(struct rt2x00_dev *rt2x00dev, bool enabled)
{
	if (rt2x00dev->led_radio.type == LED_TYPE_RADIO)
		rt2x00led_led_simple(&rt2x00dev->led_radio, enabled);
}

static int rt2x00leds_register_led(struct rt2x00_dev *rt2x00dev,
				   struct rt2x00_led *led,
				   const char *name)
{
	struct device *device = wiphy_dev(rt2x00dev->hw->wiphy);
	int retval;

	led->led_dev.name = name;
	led->led_dev.brightness = LED_OFF;

	retval = led_classdev_register(device, &led->led_dev);
	if (retval) {
		ERROR(rt2x00dev, "Failed to register led handler.\n");
		return retval;
	}

	led->flags |= LED_REGISTERED;

	return 0;
}

void rt2x00leds_register(struct rt2x00_dev *rt2x00dev)
{
	char dev_name[16];
	char name[32];
	int retval;
	unsigned long on_period;
	unsigned long off_period;

	snprintf(dev_name, sizeof(dev_name), "%s-%s",
		 rt2x00dev->ops->name, wiphy_name(rt2x00dev->hw->wiphy));

	if (rt2x00dev->led_radio.flags & LED_INITIALIZED) {
		snprintf(name, sizeof(name), "%s:radio", dev_name);

		retval = rt2x00leds_register_led(rt2x00dev,
						 &rt2x00dev->led_radio,
						 name);
		if (retval)
			goto exit_fail;
	}

	if (rt2x00dev->led_assoc.flags & LED_INITIALIZED) {
		snprintf(name, sizeof(name), "%s:assoc", dev_name);

		retval = rt2x00leds_register_led(rt2x00dev,
						 &rt2x00dev->led_assoc,
						 name);
		if (retval)
			goto exit_fail;
	}

	if (rt2x00dev->led_qual.flags & LED_INITIALIZED) {
		snprintf(name, sizeof(name), "%s:quality", dev_name);

		retval = rt2x00leds_register_led(rt2x00dev,
						 &rt2x00dev->led_qual,
						 name);
		if (retval)
			goto exit_fail;
	}

	/*
	 * Initialize blink time to default value:
	 * On period: 70ms
	 * Off period: 30ms
	 */
	if (rt2x00dev->led_radio.led_dev.blink_set) {
		on_period = 70;
		off_period = 30;
		rt2x00dev->led_radio.led_dev.blink_set(
		    &rt2x00dev->led_radio.led_dev, &on_period, &off_period);
	}

	return;

exit_fail:
	rt2x00leds_unregister(rt2x00dev);
}

static void rt2x00leds_unregister_led(struct rt2x00_led *led)
{
	led_classdev_unregister(&led->led_dev);

	/*
	 * This might look weird, but when we are unregistering while
	 * suspended the led is already off, and since we haven't
	 * fully resumed yet, access to the device might not be
	 * possible yet.
	 */
	if (!(led->led_dev.flags & LED_SUSPENDED))
		led->led_dev.brightness_set(&led->led_dev, LED_OFF);

	led->flags &= ~LED_REGISTERED;
}

void rt2x00leds_unregister(struct rt2x00_dev *rt2x00dev)
{
	if (rt2x00dev->led_qual.flags & LED_REGISTERED)
		rt2x00leds_unregister_led(&rt2x00dev->led_qual);
	if (rt2x00dev->led_assoc.flags & LED_REGISTERED)
		rt2x00leds_unregister_led(&rt2x00dev->led_assoc);
	if (rt2x00dev->led_radio.flags & LED_REGISTERED)
		rt2x00leds_unregister_led(&rt2x00dev->led_radio);
}

static inline void rt2x00leds_suspend_led(struct rt2x00_led *led)
{
	led_classdev_suspend(&led->led_dev);

	/* This shouldn't be needed, but just to be safe */
	led->led_dev.brightness_set(&led->led_dev, LED_OFF);
	led->led_dev.brightness = LED_OFF;
}

void rt2x00leds_suspend(struct rt2x00_dev *rt2x00dev)
{
	if (rt2x00dev->led_qual.flags & LED_REGISTERED)
		rt2x00leds_suspend_led(&rt2x00dev->led_qual);
	if (rt2x00dev->led_assoc.flags & LED_REGISTERED)
		rt2x00leds_suspend_led(&rt2x00dev->led_assoc);
	if (rt2x00dev->led_radio.flags & LED_REGISTERED)
		rt2x00leds_suspend_led(&rt2x00dev->led_radio);
}

static inline void rt2x00leds_resume_led(struct rt2x00_led *led)
{
	led_classdev_resume(&led->led_dev);

	/* Device might have enabled the LEDS during resume */
	led->led_dev.brightness_set(&led->led_dev, LED_OFF);
	led->led_dev.brightness = LED_OFF;
}

void rt2x00leds_resume(struct rt2x00_dev *rt2x00dev)
{
	if (rt2x00dev->led_radio.flags & LED_REGISTERED)
		rt2x00leds_resume_led(&rt2x00dev->led_radio);
	if (rt2x00dev->led_assoc.flags & LED_REGISTERED)
		rt2x00leds_resume_led(&rt2x00dev->led_assoc);
	if (rt2x00dev->led_qual.flags & LED_REGISTERED)
		rt2x00leds_resume_led(&rt2x00dev->led_qual);
}