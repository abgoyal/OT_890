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
 * Copyright (C) 2000,2001,2002,2003,2004 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/linkage.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/kernel_stat.h>

#include <asm/errno.h>
#include <asm/irq_regs.h>
#include <asm/signal.h>
#include <asm/system.h>
#include <asm/io.h>

#include <asm/sibyte/bcm1480_regs.h>
#include <asm/sibyte/bcm1480_int.h>
#include <asm/sibyte/bcm1480_scd.h>

#include <asm/sibyte/sb1250_uart.h>
#include <asm/sibyte/sb1250.h>

/*
 * These are the routines that handle all the low level interrupt stuff.
 * Actions handled here are: initialization of the interrupt map, requesting of
 * interrupt lines by handlers, dispatching if interrupts to handlers, probing
 * for interrupt lines
 */


static void end_bcm1480_irq(unsigned int irq);
static void enable_bcm1480_irq(unsigned int irq);
static void disable_bcm1480_irq(unsigned int irq);
static void ack_bcm1480_irq(unsigned int irq);
#ifdef CONFIG_SMP
static void bcm1480_set_affinity(unsigned int irq, const struct cpumask *mask);
#endif

#ifdef CONFIG_PCI
extern unsigned long ht_eoi_space;
#endif

static struct irq_chip bcm1480_irq_type = {
	.name = "BCM1480-IMR",
	.ack = ack_bcm1480_irq,
	.mask = disable_bcm1480_irq,
	.mask_ack = ack_bcm1480_irq,
	.unmask = enable_bcm1480_irq,
	.end = end_bcm1480_irq,
#ifdef CONFIG_SMP
	.set_affinity = bcm1480_set_affinity
#endif
};

/* Store the CPU id (not the logical number) */
int bcm1480_irq_owner[BCM1480_NR_IRQS];

DEFINE_SPINLOCK(bcm1480_imr_lock);

void bcm1480_mask_irq(int cpu, int irq)
{
	unsigned long flags, hl_spacing;
	u64 cur_ints;

	spin_lock_irqsave(&bcm1480_imr_lock, flags);
	hl_spacing = 0;
	if ((irq >= BCM1480_NR_IRQS_HALF) && (irq <= BCM1480_NR_IRQS)) {
		hl_spacing = BCM1480_IMR_HL_SPACING;
		irq -= BCM1480_NR_IRQS_HALF;
	}
	cur_ints = ____raw_readq(IOADDR(A_BCM1480_IMR_MAPPER(cpu) + R_BCM1480_IMR_INTERRUPT_MASK_H + hl_spacing));
	cur_ints |= (((u64) 1) << irq);
	____raw_writeq(cur_ints, IOADDR(A_BCM1480_IMR_MAPPER(cpu) + R_BCM1480_IMR_INTERRUPT_MASK_H + hl_spacing));
	spin_unlock_irqrestore(&bcm1480_imr_lock, flags);
}

void bcm1480_unmask_irq(int cpu, int irq)
{
	unsigned long flags, hl_spacing;
	u64 cur_ints;

	spin_lock_irqsave(&bcm1480_imr_lock, flags);
	hl_spacing = 0;
	if ((irq >= BCM1480_NR_IRQS_HALF) && (irq <= BCM1480_NR_IRQS)) {
		hl_spacing = BCM1480_IMR_HL_SPACING;
		irq -= BCM1480_NR_IRQS_HALF;
	}
	cur_ints = ____raw_readq(IOADDR(A_BCM1480_IMR_MAPPER(cpu) + R_BCM1480_IMR_INTERRUPT_MASK_H + hl_spacing));
	cur_ints &= ~(((u64) 1) << irq);
	____raw_writeq(cur_ints, IOADDR(A_BCM1480_IMR_MAPPER(cpu) + R_BCM1480_IMR_INTERRUPT_MASK_H + hl_spacing));
	spin_unlock_irqrestore(&bcm1480_imr_lock, flags);
}

