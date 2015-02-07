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

#ifndef LINUX_SSB_DRIVER_GIGE_H_
#define LINUX_SSB_DRIVER_GIGE_H_

#include <linux/ssb/ssb.h>
#include <linux/pci.h>
#include <linux/spinlock.h>


#ifdef CONFIG_SSB_DRIVER_GIGE


#define SSB_GIGE_PCIIO			0x0000 /* PCI I/O Registers (1024 bytes) */
#define SSB_GIGE_RESERVED		0x0400 /* Reserved (1024 bytes) */
#define SSB_GIGE_PCICFG			0x0800 /* PCI config space (256 bytes) */
#define SSB_GIGE_SHIM_FLUSHSTAT		0x0C00 /* PCI to OCP: Flush status control (32bit) */
#define SSB_GIGE_SHIM_FLUSHRDA		0x0C04 /* PCI to OCP: Flush read address (32bit) */
#define SSB_GIGE_SHIM_FLUSHTO		0x0C08 /* PCI to OCP: Flush timeout counter (32bit) */
#define SSB_GIGE_SHIM_BARRIER		0x0C0C /* PCI to OCP: Barrier register (32bit) */
#define SSB_GIGE_SHIM_MAOCPSI		0x0C10 /* PCI to OCP: MaocpSI Control (32bit) */
#define SSB_GIGE_SHIM_SIOCPMA		0x0C14 /* PCI to OCP: SiocpMa Control (32bit) */

/* TM Status High flags */
#define SSB_GIGE_TMSHIGH_RGMII		0x00010000 /* Have an RGMII PHY-bus */
/* TM Status Low flags */
#define SSB_GIGE_TMSLOW_TXBYPASS	0x00080000 /* TX bypass (no delay) */
#define SSB_GIGE_TMSLOW_RXBYPASS	0x00100000 /* RX bypass (no delay) */
#define SSB_GIGE_TMSLOW_DLLEN		0x01000000 /* Enable DLL controls */

/* Boardflags (low) */
#define SSB_GIGE_BFL_ROBOSWITCH		0x0010


#define SSB_GIGE_MEM_RES_NAME		"SSB Broadcom 47xx GigE memory"
#define SSB_GIGE_IO_RES_NAME		"SSB Broadcom 47xx GigE I/O"

struct ssb_gige {
	struct ssb_device *dev;

	spinlock_t lock;

	/* True, if the device has an RGMII bus.
	 * False, if the device has a GMII bus. */
	bool has_rgmii;

	/* The PCI controller device. */
	struct pci_controller pci_controller;
	struct pci_ops pci_ops;
	struct resource mem_resource;
	struct resource io_resource;
};

/* Check whether a PCI device is a SSB Gigabit Ethernet core. */
extern bool pdev_is_ssb_gige_core(struct pci_dev *pdev);

/* Convert a pci_dev pointer to a ssb_gige pointer. */
static inline struct ssb_gige * pdev_to_ssb_gige(struct pci_dev *pdev)
{
	if (!pdev_is_ssb_gige_core(pdev))
		return NULL;
	return container_of(pdev->bus->ops, struct ssb_gige, pci_ops);
}

/* Returns whether the PHY is connected by an RGMII bus. */
static inline bool ssb_gige_is_rgmii(struct pci_dev *pdev)
{
	struct ssb_gige *dev = pdev_to_ssb_gige(pdev);
	return (dev ? dev->has_rgmii : 0);
}

/* Returns whether we have a Roboswitch. */
static inline bool ssb_gige_have_roboswitch(struct pci_dev *pdev)
{
	struct ssb_gige *dev = pdev_to_ssb_gige(pdev);
	if (dev)
		return !!(dev->dev->bus->sprom.boardflags_lo &
			  SSB_GIGE_BFL_ROBOSWITCH);
	return 0;
}

/* Returns whether we can only do one DMA at once. */
static inline bool ssb_gige_one_dma_at_once(struct pci_dev *pdev)
{
	struct ssb_gige *dev = pdev_to_ssb_gige(pdev);
	if (dev)
		return ((dev->dev->bus->chip_id == 0x4785) &&
			(dev->dev->bus->chip_rev < 2));
	return 0;
}

/* Returns whether we must flush posted writes. */
static inline bool ssb_gige_must_flush_posted_writes(struct pci_dev *pdev)
{
	struct ssb_gige *dev = pdev_to_ssb_gige(pdev);
	if (dev)
		return (dev->dev->bus->chip_id == 0x4785);
	return 0;
}

extern char * nvram_get(const char *name);
/* Get the device MAC address */
static inline void ssb_gige_get_macaddr(struct pci_dev *pdev, u8 *macaddr)
{
#ifdef CONFIG_BCM47XX
	char *res = nvram_get("et0macaddr");
	if (res)
		memcpy(macaddr, res, 6);
#endif
}

extern int ssb_gige_pcibios_plat_dev_init(struct ssb_device *sdev,
					  struct pci_dev *pdev);
extern int ssb_gige_map_irq(struct ssb_device *sdev,
			    const struct pci_dev *pdev);

/* The GigE driver is not a standalone module, because we don't have support
 * for unregistering the driver. So we could not unload the module anyway. */
extern int ssb_gige_init(void);
static inline void ssb_gige_exit(void)
{
	/* Currently we can not unregister the GigE driver,
	 * because we can not unregister the PCI bridge. */
	BUG();
}


#else /* CONFIG_SSB_DRIVER_GIGE */
/* Gigabit Ethernet driver disabled */


static inline int ssb_gige_pcibios_plat_dev_init(struct ssb_device *sdev,
						 struct pci_dev *pdev)
{
	return -ENOSYS;
}
static inline int ssb_gige_map_irq(struct ssb_device *sdev,
				   const struct pci_dev *pdev)
{
	return -ENOSYS;
}
static inline int ssb_gige_init(void)
{
	return 0;
}
static inline void ssb_gige_exit(void)
{
}

static inline bool pdev_is_ssb_gige_core(struct pci_dev *pdev)
{
	return 0;
}
static inline struct ssb_gige * pdev_to_ssb_gige(struct pci_dev *pdev)
{
	return NULL;
}
static inline bool ssb_gige_is_rgmii(struct pci_dev *pdev)
{
	return 0;
}
static inline bool ssb_gige_have_roboswitch(struct pci_dev *pdev)
{
	return 0;
}
static inline bool ssb_gige_one_dma_at_once(struct pci_dev *pdev)
{
	return 0;
}
static inline bool ssb_gige_must_flush_posted_writes(struct pci_dev *pdev)
{
	return 0;
}

#endif /* CONFIG_SSB_DRIVER_GIGE */
#endif /* LINUX_SSB_DRIVER_GIGE_H_ */