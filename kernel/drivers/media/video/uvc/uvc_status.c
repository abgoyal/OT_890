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
 *      uvc_status.c  --  USB Video Class driver - Status endpoint
 *
 *      Copyright (C) 2007-2009
 *          Laurent Pinchart (laurent.pinchart@skynet.be)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 */

#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/usb.h>
#include <linux/usb/input.h>

#include "uvcvideo.h"

/* --------------------------------------------------------------------------
 * Input device
 */
#ifdef CONFIG_USB_VIDEO_CLASS_INPUT_EVDEV
static int uvc_input_init(struct uvc_device *dev)
{
	struct usb_device *udev = dev->udev;
	struct input_dev *input;
	char *phys = NULL;
	int ret;

	input = input_allocate_device();
	if (input == NULL)
		return -ENOMEM;

	phys = kmalloc(6 + strlen(udev->bus->bus_name) + strlen(udev->devpath),
			GFP_KERNEL);
	if (phys == NULL) {
		ret = -ENOMEM;
		goto error;
	}
	sprintf(phys, "usb-%s-%s", udev->bus->bus_name, udev->devpath);

	input->name = dev->name;
	input->phys = phys;
	usb_to_input_id(udev, &input->id);
	input->dev.parent = &dev->intf->dev;

	__set_bit(EV_KEY, input->evbit);
	__set_bit(KEY_CAMERA, input->keybit);

	if ((ret = input_register_device(input)) < 0)
		goto error;

	dev->input = input;
	return 0;

error:
	input_free_device(input);
	kfree(phys);
	return ret;
}

static void uvc_input_cleanup(struct uvc_device *dev)
{
	if (dev->input)
		input_unregister_device(dev->input);
}

static void uvc_input_report_key(struct uvc_device *dev, unsigned int code,
	int value)
{
	if (dev->input) {
		input_report_key(dev->input, code, value);
		input_sync(dev->input);
	}
}

#else
#define uvc_input_init(dev)
#define uvc_input_cleanup(dev)
#define uvc_input_report_key(dev, code, value)
#endif /* CONFIG_USB_VIDEO_CLASS_INPUT_EVDEV */

/* --------------------------------------------------------------------------
 * Status interrupt endpoint
 */
static void uvc_event_streaming(struct uvc_device *dev, __u8 *data, int len)
{
	if (len < 3) {
		uvc_trace(UVC_TRACE_STATUS, "Invalid streaming status event "
				"received.\n");
		return;
	}

	if (data[2] == 0) {
		if (len < 4)
			return;
		uvc_trace(UVC_TRACE_STATUS, "Button (intf %u) %s len %d\n",
			data[1], data[3] ? "pressed" : "released", len);
		uvc_input_report_key(dev, KEY_CAMERA, data[3]);
	} else {
		uvc_trace(UVC_TRACE_STATUS, "Stream %u error event %02x %02x "
			"len %d.\n", data[1], data[2], data[3], len);
	}
}

static void uvc_event_control(struct uvc_device *dev, __u8 *data, int len)
{
	char *attrs[3] = { "value", "info", "failure" };

	if (len < 6 || data[2] != 0 || data[4] > 2) {
		uvc_trace(UVC_TRACE_STATUS, "Invalid control status event "
				"received.\n");
		return;
	}

	uvc_trace(UVC_TRACE_STATUS, "Control %u/%u %s change len %d.\n",
		data[1], data[3], attrs[data[4]], len);
}

static void uvc_status_complete(struct urb *urb)
{
	struct uvc_device *dev = urb->context;
	int len, ret;

	switch (urb->status) {
	case 0:
		break;

	case -ENOENT:		/* usb_kill_urb() called. */
	case -ECONNRESET:	/* usb_unlink_urb() called. */
	case -ESHUTDOWN:	/* The endpoint is being disabled. */
	case -EPROTO:		/* Device is disconnected (reported by some
				 * host controller). */
		return;

	default:
		uvc_printk(KERN_WARNING, "Non-zero status (%d) in status "
			"completion handler.\n", urb->status);
		return;
	}

	len = urb->actual_length;
	if (len > 0) {
		switch (dev->status[0] & 0x0f) {
		case UVC_STATUS_TYPE_CONTROL:
			uvc_event_control(dev, dev->status, len);
			break;

		case UVC_STATUS_TYPE_STREAMING:
			uvc_event_streaming(dev, dev->status, len);
			break;

		default:
			uvc_printk(KERN_INFO, "unknown event type %u.\n",
				dev->status[0]);
			break;
		}
	}

	/* Resubmit the URB. */
	urb->interval = dev->int_ep->desc.bInterval;
	if ((ret = usb_submit_urb(urb, GFP_ATOMIC)) < 0) {
		uvc_printk(KERN_ERR, "Failed to resubmit status URB (%d).\n",
			ret);
	}
}

int uvc_status_init(struct uvc_device *dev)
{
	struct usb_host_endpoint *ep = dev->int_ep;
	unsigned int pipe;
	int interval;

	if (ep == NULL)
		return 0;

	uvc_input_init(dev);

	dev->status = kzalloc(UVC_MAX_STATUS_SIZE, GFP_KERNEL);
	if (dev->status == NULL)
		return -ENOMEM;

	dev->int_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (dev->int_urb == NULL) {
		kfree(dev->status);
		return -ENOMEM;
	}

	pipe = usb_rcvintpipe(dev->udev, ep->desc.bEndpointAddress);

	/* For high-speed interrupt endpoints, the bInterval value is used as
	 * an exponent of two. Some developers forgot about it.
	 */
	interval = ep->desc.bInterval;
	if (interval > 16 && dev->udev->speed == USB_SPEED_HIGH &&
	    (dev->quirks & UVC_QUIRK_STATUS_INTERVAL))
		interval = fls(interval) - 1;

	usb_fill_int_urb(dev->int_urb, dev->udev, pipe,
		dev->status, UVC_MAX_STATUS_SIZE, uvc_status_complete,
		dev, interval);

	return usb_submit_urb(dev->int_urb, GFP_KERNEL);
}

void uvc_status_cleanup(struct uvc_device *dev)
{
	usb_kill_urb(dev->int_urb);
	usb_free_urb(dev->int_urb);
	kfree(dev->status);
	uvc_input_cleanup(dev);
}

int uvc_status_suspend(struct uvc_device *dev)
{
	usb_kill_urb(dev->int_urb);
	return 0;
}

int uvc_status_resume(struct uvc_device *dev)
{
	if (dev->int_urb == NULL)
		return 0;

	return usb_submit_urb(dev->int_urb, GFP_NOIO);
}

