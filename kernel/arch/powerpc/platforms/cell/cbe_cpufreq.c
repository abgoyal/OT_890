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
 * cpufreq driver for the cell processor
 *
 * (C) Copyright IBM Deutschland Entwicklung GmbH 2005-2007
 *
 * Author: Christian Krafft <krafft@de.ibm.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/cpufreq.h>
#include <linux/of_platform.h>

#include <asm/machdep.h>
#include <asm/prom.h>
#include <asm/cell-regs.h>
#include "cbe_cpufreq.h"

static DEFINE_MUTEX(cbe_switch_mutex);


/* the CBE supports an 8 step frequency scaling */
static struct cpufreq_frequency_table cbe_freqs[] = {
	{1,	0},
	{2,	0},
	{3,	0},
	{4,	0},
	{5,	0},
	{6,	0},
	{8,	0},
	{10,	0},
	{0,	CPUFREQ_TABLE_END},
};

/*
 * hardware specific functions
 */

static int set_pmode(unsigned int cpu, unsigned int slow_mode)
{
	int rc;

	if (cbe_cpufreq_has_pmi)
		rc = cbe_cpufreq_set_pmode_pmi(cpu, slow_mode);
	else
		rc = cbe_cpufreq_set_pmode(cpu, slow_mode);

	pr_debug("register contains slow mode %d\n", cbe_cpufreq_get_pmode(cpu));

	return rc;
}

/*
 * cpufreq functions
 */

static int cbe_cpufreq_cpu_init(struct cpufreq_policy *policy)
{
	const u32 *max_freqp;
	u32 max_freq;
	int i, cur_pmode;
	struct device_node *cpu;

	cpu = of_get_cpu_node(policy->cpu, NULL);

	if (!cpu)
		return -ENODEV;

	pr_debug("init cpufreq on CPU %d\n", policy->cpu);

	/*
	 * Let's check we can actually get to the CELL regs
	 */
	if (!cbe_get_cpu_pmd_regs(policy->cpu) ||
	    !cbe_get_cpu_mic_tm_regs(policy->cpu)) {
		pr_info("invalid CBE regs pointers for cpufreq\n");
		return -EINVAL;
	}

	max_freqp = of_get_property(cpu, "clock-frequency", NULL);

	of_node_put(cpu);

	if (!max_freqp)
		return -EINVAL;

	/* we need the freq in kHz */
	max_freq = *max_freqp / 1000;

	pr_debug("max clock-frequency is at %u kHz\n", max_freq);
	pr_debug("initializing frequency table\n");

	/* initialize frequency table */
	for (i=0; cbe_freqs[i].frequency!=CPUFREQ_TABLE_END; i++) {
		cbe_freqs[i].frequency = max_freq / cbe_freqs[i].index;
		pr_debug("%d: %d\n", i, cbe_freqs[i].frequency);
	}

	/* if DEBUG is enabled set_pmode() measures the latency
	 * of a transition */
	policy->cpuinfo.transition_latency = 25000;

	cur_pmode = cbe_cpufreq_get_pmode(policy->cpu);
	pr_debug("current pmode is at %d\n",cur_pmode);

	policy->cur = cbe_freqs[cur_pmode].frequency;

#ifdef CONFIG_SMP
	cpumask_copy(policy->cpus, &per_cpu(cpu_sibling_map, policy->cpu));
#endif

	cpufreq_frequency_table_get_attr(cbe_freqs, policy->cpu);

	/* this ensures that policy->cpuinfo_min
	 * and policy->cpuinfo_max are set correctly */
	return cpufreq_frequency_table_cpuinfo(policy, cbe_freqs);
}

static int cbe_cpufreq_cpu_exit(struct cpufreq_policy *policy)
{
	cpufreq_frequency_table_put_attr(policy->cpu);
	return 0;
}

static int cbe_cpufreq_verify(struct cpufreq_policy *policy)
{
	return cpufreq_frequency_table_verify(policy, cbe_freqs);
}

static int cbe_cpufreq_target(struct cpufreq_policy *policy,
			      unsigned int target_freq,
			      unsigned int relation)
{
	int rc;
	struct cpufreq_freqs freqs;
	unsigned int cbe_pmode_new;

	cpufreq_frequency_table_target(policy,
				       cbe_freqs,
				       target_freq,
				       relation,
				       &cbe_pmode_new);

	freqs.old = policy->cur;
	freqs.new = cbe_freqs[cbe_pmode_new].frequency;
	freqs.cpu = policy->cpu;

	mutex_lock(&cbe_switch_mutex);
	cpufreq_notify_transition(&freqs, CPUFREQ_PRECHANGE);

	pr_debug("setting frequency for cpu %d to %d kHz, " \
		 "1/%d of max frequency\n",
		 policy->cpu,
		 cbe_freqs[cbe_pmode_new].frequency,
		 cbe_freqs[cbe_pmode_new].index);

	rc = set_pmode(policy->cpu, cbe_pmode_new);

	cpufreq_notify_transition(&freqs, CPUFREQ_POSTCHANGE);
	mutex_unlock(&cbe_switch_mutex);

	return rc;
}

static struct cpufreq_driver cbe_cpufreq_driver = {
	.verify		= cbe_cpufreq_verify,
	.target		= cbe_cpufreq_target,
	.init		= cbe_cpufreq_cpu_init,
	.exit		= cbe_cpufreq_cpu_exit,
	.name		= "cbe-cpufreq",
	.owner		= THIS_MODULE,
	.flags		= CPUFREQ_CONST_LOOPS,
};

/*
 * module init and destoy
 */

static int __init cbe_cpufreq_init(void)
{
	if (!machine_is(cell))
		return -ENODEV;

	return cpufreq_register_driver(&cbe_cpufreq_driver);
}

static void __exit cbe_cpufreq_exit(void)
{
	cpufreq_unregister_driver(&cbe_cpufreq_driver);
}

module_init(cbe_cpufreq_init);
module_exit(cbe_cpufreq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Christian Krafft <krafft@de.ibm.com>");
