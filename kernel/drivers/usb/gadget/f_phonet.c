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
 * f_phonet.c -- USB CDC Phonet function
 *
 * Copyright (C) 2007-2008 Nokia Corporation. All rights reserved.
 *
 * Author: Rémi Denis-Courmont
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <linux/kernel.h>
#include <linux/device.h>

#include <linux/netdevice.h>
#include <linux/if_ether.h>
#include <linux/if_phonet.h>
#include <linux/if_arp.h>

#include <linux/usb/ch9.h>
#include <linux/usb/cdc.h>
#include <linux/usb/composite.h>

#include "u_phonet.h"

#define PN_MEDIA_USB	0x1B

/*-------------------------------------------------------------------------*/

struct phonet_port {
	struct f_phonet			*usb;
	spinlock_t			lock;
};

struct f_phonet {
	struct usb_function		function;
	struct net_device		*dev;
	struct usb_ep			*in_ep, *out_ep;

	struct usb_request		*in_req;
	struct usb_request		*out_reqv[0];
};

static int phonet_rxq_size = 2;

static inline struct f_phonet *func_to_pn(struct usb_function *f)
{
	return container_of(f, struct f_phonet, function);
}

/*-------------------------------------------------------------------------*/

#define USB_CDC_SUBCLASS_PHONET	0xfe
#define USB_CDC_PHONET_TYPE	0xab

static struct usb_interface_descriptor
pn_control_intf_desc = {
	.bLength =		sizeof pn_control_intf_desc,
	.bDescriptorType =	USB_DT_INTERFACE,

	/* .bInterfaceNumber =	DYNAMIC, */
	.bInterfaceClass =	USB_CLASS_COMM,
	.bInterfaceSubClass =	USB_CDC_SUBCLASS_PHONET,
};

static const struct usb_cdc_header_desc
pn_header_desc = {
	.bLength =		sizeof pn_header_desc,
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubType =	USB_CDC_HEADER_TYPE,
	.bcdCDC =		__constant_cpu_to_le16(0x0110),
};

static const struct usb_cdc_header_desc
pn_phonet_desc = {
	.bLength =		sizeof pn_phonet_desc,
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubType =	USB_CDC_PHONET_TYPE,
	.bcdCDC =		__constant_cpu_to_le16(0x1505), /* ??? */
};

static struct usb_cdc_union_desc
pn_union_desc = {
	.bLength =		sizeof pn_union_desc,
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubType =	USB_CDC_UNION_TYPE,

	/* .bMasterInterface0 =	DYNAMIC, */
	/* .bSlaveInterface0 =	DYNAMIC, */
};

static struct usb_interface_descriptor
pn_data_nop_intf_desc = {
	.bLength =		sizeof pn_data_nop_intf_desc,
	.bDescriptorType =	USB_DT_INTERFACE,

	/* .bInterfaceNumber =	DYNAMIC, */
	.bAlternateSetting =	0,
	.bNumEndpoints =	0,
	.bInterfaceClass =	USB_CLASS_CDC_DATA,
};

static struct usb_interface_descriptor
pn_data_intf_desc = {
	.bLength =		sizeof pn_data_intf_desc,
	.bDescriptorType =	USB_DT_INTERFACE,

	/* .bInterfaceNumber =	DYNAMIC, */
	.bAlternateSetting =	1,
	.bNumEndpoints =	2,
	.bInterfaceClass =	USB_CLASS_CDC_DATA,
};

static struct usb_endpoint_descriptor
pn_fs_sink_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_OUT,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
};

static struct usb_endpoint_descriptor
pn_hs_sink_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_OUT,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	__constant_cpu_to_le16(512),
};

static struct usb_endpoint_descriptor
pn_fs_source_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
};

static struct usb_endpoint_descriptor
pn_hs_source_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,

	.bEndpointAddress =	USB_DIR_IN,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	__constant_cpu_to_le16(512),
};

static struct usb_descriptor_header *fs_pn_function[] = {
	(struct usb_descriptor_header *) &pn_control_intf_desc,
	(struct usb_descriptor_header *) &pn_header_desc,
	(struct usb_descriptor_header *) &pn_phonet_desc,
	(struct usb_descriptor_header *) &pn_union_desc,
	(struct usb_descriptor_header *) &pn_data_nop_intf_desc,
	(struct usb_descriptor_header *) &pn_data_intf_desc,
	(struct usb_descriptor_header *) &pn_fs_sink_desc,
	(struct usb_descriptor_header *) &pn_fs_source_desc,
	NULL,
};

