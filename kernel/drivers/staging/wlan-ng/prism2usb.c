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

#include "hfa384x_usb.c"
#include "prism2mgmt.c"
#include "prism2mib.c"
#include "prism2sta.c"

#define PRISM_USB_DEVICE(vid, pid, name) \
           USB_DEVICE(vid, pid),  \
           .driver_info = (unsigned long) name

static struct usb_device_id usb_prism_tbl[] = {
	{PRISM_USB_DEVICE(0x04bb, 0x0922, "IOData AirPort WN-B11/USBS")},
	{PRISM_USB_DEVICE(0x07aa, 0x0012, "Corega Wireless LAN USB Stick-11")},
	{PRISM_USB_DEVICE(0x09aa, 0x3642, "Prism2.x 11Mbps WLAN USB Adapter")},
	{PRISM_USB_DEVICE(0x1668, 0x0408, "Actiontec Prism2.5 11Mbps WLAN USB Adapter")},
	{PRISM_USB_DEVICE(0x1668, 0x0421, "Actiontec Prism2.5 11Mbps WLAN USB Adapter")},
	{PRISM_USB_DEVICE(0x1915, 0x2236, "Linksys WUSB11v3.0 11Mbps WLAN USB Adapter")},
	{PRISM_USB_DEVICE(0x066b, 0x2212, "Linksys WUSB11v2.5 11Mbps WLAN USB Adapter")},
	{PRISM_USB_DEVICE(0x066b, 0x2213, "Linksys WUSB12v1.1 11Mbps WLAN USB Adapter")},
	{PRISM_USB_DEVICE(0x067c, 0x1022, "Siemens SpeedStream 1022 11Mbps WLAN USB Adapter")},
	{PRISM_USB_DEVICE(0x049f, 0x0033, "Compaq/Intel W100 PRO/Wireless 11Mbps multiport WLAN Adapter")},
	{PRISM_USB_DEVICE(0x0411, 0x0016, "Melco WLI-USB-S11 11Mbps WLAN Adapter")},
	{PRISM_USB_DEVICE(0x08de, 0x7a01, "PRISM25 IEEE 802.11 Mini USB Adapter")},
	{PRISM_USB_DEVICE(0x8086, 0x1111, "Intel PRO/Wireless 2011B LAN USB Adapter")},
	{PRISM_USB_DEVICE(0x0d8e, 0x7a01, "PRISM25 IEEE 802.11 Mini USB Adapter")},
	{PRISM_USB_DEVICE(0x045e, 0x006e, "Microsoft MN510 Wireless USB Adapter")},
	{PRISM_USB_DEVICE(0x0967, 0x0204, "Acer Warplink USB Adapter")},
	{PRISM_USB_DEVICE(0x0cde, 0x0002, "Z-Com 725/726 Prism2.5 USB/USB Integrated")},
	{PRISM_USB_DEVICE(0x0cde, 0x0005, "Z-Com Xl735 Wireless 802.11b USB Adapter")},
	{PRISM_USB_DEVICE(0x413c, 0x8100, "Dell TrueMobile 1180 Wireless USB Adapter")},
	{PRISM_USB_DEVICE(0x0b3b, 0x1601, "ALLNET 0193 11Mbps WLAN USB Adapter")},
	{PRISM_USB_DEVICE(0x0b3b, 0x1602, "ZyXEL ZyAIR B200 Wireless USB Adapter")},
	{PRISM_USB_DEVICE(0x0baf, 0x00eb, "USRobotics USR1120 Wireless USB Adapter")},
	{PRISM_USB_DEVICE(0x0411, 0x0027, "Melco WLI-USB-KS11G 11Mbps WLAN Adapter")},
        {PRISM_USB_DEVICE(0x04f1, 0x3009, "JVC MP-XP7250 Builtin USB WLAN Adapter")},
	{PRISM_USB_DEVICE(0x0846, 0x4110, "NetGear MA111")},
        {PRISM_USB_DEVICE(0x03f3, 0x0020, "Adaptec AWN-8020 USB WLAN Adapter")},
//	{PRISM_USB_DEVICE(0x0ace, 0x1201, "ZyDAS ZD1201 Wireless USB Adapter")},
	{PRISM_USB_DEVICE(0x2821, 0x3300, "ASUS-WL140 Wireless USB Adapter")},
	{PRISM_USB_DEVICE(0x2001, 0x3700, "DWL-122 Wireless USB Adapter")},
	{PRISM_USB_DEVICE(0x2001, 0x3702, "DWL-120 Rev F Wireless USB Adapter")},
	{PRISM_USB_DEVICE(0x50c2, 0x4013, "Averatec USB WLAN Adapter")},
	{PRISM_USB_DEVICE(0x2c02, 0x14ea, "Planex GW-US11H WLAN USB Adapter")},
	{PRISM_USB_DEVICE(0x124a, 0x168b, "Airvast PRISM3 WLAN USB Adapter")},
	{PRISM_USB_DEVICE(0x083a, 0x3503, "T-Sinus 111 USB WLAN Adapter")},
	{PRISM_USB_DEVICE(0x2821, 0x3300, "Hawking HighDB USB Adapter")},
	{PRISM_USB_DEVICE(0x0411, 0x0044, "Melco WLI-USB-KB11 11Mbps WLAN Adapter")},
	{PRISM_USB_DEVICE(0x1668, 0x6106, "ROPEX FreeLan 802.11b USB Adapter")},
	{PRISM_USB_DEVICE(0x124a, 0x4017, "Pheenet WL-503IA 802.11b USB Adapter")},
	{PRISM_USB_DEVICE(0x0bb2, 0x0302, "Ambit Microsystems Corp.")},
	{PRISM_USB_DEVICE(0x9016, 0x182d, "Sitecom WL-022 802.11b USB Adapter")},
	{PRISM_USB_DEVICE(0x0543, 0x0f01, "ViewSonic Airsync USB Adapter 11Mbps (Prism2.5)")},
	{ /* terminator */ }
};

