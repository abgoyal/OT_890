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
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2001, 2003 Keith M Wesolowski
 * Copyright (C) 2005 Ilya A. Volynets <ilya@total-knowledge.com>
 */
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <asm/bootinfo.h>
#include <asm/io.h>
#include <asm/mipsregs.h>
#include <asm/page.h>
#include <asm/ip32/crime.h>
#include <asm/ip32/mace.h>

struct sgi_crime __iomem *crime;
struct sgi_mace __iomem *mace;

EXPORT_SYMBOL_GPL(mace);

void __init crime_init(void)
{
	unsigned int id, rev;
	const int field = 2 * sizeof(unsigned long);

	set_io_port_base((unsigned long) ioremap(MACEPCI_LOW_IO, 0x2000000));
	crime = ioremap(CRIME_BASE, sizeof(struct sgi_crime));
	mace = ioremap(MACE_BASE, sizeof(struct sgi_mace));

	id = crime->id;
	rev = id & CRIME_ID_REV;
	id = (id & CRIME_ID_IDBITS) >> 4;
	printk(KERN_INFO "CRIME id %1x rev %d at 0x%0*lx\n",
	       id, rev, field, (unsigned long) CRIME_BASE);
}

irqreturn_t crime_memerr_intr(unsigned int irq, void *dev_id)
{
	unsigned long stat, addr;
	int fatal = 0;

	stat = crime->mem_error_stat & CRIME_MEM_ERROR_STAT_MASK;
	addr = crime->mem_error_addr & CRIME_MEM_ERROR_ADDR_MASK;

	printk("CRIME memory error at 0x%08lx ST 0x%08lx<", addr, stat);

	if (stat & CRIME_MEM_ERROR_INV)
		printk("INV,");
	if (stat & CRIME_MEM_ERROR_ECC) {
		unsigned long ecc_syn =
			crime->mem_ecc_syn & CRIME_MEM_ERROR_ECC_SYN_MASK;
		unsigned long ecc_gen =
			crime->mem_ecc_chk & CRIME_MEM_ERROR_ECC_CHK_MASK;
		printk("ECC,SYN=0x%08lx,GEN=0x%08lx,", ecc_syn, ecc_gen);
	}
	if (stat & CRIME_MEM_ERROR_MULTIPLE) {
		fatal = 1;
		printk("MULTIPLE,");
	}
	if (stat & CRIME_MEM_ERROR_HARD_ERR) {
		fatal = 1;
		printk("HARD,");
	}
	if (stat & CRIME_MEM_ERROR_SOFT_ERR)
		printk("SOFT,");
	if (stat & CRIME_MEM_ERROR_CPU_ACCESS)
		printk("CPU,");
	if (stat & CRIME_MEM_ERROR_VICE_ACCESS)
		printk("VICE,");
	if (stat & CRIME_MEM_ERROR_GBE_ACCESS)
		printk("GBE,");
	if (stat & CRIME_MEM_ERROR_RE_ACCESS)
		printk("RE,REID=0x%02lx,", (stat & CRIME_MEM_ERROR_RE_ID)>>8);
	if (stat & CRIME_MEM_ERROR_MACE_ACCESS)
		printk("MACE,MACEID=0x%02lx,", stat & CRIME_MEM_ERROR_MACE_ID);

	crime->mem_error_stat = 0;

	if (fatal) {
		printk("FATAL>\n");
		panic("Fatal memory error.");
	} else
		printk("NONFATAL>\n");

	return IRQ_HANDLED;
}

irqreturn_t crime_cpuerr_intr(unsigned int irq, void *dev_id)
{
	unsigned long stat = crime->cpu_error_stat & CRIME_CPU_ERROR_MASK;
	unsigned long addr = crime->cpu_error_addr & CRIME_CPU_ERROR_ADDR_MASK;

	addr <<= 2;
	printk("CRIME CPU error at 0x%09lx status 0x%08lx\n", addr, stat);
	crime->cpu_error_stat = 0;

	return IRQ_HANDLED;
}