static struct usb_descriptor_header *hs_pn_function[] = {
	(struct usb_descriptor_header *) &pn_control_intf_desc,
	(struct usb_descriptor_header *) &pn_header_desc,
	(struct usb_descriptor_header *) &pn_phonet_desc,
	(struct usb_descriptor_header *) &pn_union_desc,
	(struct usb_descriptor_header *) &pn_data_nop_intf_desc,
	(struct usb_descriptor_header *) &pn_data_intf_desc,
	(struct usb_descriptor_header *) &pn_hs_sink_desc,
	(struct usb_descriptor_header *) &pn_hs_source_desc,
	NULL,
};

/*-------------------------------------------------------------------------*/

static int pn_net_open(struct net_device *dev)
{
	if (netif_carrier_ok(dev))
		netif_wake_queue(dev);
	return 0;
}

static int pn_net_close(struct net_device *dev)
{
	netif_stop_queue(dev);
	return 0;
}

static void pn_tx_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct f_phonet *fp = ep->driver_data;
	struct net_device *dev = fp->dev;
	struct sk_buff *skb = req->context;

	switch (req->status) {
	case 0:
		dev->stats.tx_packets++;
		dev->stats.tx_bytes += skb->len;
		break;

	case -ESHUTDOWN: /* disconnected */
	case -ECONNRESET: /* disabled */
		dev->stats.tx_aborted_errors++;
	default:
		dev->stats.tx_errors++;
	}

	dev_kfree_skb_any(skb);
	if (netif_carrier_ok(dev))
		netif_wake_queue(dev);
}

static int pn_net_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct phonet_port *port = netdev_priv(dev);
	struct f_phonet *fp;
	struct usb_request *req;
	unsigned long flags;

	if (skb->protocol != htons(ETH_P_PHONET))
		goto out;

	spin_lock_irqsave(&port->lock, flags);
	fp = port->usb;
	if (unlikely(!fp)) /* race with carrier loss */
		goto out_unlock;

	req = fp->in_req;
	req->buf = skb->data;
	req->length = skb->len;
	req->complete = pn_tx_complete;
	req->zero = 1;
	req->context = skb;

	if (unlikely(usb_ep_queue(fp->in_ep, req, GFP_ATOMIC)))
		goto out_unlock;

	netif_stop_queue(dev);
	skb = NULL;

out_unlock:
	spin_unlock_irqrestore(&port->lock, flags);
out:
	if (unlikely(skb)) {
		dev_kfree_skb_any(skb);
		dev->stats.tx_dropped++;
	}
	return 0;
}

static int pn_net_mtu(struct net_device *dev, int new_mtu)
{
	struct phonet_port *port = netdev_priv(dev);
	unsigned long flags;
	int err = -EBUSY;

	if ((new_mtu < PHONET_MIN_MTU) || (new_mtu > PHONET_MAX_MTU))
		return -EINVAL;

	spin_lock_irqsave(&port->lock, flags);
	if (!netif_carrier_ok(dev)) {
		dev->mtu = new_mtu;
		err = 0;
	}
	spin_unlock_irqrestore(&port->lock, flags);
	return err;
}

static const struct net_device_ops pn_netdev_ops = {
	.ndo_open	= pn_net_open,
	.ndo_stop	= pn_net_close,
	.ndo_start_xmit	= pn_net_xmit,
	.ndo_change_mtu	= pn_net_mtu,
};

static void pn_net_setup(struct net_device *dev)
{
	dev->features		= 0;
	dev->type		= ARPHRD_PHONET;
	dev->flags		= IFF_POINTOPOINT | IFF_NOARP;
	dev->mtu		= PHONET_DEV_MTU;
	dev->hard_header_len	= 1;
	dev->dev_addr[0]	= PN_MEDIA_USB;
	dev->addr_len		= 1;
	dev->tx_queue_len	= 1;

	dev->netdev_ops		= &pn_netdev_ops;
	dev->destructor		= free_netdev;
	dev->header_ops		= &phonet_header_ops;
}

/*-------------------------------------------------------------------------*/

/*
 * Queue buffer for data from the host
 */
static int
pn_rx_submit(struct f_phonet *fp, struct usb_request *req, gfp_t gfp_flags)
{
	struct sk_buff *skb;
	const size_t size = fp->dev->mtu;
	int err;

	skb = alloc_skb(size, gfp_flags);
	if (!skb)
		return -ENOMEM;

	req->buf = skb->data;
	req->length = size;
	req->context = skb;

	err = usb_ep_queue(fp->out_ep, req, gfp_flags);
	if (unlikely(err))
		dev_kfree_skb_any(skb);
	return err;
}

