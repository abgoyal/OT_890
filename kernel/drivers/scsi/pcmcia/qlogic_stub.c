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

/*======================================================================

    A driver for the Qlogic SCSI card

    qlogic_cs.c 1.79 2000/06/12 21:27:26

    The contents of this file are subject to the Mozilla Public
    License Version 1.1 (the "License"); you may not use this file
    except in compliance with the License. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/

    Software distributed under the License is distributed on an "AS
    IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
    implied. See the License for the specific language governing
    rights and limitations under the License.

    The initial developer of the original code is David A. Hinds
    <dahinds@users.sourceforge.net>.  Portions created by David A. Hinds
    are Copyright (C) 1999 David A. Hinds.  All Rights Reserved.

    Alternatively, the contents of this file may be used under the
    terms of the GNU General Public License version 2 (the "GPL"), in which
    case the provisions of the GPL are applicable instead of the
    above.  If you wish to allow the use of your version of this file
    only under the terms of the GPL and not to allow others to use
    your version of this file under the MPL, indicate your decision
    by deleting the provisions above and replace them with the notice
    and other provisions required by the GPL.  If you do not delete
    the provisions above, a recipient may use your version of this
    file under either the MPL or the GPL.
    
======================================================================*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <scsi/scsi.h>
#include <linux/major.h>
#include <linux/blkdev.h>
#include <scsi/scsi_ioctl.h>
#include <linux/interrupt.h>

#include "scsi.h"
#include <scsi/scsi_host.h>
#include "../qlogicfas408.h"

#include <pcmcia/cs_types.h>
#include <pcmcia/cs.h>
#include <pcmcia/cistpl.h>
#include <pcmcia/ds.h>
#include <pcmcia/ciscode.h>

/* Set the following to 2 to use normal interrupt (active high/totempole-
 * tristate), otherwise use 0 (REQUIRED FOR PCMCIA) for active low, open
 * drain
 */
#define INT_TYPE	0

static char qlogic_name[] = "qlogic_cs";

#ifdef PCMCIA_DEBUG
static int pc_debug = PCMCIA_DEBUG;
module_param(pc_debug, int, 0644);
#define DEBUG(n, args...) if (pc_debug>(n)) printk(KERN_DEBUG args)
static char *version = "qlogic_cs.c 1.79-ac 2002/10/26 (David Hinds)";
#else
#define DEBUG(n, args...)
#endif

static struct scsi_host_template qlogicfas_driver_template = {
	.module			= THIS_MODULE,
	.name			= qlogic_name,
	.proc_name		= qlogic_name,
	.info			= qlogicfas408_info,
	.queuecommand		= qlogicfas408_queuecommand,
	.eh_abort_handler	= qlogicfas408_abort,
	.eh_bus_reset_handler	= qlogicfas408_bus_reset,
	.bios_param		= qlogicfas408_biosparam,
	.can_queue		= 1,
	.this_id		= -1,
	.sg_tablesize		= SG_ALL,
	.cmd_per_lun		= 1,
	.use_clustering		= DISABLE_CLUSTERING,
};

/*====================================================================*/

typedef struct scsi_info_t {
	struct pcmcia_device	*p_dev;
	dev_node_t node;
	struct Scsi_Host *host;
	unsigned short manf_id;
} scsi_info_t;

static void qlogic_release(struct pcmcia_device *link);
static void qlogic_detach(struct pcmcia_device *p_dev);
static int qlogic_config(struct pcmcia_device * link);

static struct Scsi_Host *qlogic_detect(struct scsi_host_template *host,
				struct pcmcia_device *link, int qbase, int qlirq)
{
	int qltyp;		/* type of chip */
	int qinitid;
	struct Scsi_Host *shost;	/* registered host structure */
	struct qlogicfas408_priv *priv;

	qltyp = qlogicfas408_get_chip_type(qbase, INT_TYPE);
	qinitid = host->this_id;
	if (qinitid < 0)
		qinitid = 7;	/* if no ID, use 7 */

	qlogicfas408_setup(qbase, qinitid, INT_TYPE);

	host->name = qlogic_name;
	shost = scsi_host_alloc(host, sizeof(struct qlogicfas408_priv));
	if (!shost)
		goto err;
	shost->io_port = qbase;
	shost->n_io_port = 16;
	shost->dma_channel = -1;
	if (qlirq != -1)
		shost->irq = qlirq;

	priv = get_priv_by_host(shost);
	priv->qlirq = qlirq;
	priv->qbase = qbase;
	priv->qinitid = qinitid;
	priv->shost = shost;
	priv->int_type = INT_TYPE;					

