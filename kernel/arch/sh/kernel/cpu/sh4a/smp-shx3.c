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
 * SH-X3 SMP
 *
 *  Copyright (C) 2007 - 2008  Paul Mundt
 *  Copyright (C) 2007  Magnus Damm
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */
#include <linux/init.h>
#include <linux/cpumask.h>
#include <linux/smp.h>
#include <linux/interrupt.h>
#include <linux/io.h>

static irqreturn_t ipi_interrupt_handler(int irq, void *arg)
{
	unsigned int message = (unsigned int)(long)arg;
	unsigned int cpu = hard_smp_processor_id();
	unsigned int offs = 4 * cpu;
	unsigned int x;

	x = ctrl_inl(0xfe410070 + offs); /* C0INITICI..CnINTICI */
	x &= (1 << (message << 2));
	ctrl_outl(x, 0xfe410080 + offs); /* C0INTICICLR..CnINTICICLR */

	smp_message_recv(message);

	return IRQ_HANDLED;
}

void __init plat_smp_setup(void)
{
	unsigned int cpu = 0;
	int i, num;

	cpus_clear(cpu_possible_map);
	cpu_set(cpu, cpu_possible_map);

	__cpu_number_map[0] = 0;
	__cpu_logical_map[0] = 0;

	/*
	 * Do this stupidly for now.. we don't have an easy way to probe
	 * for the total number of cores.
	 */
	for (i = 1, num = 0; i < NR_CPUS; i++) {
		cpu_set(i, cpu_possible_map);
		__cpu_number_map[i] = ++num;
		__cpu_logical_map[num] = i;
	}

        printk(KERN_INFO "Detected %i available secondary CPU(s)\n", num);
}

void __init plat_prepare_cpus(unsigned int max_cpus)
{
	int i;

	BUILD_BUG_ON(SMP_MSG_NR >= 8);

	for (i = 0; i < SMP_MSG_NR; i++)
		request_irq(104 + i, ipi_interrupt_handler, IRQF_DISABLED,
			    "IPI", (void *)(long)i);
}

#define STBCR_REG(phys_id) (0xfe400004 | (phys_id << 12))
#define RESET_REG(phys_id) (0xfe400008 | (phys_id << 12))

#define STBCR_MSTP	0x00000001
#define STBCR_RESET	0x00000002
#define STBCR_LTSLP	0x80000000

#define STBCR_AP_VAL	(STBCR_RESET | STBCR_LTSLP)

void plat_start_cpu(unsigned int cpu, unsigned long entry_point)
{
	ctrl_outl(entry_point, RESET_REG(cpu));

	if (!(ctrl_inl(STBCR_REG(cpu)) & STBCR_MSTP))
		ctrl_outl(STBCR_MSTP, STBCR_REG(cpu));

	while (!(ctrl_inl(STBCR_REG(cpu)) & STBCR_MSTP))
		cpu_relax();

	/* Start up secondary processor by sending a reset */
	ctrl_outl(STBCR_AP_VAL, STBCR_REG(cpu));
}

int plat_smp_processor_id(void)
{
	return ctrl_inl(0xff000048); /* CPIDR */
}

void plat_send_ipi(unsigned int cpu, unsigned int message)
{
	unsigned long addr = 0xfe410070 + (cpu * 4);

	BUG_ON(cpu >= 4);

	ctrl_outl(1 << (message << 2), addr); /* C0INTICI..CnINTICI */
}
