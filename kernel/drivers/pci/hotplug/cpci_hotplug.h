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
 * CompactPCI Hot Plug Core Functions
 *
 * Copyright (C) 2002 SOMA Networks, Inc.
 * Copyright (C) 2001 Greg Kroah-Hartman (greg@kroah.com)
 * Copyright (C) 2001 IBM Corp.
 *
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 * NON INFRINGEMENT.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Send feedback to <scottm@somanetworks.com>
 */

#ifndef _CPCI_HOTPLUG_H
#define _CPCI_HOTPLUG_H

#include <linux/types.h>
#include <linux/pci.h>
#include <linux/pci_hotplug.h>

/* PICMG 2.1 R2.0 HS CSR bits: */
#define HS_CSR_INS	0x0080
#define HS_CSR_EXT	0x0040
#define HS_CSR_PI	0x0030
#define HS_CSR_LOO	0x0008
#define HS_CSR_PIE	0x0004
#define HS_CSR_EIM	0x0002
#define HS_CSR_DHA	0x0001

struct slot {
	u8 number;
	unsigned int devfn;
	struct pci_bus *bus;
	struct pci_dev *dev;
	unsigned int extracting;
	struct hotplug_slot *hotplug_slot;
	struct list_head slot_list;
};

struct cpci_hp_controller_ops {
	int (*query_enum) (void);
	int (*enable_irq) (void);
	int (*disable_irq) (void);
	int (*check_irq) (void *dev_id);
	int (*hardware_test) (struct slot* slot, u32 value);
	u8  (*get_power) (struct slot* slot);
	int (*set_power) (struct slot* slot, int value);
};

struct cpci_hp_controller {
	unsigned int irq;
	unsigned long irq_flags;
	char *devname;
	void *dev_id;
	char *name;
	struct cpci_hp_controller_ops *ops;
};

static inline const char *slot_name(struct slot *slot)
{
	return hotplug_slot_name(slot->hotplug_slot);
}

extern int cpci_hp_register_controller(struct cpci_hp_controller *controller);
extern int cpci_hp_unregister_controller(struct cpci_hp_controller *controller);
extern int cpci_hp_register_bus(struct pci_bus *bus, u8 first, u8 last);
extern int cpci_hp_unregister_bus(struct pci_bus *bus);
extern int cpci_hp_start(void);
extern int cpci_hp_stop(void);

/*
 * Internal function prototypes, these functions should not be used by
 * board/chassis drivers.
 */
extern u8 cpci_get_attention_status(struct slot *slot);
extern u8 cpci_get_latch_status(struct slot *slot);
extern u8 cpci_get_adapter_status(struct slot *slot);
extern u16 cpci_get_hs_csr(struct slot * slot);
extern int cpci_set_attention_status(struct slot *slot, int status);
extern int cpci_check_and_clear_ins(struct slot * slot);
extern int cpci_check_ext(struct slot * slot);
extern int cpci_clear_ext(struct slot * slot);
extern int cpci_led_on(struct slot * slot);
extern int cpci_led_off(struct slot * slot);
extern int cpci_configure_slot(struct slot *slot);
extern int cpci_unconfigure_slot(struct slot *slot);

#endif	/* _CPCI_HOTPLUG_H */