static void pn_rx_complete(struct usb_ep *ep, struct usb_request *req)
{
	struct f_phonet *fp = ep->driver_data;
	struct net_device *dev = fp->dev;
	struct sk_buff *skb = req->context;
	int status = req->status;

	switch (status) {
	case 0:
		if (unlikely(!netif_running(dev)))
			break;
		if (unlikely(req->actual < 1))
			break;
		skb_put(skb, req->actual);
		skb->protocol = htons(ETH_P_PHONET);
		skb_reset_mac_header(skb);
		__skb_pull(skb, 1);
		skb->dev = dev;
		dev->stats.rx_packets++;
		dev->stats.rx_bytes += skb->len;

		netif_rx(skb);
		skb = NULL;
		break;

	/* Do not resubmit in these cases: */
	case -ESHUTDOWN: /* disconnect */
	case -ECONNABORTED: /* hw reset */
	case -ECONNRESET: /* dequeued (unlink or netif down) */
		req = NULL;
		break;

	/* Do resubmit in these cases: */
	case -EOVERFLOW: /* request buffer overflow */
		dev->stats.rx_over_errors++;
	default:
		dev->stats.rx_errors++;
		break;
	}

	if (skb)
		dev_kfree_skb_any(skb);
	if (req)
		pn_rx_submit(fp, req, GFP_ATOMIC);
}

/*-------------------------------------------------------------------------*/

static void __pn_reset(struct usb_function *f)
{
	struct f_phonet *fp = func_to_pn(f);
	struct net_device *dev = fp->dev;
	struct phonet_port *port = netdev_priv(dev);

	netif_carrier_off(dev);
	netif_stop_queue(dev);
	port->usb = NULL;

	usb_ep_disable(fp->out_ep);
	usb_ep_disable(fp->in_ep);
}

static int pn_set_alt(struct usb_function *f, unsigned intf, unsigned alt)
{
	struct f_phonet *fp = func_to_pn(f);
	struct usb_gadget *gadget = fp->function.config->cdev->gadget;

	if (intf == pn_control_intf_desc.bInterfaceNumber)
		/* control interface, no altsetting */
		return (alt > 0) ? -EINVAL : 0;

	if (intf == pn_data_intf_desc.bInterfaceNumber) {
		struct net_device *dev = fp->dev;
		struct phonet_port *port = netdev_priv(dev);

		/* data intf (0: inactive, 1: active) */
		if (alt > 1)
			return -EINVAL;

		spin_lock(&port->lock);
		__pn_reset(f);
		if (alt == 1) {
			struct usb_endpoint_descriptor *out, *in;
			int i;

			out = ep_choose(gadget,
					&pn_hs_sink_desc,
					&pn_fs_sink_desc);
			in = ep_choose(gadget,
					&pn_hs_source_desc,
					&pn_fs_source_desc);
			usb_ep_enable(fp->out_ep, out);
			usb_ep_enable(fp->in_ep, in);

			port->usb = fp;
			fp->out_ep->driver_data = fp;
			fp->in_ep->driver_data = fp;

			netif_carrier_on(dev);
			if (netif_running(dev))
				netif_wake_queue(dev);
			for (i = 0; i < phonet_rxq_size; i++)
				pn_rx_submit(fp, fp->out_reqv[i], GFP_ATOMIC);
		}
		spin_unlock(&port->lock);
		return 0;
	}

	return -EINVAL;
}

static int pn_get_alt(struct usb_function *f, unsigned intf)
{
	struct f_phonet *fp = func_to_pn(f);

	if (intf == pn_control_intf_desc.bInterfaceNumber)
		return 0;

	if (intf == pn_data_intf_desc.bInterfaceNumber) {
		struct phonet_port *port = netdev_priv(fp->dev);
		u8 alt;

		spin_lock(&port->lock);
		alt = port->usb != NULL;
		spin_unlock(&port->lock);
		return alt;
	}

	return -EINVAL;
}

static void pn_disconnect(struct usb_function *f)
{
	struct f_phonet *fp = func_to_pn(f);
	struct phonet_port *port = netdev_priv(fp->dev);
	unsigned long flags;

	/* remain disabled until set_alt */
	spin_lock_irqsave(&port->lock, flags);
	__pn_reset(f);
	spin_unlock_irqrestore(&port->lock, flags);
}

/*-------------------------------------------------------------------------*/