#ifdef CONFIG_SMP
static void bcm1480_set_affinity(unsigned int irq, const struct cpumask *mask)
{
	int i = 0, old_cpu, cpu, int_on, k;
	u64 cur_ints;
	struct irq_desc *desc = irq_desc + irq;
	unsigned long flags;
	unsigned int irq_dirty;

	if (cpumask_weight(mask) != 1) {
		printk("attempted to set irq affinity for irq %d to multiple CPUs\n", irq);
		return;
	}
	i = cpumask_first(mask);

	/* Convert logical CPU to physical CPU */
	cpu = cpu_logical_map(i);

	/* Protect against other affinity changers and IMR manipulation */
	spin_lock_irqsave(&desc->lock, flags);
	spin_lock(&bcm1480_imr_lock);

	/* Swizzle each CPU's IMR (but leave the IP selection alone) */
	old_cpu = bcm1480_irq_owner[irq];
	irq_dirty = irq;
	if ((irq_dirty >= BCM1480_NR_IRQS_HALF) && (irq_dirty <= BCM1480_NR_IRQS)) {
		irq_dirty -= BCM1480_NR_IRQS_HALF;
	}

	for (k=0; k<2; k++) { /* Loop through high and low interrupt mask register */
		cur_ints = ____raw_readq(IOADDR(A_BCM1480_IMR_MAPPER(old_cpu) + R_BCM1480_IMR_INTERRUPT_MASK_H + (k*BCM1480_IMR_HL_SPACING)));
		int_on = !(cur_ints & (((u64) 1) << irq_dirty));
		if (int_on) {
			/* If it was on, mask it */
			cur_ints |= (((u64) 1) << irq_dirty);
			____raw_writeq(cur_ints, IOADDR(A_BCM1480_IMR_MAPPER(old_cpu) + R_BCM1480_IMR_INTERRUPT_MASK_H + (k*BCM1480_IMR_HL_SPACING)));
		}
		bcm1480_irq_owner[irq] = cpu;
		if (int_on) {
			/* unmask for the new CPU */
			cur_ints = ____raw_readq(IOADDR(A_BCM1480_IMR_MAPPER(cpu) + R_BCM1480_IMR_INTERRUPT_MASK_H + (k*BCM1480_IMR_HL_SPACING)));
			cur_ints &= ~(((u64) 1) << irq_dirty);
			____raw_writeq(cur_ints, IOADDR(A_BCM1480_IMR_MAPPER(cpu) + R_BCM1480_IMR_INTERRUPT_MASK_H + (k*BCM1480_IMR_HL_SPACING)));
		}
	}
	spin_unlock(&bcm1480_imr_lock);
	spin_unlock_irqrestore(&desc->lock, flags);
}
#endif


/*****************************************************************************/

static void disable_bcm1480_irq(unsigned int irq)
{
	bcm1480_mask_irq(bcm1480_irq_owner[irq], irq);
}

static void enable_bcm1480_irq(unsigned int irq)
{
	bcm1480_unmask_irq(bcm1480_irq_owner[irq], irq);
}


