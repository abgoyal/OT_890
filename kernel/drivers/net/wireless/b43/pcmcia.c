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

  Broadcom B43 wireless driver

  Copyright (c) 2007 Michael Buesch <mb@bu3sch.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
  Boston, MA 02110-1301, USA.

*/

#include "pcmcia.h"

#include <linux/ssb/ssb.h>

#include <pcmcia/cs_types.h>
#include <pcmcia/cs.h>
#include <pcmcia/cistpl.h>
#include <pcmcia/ciscode.h>
#include <pcmcia/ds.h>
#include <pcmcia/cisreg.h>


static /*const */ struct pcmcia_device_id b43_pcmcia_tbl[] = {
	PCMCIA_DEVICE_MANF_CARD(0x2D0, 0x448),
	PCMCIA_DEVICE_NULL,
};

MODULE_DEVICE_TABLE(pcmcia, b43_pcmcia_tbl);

#ifdef CONFIG_PM
static int b43_pcmcia_suspend(struct pcmcia_device *dev)
{
	struct ssb_bus *ssb = dev->priv;

	return ssb_bus_suspend(ssb);
}

static int b43_pcmcia_resume(struct pcmcia_device *dev)
{
	struct ssb_bus *ssb = dev->priv;

	return ssb_bus_resume(ssb);
}
#else /* CONFIG_PM */
# define b43_pcmcia_suspend		NULL
# define b43_pcmcia_resume		NULL
#endif /* CONFIG_PM */

static int __devinit b43_pcmcia_probe(struct pcmcia_device *dev)
{
	struct ssb_bus *ssb;
	win_req_t win;
	memreq_t mem;
	tuple_t tuple;
	cisparse_t parse;
	int err = -ENOMEM;
	int res = 0;
	unsigned char buf[64];

	ssb = kzalloc(sizeof(*ssb), GFP_KERNEL);
	if (!ssb)
		goto out_error;

	err = -ENODEV;
	tuple.DesiredTuple = CISTPL_CONFIG;
	tuple.Attributes = 0;
	tuple.TupleData = buf;
	tuple.TupleDataMax = sizeof(buf);
	tuple.TupleOffset = 0;

	res = pcmcia_get_first_tuple(dev, &tuple);
	if (res != 0)
		goto err_kfree_ssb;
	res = pcmcia_get_tuple_data(dev, &tuple);
	if (res != 0)
		goto err_kfree_ssb;
	res = pcmcia_parse_tuple(&tuple, &parse);
	if (res != 0)
		goto err_kfree_ssb;

	dev->conf.ConfigBase = parse.config.base;
	dev->conf.Present = parse.config.rmask[0];
	dev->conf.Attributes = CONF_ENABLE_IRQ;
	dev->conf.IntType = INT_MEMORY_AND_IO;

	dev->io.BasePort2 = 0;
	dev->io.NumPorts2 = 0;
	dev->io.Attributes2 = 0;

	win.Attributes = WIN_ADDR_SPACE_MEM | WIN_MEMORY_TYPE_CM |
			 WIN_ENABLE | WIN_DATA_WIDTH_16 |
			 WIN_USE_WAIT;
	win.Base = 0;
	win.Size = SSB_CORE_SIZE;
	win.AccessSpeed = 250;
	res = pcmcia_request_window(&dev, &win, &dev->win);
	if (res != 0)
		goto err_kfree_ssb;

	mem.CardOffset = 0;
	mem.Page = 0;
	res = pcmcia_map_mem_page(dev->win, &mem);
	if (res != 0)
		goto err_disable;

	dev->irq.Attributes = IRQ_TYPE_DYNAMIC_SHARING;
	dev->irq.IRQInfo1 = IRQ_LEVEL_ID;
	dev->irq.Handler = NULL; /* The handler is registered later. */
	dev->irq.Instance = NULL;
	res = pcmcia_request_irq(dev, &dev->irq);
	if (res != 0)
		goto err_disable;

	res = pcmcia_request_configuration(dev, &dev->conf);
	if (res != 0)
		goto err_disable;

	err = ssb_bus_pcmciabus_register(ssb, dev, win.Base);
	if (err)
		goto err_disable;
	dev->priv = ssb;

	return 0;

err_disable:
	pcmcia_disable_device(dev);
err_kfree_ssb:
	kfree(ssb);
out_error:
	printk(KERN_ERR "b43-pcmcia: Initialization failed (%d, %d)\n",
	       res, err);
	return err;
}

static void __devexit b43_pcmcia_remove(struct pcmcia_device *dev)
{
	struct ssb_bus *ssb = dev->priv;

	ssb_bus_unregister(ssb);
	pcmcia_disable_device(dev);
	kfree(ssb);
	dev->priv = NULL;
}

static struct pcmcia_driver b43_pcmcia_driver = {
	.owner		= THIS_MODULE,
	.drv		= {
				.name = "b43-pcmcia",
			},
	.id_table	= b43_pcmcia_tbl,
	.probe		= b43_pcmcia_probe,
	.remove		= __devexit_p(b43_pcmcia_remove),
	.suspend	= b43_pcmcia_suspend,
	.resume		= b43_pcmcia_resume,
};

int b43_pcmcia_init(void)
{
	return pcmcia_register_driver(&b43_pcmcia_driver);
}

void b43_pcmcia_exit(void)
{
	pcmcia_unregister_driver(&b43_pcmcia_driver);
}
