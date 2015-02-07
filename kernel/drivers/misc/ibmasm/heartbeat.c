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
 * IBM ASM Service Processor Device Driver
 *
 * This program is free software; you can redistribute it and/or modify
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Copyright (C) IBM Corporation, 2004
 *
 * Author: Max Asb�ck <amax@us.ibm.com>
 *
 */

#include <linux/notifier.h>
#include "ibmasm.h"
#include "dot_command.h"
#include "lowlevel.h"

static int suspend_heartbeats = 0;

/*
 * Once the driver indicates to the service processor that it is running
 * - see send_os_state() - the service processor sends periodic heartbeats
 * to the driver. The driver must respond to the heartbeats or else the OS
 * will be rebooted.
 * In the case of a panic the interrupt handler continues to work and thus
 * continues to respond to heartbeats, making the service processor believe
 * the OS is still running and thus preventing a reboot.
 * To prevent this from happening a callback is added the panic_notifier_list.
 * Before responding to a heartbeat the driver checks if a panic has happened,
 * if yes it suspends heartbeat, causing the service processor to reboot as
 * expected.
 */
static int panic_happened(struct notifier_block *n, unsigned long val, void *v)
{
	suspend_heartbeats = 1;
	return 0;
}

static struct notifier_block panic_notifier = { panic_happened, NULL, 1 };

void ibmasm_register_panic_notifier(void)
{
	atomic_notifier_chain_register(&panic_notifier_list, &panic_notifier);
}

void ibmasm_unregister_panic_notifier(void)
{
	atomic_notifier_chain_unregister(&panic_notifier_list,
			&panic_notifier);
}


int ibmasm_heartbeat_init(struct service_processor *sp)
{
	sp->heartbeat = ibmasm_new_command(sp, HEARTBEAT_BUFFER_SIZE);
	if (sp->heartbeat == NULL)
		return -ENOMEM;

	return 0;
}

void ibmasm_heartbeat_exit(struct service_processor *sp)
{
	char tsbuf[32];

	dbg("%s:%d at %s\n", __func__, __LINE__, get_timestamp(tsbuf));
	ibmasm_wait_for_response(sp->heartbeat, IBMASM_CMD_TIMEOUT_NORMAL);
	dbg("%s:%d at %s\n", __func__, __LINE__, get_timestamp(tsbuf));
	suspend_heartbeats = 1;
	command_put(sp->heartbeat);
}

void ibmasm_receive_heartbeat(struct service_processor *sp,  void *message, size_t size)
{
	struct command *cmd = sp->heartbeat;
	struct dot_command_header *header = (struct dot_command_header *)cmd->buffer;
	char tsbuf[32];

	dbg("%s:%d at %s\n", __func__, __LINE__, get_timestamp(tsbuf));
	if (suspend_heartbeats)
		return;

	/* return the received dot command to sender */
	cmd->status = IBMASM_CMD_PENDING;
	size = min(size, cmd->buffer_size);
	memcpy_fromio(cmd->buffer, message, size);
	header->type = sp_write;
	ibmasm_exec_command(sp, cmd);
}