static void ack_bcm1480_irq(unsigned int irq)
{
	u64 pending;
	unsigned int irq_dirty;
	int k;

	/*
	 * If the interrupt was an HT interrupt, now is the time to
	 * clear it.  NOTE: we assume the HT bridge was set up to
	 * deliver the interrupts to all CPUs (which makes affinity
	 * changing easier for us)
	 */
	irq_dirty = irq;
	if ((irq_dirty >= BCM1480_NR_IRQS_HALF) && (irq_dirty <= BCM1480_NR_IRQS)) {
		irq_dirty -= BCM1480_NR_IRQS_HALF;
	}
	for (k=0; k<2; k++) { /* Loop through high and low LDT interrupts */
		pending = __raw_readq(IOADDR(A_BCM1480_IMR_REGISTER(bcm1480_irq_owner[irq],
						R_BCM1480_IMR_LDT_INTERRUPT_H + (k*BCM1480_IMR_HL_SPACING))));
		pending &= ((u64)1 << (irq_dirty));
		if (pending) {
#ifdef CONFIG_SMP
			int i;
			for (i=0; i<NR_CPUS; i++) {
				/*
				 * Clear for all CPUs so an affinity switch
				 * doesn't find an old status
				 */
				__raw_writeq(pending, IOADDR(A_BCM1480_IMR_REGISTER(cpu_logical_map(i),
								R_BCM1480_IMR_LDT_INTERRUPT_CLR_H + (k*BCM1480_IMR_HL_SPACING))));
			}
#else
			__raw_writeq(pending, IOADDR(A_BCM1480_IMR_REGISTER(0, R_BCM1480_IMR_LDT_INTERRUPT_CLR_H + (k*BCM1480_IMR_HL_SPACING))));
#endif

			/*
			 * Generate EOI.  For Pass 1 parts, EOI is a nop.  For
			 * Pass 2, the LDT world may be edge-triggered, but
			 * this EOI shouldn't hurt.  If they are
			 * level-sensitive, the EOI is required.
			 */
#ifdef CONFIG_PCI
			if (ht_eoi_space)
				*(uint32_t *)(ht_eoi_space+(irq<<16)+(7<<2)) = 0;
#endif
		}
	}
	bcm1480_mask_irq(bcm1480_irq_owner[irq], irq);
}


static void end_bcm1480_irq(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS))) {
		bcm1480_unmask_irq(bcm1480_irq_owner[irq], irq);
	}
}


void __init init_bcm1480_irqs(void)
{
	int i;

	for (i = 0; i < BCM1480_NR_IRQS; i++) {
		set_irq_chip(i, &bcm1480_irq_type);
		bcm1480_irq_owner[i] = 0;
	}
}

/*
 *  init_IRQ is called early in the boot sequence from init/main.c.  It
 *  is responsible for setting up the interrupt mapper and installing the
 *  handler that will be responsible for dispatching interrupts to the
 *  "right" place.
 */
/*
 * For now, map all interrupts to IP[2].  We could save
 * some cycles by parceling out system interrupts to different
 * IP lines, but keep it simple for bringup.  We'll also direct
 * all interrupts to a single CPU; we should probably route
 * PCI and LDT to one cpu and everything else to the other
 * to balance the load a bit.
 *
 * On the second cpu, everything is set to IP5, which is
 * ignored, EXCEPT the mailbox interrupt.  That one is
 * set to IP[2] so it is handled.  This is needed so we
 * can do cross-cpu function calls, as requred by SMP
 */

#define IMR_IP2_VAL	K_BCM1480_INT_MAP_I0
#define IMR_IP3_VAL	K_BCM1480_INT_MAP_I1
#define IMR_IP4_VAL	K_BCM1480_INT_MAP_I2
#define IMR_IP5_VAL	K_BCM1480_INT_MAP_I3
#define IMR_IP6_VAL	K_BCM1480_INT_MAP_I4

