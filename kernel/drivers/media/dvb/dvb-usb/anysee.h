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
 * DVB USB Linux driver for Anysee E30 DVB-C & DVB-T USB2.0 receiver
 *
 * Copyright (C) 2007 Antti Palosaari <crope@iki.fi>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * TODO:
 * - add smart card reader support for Conditional Access (CA)
 *
 * Card reader in Anysee is nothing more than ISO 7816 card reader.
 * There is no hardware CAM in any Anysee device sold.
 * In my understanding it should be implemented by making own module
 * for ISO 7816 card reader, like dvb_ca_en50221 is implemented. This
 * module registers serial interface that can be used to communicate
 * with any ISO 7816 smart card.
 *
 * Any help according to implement serial smart card reader support
 * is highly welcome!
 */

#ifndef _DVB_USB_ANYSEE_H_
#define _DVB_USB_ANYSEE_H_

#define DVB_USB_LOG_PREFIX "anysee"
#include "dvb-usb.h"

#define deb_info(args...) dprintk(dvb_usb_anysee_debug, 0x01, args)
#define deb_xfer(args...) dprintk(dvb_usb_anysee_debug, 0x02, args)
#define deb_rc(args...)   dprintk(dvb_usb_anysee_debug, 0x04, args)
#define deb_reg(args...)  dprintk(dvb_usb_anysee_debug, 0x08, args)
#define deb_i2c(args...)  dprintk(dvb_usb_anysee_debug, 0x10, args)
#define deb_fw(args...)   dprintk(dvb_usb_anysee_debug, 0x20, args)

enum cmd {
	CMD_I2C_READ            = 0x33,
	CMD_I2C_WRITE           = 0x31,
	CMD_REG_READ            = 0xb0,
	CMD_REG_WRITE           = 0xb1,
	CMD_STREAMING_CTRL      = 0x12,
	CMD_LED_AND_IR_CTRL     = 0x16,
	CMD_GET_IR_CODE         = 0x41,
	CMD_GET_HW_INFO         = 0x19,
	CMD_SMARTCARD           = 0x34,
};

struct anysee_state {
	u8 tuner;
	u8 seq;
};

#endif

