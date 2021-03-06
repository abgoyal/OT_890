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

/**
 * @file me6000_dio.c
 *
 * @brief ME-6000 digital input/output subdevice instance.
 * @note Copyright (C) 2007 Meilhaus Electronic GmbH (support@meilhaus.de)
 * @author Guenter Gebhardt
 * @author Krzysztof Gantzke	(k.gantzke@meilhaus.de)
 */

/*
 * Copyright (C) 2007 Meilhaus Electronic GmbH (support@meilhaus.de)
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#ifndef __KERNEL__
#  define __KERNEL__
#endif

/*
 * Includes
 */
#include <linux/module.h>

#include <linux/slab.h>
#include <linux/spinlock.h>
#include <asm/io.h>
#include <linux/types.h>

#include "medefines.h"
#include "meinternal.h"
#include "meerror.h"

#include "medebug.h"
#include "me6000_dio_reg.h"
#include "me6000_dio.h"

/*
 * Defines
 */

/*
 * Functions
 */

static int me6000_dio_io_reset_subdevice(struct me_subdevice *subdevice,
					 struct file *filep, int flags)
{
	me6000_dio_subdevice_t *instance;
	uint8_t mode;

	PDEBUG("executed.\n");

	instance = (me6000_dio_subdevice_t *) subdevice;

	if (flags) {
		PERROR("Invalid flag specified.\n");
		return ME_ERRNO_INVALID_FLAGS;
	}

	ME_SUBDEVICE_ENTER;

	spin_lock(&instance->subdevice_lock);
	spin_lock(instance->ctrl_reg_lock);
	mode = inb(instance->ctrl_reg);
	mode &= ~(0x3 << (instance->dio_idx * 2));
	outb(mode, instance->ctrl_reg);
	PDEBUG_REG("ctrl_reg outl(0x%lX+0x%lX)=0x%x\n", instance->reg_base,
		   instance->ctrl_reg - instance->reg_base, mode);
	spin_unlock(instance->ctrl_reg_lock);

	outb(0x00, instance->port_reg);
	PDEBUG_REG("ctrl_reg outl(0x%lX+0x%lX)=0x%x\n", instance->reg_base,
		   instance->ctrl_reg - instance->reg_base, 0x00);
	spin_unlock(&instance->subdevice_lock);

	ME_SUBDEVICE_EXIT;

	return ME_ERRNO_SUCCESS;
}

static int me6000_dio_io_single_config(me_subdevice_t * subdevice,
				       struct file *filep,
				       int channel,
				       int single_config,
				       int ref,
				       int trig_chan,
				       int trig_type, int trig_edge, int flags)
{
	me6000_dio_subdevice_t *instance;
	int err = ME_ERRNO_SUCCESS;
	uint8_t mode;
	int size =
	    flags & (ME_IO_SINGLE_CONFIG_DIO_BIT | ME_IO_SINGLE_CONFIG_DIO_BYTE
		     | ME_IO_SINGLE_CONFIG_DIO_WORD |
		     ME_IO_SINGLE_CONFIG_DIO_DWORD);

	PDEBUG("executed.\n");

	instance = (me6000_dio_subdevice_t *) subdevice;

	ME_SUBDEVICE_ENTER spin_lock(&instance->subdevice_lock);
	spin_lock(instance->ctrl_reg_lock);
	mode = inb(instance->ctrl_reg);
	switch (size) {
	case ME_IO_SINGLE_CONFIG_NO_FLAGS:
	case ME_IO_SINGLE_CONFIG_DIO_BYTE:
		if (channel == 0) {
			if (single_config == ME_SINGLE_CONFIG_DIO_INPUT) {
				mode &=
				    ~((ME6000_DIO_CTRL_BIT_MODE_0 |
				       ME6000_DIO_CTRL_BIT_MODE_1) <<
				      (instance->dio_idx * 2));
			} else if (single_config == ME_SINGLE_CONFIG_DIO_OUTPUT) {
				mode &=
				    ~((ME6000_DIO_CTRL_BIT_MODE_0 |
				       ME6000_DIO_CTRL_BIT_MODE_1) <<
				      (instance->dio_idx * 2));
				mode |=
				    ME6000_DIO_CTRL_BIT_MODE_0 << (instance->
								   dio_idx * 2);
			} else {
				PERROR
				    ("Invalid port configuration specified.\n");
				err = ME_ERRNO_INVALID_SINGLE_CONFIG;
			}
		} else {
			PERROR("Invalid channel number.\n");
			err = ME_ERRNO_INVALID_CHANNEL;
		}
		break;

	default:
		PERROR("Invalid flags.\n");
		err = ME_ERRNO_INVALID_FLAGS;
	}

	if (!err) {
		outb(mode, instance->ctrl_reg);
		PDEBUG_REG("ctrl_reg outl(0x%lX+0x%lX)=0x%x\n",
			   instance->reg_base,
			   instance->ctrl_reg - instance->reg_base, mode);
	}
	spin_unlock(instance->ctrl_reg_lock);
	spin_unlock(&instance->subdevice_lock);

	ME_SUBDEVICE_EXIT;

	return err;
}

