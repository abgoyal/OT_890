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

/* auxio.c: Probing for the Sparc AUXIO register at boot time.
 *
 * Copyright (C) 1996 David S. Miller (davem@caip.rutgers.edu)
 */

#include <linux/stddef.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <asm/oplib.h>
#include <asm/io.h>
#include <asm/auxio.h>
#include <asm/string.h>		/* memset(), Linux has no bzero() */

/* Probe and map in the Auxiliary I/O register */

/* auxio_register is not static because it is referenced 
 * in entry.S::floppy_tdone
 */
void __iomem *auxio_register = NULL;
static DEFINE_SPINLOCK(auxio_lock);

void __init auxio_probe(void)
{
	int node, auxio_nd;
	struct linux_prom_registers auxregs[1];
	struct resource r;

	switch (sparc_cpu_model) {
	case sun4d:
	case sun4:
		return;
	default:
		break;
	}
	node = prom_getchild(prom_root_node);
	auxio_nd = prom_searchsiblings(node, "auxiliary-io");
	if(!auxio_nd) {
		node = prom_searchsiblings(node, "obio");
		node = prom_getchild(node);
		auxio_nd = prom_searchsiblings(node, "auxio");
		if(!auxio_nd) {
#ifdef CONFIG_PCI
			/* There may be auxio on Ebus */
			return;
#else
			if(prom_searchsiblings(node, "leds")) {
				/* VME chassis sun4m machine, no auxio exists. */
				return;
			}
			prom_printf("Cannot find auxio node, cannot continue...\n");
			prom_halt();
#endif
		}
	}
	if(prom_getproperty(auxio_nd, "reg", (char *) auxregs, sizeof(auxregs)) <= 0)
		return;
	prom_apply_obio_ranges(auxregs, 0x1);
	/* Map the register both read and write */
	r.flags = auxregs[0].which_io & 0xF;
	r.start = auxregs[0].phys_addr;
	r.end = auxregs[0].phys_addr + auxregs[0].reg_size - 1;
	auxio_register = of_ioremap(&r, 0, auxregs[0].reg_size, "auxio");
	/* Fix the address on sun4m and sun4c. */
	if((((unsigned long) auxregs[0].phys_addr) & 3) == 3 ||
	   sparc_cpu_model == sun4c)
		auxio_register += (3 - ((unsigned long)auxio_register & 3));

	set_auxio(AUXIO_LED, 0);
}

unsigned char get_auxio(void)
{
	if(auxio_register) 
		return sbus_readb(auxio_register);
	return 0;
}
EXPORT_SYMBOL(get_auxio);

void set_auxio(unsigned char bits_on, unsigned char bits_off)
{
	unsigned char regval;
	unsigned long flags;
	spin_lock_irqsave(&auxio_lock, flags);
	switch(sparc_cpu_model) {
	case sun4c:
		regval = sbus_readb(auxio_register);
		sbus_writeb(((regval | bits_on) & ~bits_off) | AUXIO_ORMEIN,
			auxio_register);
		break;
	case sun4m:
		if(!auxio_register)
			break;     /* VME chassis sun4m, no auxio. */
		regval = sbus_readb(auxio_register);
		sbus_writeb(((regval | bits_on) & ~bits_off) | AUXIO_ORMEIN4M,
			auxio_register);
		break;
	case sun4d:
		break;
	default:
		panic("Can't set AUXIO register on this machine.");
	};
	spin_unlock_irqrestore(&auxio_lock, flags);
}
EXPORT_SYMBOL(set_auxio);

/* sun4m power control register (AUXIO2) */

volatile unsigned char * auxio_power_register = NULL;

void __init auxio_power_probe(void)
{
	struct linux_prom_registers regs;
	int node;
	struct resource r;

	/* Attempt to find the sun4m power control node. */
	node = prom_getchild(prom_root_node);
	node = prom_searchsiblings(node, "obio");
	node = prom_getchild(node);
	node = prom_searchsiblings(node, "power");
	if (node == 0 || node == -1)
		return;

	/* Map the power control register. */
	if (prom_getproperty(node, "reg", (char *)&regs, sizeof(regs)) <= 0)
		return;
	prom_apply_obio_ranges(&regs, 1);
	memset(&r, 0, sizeof(r));
	r.flags = regs.which_io & 0xF;
	r.start = regs.phys_addr;
	r.end = regs.phys_addr + regs.reg_size - 1;
	auxio_power_register = (unsigned char *) of_ioremap(&r, 0,
	    regs.reg_size, "auxpower");

	/* Display a quick message on the console. */
	if (auxio_power_register)
		printk(KERN_INFO "Power off control detected.\n");
}
