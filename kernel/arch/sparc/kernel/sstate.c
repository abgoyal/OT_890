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

/* sstate.c: System soft state support.
 *
 * Copyright (C) 2007, 2008 David S. Miller <davem@davemloft.net>
 */

#include <linux/kernel.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/init.h>

#include <asm/hypervisor.h>
#include <asm/spitfire.h>
#include <asm/oplib.h>
#include <asm/head.h>
#include <asm/io.h>

static int hv_supports_soft_state;

static unsigned long kimage_addr_to_ra(const char *p)
{
	unsigned long val = (unsigned long) p;

	return kern_base + (val - KERNBASE);
}

static void do_set_sstate(unsigned long state, const char *msg)
{
	unsigned long err;

	if (!hv_supports_soft_state)
		return;

	err = sun4v_mach_set_soft_state(state, kimage_addr_to_ra(msg));
	if (err) {
		printk(KERN_WARNING "SSTATE: Failed to set soft-state to "
		       "state[%lx] msg[%s], err=%lu\n",
		       state, msg, err);
	}
}

static const char booting_msg[32] __attribute__((aligned(32))) =
	"Linux booting";
static const char running_msg[32] __attribute__((aligned(32))) =
	"Linux running";
static const char halting_msg[32] __attribute__((aligned(32))) =
	"Linux halting";
static const char poweroff_msg[32] __attribute__((aligned(32))) =
	"Linux powering off";
static const char rebooting_msg[32] __attribute__((aligned(32))) =
	"Linux rebooting";
static const char panicing_msg[32] __attribute__((aligned(32))) =
	"Linux panicing";

static int sstate_reboot_call(struct notifier_block *np, unsigned long type, void *_unused)
{
	const char *msg;

	switch (type) {
	case SYS_DOWN:
	default:
		msg = rebooting_msg;
		break;

	case SYS_HALT:
		msg = halting_msg;
		break;

	case SYS_POWER_OFF:
		msg = poweroff_msg;
		break;
	}

	do_set_sstate(HV_SOFT_STATE_TRANSITION, msg);

	return NOTIFY_OK;
}

static struct notifier_block sstate_reboot_notifier = {
	.notifier_call = sstate_reboot_call,
};

static int sstate_panic_event(struct notifier_block *n, unsigned long event, void *ptr)
{
	do_set_sstate(HV_SOFT_STATE_TRANSITION, panicing_msg);

	return NOTIFY_DONE;
}

static struct notifier_block sstate_panic_block = {
	.notifier_call	=	sstate_panic_event,
	.priority	=	INT_MAX,
};

static int __init sstate_init(void)
{
	unsigned long major, minor;

	if (tlb_type != hypervisor)
		return 0;

	major = 1;
	minor = 0;
	if (sun4v_hvapi_register(HV_GRP_SOFT_STATE, major, &minor))
		return 0;

	hv_supports_soft_state = 1;

	prom_sun4v_guest_soft_state();

	do_set_sstate(HV_SOFT_STATE_TRANSITION, booting_msg);

	atomic_notifier_chain_register(&panic_notifier_list,
				       &sstate_panic_block);
	register_reboot_notifier(&sstate_reboot_notifier);

	return 0;
}

core_initcall(sstate_init);

static int __init sstate_running(void)
{
	do_set_sstate(HV_SOFT_STATE_NORMAL, running_msg);
	return 0;
}

late_initcall(sstate_running);