	if (request_irq(qlirq, qlogicfas408_ihandl, 0, qlogic_name, shost))
		goto free_scsi_host;

	sprintf(priv->qinfo,
		"Qlogicfas Driver version 0.46, chip %02X at %03X, IRQ %d, TPdma:%d",
		qltyp, qbase, qlirq, QL_TURBO_PDMA);

	if (scsi_add_host(shost, NULL))
		goto free_interrupt;

	scsi_scan_host(shost);

	return shost;

free_interrupt:
	free_irq(qlirq, shost);

free_scsi_host:
	scsi_host_put(shost);
	
err:
	return NULL;
}
static int qlogic_probe(struct pcmcia_device *link)
{
	scsi_info_t *info;

	DEBUG(0, "qlogic_attach()\n");

	/* Create new SCSI device */
	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;
	info->p_dev = link;
	link->priv = info;
	link->io.NumPorts1 = 16;
	link->io.Attributes1 = IO_DATA_PATH_WIDTH_AUTO;
	link->io.IOAddrLines = 10;
	link->irq.Attributes = IRQ_TYPE_EXCLUSIVE;
	link->irq.IRQInfo1 = IRQ_LEVEL_ID;
	link->conf.Attributes = CONF_ENABLE_IRQ;
	link->conf.IntType = INT_MEMORY_AND_IO;
	link->conf.Present = PRESENT_OPTION;

	return qlogic_config(link);
}				/* qlogic_attach */

/*====================================================================*/

static void qlogic_detach(struct pcmcia_device *link)
{
	DEBUG(0, "qlogic_detach(0x%p)\n", link);

	qlogic_release(link);
	kfree(link->priv);

}				/* qlogic_detach */

/*====================================================================*/

#define CS_CHECK(fn, ret) \
do { last_fn = (fn); if ((last_ret = (ret)) != 0) goto cs_failed; } while (0)

static int qlogic_config_check(struct pcmcia_device *p_dev,
			       cistpl_cftable_entry_t *cfg,
			       cistpl_cftable_entry_t *dflt,
			       unsigned int vcc,
			       void *priv_data)
{
	p_dev->io.BasePort1 = cfg->io.win[0].base;
	p_dev->io.NumPorts1 = cfg->io.win[0].len;

	if (p_dev->io.BasePort1 == 0)
		return -ENODEV;

	return pcmcia_request_io(p_dev, &p_dev->io);
}

static int qlogic_config(struct pcmcia_device * link)
{
	scsi_info_t *info = link->priv;
	int last_ret, last_fn;
	struct Scsi_Host *host;

	DEBUG(0, "qlogic_config(0x%p)\n", link);

	last_ret = pcmcia_loop_config(link, qlogic_config_check, NULL);
	if (last_ret) {
		cs_error(link, RequestIO, last_ret);
		goto failed;
	}

	CS_CHECK(RequestIRQ, pcmcia_request_irq(link, &link->irq));
	CS_CHECK(RequestConfiguration, pcmcia_request_configuration(link, &link->conf));

	if ((info->manf_id == MANFID_MACNICA) || (info->manf_id == MANFID_PIONEER) || (info->manf_id == 0x0098)) {
		/* set ATAcmd */
		outb(0xb4, link->io.BasePort1 + 0xd);
		outb(0x24, link->io.BasePort1 + 0x9);
		outb(0x04, link->io.BasePort1 + 0xd);
	}

	/* The KXL-810AN has a bigger IO port window */
	if (link->io.NumPorts1 == 32)
		host = qlogic_detect(&qlogicfas_driver_template, link,
			link->io.BasePort1 + 16, link->irq.AssignedIRQ);
	else
		host = qlogic_detect(&qlogicfas_driver_template, link,
			link->io.BasePort1, link->irq.AssignedIRQ);
	
	if (!host) {
		printk(KERN_INFO "%s: no SCSI devices found\n", qlogic_name);
		goto cs_failed;
	}

	sprintf(info->node.dev_name, "scsi%d", host->host_no);
	link->dev_node = &info->node;
	info->host = host;

	return 0;

cs_failed:
	cs_error(link, last_fn, last_ret);
	pcmcia_disable_device(link);
failed:
	return -ENODEV;

}				/* qlogic_config */

/*====================================================================*/

