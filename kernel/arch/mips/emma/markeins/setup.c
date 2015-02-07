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
 *  arch/mips/emma2rh/markeins/setup.c
 *      This file is setup for EMMA2RH Mark-eins.
 *
 *  Copyright (C) NEC Electronics Corporation 2004-2006
 *
 *  This file is based on the arch/mips/ddb5xxx/ddb5477/setup.c.
 *
 *	Copyright 2001 MontaVista Software Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>

#include <asm/time.h>
#include <asm/reboot.h>

#include <asm/emma/emma2rh.h>

#define	USE_CPU_COUNTER_TIMER	/* whether we use cpu counter */

extern void markeins_led(const char *);

static int bus_frequency = 0;

static void markeins_machine_restart(char *command)
{
	static void (*back_to_prom) (void) = (void (*)(void))0xbfc00000;

	printk("cannot EMMA2RH Mark-eins restart.\n");
	markeins_led("restart.");
	back_to_prom();
}

static void markeins_machine_halt(void)
{
	printk("EMMA2RH Mark-eins halted.\n");
	markeins_led("halted.");
	while (1) ;
}

static void markeins_machine_power_off(void)
{
	printk("EMMA2RH Mark-eins halted. Please turn off the power.\n");
	markeins_led("poweroff.");
	while (1) ;
}

static unsigned long __initdata emma2rh_clock[4] = {
	166500000, 187312500, 199800000, 210600000
};

static unsigned int __init detect_bus_frequency(unsigned long rtc_base)
{
	u32 reg;

	/* detect from boot strap */
	reg = emma2rh_in32(EMMA2RH_BHIF_STRAP_0);
	reg = (reg >> 4) & 0x3;

	return emma2rh_clock[reg];
}

void __init plat_time_init(void)
{
	u32 reg;
	if (bus_frequency == 0)
		bus_frequency = detect_bus_frequency(0);

	reg = emma2rh_in32(EMMA2RH_BHIF_STRAP_0);
	if ((reg & 0x3) == 0)
		reg = (reg >> 6) & 0x3;
	else {
		reg = emma2rh_in32(EMMA2RH_BHIF_MAIN_CTRL);
		reg = (reg >> 4) & 0x3;
	}
	mips_hpt_frequency = (bus_frequency * (4 + reg)) / 4 / 2;
}

static void markeins_board_init(void);
extern void markeins_irq_setup(void);

static void inline __init markeins_sio_setup(void)
{
}

void __init plat_mem_setup(void)
{
	/* initialize board - we don't trust the loader */
	markeins_board_init();

	set_io_port_base(KSEG1ADDR(EMMA2RH_PCI_IO_BASE));

	_machine_restart = markeins_machine_restart;
	_machine_halt = markeins_machine_halt;
	pm_power_off = markeins_machine_power_off;

	/* setup resource limits */
	ioport_resource.start = EMMA2RH_PCI_IO_BASE;
	ioport_resource.end = EMMA2RH_PCI_IO_BASE + EMMA2RH_PCI_IO_SIZE - 1;
	iomem_resource.start = EMMA2RH_IO_BASE;
	iomem_resource.end = EMMA2RH_ROM_BASE - 1;

	/* Reboot on panic */
	panic_timeout = 180;

	markeins_sio_setup();
}

static void __init markeins_board_init(void)
{
	u32 val;

	val = emma2rh_in32(EMMA2RH_PBRD_INT_EN);	/* open serial interrupts. */
	emma2rh_out32(EMMA2RH_PBRD_INT_EN, val | 0xaa);
	val = emma2rh_in32(EMMA2RH_PBRD_CLKSEL);	/* set serial clocks. */
	emma2rh_out32(EMMA2RH_PBRD_CLKSEL, val | 0x5);	/* 18MHz */
	emma2rh_out32(EMMA2RH_PCI_CONTROL, 0);

	markeins_led("MVL E2RH");
}
