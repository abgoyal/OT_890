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
 * @file mefirmware.c
 *
 * @brief Implements the firmware handling.
 * @note Copyright (C) 2007 Meilhaus Electronic GmbH (support@meilhaus.de)
 * @author Guenter Gebhardt
 * @author Krzysztof Gantzke	(k.gantzke@meilhaus.de)
 */

/***************************************************************************
 *   Copyright (C) 2007 Meilhaus Electronic GmbH (support@meilhaus.de)     *
 *   Copyright (C) 2007 by Krzysztof Gantzke k.gantzke@meilhaus.de         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __KERNEL__
# define __KERNEL__
#endif

#ifndef KBUILD_MODNAME
#  define KBUILD_MODNAME KBUILD_STR(mefirmware)
#endif

#include <linux/pci.h>
#include <linux/delay.h>

#include <linux/firmware.h>

#include "meplx_reg.h"
#include "medebug.h"

#include "mefirmware.h"

int me_xilinx_download(unsigned long register_base_control,
		       unsigned long register_base_data,
		       struct device *dev, const char *firmware_name)
{
	int err = ME_ERRNO_FIRMWARE;
	uint32_t value = 0;
	int idx = 0;

	const struct firmware *fw;

	PDEBUG("executed.\n");

	if (!firmware_name) {
		PERROR("Request for firmware failed. No name provided. \n");
		return err;
	}

	PINFO("Request '%s' firmware.\n", firmware_name);
	err = request_firmware(&fw, firmware_name, dev);

	if (err) {
		PERROR("Request for firmware failed.\n");
		return err;
	}
	// Set PLX local interrupt 2 polarity to high.
	// Interrupt is thrown by init pin of xilinx.
	outl(PLX_INTCSR_LOCAL_INT2_POL, register_base_control + PLX_INTCSR);

	// Set /CS and /WRITE of the Xilinx
	value = inl(register_base_control + PLX_ICR);
	value |= ME_FIRMWARE_CS_WRITE;
	outl(value, register_base_control + PLX_ICR);

	// Init Xilinx with CS1
	inl(register_base_data + ME_XILINX_CS1_REG);

	// Wait for init to complete
	udelay(20);

	// Checkl /INIT pin
	if (!
	    (inl(register_base_control + PLX_INTCSR) &
	     PLX_INTCSR_LOCAL_INT2_STATE)) {
		PERROR("Can't init Xilinx.\n");
		release_firmware(fw);
		return -EIO;
	}
	// Reset /CS and /WRITE of the Xilinx
	value = inl(register_base_control + PLX_ICR);
	value &= ~ME_FIRMWARE_CS_WRITE;
	outl(value, register_base_control + PLX_ICR);

	// Download Xilinx firmware
	udelay(10);

	for (idx = 0; idx < fw->size; idx++) {
		outl(fw->data[idx], register_base_data);
#ifdef ME6000_v2_4
///     This checking only for board's version 2.4
		// Check if BUSY flag is set (low = ready, high = busy)
		if (inl(register_base_control + PLX_ICR) &
		    ME_FIRMWARE_BUSY_FLAG) {
			PERROR("Xilinx is still busy (idx = %d)\n", idx);
			release_firmware(fw);
			return -EIO;
		}
#endif //ME6000_v2_4
	}
	PDEBUG("Download finished. %d bytes written to PLX.\n", idx);

	// If done flag is high download was successful
	if (inl(register_base_control + PLX_ICR) & ME_FIRMWARE_DONE_FLAG) {
		PDEBUG("SUCCESS. Done flag is set.\n");
	} else {
		PERROR("FAILURE. DONE flag is not set.\n");
		release_firmware(fw);
		return -EIO;
	}

	// Set /CS and /WRITE
	value = inl(register_base_control + PLX_ICR);
	value |= ME_FIRMWARE_CS_WRITE;
	outl(value, register_base_control + PLX_ICR);

	PDEBUG("Enable interrupts on the PCI interface.\n");
	outl(ME_PLX_PCI_ACTIVATE, register_base_control + PLX_INTCSR);
	release_firmware(fw);

	return 0;
}
