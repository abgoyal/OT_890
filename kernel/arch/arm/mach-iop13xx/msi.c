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
 * arch/arm/mach-iop13xx/msi.c
 *
 * PCI MSI support for the iop13xx processor
 *
 * Copyright (c) 2006, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307 USA.
 *
 */
#include <linux/pci.h>
#include <linux/msi.h>
#include <asm/mach/irq.h>
#include <asm/irq.h>


#define IOP13XX_NUM_MSI_IRQS 128
static DECLARE_BITMAP(msi_irq_in_use, IOP13XX_NUM_MSI_IRQS);

/* IMIPR0 CP6 R8 Page 1
 */
static u32 read_imipr_0(void)
{
	u32 val;
	asm volatile("mrc p6, 0, %0, c8, c1, 0":"=r" (val));
	return val;
}
static void write_imipr_0(u32 val)
{
	asm volatile("mcr p6, 0, %0, c8, c1, 0"::"r" (val));
}

/* IMIPR1 CP6 R9 Page 1
 */
static u32 read_imipr_1(void)
{
	u32 val;
	asm volatile("mrc p6, 0, %0, c9, c1, 0":"=r" (val));
	return val;
}
static void write_imipr_1(u32 val)
{
	asm volatile("mcr p6, 0, %0, c9, c1, 0"::"r" (val));
}

/* IMIPR2 CP6 R10 Page 1
 */
static u32 read_imipr_2(void)
{
	u32 val;
	asm volatile("mrc p6, 0, %0, c10, c1, 0":"=r" (val));
	return val;
}
static void write_imipr_2(u32 val)
{
	asm volatile("mcr p6, 0, %0, c10, c1, 0"::"r" (val));
}

/* IMIPR3 CP6 R11 Page 1
 */
static u32 read_imipr_3(void)
{
	u32 val;
	asm volatile("mrc p6, 0, %0, c11, c1, 0":"=r" (val));
	return val;
}
static void write_imipr_3(u32 val)
{
	asm volatile("mcr p6, 0, %0, c11, c1, 0"::"r" (val));
}

static u32 (*read_imipr[])(void) = {
	read_imipr_0,
	read_imipr_1,
	read_imipr_2,
	read_imipr_3,
};

static void (*write_imipr[])(u32) = {
	write_imipr_0,
	write_imipr_1,
	write_imipr_2,
	write_imipr_3,
};

static void iop13xx_msi_handler(unsigned int irq, struct irq_desc *desc)
{
	int i, j;
	unsigned long status;

	/* read IMIPR registers and find any active interrupts,
	 * then call ISR for each active interrupt
	 */
	for (i = 0; i < ARRAY_SIZE(read_imipr); i++) {
		status = (read_imipr[i])();
		if (!status)
			continue;

		do {
			j = find_first_bit(&status, 32);
			(write_imipr[i])(1 << j); /* write back to clear bit */
			generic_handle_irq(IRQ_IOP13XX_MSI_0 + j + (32*i));
			status = (read_imipr[i])();
		} while (status);
	}
}

void __init iop13xx_msi_init(void)
{
	set_irq_chained_handler(IRQ_IOP13XX_INBD_MSI, iop13xx_msi_handler);
}

/*
 * Dynamic irq allocate and deallocation
 */
int create_irq(void)
{
	int irq, pos;

again:
	pos = find_first_zero_bit(msi_irq_in_use, IOP13XX_NUM_MSI_IRQS);
	irq = IRQ_IOP13XX_MSI_0 + pos;
	if (irq > NR_IRQS)
		return -ENOSPC;
	/* test_and_set_bit operates on 32-bits at a time */
	if (test_and_set_bit(pos, msi_irq_in_use))
		goto again;

	dynamic_irq_init(irq);

	return irq;
}

void destroy_irq(unsigned int irq)
{
	int pos = irq - IRQ_IOP13XX_MSI_0;

	dynamic_irq_cleanup(irq);

	clear_bit(pos, msi_irq_in_use);
}

void arch_teardown_msi_irq(unsigned int irq)
{
	destroy_irq(irq);
}

static void iop13xx_msi_nop(unsigned int irq)
{
	return;
}

static struct irq_chip iop13xx_msi_chip = {
	.name = "PCI-MSI",
	.ack = iop13xx_msi_nop,
	.enable = unmask_msi_irq,
	.disable = mask_msi_irq,
	.mask = mask_msi_irq,
	.unmask = unmask_msi_irq,
};

int arch_setup_msi_irq(struct pci_dev *pdev, struct msi_desc *desc)
{
	int id, irq = create_irq();
	struct msi_msg msg;

	if (irq < 0)
		return irq;

	set_irq_msi(irq, desc);

	msg.address_hi = 0x0;
	msg.address_lo = IOP13XX_MU_MIMR_PCI;

	id = iop13xx_cpu_id();
	msg.data = (id << IOP13XX_MU_MIMR_CORE_SELECT) | (irq & 0x7f);

	write_msi_msg(irq, &msg);
	set_irq_chip_and_handler(irq, &iop13xx_msi_chip, handle_simple_irq);

	return 0;
}
