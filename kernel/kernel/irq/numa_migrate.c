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
 * NUMA irq-desc migration code
 *
 * Migrate IRQ data structures (irq_desc, chip_data, etc.) over to
 * the new "home node" of the IRQ.
 */

#include <linux/irq.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>

#include "internals.h"

static void init_copy_kstat_irqs(struct irq_desc *old_desc,
				 struct irq_desc *desc,
				 int cpu, int nr)
{
	unsigned long bytes;

	init_kstat_irqs(desc, cpu, nr);

	if (desc->kstat_irqs != old_desc->kstat_irqs) {
		/* Compute how many bytes we need per irq and allocate them */
		bytes = nr * sizeof(unsigned int);

		memcpy(desc->kstat_irqs, old_desc->kstat_irqs, bytes);
	}
}

static void free_kstat_irqs(struct irq_desc *old_desc, struct irq_desc *desc)
{
	if (old_desc->kstat_irqs == desc->kstat_irqs)
		return;

	kfree(old_desc->kstat_irqs);
	old_desc->kstat_irqs = NULL;
}

static void init_copy_one_irq_desc(int irq, struct irq_desc *old_desc,
		 struct irq_desc *desc, int cpu)
{
	memcpy(desc, old_desc, sizeof(struct irq_desc));
	spin_lock_init(&desc->lock);
	desc->cpu = cpu;
	lockdep_set_class(&desc->lock, &irq_desc_lock_class);
	init_copy_kstat_irqs(old_desc, desc, cpu, nr_cpu_ids);
	arch_init_copy_chip_data(old_desc, desc, cpu);
}

static void free_one_irq_desc(struct irq_desc *old_desc, struct irq_desc *desc)
{
	free_kstat_irqs(old_desc, desc);
	arch_free_chip_data(old_desc, desc);
}

static struct irq_desc *__real_move_irq_desc(struct irq_desc *old_desc,
						int cpu)
{
	struct irq_desc *desc;
	unsigned int irq;
	unsigned long flags;
	int node;

	irq = old_desc->irq;

	spin_lock_irqsave(&sparse_irq_lock, flags);

	/* We have to check it to avoid races with another CPU */
	desc = irq_desc_ptrs[irq];

	if (desc && old_desc != desc)
		goto out_unlock;

	node = cpu_to_node(cpu);
	desc = kzalloc_node(sizeof(*desc), GFP_ATOMIC, node);
	if (!desc) {
		printk(KERN_ERR "irq %d: can not get new irq_desc for migration.\n", irq);
		/* still use old one */
		desc = old_desc;
		goto out_unlock;
	}
	init_copy_one_irq_desc(irq, old_desc, desc, cpu);

	irq_desc_ptrs[irq] = desc;
	spin_unlock_irqrestore(&sparse_irq_lock, flags);

	/* free the old one */
	free_one_irq_desc(old_desc, desc);
	spin_unlock(&old_desc->lock);
	kfree(old_desc);
	spin_lock(&desc->lock);

	return desc;

out_unlock:
	spin_unlock_irqrestore(&sparse_irq_lock, flags);

	return desc;
}

struct irq_desc *move_irq_desc(struct irq_desc *desc, int cpu)
{
	int old_cpu;
	int node, old_node;

	/* those all static, do move them */
	if (desc->irq < NR_IRQS_LEGACY)
		return desc;

	old_cpu = desc->cpu;
	if (old_cpu != cpu) {
		node = cpu_to_node(cpu);
		old_node = cpu_to_node(old_cpu);
		if (old_node != node)
			desc = __real_move_irq_desc(desc, cpu);
		else
			desc->cpu = cpu;
	}

	return desc;
}