/***************************************************************************
 * USB API description (reverse engineered)
 ***************************************************************************

Transaction flow:
=================
BULK[00001] >>> REQUEST PACKET 64 bytes
BULK[00081] <<< REPLY PACKET #1 64 bytes (PREVIOUS TRANSACTION REPLY)
BULK[00081] <<< REPLY PACKET #2 64 bytes (CURRENT TRANSACTION REPLY)

General reply packet(s) are always used if not own reply defined.

============================================================================
| 00-63 | GENERAL REPLY PACKET #1 (PREVIOUS REPLY)
============================================================================
|    00 | reply data (if any) from previous transaction
|       | Just same reply packet as returned during previous transaction.
|       | Needed only if reply is missed in previous transaction.
|       | Just skip normally.
----------------------------------------------------------------------------
| 01-59 | don't care
----------------------------------------------------------------------------
|    60 | packet sequence number
----------------------------------------------------------------------------
| 61-63 | don't care
----------------------------------------------------------------------------

============================================================================
| 00-63 | GENERAL REPLY PACKET #2 (CURRENT REPLY)
============================================================================
|    00 | reply data (if any)
----------------------------------------------------------------------------
| 01-59 | don't care
----------------------------------------------------------------------------
|    60 | packet sequence number
----------------------------------------------------------------------------
| 61-63 | don't care
----------------------------------------------------------------------------

============================================================================
| 00-63 | I2C WRITE REQUEST PACKET
============================================================================
|    00 | 0x31 I2C write command
----------------------------------------------------------------------------
|    01 | i2c address
----------------------------------------------------------------------------
|    02 | data length
|       | 0x02 (for typical I2C reg / val pair)
----------------------------------------------------------------------------
|    03 | 0x01
----------------------------------------------------------------------------
| 04-   | data
----------------------------------------------------------------------------
|   -59 | don't care
----------------------------------------------------------------------------
|    60 | packet sequence number
----------------------------------------------------------------------------
| 61-63 | don't care
----------------------------------------------------------------------------

============================================================================
| 00-63 | I2C READ REQUEST PACKET
============================================================================
|    00 | 0x33 I2C read command
----------------------------------------------------------------------------
|    01 | i2c address + 1
----------------------------------------------------------------------------
|    02 | register
----------------------------------------------------------------------------
|    03 | 0x00
----------------------------------------------------------------------------
|    04 | 0x00
----------------------------------------------------------------------------
|    05 | 0x01
----------------------------------------------------------------------------
| 06-59 | don't care
----------------------------------------------------------------------------
|    60 | packet sequence number
----------------------------------------------------------------------------
| 61-63 | don't care
----------------------------------------------------------------------------

============================================================================
| 00-63 | USB CONTROLLER REGISTER WRITE REQUEST PACKET
============================================================================
|    00 | 0xb1 register write command
----------------------------------------------------------------------------
| 01-02 | register
----------------------------------------------------------------------------
|    03 | 0x01
----------------------------------------------------------------------------
|    04 | value
----------------------------------------------------------------------------
| 05-59 | don't care
----------------------------------------------------------------------------
|    60 | packet sequence number
----------------------------------------------------------------------------
| 61-63 | don't care
----------------------------------------------------------------------------

============================================================================
| 00-63 | USB CONTROLLER REGISTER READ REQUEST PACKET
============================================================================
|    00 | 0xb0 register read command
----------------------------------------------------------------------------
| 01-02 | register
----------------------------------------------------------------------------
|    03 | 0x01
----------------------------------------------------------------------------
| 04-59 | don't care
----------------------------------------------------------------------------
|    60 | packet sequence number
----------------------------------------------------------------------------
| 61-63 | don't care
----------------------------------------------------------------------------

============================================================================
| 00-63 | LED CONTROL REQUEST PACKET
============================================================================
|    00 | 0x16 LED and IR control command
----------------------------------------------------------------------------
|    01 | 0x01 (LED)
----------------------------------------------------------------------------
|    03 | 0x00 blink
|       | 0x01 lights continuously
----------------------------------------------------------------------------
|    04 | blink interval
|       | 0x00 fastest (looks like LED lights continuously)
|       | 0xff slowest
----------------------------------------------------------------------------
| 05-59 | don't care
----------------------------------------------------------------------------
|    60 | packet sequence number
----------------------------------------------------------------------------
| 61-63 | don't care
----------------------------------------------------------------------------

============================================================================
| 00-63 | IR CONTROL REQUEST PACKET
============================================================================
|    00 | 0x16 LED and IR control command
----------------------------------------------------------------------------
|    01 | 0x02 (IR)
----------------------------------------------------------------------------
|    03 | 0x00 IR disabled
|       | 0x01 IR enabled
----------------------------------------------------------------------------
| 04-59 | don't care
----------------------------------------------------------------------------
|    60 | packet sequence number
----------------------------------------------------------------------------
| 61-63 | don't care
----------------------------------------------------------------------------

============================================================================
| 00-63 | STREAMING CONTROL REQUEST PACKET
============================================================================
|    00 | 0x12 streaming control command
----------------------------------------------------------------------------
|    01 | 0x00 streaming disabled
|       | 0x01 streaming enabled
----------------------------------------------------------------------------
|    02 | 0x00
----------------------------------------------------------------------------
| 03-59 | don't care
----------------------------------------------------------------------------
|    60 | packet sequence number
----------------------------------------------------------------------------
| 61-63 | don't care
----------------------------------------------------------------------------

============================================================================
| 00-63 | REMOTE CONTROL REQUEST PACKET
============================================================================
|    00 | 0x41 remote control command
----------------------------------------------------------------------------
| 01-59 | don't care
----------------------------------------------------------------------------
|    60 | packet sequence number
----------------------------------------------------------------------------
| 61-63 | don't care
----------------------------------------------------------------------------

============================================================================
| 00-63 | REMOTE CONTROL REPLY PACKET
============================================================================
|    00 | 0x00 code not received
|       | 0x01 code received
----------------------------------------------------------------------------
|    01 | remote control code
----------------------------------------------------------------------------
| 02-59 | don't care
----------------------------------------------------------------------------
|    60 | packet sequence number
----------------------------------------------------------------------------
| 61-63 | don't care
----------------------------------------------------------------------------

============================================================================
| 00-63 | GET HARDWARE INFO REQUEST PACKET
============================================================================
|    00 | 0x19 get hardware info command
----------------------------------------------------------------------------
| 01-59 | don't care
----------------------------------------------------------------------------
|    60 | packet sequence number
----------------------------------------------------------------------------
| 61-63 | don't care
----------------------------------------------------------------------------

============================================================================
| 00-63 | GET HARDWARE INFO REPLY PACKET
============================================================================
|    00 | hardware id
----------------------------------------------------------------------------
| 01-02 | firmware version
----------------------------------------------------------------------------
| 03-59 | don't care
----------------------------------------------------------------------------
|    60 | packet sequence number
----------------------------------------------------------------------------
| 61-63 | don't care
----------------------------------------------------------------------------

============================================================================
| 00-63 | SMART CARD READER PACKET
============================================================================
|    00 | 0x34 smart card reader command
----------------------------------------------------------------------------
|    xx |
----------------------------------------------------------------------------
| xx-59 | don't care
----------------------------------------------------------------------------
|    60 | packet sequence number
----------------------------------------------------------------------------
| 61-63 | don't care
----------------------------------------------------------------------------

*/