static __init
int pn_bind(struct usb_configuration *c, struct usb_function *f)
{
	struct usb_composite_dev *cdev = c->cdev;
	struct usb_gadget *gadget = cdev->gadget;
	struct f_phonet *fp = func_to_pn(f);
	struct usb_ep *ep;
	int status, i;

	/* Reserve interface IDs */
	status = usb_interface_id(c, f);
	if (status < 0)
		goto err;
	pn_control_intf_desc.bInterfaceNumber = status;
	pn_union_desc.bMasterInterface0 = status;

	status = usb_interface_id(c, f);
	if (status < 0)
		goto err;
	pn_data_nop_intf_desc.bInterfaceNumber = status;
	pn_data_intf_desc.bInterfaceNumber = status;
	pn_union_desc.bSlaveInterface0 = status;

	/* Reserve endpoints */
	status = -ENODEV;
	ep = usb_ep_autoconfig(gadget, &pn_fs_sink_desc);
	if (!ep)
		goto err;
	fp->out_ep = ep;
	ep->driver_data = fp; /* Claim */

	ep = usb_ep_autoconfig(gadget, &pn_fs_source_desc);
	if (!ep)
		goto err;
	fp->in_ep = ep;
	ep->driver_data = fp; /* Claim */

	pn_hs_sink_desc.bEndpointAddress =
		pn_fs_sink_desc.bEndpointAddress;
	pn_hs_source_desc.bEndpointAddress =
		pn_fs_source_desc.bEndpointAddress;

	/* Do not try to bind Phonet twice... */
	fp->function.descriptors = fs_pn_function;
	fp->function.hs_descriptors = hs_pn_function;

	/* Incoming USB requests */
	status = -ENOMEM;
	for (i = 0; i < phonet_rxq_size; i++) {
		struct usb_request *req;

		req = usb_ep_alloc_request(fp->out_ep, GFP_KERNEL);
		if (!req)
			goto err;

		req->complete = pn_rx_complete;
		fp->out_reqv[i] = req;
	}

	/* Outgoing USB requests */
	fp->in_req = usb_ep_alloc_request(fp->in_ep, GFP_KERNEL);
	if (!fp->in_req)
		goto err;

	INFO(cdev, "USB CDC Phonet function\n");
	INFO(cdev, "using %s, OUT %s, IN %s\n", cdev->gadget->name,
		fp->out_ep->name, fp->in_ep->name);
	return 0;

err:
	if (fp->out_ep)
		fp->out_ep->driver_data = NULL;
	if (fp->in_ep)
		fp->in_ep->driver_data = NULL;
	ERROR(cdev, "USB CDC Phonet: cannot autoconfigure\n");
	return status;
}

static void
pn_unbind(struct usb_configuration *c, struct usb_function *f)
{
	struct f_phonet *fp = func_to_pn(f);
	int i;

	/* We are already disconnected */
	if (fp->in_req)
		usb_ep_free_request(fp->in_ep, fp->in_req);
	for (i = 0; i < phonet_rxq_size; i++)
		if (fp->out_reqv[i])
			usb_ep_free_request(fp->out_ep, fp->out_reqv[i]);

	kfree(fp);
}

/*-------------------------------------------------------------------------*/

static struct net_device *dev;

int __init phonet_bind_config(struct usb_configuration *c)
{
	struct f_phonet *fp;
	int err;

	fp = kzalloc(sizeof(*fp), GFP_KERNEL);
	if (!fp)
		return -ENOMEM;

	fp->dev = dev;
	fp->function.name = "phonet";
	fp->function.bind = pn_bind;
	fp->function.unbind = pn_unbind;
	fp->function.set_alt = pn_set_alt;
	fp->function.get_alt = pn_get_alt;
	fp->function.disable = pn_disconnect;

	err = usb_add_function(c, &fp->function);
	if (err)
		kfree(fp);
	return err;
}

int __init gphonet_setup(struct usb_gadget *gadget)
{
	struct phonet_port *port;
	int err;

	/* Create net device */
	BUG_ON(dev);
	dev = alloc_netdev(sizeof(*port)
		+ (phonet_rxq_size * sizeof(struct usb_request *)),
				"upnlink%d", pn_net_setup);
	if (!dev)
		return -ENOMEM;

	port = netdev_priv(dev);
	spin_lock_init(&port->lock);
	netif_carrier_off(dev);
	netif_stop_queue(dev);
	SET_NETDEV_DEV(dev, &gadget->dev);

	err = register_netdev(dev);
	if (err)
		free_netdev(dev);
	return err;
}

void gphonet_cleanup(void)
{
	unregister_netdev(dev);
}