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
 * CS5536 PATA support
 * (C) 2007 Martin K. Petersen <mkp@mkp.net>
 * (C) 2009 Bartlomiej Zolnierkiewicz
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Documentation:
 *	Available from AMD web site.
 *
 * The IDE timing registers for the CS5536 live in the Geode Machine
 * Specific Register file and not PCI config space.  Most BIOSes
 * virtualize the PCI registers so the chip looks like a standard IDE
 * controller.  Unfortunately not all implementations get this right.
 * In particular some have problems with unaligned accesses to the
 * virtualized PCI registers.  This driver always does full dword
 * writes to work around the issue.  Also, in case of a bad BIOS this
 * driver can be loaded with the "msr=1" parameter which forces using
 * the Machine Specific Registers to configure the device.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/ide.h>
#include <asm/msr.h>

#define DRV_NAME	"cs5536"

enum {
	MSR_IDE_CFG		= 0x51300010,
	PCI_IDE_CFG		= 0x40,

	CFG			= 0,
	DTC			= 2,
	CAST			= 3,
	ETC			= 4,

	IDE_CFG_CHANEN		= (1 << 1),
	IDE_CFG_CABLE		= (1 << 17) | (1 << 16),

	IDE_D0_SHIFT		= 24,
	IDE_D1_SHIFT		= 16,
	IDE_DRV_MASK		= 0xff,

	IDE_CAST_D0_SHIFT	= 6,
	IDE_CAST_D1_SHIFT	= 4,
	IDE_CAST_DRV_MASK	= 0x3,

	IDE_CAST_CMD_SHIFT	= 24,
	IDE_CAST_CMD_MASK	= 0xff,

	IDE_ETC_UDMA_MASK	= 0xc0,
};

static int use_msr;

static int cs5536_read(struct pci_dev *pdev, int reg, u32 *val)
{
	if (unlikely(use_msr)) {
		u32 dummy;

		rdmsr(MSR_IDE_CFG + reg, *val, dummy);
		return 0;
	}

	return pci_read_config_dword(pdev, PCI_IDE_CFG + reg * 4, val);
}

static int cs5536_write(struct pci_dev *pdev, int reg, int val)
{
	if (unlikely(use_msr)) {
		wrmsr(MSR_IDE_CFG + reg, val, 0);
		return 0;
	}

	return pci_write_config_dword(pdev, PCI_IDE_CFG + reg * 4, val);
}

static void cs5536_program_dtc(ide_drive_t *drive, u8 tim)
{
	struct pci_dev *pdev = to_pci_dev(drive->hwif->dev);
	int dshift = (drive->dn & 1) ? IDE_D1_SHIFT : IDE_D0_SHIFT;
	u32 dtc;

	cs5536_read(pdev, DTC, &dtc);
	dtc &= ~(IDE_DRV_MASK << dshift);
	dtc |= tim << dshift;
	cs5536_write(pdev, DTC, dtc);
}

/**
 *	cs5536_cable_detect	-	detect cable type
 *	@hwif: Port to detect on
 *
 *	Perform cable detection for ATA66 capable cable.
 *
 *	Returns a cable type.
 */

static u8 cs5536_cable_detect(ide_hwif_t *hwif)
{
	struct pci_dev *pdev = to_pci_dev(hwif->dev);
	u32 cfg;

	cs5536_read(pdev, CFG, &cfg);

	if (cfg & IDE_CFG_CABLE)
		return ATA_CBL_PATA80;
	else
		return ATA_CBL_PATA40;
}

/**
 *	cs5536_set_pio_mode		-	PIO timing setup
 *	@drive: ATA device
 *	@pio: PIO mode number
 */

static void cs5536_set_pio_mode(ide_drive_t *drive, const u8 pio)
{
	static const u8 drv_timings[5] = {
		0x98, 0x55, 0x32, 0x21, 0x20,
	};

	static const u8 addr_timings[5] = {
		0x2, 0x1, 0x0, 0x0, 0x0,
	};

	static const u8 cmd_timings[5] = {
		0x99, 0x92, 0x90, 0x22, 0x20,
	};

	struct pci_dev *pdev = to_pci_dev(drive->hwif->dev);
	ide_drive_t *pair = ide_get_pair_dev(drive);
	int cshift = (drive->dn & 1) ? IDE_CAST_D1_SHIFT : IDE_CAST_D0_SHIFT;
	u32 cast;
	u8 cmd_pio = pio;

	if (pair)
		cmd_pio = min(pio, ide_get_best_pio_mode(pair, 255, 4));

	drive->drive_data &= (IDE_DRV_MASK << 8);
	drive->drive_data |= drv_timings[pio];

	cs5536_program_dtc(drive, drv_timings[pio]);

	cs5536_read(pdev, CAST, &cast);

	cast &= ~(IDE_CAST_DRV_MASK << cshift);
	cast |= addr_timings[pio] << cshift;

	cast &= ~(IDE_CAST_CMD_MASK << IDE_CAST_CMD_SHIFT);
	cast |= cmd_timings[cmd_pio] << IDE_CAST_CMD_SHIFT;

	cs5536_write(pdev, CAST, cast);
}