static int me6000_dio_io_single_read(me_subdevice_t * subdevice,
				     struct file *filep,
				     int channel,
				     int *value, int time_out, int flags)
{
	me6000_dio_subdevice_t *instance;
	int err = ME_ERRNO_SUCCESS;
	uint8_t mode;

	PDEBUG("executed.\n");

	instance = (me6000_dio_subdevice_t *) subdevice;

	ME_SUBDEVICE_ENTER spin_lock(&instance->subdevice_lock);
	spin_lock(instance->ctrl_reg_lock);
	switch (flags) {
	case ME_IO_SINGLE_TYPE_DIO_BIT:
		if ((channel >= 0) && (channel < 8)) {
			mode =
			    inb(instance->
				ctrl_reg) & ((ME6000_DIO_CTRL_BIT_MODE_0 |
					      ME6000_DIO_CTRL_BIT_MODE_1) <<
					     (instance->dio_idx * 2));
			if ((mode ==
			     (ME6000_DIO_CTRL_BIT_MODE_0 <<
			      (instance->dio_idx * 2))) || !mode) {
				*value =
				    inb(instance->port_reg) & (0x1 << channel);
			} else {
				PERROR("Port not in output or input mode.\n");
				err = ME_ERRNO_PREVIOUS_CONFIG;
			}
		} else {
			PERROR("Invalid bit number specified.\n");
			err = ME_ERRNO_INVALID_CHANNEL;
		}
		break;

	case ME_IO_SINGLE_NO_FLAGS:
	case ME_IO_SINGLE_TYPE_DIO_BYTE:
		if (channel == 0) {
			mode =
			    inb(instance->
				ctrl_reg) & ((ME6000_DIO_CTRL_BIT_MODE_0 |
					      ME6000_DIO_CTRL_BIT_MODE_1) <<
					     (instance->dio_idx * 2));
			if ((mode ==
			     (ME6000_DIO_CTRL_BIT_MODE_0 <<
			      (instance->dio_idx * 2))) || !mode) {
				*value = inb(instance->port_reg) & 0x00FF;
			} else {
				PERROR("Port not in output or input mode.\n");
				err = ME_ERRNO_PREVIOUS_CONFIG;
			}
		} else {
			PERROR("Invalid byte number specified.\n");
			err = ME_ERRNO_INVALID_CHANNEL;
		}
		break;

	default:
		PERROR("Invalid flags specified.\n");
		err = ME_ERRNO_INVALID_FLAGS;
	}
	spin_unlock(instance->ctrl_reg_lock);
	spin_unlock(&instance->subdevice_lock);

	ME_SUBDEVICE_EXIT;

	return err;
}

static int me6000_dio_io_single_write(me_subdevice_t * subdevice,
				      struct file *filep,
				      int channel,
				      int value, int time_out, int flags)
{
	me6000_dio_subdevice_t *instance;
	int err = ME_ERRNO_SUCCESS;
	uint8_t mode;
	uint8_t byte;

	PDEBUG("executed.\n");

	instance = (me6000_dio_subdevice_t *) subdevice;

	ME_SUBDEVICE_ENTER spin_lock(&instance->subdevice_lock);
	spin_lock(instance->ctrl_reg_lock);
	switch (flags) {
	case ME_IO_SINGLE_TYPE_DIO_BIT:
		if ((channel >= 0) && (channel < 8)) {
			mode =
			    inb(instance->
				ctrl_reg) & ((ME6000_DIO_CTRL_BIT_MODE_0 |
					      ME6000_DIO_CTRL_BIT_MODE_1) <<
					     (instance->dio_idx * 2));

			if (mode ==
			    (ME6000_DIO_CTRL_BIT_MODE_0 <<
			     (instance->dio_idx * 2))) {
				byte = inb(instance->port_reg) & 0x00FF;

				if (value)
					byte |= 0x1 << channel;
				else
					byte &= ~(0x1 << channel);

				outb(byte, instance->port_reg);
			} else {
				PERROR("Port not in output or input mode.\n");
				err = ME_ERRNO_PREVIOUS_CONFIG;
			}
		} else {
			PERROR("Invalid bit number specified.\n");
			err = ME_ERRNO_INVALID_CHANNEL;
		}
		break;

	case ME_IO_SINGLE_NO_FLAGS:
	case ME_IO_SINGLE_TYPE_DIO_BYTE:
		if (channel == 0) {
			mode =
			    inb(instance->
				ctrl_reg) & ((ME6000_DIO_CTRL_BIT_MODE_0 |
					      ME6000_DIO_CTRL_BIT_MODE_1) <<
					     (instance->dio_idx * 2));

			if (mode ==
			    (ME6000_DIO_CTRL_BIT_MODE_0 <<
			     (instance->dio_idx * 2))) {
				outb(value, instance->port_reg);
			} else {
				PERROR("Port not in output or input mode.\n");
				err = ME_ERRNO_PREVIOUS_CONFIG;
			}
		} else {
			PERROR("Invalid byte number specified.\n");
			err = ME_ERRNO_INVALID_CHANNEL;
		}
		break;

	default:
		PERROR("Invalid flags specified.\n");
		err = ME_ERRNO_INVALID_FLAGS;
	}
	spin_unlock(instance->ctrl_reg_lock);
	spin_unlock(&instance->subdevice_lock);

	ME_SUBDEVICE_EXIT;

	return err;
}