MODULE_DEVICE_TABLE(usb, usb_prism_tbl);

/*----------------------------------------------------------------
* prism2sta_probe_usb
*
* Probe routine called by the USB subsystem.
*
* Arguments:
*	dev		ptr to the usb_device struct
*	ifnum		interface number being offered
*
* Returns:
*	NULL		- we're not claiming the device+interface
*	non-NULL	- we are claiming the device+interface and
*			  this is a ptr to the data we want back
*			  when disconnect is called.
*
* Side effects:
*
* Call context:
*	I'm not sure, assume it's interrupt.
*
----------------------------------------------------------------*/
static int prism2sta_probe_usb(
	struct usb_interface *interface,
	const struct usb_device_id *id)
{
	struct usb_device *dev;

	wlandevice_t	*wlandev = NULL;
	hfa384x_t	*hw = NULL;
	int              result = 0;

	DBFENTER;

	dev = interface_to_usbdev(interface);

	if ((wlandev = create_wlan()) == NULL) {
		WLAN_LOG_ERROR("%s: Memory allocation failure.\n", dev_info);
		result = -EIO;
		goto failed;
	}
	hw = wlandev->priv;

	if ( wlan_setup(wlandev) != 0 ) {
		WLAN_LOG_ERROR("%s: wlan_setup() failed.\n", dev_info);
		result = -EIO;
		goto failed;
	}

	/* Initialize the hw data */
	hfa384x_create(hw, dev);
	hw->wlandev = wlandev;

	/* Register the wlandev, this gets us a name and registers the
	 * linux netdevice.
	 */
	SET_NETDEV_DEV(wlandev->netdev, &(interface->dev));

	/* Do a chip-level reset on the MAC */
	if (prism2_doreset) {
		result = hfa384x_corereset(hw,
				prism2_reset_holdtime,
				prism2_reset_settletime, 0);
		if (result != 0) {
			unregister_wlandev(wlandev);
			hfa384x_destroy(hw);
			result = -EIO;
			WLAN_LOG_ERROR(
				"%s: hfa384x_corereset() failed.\n",
				dev_info);
			goto failed;
		}
	}

	usb_get_dev(dev);

	wlandev->msdstate = WLAN_MSD_HWPRESENT;

        if ( register_wlandev(wlandev) != 0 ) {
		WLAN_LOG_ERROR("%s: register_wlandev() failed.\n", dev_info);
		result = -EIO;
		goto failed;
        }

/* enable the card */
	prism2sta_ifstate(wlandev, P80211ENUM_ifstate_enable);

	goto done;

 failed:
	if (wlandev)	kfree(wlandev);
	if (hw)		kfree(hw);
	wlandev = NULL;

 done:
	DBFEXIT;

	usb_set_intfdata(interface, wlandev);
	return result;
}


