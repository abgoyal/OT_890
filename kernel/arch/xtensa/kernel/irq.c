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
 * linux/arch/xtensa/kernel/irq.c
 *
 * Xtensa built-in interrupt controller and some generic functions copied
 * from i386.
 *
 * Copyright (C) 2002 - 2006 Tensilica, Inc.
 * Copyright (C) 1992, 1998 Linus Torvalds, Ingo Molnar
 *
 *
 * Chris Zankel <chris@zankel.net>
 * Kevin Chea
 *
 */

#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel_stat.h>

#include <asm/uaccess.h>
#include <asm/platform.h>

static unsigned int cached_irq_mask;

atomic_t irq_err_count;

/*
 * 'what should we do if we get a hw irq event on an illegal vector'.
 * each architecture has to answer this themselves.
 */
void ack_bad_irq(unsigned int irq)
{
          printk("unexpected IRQ trap at vector %02x\n", irq);
}

/*
 * do_IRQ handles all normal device IRQ's (the special
 * SMP cross-CPU interrupts have their own specific
 * handlers).
 */

asmlinkage void do_IRQ(int irq, struct pt_regs *regs)
{
	struct pt_regs *old_regs = set_irq_regs(regs);
	struct irq_desc *desc = irq_desc + irq;

	if (irq >= NR_IRQS) {
		printk(KERN_EMERG "%s: cannot handle IRQ %d\n",
				__func__, irq);
	}

	irq_enter();

#ifdef CONFIG_DEBUG_STACKOVERFLOW
	/* Debugging check for stack overflow: is there less than 1KB free? */
	{
		unsigned long sp;

		__asm__ __volatile__ ("mov %0, a1\n" : "=a" (sp));
		sp &= THREAD_SIZE - 1;

		if (unlikely(sp < (sizeof(thread_info) + 1024)))
			printk("Stack overflow in do_IRQ: %ld\n",
			       sp - sizeof(struct thread_info));
	}
#endif
	desc->handle_irq(irq, desc);

	irq_exit();
	set_irq_regs(old_regs);
}

/*
 * Generic, controller-independent functions:
 */

int show_interrupts(struct seq_file *p, void *v)
{
	int i = *(loff_t *) v, j;
	struct irqaction * action;
	unsigned long flags;

	if (i == 0) {
		seq_printf(p, "           ");
		for_each_online_cpu(j)
			seq_printf(p, "CPU%d       ",j);
		seq_putc(p, '\n');
	}

	if (i < NR_IRQS) {
		spin_lock_irqsave(&irq_desc[i].lock, flags);
		action = irq_desc[i].action;
		if (!action)
			goto skip;
		seq_printf(p, "%3d: ",i);
#ifndef CONFIG_SMP
		seq_printf(p, "%10u ", kstat_irqs(i));
#else
		for_each_online_cpu(j)
			seq_printf(p, "%10u ", kstat_cpu(j).irqs[i]);
#endif
		seq_printf(p, " %14s", irq_desc[i].chip->typename);
		seq_printf(p, "  %s", action->name);

		for (action=action->next; action; action = action->next)
			seq_printf(p, ", %s", action->name);

		seq_putc(p, '\n');
skip:
		spin_unlock_irqrestore(&irq_desc[i].lock, flags);
	} else if (i == NR_IRQS) {
		seq_printf(p, "NMI: ");
		for_each_online_cpu(j)
			seq_printf(p, "%10u ", nmi_count(j));
		seq_putc(p, '\n');
		seq_printf(p, "ERR: %10u\n", atomic_read(&irq_err_count));
	}
	return 0;
}

static void xtensa_irq_mask(unsigned int irq)
{
	cached_irq_mask &= ~(1 << irq);
	set_sr (cached_irq_mask, INTENABLE);
}

static void xtensa_irq_unmask(unsigned int irq)
{
	cached_irq_mask |= 1 << irq;
	set_sr (cached_irq_mask, INTENABLE);
}

static void xtensa_irq_ack(unsigned int irq)
{
	set_sr(1 << irq, INTCLEAR);
}

static int xtensa_irq_retrigger(unsigned int irq)
{
	set_sr (1 << irq, INTSET);
	return 1;
}


static struct irq_chip xtensa_irq_chip = {
	.name		= "xtensa",
	.mask		= xtensa_irq_mask,
	.unmask		= xtensa_irq_unmask,
	.ack		= xtensa_irq_ack,
	.retrigger	= xtensa_irq_retrigger,
};

void __init init_IRQ(void)
{
	int index;

	for (index = 0; index < XTENSA_NR_IRQS; index++) {
		int mask = 1 << index;

		if (mask & XCHAL_INTTYPE_MASK_SOFTWARE)
			set_irq_chip_and_handler(index, &xtensa_irq_chip,
						 handle_simple_irq);

		else if (mask & XCHAL_INTTYPE_MASK_EXTERN_EDGE)
			set_irq_chip_and_handler(index, &xtensa_irq_chip,
						 handle_edge_irq);

		else if (mask & XCHAL_INTTYPE_MASK_EXTERN_LEVEL)
			set_irq_chip_and_handler(index, &xtensa_irq_chip,
						 handle_level_irq);

		else if (mask & XCHAL_INTTYPE_MASK_TIMER)
			set_irq_chip_and_handler(index, &xtensa_irq_chip,
						 handle_edge_irq);

		else	/* XCHAL_INTTYPE_MASK_WRITE_ERROR */
			/* XCHAL_INTTYPE_MASK_NMI */

			set_irq_chip_and_handler(index, &xtensa_irq_chip,
						 handle_level_irq);
	}

	cached_irq_mask = 0;
}