static int me6000_dio_query_number_channels(me_subdevice_t * subdevice,
					    int *number)
{
	PDEBUG("executed.\n");
	*number = 8;
	return ME_ERRNO_SUCCESS;
}

static int me6000_dio_query_subdevice_type(me_subdevice_t * subdevice,
					   int *type, int *subtype)
{
	PDEBUG("executed.\n");
	*type = ME_TYPE_DIO;
	*subtype = ME_SUBTYPE_SINGLE;
	return ME_ERRNO_SUCCESS;
}

static int me6000_dio_query_subdevice_caps(me_subdevice_t * subdevice,
					   int *caps)
{
	PDEBUG("executed.\n");
	*caps = ME_CAPS_DIO_DIR_BYTE;
	return ME_ERRNO_SUCCESS;
}

me6000_dio_subdevice_t *me6000_dio_constructor(uint32_t reg_base,
					       unsigned int dio_idx,
					       spinlock_t * ctrl_reg_lock)
{
	me6000_dio_subdevice_t *subdevice;
	int err;

	PDEBUG("executed.\n");

	/* Allocate memory for subdevice instance */
	subdevice = kmalloc(sizeof(me6000_dio_subdevice_t), GFP_KERNEL);

	if (!subdevice) {
		PERROR("Cannot get memory for subdevice instance.\n");
		return NULL;
	}

	memset(subdevice, 0, sizeof(me6000_dio_subdevice_t));

	/* Initialize subdevice base class */
	err = me_subdevice_init(&subdevice->base);
	if (err) {
		PERROR("Cannot initialize subdevice base class instance.\n");
		kfree(subdevice);
		return NULL;
	}

	/* Set the subdevice ports */
	subdevice->ctrl_reg = reg_base + ME6000_DIO_CTRL_REG;
	switch (dio_idx) {
	case 0:
		subdevice->port_reg = reg_base + ME6000_DIO_PORT_0_REG;
		break;
	case 1:
		subdevice->port_reg = reg_base + ME6000_DIO_PORT_1_REG;
		break;
	default:
		err = ME_ERRNO_INVALID_SUBDEVICE;
	}

	if (err) {
		PERROR("Cannot initialize subdevice base class instance.\n");
		kfree(subdevice);
		return NULL;
	}
	// Initialize spin locks.
	spin_lock_init(&subdevice->subdevice_lock);

	subdevice->ctrl_reg_lock = ctrl_reg_lock;

	/* Save digital i/o index */
	subdevice->dio_idx = dio_idx;

#ifdef MEDEBUG_DEBUG_REG
	subdevice->reg_base = reg_base;
#endif

	/* Overload base class methods. */
	subdevice->base.me_subdevice_io_reset_subdevice =
	    me6000_dio_io_reset_subdevice;
	subdevice->base.me_subdevice_io_single_config =
	    me6000_dio_io_single_config;
	subdevice->base.me_subdevice_io_single_read = me6000_dio_io_single_read;
	subdevice->base.me_subdevice_io_single_write =
	    me6000_dio_io_single_write;
	subdevice->base.me_subdevice_query_number_channels =
	    me6000_dio_query_number_channels;
	subdevice->base.me_subdevice_query_subdevice_type =
	    me6000_dio_query_subdevice_type;
	subdevice->base.me_subdevice_query_subdevice_caps =
	    me6000_dio_query_subdevice_caps;

	return subdevice;
}