void __init arch_init_irq(void)
{
	unsigned int i, cpu;
	u64 tmp;
	unsigned int imask = STATUSF_IP4 | STATUSF_IP3 | STATUSF_IP2 |
		STATUSF_IP1 | STATUSF_IP0;

	/* Default everything to IP2 */
	/* Start with _high registers which has no bit 0 interrupt source */
	for (i = 1; i < BCM1480_NR_IRQS_HALF; i++) {	/* was I0 */
		for (cpu = 0; cpu < 4; cpu++) {
			__raw_writeq(IMR_IP2_VAL,
				     IOADDR(A_BCM1480_IMR_REGISTER(cpu,
								   R_BCM1480_IMR_INTERRUPT_MAP_BASE_H) + (i << 3)));
		}
	}

	/* Now do _low registers */
	for (i = 0; i < BCM1480_NR_IRQS_HALF; i++) {
		for (cpu = 0; cpu < 4; cpu++) {
			__raw_writeq(IMR_IP2_VAL,
				     IOADDR(A_BCM1480_IMR_REGISTER(cpu,
								   R_BCM1480_IMR_INTERRUPT_MAP_BASE_L) + (i << 3)));
		}
	}

	init_bcm1480_irqs();

	/*
	 * Map the high 16 bits of mailbox_0 registers to IP[3], for
	 * inter-cpu messages
	 */
	/* Was I1 */
	for (cpu = 0; cpu < 4; cpu++) {
		__raw_writeq(IMR_IP3_VAL, IOADDR(A_BCM1480_IMR_REGISTER(cpu, R_BCM1480_IMR_INTERRUPT_MAP_BASE_H) +
						 (K_BCM1480_INT_MBOX_0_0 << 3)));
        }


	/* Clear the mailboxes.  The firmware may leave them dirty */
	for (cpu = 0; cpu < 4; cpu++) {
		__raw_writeq(0xffffffffffffffffULL,
			     IOADDR(A_BCM1480_IMR_REGISTER(cpu, R_BCM1480_IMR_MAILBOX_0_CLR_CPU)));
		__raw_writeq(0xffffffffffffffffULL,
			     IOADDR(A_BCM1480_IMR_REGISTER(cpu, R_BCM1480_IMR_MAILBOX_1_CLR_CPU)));
	}


	/* Mask everything except the high 16 bit of mailbox_0 registers for all cpus */
	tmp = ~((u64) 0) ^ ( (((u64) 1) << K_BCM1480_INT_MBOX_0_0));
	for (cpu = 0; cpu < 4; cpu++) {
		__raw_writeq(tmp, IOADDR(A_BCM1480_IMR_REGISTER(cpu, R_BCM1480_IMR_INTERRUPT_MASK_H)));
	}
	tmp = ~((u64) 0);
	for (cpu = 0; cpu < 4; cpu++) {
		__raw_writeq(tmp, IOADDR(A_BCM1480_IMR_REGISTER(cpu, R_BCM1480_IMR_INTERRUPT_MASK_L)));
	}

	/*
	 * Note that the timer interrupts are also mapped, but this is
	 * done in bcm1480_time_init().  Also, the profiling driver
	 * does its own management of IP7.
	 */

	/* Enable necessary IPs, disable the rest */
	change_c0_status(ST0_IM, imask);
}

extern void bcm1480_mailbox_interrupt(void);

static inline void dispatch_ip2(void)
{
	unsigned long long mask_h, mask_l;
	unsigned int cpu = smp_processor_id();
	unsigned long base;

	/*
	 * Default...we've hit an IP[2] interrupt, which means we've got to
	 * check the 1480 interrupt registers to figure out what to do.  Need
	 * to detect which CPU we're on, now that smp_affinity is supported.
	 */
	base = A_BCM1480_IMR_MAPPER(cpu);
	mask_h = __raw_readq(
		IOADDR(base + R_BCM1480_IMR_INTERRUPT_STATUS_BASE_H));
	mask_l = __raw_readq(
		IOADDR(base + R_BCM1480_IMR_INTERRUPT_STATUS_BASE_L));

	if (mask_h) {
		if (mask_h ^ 1)
			do_IRQ(fls64(mask_h) - 1);
		else if (mask_l)
			do_IRQ(63 + fls64(mask_l));
	}
}

asmlinkage void plat_irq_dispatch(void)
{
	unsigned int cpu = smp_processor_id();
	unsigned int pending;

#ifdef CONFIG_SIBYTE_BCM1480_PROF
	/* Set compare to count to silence count/compare timer interrupts */
	write_c0_compare(read_c0_count());
#endif

	pending = read_c0_cause() & read_c0_status();

#ifdef CONFIG_SIBYTE_BCM1480_PROF
	if (pending & CAUSEF_IP7)	/* Cpu performance counter interrupt */
		sbprof_cpu_intr();
	else
#endif

	if (pending & CAUSEF_IP4)
		do_IRQ(K_BCM1480_INT_TIMER_0 + cpu);
#ifdef CONFIG_SMP
	else if (pending & CAUSEF_IP3)
		bcm1480_mailbox_interrupt();
#endif

	else if (pending & CAUSEF_IP2)
		dispatch_ip2();
}
