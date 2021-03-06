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
 * Copyright (C) 2004-2005 Advanced Micro Devices, Inc.
 * Copyright (C)      2007 Bartlomiej Zolnierkiewicz
 *
 * History:
 * 09/20/2005 - Jaya Kumar <jayakumar.ide@gmail.com>
 * - Reworked tuneproc, set_drive, misc mods to prep for mainline
 * - Work was sponsored by CIS (M) Sdn Bhd.
 * Ported to Kernel 2.6.11 on June 26, 2005 by
 *   Wolfgang Zuleger <wolfgang.zuleger@gmx.de>
 *   Alexander Kiausch <alex.kiausch@t-online.de>
 * Originally developed by AMD for 2.4/2.6
 *
 * Development of this chipset driver was funded
 * by the nice folks at National Semiconductor/AMD.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Documentation:
 *  CS5535 documentation available from AMD
 */

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/ide.h>

#define DRV_NAME "cs5535"

#define MSR_ATAC_BASE		0x51300000
#define ATAC_GLD_MSR_CAP	(MSR_ATAC_BASE+0)
#define ATAC_GLD_MSR_CONFIG	(MSR_ATAC_BASE+0x01)
#define ATAC_GLD_MSR_SMI	(MSR_ATAC_BASE+0x02)
#define ATAC_GLD_MSR_ERROR	(MSR_ATAC_BASE+0x03)
#define ATAC_GLD_MSR_PM		(MSR_ATAC_BASE+0x04)
#define ATAC_GLD_MSR_DIAG	(MSR_ATAC_BASE+0x05)
#define ATAC_IO_BAR		(MSR_ATAC_BASE+0x08)
#define ATAC_RESET		(MSR_ATAC_BASE+0x10)
#define ATAC_CH0D0_PIO		(MSR_ATAC_BASE+0x20)
#define ATAC_CH0D0_DMA		(MSR_ATAC_BASE+0x21)
#define ATAC_CH0D1_PIO		(MSR_ATAC_BASE+0x22)
#define ATAC_CH0D1_DMA		(MSR_ATAC_BASE+0x23)
#define ATAC_PCI_ABRTERR	(MSR_ATAC_BASE+0x24)
#define ATAC_BM0_CMD_PRIM	0x00
#define ATAC_BM0_STS_PRIM	0x02
#define ATAC_BM0_PRD		0x04
#define CS5535_CABLE_DETECT	0x48

/* Format I PIO settings. We separate out cmd and data for safer timings */

static unsigned int cs5535_pio_cmd_timings[5] =
{ 0xF7F4, 0x53F3, 0x13F1, 0x5131, 0x1131 };
static unsigned int cs5535_pio_dta_timings[5] =
{ 0xF7F4, 0xF173, 0x8141, 0x5131, 0x1131 };

static unsigned int cs5535_mwdma_timings[3] =
{ 0x7F0FFFF3, 0x7F035352, 0x7f024241 };

static unsigned int cs5535_udma_timings[5] =
{ 0x7F7436A1, 0x7F733481, 0x7F723261, 0x7F713161, 0x7F703061 };

/* Macros to check if the register is the reset value -  reset value is an
   invalid timing and indicates the register has not been set previously */

#define CS5535_BAD_PIO(timings) ( (timings&~0x80000000UL) == 0x00009172 )
#define CS5535_BAD_DMA(timings) ( (timings & 0x000FFFFF) == 0x00077771 )

/****
 *	cs5535_set_speed         -     Configure the chipset to the new speed
 *	@drive: Drive to set up
 *	@speed: desired speed
 *
 *	cs5535_set_speed() configures the chipset to a new speed.
 */
