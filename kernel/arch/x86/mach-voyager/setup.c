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
 *	Machine specific setup for generic
 */

#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/arch_hooks.h>
#include <asm/voyager.h>
#include <asm/e820.h>
#include <asm/io.h>
#include <asm/setup.h>

void __init pre_intr_init_hook(void)
{
	init_ISA_irqs();
}

/*
 * IRQ2 is cascade interrupt to second interrupt controller
 */
static struct irqaction irq2 = {
	.handler = no_action,
	.mask = CPU_MASK_NONE,
	.name = "cascade",
};

void __init intr_init_hook(void)
{
#ifdef CONFIG_SMP
	voyager_smp_intr_init();
#endif

	setup_irq(2, &irq2);
}

static void voyager_disable_tsc(void)
{
	/* Voyagers run their CPUs from independent clocks, so disable
	 * the TSC code because we can't sync them */
	setup_clear_cpu_cap(X86_FEATURE_TSC);
}

void __init pre_setup_arch_hook(void)
{
	voyager_disable_tsc();
}

void __init pre_time_init_hook(void)
{
	voyager_disable_tsc();
}

void __init trap_init_hook(void)
{
}

static struct irqaction irq0 = {
	.handler = timer_interrupt,
	.flags = IRQF_DISABLED | IRQF_NOBALANCING | IRQF_IRQPOLL | IRQF_TIMER,
	.mask = CPU_MASK_NONE,
	.name = "timer"
};

void __init time_init_hook(void)
{
	irq0.mask = cpumask_of_cpu(safe_smp_processor_id());
	setup_irq(0, &irq0);
}

/* Hook for machine specific memory setup. */

char *__init machine_specific_memory_setup(void)
{
	char *who;
	int new_nr;

	who = "NOT VOYAGER";

	if (voyager_level == 5) {
		__u32 addr, length;
		int i;

		who = "Voyager-SUS";

		e820.nr_map = 0;
		for (i = 0; voyager_memory_detect(i, &addr, &length); i++) {
			e820_add_region(addr, length, E820_RAM);
		}
		return who;
	} else if (voyager_level == 4) {
		__u32 tom;
		__u16 catbase = inb(VOYAGER_SSPB_RELOCATION_PORT) << 8;
		/* select the DINO config space */
		outb(VOYAGER_DINO, VOYAGER_CAT_CONFIG_PORT);
		/* Read DINO top of memory register */
		tom = ((inb(catbase + 0x4) & 0xf0) << 16)
		    + ((inb(catbase + 0x5) & 0x7f) << 24);

		if (inb(catbase) != VOYAGER_DINO) {
			printk(KERN_ERR
			       "Voyager: Failed to get DINO for L4, setting tom to EXT_MEM_K\n");
			tom = (boot_params.screen_info.ext_mem_k) << 10;
		}
		who = "Voyager-TOM";
		e820_add_region(0, 0x9f000, E820_RAM);
		/* map from 1M to top of memory */
		e820_add_region(1 * 1024 * 1024, tom - 1 * 1024 * 1024,
				  E820_RAM);
		/* FIXME: Should check the ASICs to see if I need to
		 * take out the 8M window.  Just do it at the moment
		 * */
		e820_add_region(8 * 1024 * 1024, 8 * 1024 * 1024,
				  E820_RESERVED);
		return who;
	}

	return default_machine_specific_memory_setup();
}