/**
 *	cs5536_set_dma_mode		-	DMA timing setup
 *	@drive: ATA device
 *	@mode: DMA mode
 */

static void cs5536_set_dma_mode(ide_drive_t *drive, const u8 mode)
{
	static const u8 udma_timings[6] = {
		0xc2, 0xc1, 0xc0, 0xc4, 0xc5, 0xc6,
	};

	static const u8 mwdma_timings[3] = {
		0x67, 0x21, 0x20,
	};

	struct pci_dev *pdev = to_pci_dev(drive->hwif->dev);
	int dshift = (drive->dn & 1) ? IDE_D1_SHIFT : IDE_D0_SHIFT;
	u32 etc;

	cs5536_read(pdev, ETC, &etc);

	if (mode >= XFER_UDMA_0) {
		etc &= ~(IDE_DRV_MASK << dshift);
		etc |= udma_timings[mode - XFER_UDMA_0] << dshift;
	} else { /* MWDMA */
		etc &= ~(IDE_ETC_UDMA_MASK << dshift);
		drive->drive_data &= IDE_DRV_MASK;
		drive->drive_data |= mwdma_timings[mode - XFER_MW_DMA_0] << 8;
	}

	cs5536_write(pdev, ETC, etc);
}

static void cs5536_dma_start(ide_drive_t *drive)
{
	if (drive->current_speed < XFER_UDMA_0 &&
	    (drive->drive_data >> 8) != (drive->drive_data & IDE_DRV_MASK))
		cs5536_program_dtc(drive, drive->drive_data >> 8);

	ide_dma_start(drive);
}

static int cs5536_dma_end(ide_drive_t *drive)
{
	int ret = ide_dma_end(drive);

	if (drive->current_speed < XFER_UDMA_0 &&
	    (drive->drive_data >> 8) != (drive->drive_data & IDE_DRV_MASK))
		cs5536_program_dtc(drive, drive->drive_data & IDE_DRV_MASK);

	return ret;
}

static const struct ide_port_ops cs5536_port_ops = {
	.set_pio_mode		= cs5536_set_pio_mode,
	.set_dma_mode		= cs5536_set_dma_mode,
	.cable_detect		= cs5536_cable_detect,
};

static const struct ide_dma_ops cs5536_dma_ops = {
	.dma_host_set		= ide_dma_host_set,
	.dma_setup		= ide_dma_setup,
	.dma_exec_cmd		= ide_dma_exec_cmd,
	.dma_start		= cs5536_dma_start,
	.dma_end		= cs5536_dma_end,
	.dma_test_irq		= ide_dma_test_irq,
	.dma_lost_irq		= ide_dma_lost_irq,
	.dma_timeout		= ide_dma_timeout,
};

static const struct ide_port_info cs5536_info = {
	.name		= DRV_NAME,
	.port_ops	= &cs5536_port_ops,
	.dma_ops	= &cs5536_dma_ops,
	.host_flags	= IDE_HFLAG_SINGLE,
	.pio_mask	= ATA_PIO4,
	.mwdma_mask	= ATA_MWDMA2,
	.udma_mask	= ATA_UDMA5,
};

/**
 *	cs5536_init_one
 *	@dev: PCI device
 *	@id: Entry in match table
 */

static int cs5536_init_one(struct pci_dev *dev, const struct pci_device_id *id)
{
	u32 cfg;

	if (use_msr)
		printk(KERN_INFO DRV_NAME ": Using MSR regs instead of PCI\n");

	cs5536_read(dev, CFG, &cfg);

	if ((cfg & IDE_CFG_CHANEN) == 0) {
		printk(KERN_ERR DRV_NAME ": disabled by BIOS\n");
		return -ENODEV;
	}

	return ide_pci_init_one(dev, &cs5536_info, NULL);
}

static const struct pci_device_id cs5536_pci_tbl[] = {
	{ PCI_VDEVICE(AMD, PCI_DEVICE_ID_AMD_CS5536_IDE), },
	{ },
};

static struct pci_driver cs5536_pci_driver = {
	.name		= DRV_NAME,
	.id_table	= cs5536_pci_tbl,
	.probe		= cs5536_init_one,
	.remove		= ide_pci_remove,
	.suspend	= ide_pci_suspend,
	.resume		= ide_pci_resume,
};

static int __init cs5536_init(void)
{
	return pci_register_driver(&cs5536_pci_driver);
}

static void __exit cs5536_exit(void)
{
	pci_unregister_driver(&cs5536_pci_driver);
}

MODULE_AUTHOR("Martin K. Petersen, Bartlomiej Zolnierkiewicz");
MODULE_DESCRIPTION("low-level driver for the CS5536 IDE controller");
MODULE_LICENSE("GPL");
MODULE_DEVICE_TABLE(pci, cs5536_pci_tbl);

module_param_named(msr, use_msr, int, 0644);
MODULE_PARM_DESC(msr, "Force using MSR to configure IDE function (Default: 0)");

module_init(cs5536_init);
module_exit(cs5536_exit);