static void cs5535_set_speed(ide_drive_t *drive, const u8 speed)
{
	u32 reg = 0, dummy;
	u8 unit = drive->dn & 1;

	/* Set the PIO timings */
	if (speed < XFER_SW_DMA_0) {
		ide_drive_t *pair = ide_get_pair_dev(drive);
		u8 cmd, pioa;

		cmd = pioa = speed - XFER_PIO_0;

		if (pair) {
			u8 piob = ide_get_best_pio_mode(pair, 255, 4);

			if (piob < cmd)
				cmd = piob;
		}

		/* Write the speed of the current drive */
		reg = (cs5535_pio_cmd_timings[cmd] << 16) |
			cs5535_pio_dta_timings[pioa];
		wrmsr(unit ? ATAC_CH0D1_PIO : ATAC_CH0D0_PIO, reg, 0);

		/* And if nessesary - change the speed of the other drive */
		rdmsr(unit ?  ATAC_CH0D0_PIO : ATAC_CH0D1_PIO, reg, dummy);

		if (((reg >> 16) & cs5535_pio_cmd_timings[cmd]) !=
			cs5535_pio_cmd_timings[cmd]) {
			reg &= 0x0000FFFF;
			reg |= cs5535_pio_cmd_timings[cmd] << 16;
			wrmsr(unit ? ATAC_CH0D0_PIO : ATAC_CH0D1_PIO, reg, 0);
		}

		/* Set bit 31 of the DMA register for PIO format 1 timings */
		rdmsr(unit ?  ATAC_CH0D1_DMA : ATAC_CH0D0_DMA, reg, dummy);
		wrmsr(unit ? ATAC_CH0D1_DMA : ATAC_CH0D0_DMA,
					reg | 0x80000000UL, 0);
	} else {
		rdmsr(unit ? ATAC_CH0D1_DMA : ATAC_CH0D0_DMA, reg, dummy);

		reg &= 0x80000000UL;  /* Preserve the PIO format bit */

		if (speed >= XFER_UDMA_0 && speed <= XFER_UDMA_4)
			reg |= cs5535_udma_timings[speed - XFER_UDMA_0];
		else if (speed >= XFER_MW_DMA_0 && speed <= XFER_MW_DMA_2)
			reg |= cs5535_mwdma_timings[speed - XFER_MW_DMA_0];
		else
			return;

		wrmsr(unit ? ATAC_CH0D1_DMA : ATAC_CH0D0_DMA, reg, 0);
	}
}

/**
 *	cs5535_set_dma_mode	-	set host controller for DMA mode
 *	@drive: drive
 *	@speed: DMA mode
 *
 *	Programs the chipset for DMA mode.
 */

static void cs5535_set_dma_mode(ide_drive_t *drive, const u8 speed)
{
	cs5535_set_speed(drive, speed);
}

/**
 *	cs5535_set_pio_mode	-	set host controller for PIO mode
 *	@drive: drive
 *	@pio: PIO mode number
 *
 *	A callback from the upper layers for PIO-only tuning.
 */

static void cs5535_set_pio_mode(ide_drive_t *drive, const u8 pio)
{
	cs5535_set_speed(drive, XFER_PIO_0 + pio);
}

static u8 cs5535_cable_detect(ide_hwif_t *hwif)
{
	struct pci_dev *dev = to_pci_dev(hwif->dev);
	u8 bit;

	/* if a 80 wire cable was detected */
	pci_read_config_byte(dev, CS5535_CABLE_DETECT, &bit);

	return (bit & 1) ? ATA_CBL_PATA80 : ATA_CBL_PATA40;
}

static const struct ide_port_ops cs5535_port_ops = {
	.set_pio_mode		= cs5535_set_pio_mode,
	.set_dma_mode		= cs5535_set_dma_mode,
	.cable_detect		= cs5535_cable_detect,
};

static const struct ide_port_info cs5535_chipset __devinitdata = {
	.name		= DRV_NAME,
	.port_ops	= &cs5535_port_ops,
	.host_flags	= IDE_HFLAG_SINGLE | IDE_HFLAG_POST_SET_MODE,
	.pio_mask	= ATA_PIO4,
	.mwdma_mask	= ATA_MWDMA2,
	.udma_mask	= ATA_UDMA4,
};

static int __devinit cs5535_init_one(struct pci_dev *dev,
					const struct pci_device_id *id)
{
	return ide_pci_init_one(dev, &cs5535_chipset, NULL);
}

static const struct pci_device_id cs5535_pci_tbl[] = {
	{ PCI_VDEVICE(NS, PCI_DEVICE_ID_NS_CS5535_IDE), 0 },
	{ 0, },
};

MODULE_DEVICE_TABLE(pci, cs5535_pci_tbl);

static struct pci_driver cs5535_pci_driver = {
	.name		= "CS5535_IDE",
	.id_table	= cs5535_pci_tbl,
	.probe		= cs5535_init_one,
	.remove		= ide_pci_remove,
	.suspend	= ide_pci_suspend,
	.resume		= ide_pci_resume,
};

static int __init cs5535_ide_init(void)
{
	return ide_pci_register_driver(&cs5535_pci_driver);
}

static void __exit cs5535_ide_exit(void)
{
	pci_unregister_driver(&cs5535_pci_driver);
}

module_init(cs5535_ide_init);
module_exit(cs5535_ide_exit);

MODULE_AUTHOR("AMD");
MODULE_DESCRIPTION("PCI driver module for AMD/NS CS5535 IDE");
MODULE_LICENSE("GPL");