/*----------------------------------------------------------------
* prism2sta_disconnect_usb
*
* Called when a device previously claimed by probe is removed
* from the USB.
*
* Arguments:
*	dev		ptr to the usb_device struct
*	ptr		ptr returned by probe() when the device
*                       was claimed.
*
* Returns:
*	Nothing
*
* Side effects:
*
* Call context:
*	process
----------------------------------------------------------------*/
static void
prism2sta_disconnect_usb(struct usb_interface *interface)
{
	wlandevice_t		*wlandev;

        DBFENTER;

	wlandev = (wlandevice_t *) usb_get_intfdata(interface);

	if ( wlandev != NULL ) {
		LIST_HEAD(cleanlist);
		struct list_head	*entry;
		struct list_head	*temp;
		unsigned long		flags;

		hfa384x_t		*hw = wlandev->priv;

		if (!hw)
			goto exit;

		spin_lock_irqsave(&hw->ctlxq.lock, flags);

		p80211netdev_hwremoved(wlandev);
		list_splice_init(&hw->ctlxq.reapable, &cleanlist);
		list_splice_init(&hw->ctlxq.completing, &cleanlist);
		list_splice_init(&hw->ctlxq.pending, &cleanlist);
		list_splice_init(&hw->ctlxq.active, &cleanlist);

		spin_unlock_irqrestore(&hw->ctlxq.lock, flags);

		/* There's no hardware to shutdown, but the driver
		 * might have some tasks or tasklets that must be
		 * stopped before we can tear everything down.
		 */
		prism2sta_ifstate(wlandev, P80211ENUM_ifstate_disable);

		del_singleshot_timer_sync(&hw->throttle);
		del_singleshot_timer_sync(&hw->reqtimer);
		del_singleshot_timer_sync(&hw->resptimer);

		/* Unlink all the URBs. This "removes the wheels"
		 * from the entire CTLX handling mechanism.
		 */
		usb_kill_urb(&hw->rx_urb);
		usb_kill_urb(&hw->tx_urb);
		usb_kill_urb(&hw->ctlx_urb);

		tasklet_kill(&hw->completion_bh);
		tasklet_kill(&hw->reaper_bh);

		flush_scheduled_work();

		/* Now we complete any outstanding commands
		 * and tell everyone who is waiting for their
		 * responses that we have shut down.
		 */
		list_for_each(entry, &cleanlist) {
			hfa384x_usbctlx_t	*ctlx;

			ctlx = list_entry(entry, hfa384x_usbctlx_t, list);
			complete(&ctlx->done);
		}

		/* Give any outstanding synchronous commands
		 * a chance to complete. All they need to do
		 * is "wake up", so that's easy.
		 * (I'd like a better way to do this, really.)
		 */
		msleep(100);

		/* Now delete the CTLXs, because no-one else can now. */
		list_for_each_safe(entry, temp, &cleanlist) {
			hfa384x_usbctlx_t *ctlx;

			ctlx = list_entry(entry, hfa384x_usbctlx_t, list);
			kfree(ctlx);
		}

		/* Unhook the wlandev */
		unregister_wlandev(wlandev);
		wlan_unsetup(wlandev);

		usb_put_dev(hw->usb);

		hfa384x_destroy(hw);
		kfree(hw);

		kfree(wlandev);
	}

 exit:

	usb_set_intfdata(interface, NULL);
	DBFEXIT;
}


static struct usb_driver prism2_usb_driver = {
	.name = "prism2_usb",
	.probe = prism2sta_probe_usb,
	.disconnect = prism2sta_disconnect_usb,
	.id_table = usb_prism_tbl,
	/* fops, minor? */
};

static int __init prism2usb_init(void)
{
        DBFENTER;

	/* This call will result in calls to prism2sta_probe_usb. */
	return usb_register(&prism2_usb_driver);

	DBFEXIT;
};

static void __exit prism2usb_cleanup(void)
{
        DBFENTER;

	usb_deregister(&prism2_usb_driver);

	DBFEXIT;
};

module_init(prism2usb_init);
module_exit(prism2usb_cleanup);
