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

/*******************************************************************************

  Intel 10 Gigabit PCI Express Linux driver
  Copyright(c) 1999 - 2008 Intel Corporation.

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information:
  e1000-devel Mailing List <e1000-devel@lists.sourceforge.net>
  Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497

*******************************************************************************/

#ifndef _IXGBE_PHY_H_
#define _IXGBE_PHY_H_

#include "ixgbe_type.h"
#define IXGBE_I2C_EEPROM_DEV_ADDR    0xA0

/* EEPROM byte offsets */
#define IXGBE_SFF_IDENTIFIER         0x0
#define IXGBE_SFF_IDENTIFIER_SFP     0x3
#define IXGBE_SFF_VENDOR_OUI_BYTE0   0x25
#define IXGBE_SFF_VENDOR_OUI_BYTE1   0x26
#define IXGBE_SFF_VENDOR_OUI_BYTE2   0x27
#define IXGBE_SFF_1GBE_COMP_CODES    0x6
#define IXGBE_SFF_10GBE_COMP_CODES   0x3
#define IXGBE_SFF_TRANSMISSION_MEDIA 0x9

/* Bitmasks */
#define IXGBE_SFF_TWIN_AX_CAPABLE            0x80
#define IXGBE_SFF_1GBASESX_CAPABLE           0x1
#define IXGBE_SFF_10GBASESR_CAPABLE          0x10
#define IXGBE_SFF_10GBASELR_CAPABLE          0x20
#define IXGBE_I2C_EEPROM_READ_MASK           0x100
#define IXGBE_I2C_EEPROM_STATUS_MASK         0x3
#define IXGBE_I2C_EEPROM_STATUS_NO_OPERATION 0x0
#define IXGBE_I2C_EEPROM_STATUS_PASS         0x1
#define IXGBE_I2C_EEPROM_STATUS_FAIL         0x2
#define IXGBE_I2C_EEPROM_STATUS_IN_PROGRESS  0x3

/* Bit-shift macros */
#define IXGBE_SFF_VENDOR_OUI_BYTE0_SHIFT    12
#define IXGBE_SFF_VENDOR_OUI_BYTE1_SHIFT    8
#define IXGBE_SFF_VENDOR_OUI_BYTE2_SHIFT    4

/* Vendor OUIs: format of OUI is 0x[byte0][byte1][byte2][00] */
#define IXGBE_SFF_VENDOR_OUI_TYCO     0x00407600
#define IXGBE_SFF_VENDOR_OUI_FTL      0x00906500
#define IXGBE_SFF_VENDOR_OUI_AVAGO    0x00176A00

/* I2C SDA and SCL timing parameters for standard mode */
#define IXGBE_I2C_T_HD_STA  4
#define IXGBE_I2C_T_LOW     5
#define IXGBE_I2C_T_HIGH    4
#define IXGBE_I2C_T_SU_STA  5
#define IXGBE_I2C_T_HD_DATA 5
#define IXGBE_I2C_T_SU_DATA 1
#define IXGBE_I2C_T_RISE    1
#define IXGBE_I2C_T_FALL    1
#define IXGBE_I2C_T_SU_STO  4
#define IXGBE_I2C_T_BUF     5


s32 ixgbe_init_phy_ops_generic(struct ixgbe_hw *hw);
s32 ixgbe_identify_phy_generic(struct ixgbe_hw *hw);
s32 ixgbe_reset_phy_generic(struct ixgbe_hw *hw);
s32 ixgbe_read_phy_reg_generic(struct ixgbe_hw *hw, u32 reg_addr,
                               u32 device_type, u16 *phy_data);
s32 ixgbe_write_phy_reg_generic(struct ixgbe_hw *hw, u32 reg_addr,
                                u32 device_type, u16 phy_data);
s32 ixgbe_setup_phy_link_generic(struct ixgbe_hw *hw);
s32 ixgbe_setup_phy_link_speed_generic(struct ixgbe_hw *hw,
                                       ixgbe_link_speed speed,
                                       bool autoneg,
                                       bool autoneg_wait_to_complete);

/* PHY specific */
s32 ixgbe_check_phy_link_tnx(struct ixgbe_hw *hw,
                             ixgbe_link_speed *speed,
                             bool *link_up);
s32 ixgbe_get_phy_firmware_version_tnx(struct ixgbe_hw *hw,
                                       u16 *firmware_version);

s32 ixgbe_reset_phy_nl(struct ixgbe_hw *hw);
s32 ixgbe_identify_sfp_module_generic(struct ixgbe_hw *hw);
s32 ixgbe_get_sfp_init_sequence_offsets(struct ixgbe_hw *hw,
                                        u16 *list_offset,
                                        u16 *data_offset);

#endif /* _IXGBE_PHY_H_ */
