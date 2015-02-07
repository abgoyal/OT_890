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
 *      linux/arch/alpha/kernel/irq_i8259.c
 *
 * This is the 'legacy' 8259A Programmable Interrupt Controller,
 * present in the majority of PC/AT boxes.
 *
 * Started hacking from linux-2.3.30pre6/arch/i386/kernel/i8259.c.
 */

#include <linux/init.h>
#include <linux/cache.h>
#include <linux/sched.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

#include <asm/io.h>

#include "proto.h"
#include "irq_impl.h"


/* Note mask bit is true for DISABLED irqs.  */
static unsigned int cached_irq_mask = 0xffff;
static DEFINE_SPINLOCK(i8259_irq_lock);

static inline void
i8259_update_irq_hw(unsigned int irq, unsigned long mask)
{
	int port = 0x21;
	if (irq & 8) mask >>= 8;
	if (irq & 8) port = 0xA1;
	outb(mask, port);
}

inline void
i8259a_enable_irq(unsigned int irq)
{
	spin_lock(&i8259_irq_lock);
	i8259_update_irq_hw(irq, cached_irq_mask &= ~(1 << irq));
	spin_unlock(&i8259_irq_lock);
}

static inline void
__i8259a_disable_irq(unsigned int irq)
{
	i8259_update_irq_hw(irq, cached_irq_mask |= 1 << irq);
}

void
i8259a_disable_irq(unsigned int irq)
{
	spin_lock(&i8259_irq_lock);
	__i8259a_disable_irq(irq);
	spin_unlock(&i8259_irq_lock);
}

void
i8259a_mask_and_ack_irq(unsigned int irq)
{
	spin_lock(&i8259_irq_lock);
	__i8259a_disable_irq(irq);

	/* Ack the interrupt making it the lowest priority.  */
	if (irq >= 8) {
		outb(0xE0 | (irq - 8), 0xa0);   /* ack the slave */
		irq = 2;
	}
	outb(0xE0 | irq, 0x20);			/* ack the master */
	spin_unlock(&i8259_irq_lock);
}

unsigned int
i8259a_startup_irq(unsigned int irq)
{
	i8259a_enable_irq(irq);
	return 0; /* never anything pending */
}

void
i8259a_end_irq(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
		i8259a_enable_irq(irq);
}

struct hw_interrupt_type i8259a_irq_type = {
	.typename	= "XT-PIC",
	.startup	= i8259a_startup_irq,
	.shutdown	= i8259a_disable_irq,
	.enable		= i8259a_enable_irq,
	.disable	= i8259a_disable_irq,
	.ack		= i8259a_mask_and_ack_irq,
	.end		= i8259a_end_irq,
};

void __init
init_i8259a_irqs(void)
{
	static struct irqaction cascade = {
		.handler	= no_action,
		.name		= "cascade",
	};

	long i;

	outb(0xff, 0x21);	/* mask all of 8259A-1 */
	outb(0xff, 0xA1);	/* mask all of 8259A-2 */

	for (i = 0; i < 16; i++) {
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].chip = &i8259a_irq_type;
	}

	setup_irq(2, &cascade);
}


#if defined(CONFIG_ALPHA_GENERIC)
# define IACK_SC	alpha_mv.iack_sc
#elif defined(CONFIG_ALPHA_APECS)
# define IACK_SC	APECS_IACK_SC
#elif defined(CONFIG_ALPHA_LCA)
# define IACK_SC	LCA_IACK_SC
#elif defined(CONFIG_ALPHA_CIA)
# define IACK_SC	CIA_IACK_SC
#elif defined(CONFIG_ALPHA_PYXIS)
# define IACK_SC	PYXIS_IACK_SC
#elif defined(CONFIG_ALPHA_TITAN)
# define IACK_SC	TITAN_IACK_SC
#elif defined(CONFIG_ALPHA_TSUNAMI)
# define IACK_SC	TSUNAMI_IACK_SC
#elif defined(CONFIG_ALPHA_IRONGATE)
# define IACK_SC        IRONGATE_IACK_SC
#endif
/* Note that CONFIG_ALPHA_POLARIS is intentionally left out here, since
   sys_rx164 wants to use isa_no_iack_sc_device_interrupt for some reason.  */

#if defined(IACK_SC)
void
isa_device_interrupt(unsigned long vector)
{
	/*
	 * Generate a PCI interrupt acknowledge cycle.  The PIC will
	 * respond with the interrupt vector of the highest priority
	 * interrupt that is pending.  The PALcode sets up the
	 * interrupts vectors such that irq level L generates vector L.
	 */
	int j = *(vuip) IACK_SC;
	j &= 0xff;
	handle_irq(j);
}
#endif

#if defined(CONFIG_ALPHA_GENERIC) || !defined(IACK_SC)
void
isa_no_iack_sc_device_interrupt(unsigned long vector)
{
	unsigned long pic;

	/*
	 * It seems to me that the probability of two or more *device*
	 * interrupts occurring at almost exactly the same time is
	 * pretty low.  So why pay the price of checking for
	 * additional interrupts here if the common case can be
	 * handled so much easier?
	 */
	/* 
	 *  The first read of gives you *all* interrupting lines.
	 *  Therefore, read the mask register and and out those lines
	 *  not enabled.  Note that some documentation has 21 and a1 
	 *  write only.  This is not true.
	 */
	pic = inb(0x20) | (inb(0xA0) << 8);	/* read isr */
	pic &= 0xFFFB;				/* mask out cascade & hibits */

	while (pic) {
		int j = ffz(~pic);
		pic &= pic - 1;
		handle_irq(j);
	}
}
#endif
