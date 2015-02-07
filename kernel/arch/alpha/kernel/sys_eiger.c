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
 *	linux/arch/alpha/kernel/sys_eiger.c
 *
 *	Copyright (C) 1995 David A Rusling
 *	Copyright (C) 1996, 1999 Jay A Estabrook
 *	Copyright (C) 1998, 1999 Richard Henderson
 *	Copyright (C) 1999 Iain Grant
 *
 * Code supporting the EIGER (EV6+TSUNAMI).
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/bitops.h>

#include <asm/ptrace.h>
#include <asm/system.h>
#include <asm/dma.h>
#include <asm/irq.h>
#include <asm/mmu_context.h>
#include <asm/io.h>
#include <asm/pci.h>
#include <asm/pgtable.h>
#include <asm/core_tsunami.h>
#include <asm/hwrpb.h>
#include <asm/tlbflush.h>

#include "proto.h"
#include "irq_impl.h"
#include "pci_impl.h"
#include "machvec_impl.h"


/* Note that this interrupt code is identical to TAKARA.  */

/* Note mask bit is true for DISABLED irqs.  */
static unsigned long cached_irq_mask[2] = { -1, -1 };

static inline void
eiger_update_irq_hw(unsigned long irq, unsigned long mask)
{
	int regaddr;

	mask = (irq >= 64 ? mask << 16 : mask >> ((irq - 16) & 0x30));
	regaddr = 0x510 + (((irq - 16) >> 2) & 0x0c);
	outl(mask & 0xffff0000UL, regaddr);
}

static inline void
eiger_enable_irq(unsigned int irq)
{
	unsigned long mask;
	mask = (cached_irq_mask[irq >= 64] &= ~(1UL << (irq & 63)));
	eiger_update_irq_hw(irq, mask);
}

static void
eiger_disable_irq(unsigned int irq)
{
	unsigned long mask;
	mask = (cached_irq_mask[irq >= 64] |= 1UL << (irq & 63));
	eiger_update_irq_hw(irq, mask);
}

static unsigned int
eiger_startup_irq(unsigned int irq)
{
	eiger_enable_irq(irq);
	return 0; /* never anything pending */
}

static void
eiger_end_irq(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
		eiger_enable_irq(irq);
}

static struct hw_interrupt_type eiger_irq_type = {
	.typename	= "EIGER",
	.startup	= eiger_startup_irq,
	.shutdown	= eiger_disable_irq,
	.enable		= eiger_enable_irq,
	.disable	= eiger_disable_irq,
	.ack		= eiger_disable_irq,
	.end		= eiger_end_irq,
};

static void
eiger_device_interrupt(unsigned long vector)
{
	unsigned intstatus;

	/*
	 * The PALcode will have passed us vectors 0x800 or 0x810,
	 * which are fairly arbitrary values and serve only to tell
	 * us whether an interrupt has come in on IRQ0 or IRQ1. If
	 * it's IRQ1 it's a PCI interrupt; if it's IRQ0, it's
	 * probably ISA, but PCI interrupts can come through IRQ0
	 * as well if the interrupt controller isn't in accelerated
	 * mode.
	 *
	 * OTOH, the accelerator thing doesn't seem to be working
	 * overly well, so what we'll do instead is try directly
	 * examining the Master Interrupt Register to see if it's a
	 * PCI interrupt, and if _not_ then we'll pass it on to the
	 * ISA handler.
	 */

	intstatus = inw(0x500) & 15;
	if (intstatus) {
		/*
		 * This is a PCI interrupt. Check each bit and
		 * despatch an interrupt if it's set.
		 */

		if (intstatus & 8) handle_irq(16+3);
		if (intstatus & 4) handle_irq(16+2);
		if (intstatus & 2) handle_irq(16+1);
		if (intstatus & 1) handle_irq(16+0);
	} else {
		isa_device_interrupt(vector);
	}
}

static void
eiger_srm_device_interrupt(unsigned long vector)
{
	int irq = (vector - 0x800) >> 4;
	handle_irq(irq);
}

static void __init
eiger_init_irq(void)
{
	long i;

	outb(0, DMA1_RESET_REG);
	outb(0, DMA2_RESET_REG);
	outb(DMA_MODE_CASCADE, DMA2_MODE_REG);
	outb(0, DMA2_MASK_REG);

	if (alpha_using_srm)
		alpha_mv.device_interrupt = eiger_srm_device_interrupt;

	for (i = 16; i < 128; i += 16)
		eiger_update_irq_hw(i, -1);

	init_i8259a_irqs();

	for (i = 16; i < 128; ++i) {
		irq_desc[i].status = IRQ_DISABLED | IRQ_LEVEL;
		irq_desc[i].chip = &eiger_irq_type;
	}
}

static int __init
eiger_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
	u8 irq_orig;

	/* The SRM console has already calculated out the IRQ value's for
	   option cards. As this works lets just read in the value already
	   set and change it to a useable value by Linux.

	   All the IRQ values generated by the console are greater than 90,
	   so we subtract 80 because it is (90 - allocated ISA IRQ's).  */

	pci_read_config_byte(dev, PCI_INTERRUPT_LINE, &irq_orig);

	return irq_orig - 0x80;
}

static u8 __init
eiger_swizzle(struct pci_dev *dev, u8 *pinp)
{
	struct pci_controller *hose = dev->sysdata;
	int slot, pin = *pinp;
	int bridge_count = 0;

	/* Find the number of backplane bridges.  */
	int backplane = inw(0x502) & 0x0f;

	switch (backplane)
	{
	   case 0x00: bridge_count = 0; break; /* No bridges */
	   case 0x01: bridge_count = 1; break; /* 1 */
	   case 0x03: bridge_count = 2; break; /* 2 */
	   case 0x07: bridge_count = 3; break; /* 3 */
	   case 0x0f: bridge_count = 4; break; /* 4 */
	};

	slot = PCI_SLOT(dev->devfn);
	while (dev->bus->self) {
		/* Check for built-in bridges on hose 0. */
		if (hose->index == 0
		    && (PCI_SLOT(dev->bus->self->devfn)
			> 20 - bridge_count)) {
			slot = PCI_SLOT(dev->devfn);
			break;
		}
		/* Must be a card-based bridge.  */
		pin = pci_swizzle_interrupt_pin(dev, pin);

		/* Move up the chain of bridges.  */
		dev = dev->bus->self;
	}
	*pinp = pin;
	return slot;
}

/*
 * The System Vectors
 */

struct alpha_machine_vector eiger_mv __initmv = {
	.vector_name		= "Eiger",
	DO_EV6_MMU,
	DO_DEFAULT_RTC,
	DO_TSUNAMI_IO,
	.machine_check		= tsunami_machine_check,
	.max_isa_dma_address	= ALPHA_MAX_ISA_DMA_ADDRESS,
	.min_io_address		= DEFAULT_IO_BASE,
	.min_mem_address	= DEFAULT_MEM_BASE,
	.pci_dac_offset		= TSUNAMI_DAC_OFFSET,

	.nr_irqs		= 128,
	.device_interrupt	= eiger_device_interrupt,

	.init_arch		= tsunami_init_arch,
	.init_irq		= eiger_init_irq,
	.init_rtc		= common_init_rtc,
	.init_pci		= common_init_pci,
	.kill_arch		= tsunami_kill_arch,
	.pci_map_irq		= eiger_map_irq,
	.pci_swizzle		= eiger_swizzle,
};
ALIAS_MV(eiger)