static void qlogic_release(struct pcmcia_device *link)
{
	scsi_info_t *info = link->priv;

	DEBUG(0, "qlogic_release(0x%p)\n", link);

	scsi_remove_host(info->host);

	free_irq(link->irq.AssignedIRQ, info->host);
	pcmcia_disable_device(link);

	scsi_host_put(info->host);
}

/*====================================================================*/

static int qlogic_resume(struct pcmcia_device *link)
{
	scsi_info_t *info = link->priv;

	pcmcia_request_configuration(link, &link->conf);
	if ((info->manf_id == MANFID_MACNICA) ||
	    (info->manf_id == MANFID_PIONEER) ||
	    (info->manf_id == 0x0098)) {
		outb(0x80, link->io.BasePort1 + 0xd);
		outb(0x24, link->io.BasePort1 + 0x9);
		outb(0x04, link->io.BasePort1 + 0xd);
	}
	/* Ugggglllyyyy!!! */
	qlogicfas408_bus_reset(NULL);

	return 0;
}

static struct pcmcia_device_id qlogic_ids[] = {
	PCMCIA_DEVICE_PROD_ID12("EIger Labs", "PCMCIA-to-SCSI Adapter", 0x88395fa7, 0x33b7a5e6),
	PCMCIA_DEVICE_PROD_ID12("EPSON", "SCSI-2 PC Card SC200", 0xd361772f, 0x299d1751),
	PCMCIA_DEVICE_PROD_ID12("MACNICA", "MIRACLE SCSI-II mPS110", 0x20841b68, 0xab3c3b6d),
	PCMCIA_DEVICE_PROD_ID12("MIDORI ELECTRONICS ", "CN-SC43", 0x6534382a, 0xd67eee79),
	PCMCIA_DEVICE_PROD_ID12("NEC", "PC-9801N-J03R", 0x18df0ba0, 0x24662e8a),
	PCMCIA_DEVICE_PROD_ID12("KME ", "KXLC003", 0x82375a27, 0xf68e5bf7),
	PCMCIA_DEVICE_PROD_ID12("KME ", "KXLC004", 0x82375a27, 0x68eace54),
	PCMCIA_DEVICE_PROD_ID12("KME", "KXLC101", 0x3faee676, 0x194250ec),
	PCMCIA_DEVICE_PROD_ID12("QLOGIC CORPORATION", "pc05", 0xd77b2930, 0xa85b2735),
	PCMCIA_DEVICE_PROD_ID12("QLOGIC CORPORATION", "pc05 rev 1.10", 0xd77b2930, 0x70f8b5f8),
	PCMCIA_DEVICE_PROD_ID123("KME", "KXLC002", "00", 0x3faee676, 0x81896b61, 0xf99f065f),
	PCMCIA_DEVICE_PROD_ID12("RATOC System Inc.", "SCSI2 CARD 37", 0x85c10e17, 0x1a2640c1),
	PCMCIA_DEVICE_PROD_ID12("TOSHIBA", "SCSC200A PC CARD SCSI", 0xb4585a1a, 0xa6f06ebe),
	PCMCIA_DEVICE_PROD_ID12("TOSHIBA", "SCSC200B PC CARD SCSI-10", 0xb4585a1a, 0x0a88dea0),
	/* these conflict with other cards! */
	/* PCMCIA_DEVICE_PROD_ID123("MACNICA", "MIRACLE SCSI", "mPS100", 0x20841b68, 0xf8dedaeb, 0x89f7fafb), */
	/* PCMCIA_DEVICE_PROD_ID123("MACNICA", "MIRACLE SCSI", "mPS100", 0x20841b68, 0xf8dedaeb, 0x89f7fafb), */
	PCMCIA_DEVICE_NULL,
};
MODULE_DEVICE_TABLE(pcmcia, qlogic_ids);

static struct pcmcia_driver qlogic_cs_driver = {
	.owner		= THIS_MODULE,
	.drv		= {
	.name		= "qlogic_cs",
	},
	.probe		= qlogic_probe,
	.remove		= qlogic_detach,
	.id_table       = qlogic_ids,
	.resume		= qlogic_resume,
};

static int __init init_qlogic_cs(void)
{
	return pcmcia_register_driver(&qlogic_cs_driver);
}

static void __exit exit_qlogic_cs(void)
{
	pcmcia_unregister_driver(&qlogic_cs_driver);
}

MODULE_AUTHOR("Tom Zerucha, Michael Griffith");
MODULE_DESCRIPTION("Driver for the PCMCIA Qlogic FAS SCSI controllers");
MODULE_LICENSE("GPL");
module_init(init_qlogic_cs);
module_exit(exit_qlogic_cs);
