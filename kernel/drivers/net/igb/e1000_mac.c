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

  Intel(R) Gigabit Ethernet Linux driver
  Copyright(c) 2007 Intel Corporation.

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

#include <linux/if_ether.h>
#include <linux/delay.h>
#include <linux/pci.h>
#include <linux/netdevice.h>

#include "e1000_mac.h"

#include "igb.h"

static s32 igb_set_default_fc(struct e1000_hw *hw);
static s32 igb_set_fc_watermarks(struct e1000_hw *hw);

/**
 *  igb_remove_device - Free device specific structure
 *  @hw: pointer to the HW structure
 *
 *  If a device specific structure was allocated, this function will
 *  free it.
 **/
void igb_remove_device(struct e1000_hw *hw)
{
	/* Freeing the dev_spec member of e1000_hw structure */
	kfree(hw->dev_spec);
}

static s32 igb_read_pcie_cap_reg(struct e1000_hw *hw, u32 reg, u16 *value)
{
	struct igb_adapter *adapter = hw->back;
	u16 cap_offset;

	cap_offset = pci_find_capability(adapter->pdev, PCI_CAP_ID_EXP);
	if (!cap_offset)
		return -E1000_ERR_CONFIG;

	pci_read_config_word(adapter->pdev, cap_offset + reg, value);

	return 0;
}

/**
 *  igb_get_bus_info_pcie - Get PCIe bus information
 *  @hw: pointer to the HW structure
 *
 *  Determines and stores the system bus information for a particular
 *  network interface.  The following bus information is determined and stored:
 *  bus speed, bus width, type (PCIe), and PCIe function.
 **/
s32 igb_get_bus_info_pcie(struct e1000_hw *hw)
{
	struct e1000_bus_info *bus = &hw->bus;
	s32 ret_val;
	u32 reg;
	u16 pcie_link_status;

	bus->type = e1000_bus_type_pci_express;
	bus->speed = e1000_bus_speed_2500;

	ret_val = igb_read_pcie_cap_reg(hw,
					  PCIE_LINK_STATUS,
					  &pcie_link_status);
	if (ret_val)
		bus->width = e1000_bus_width_unknown;
	else
		bus->width = (enum e1000_bus_width)((pcie_link_status &
						     PCIE_LINK_WIDTH_MASK) >>
						     PCIE_LINK_WIDTH_SHIFT);

	reg = rd32(E1000_STATUS);
	bus->func = (reg & E1000_STATUS_FUNC_MASK) >> E1000_STATUS_FUNC_SHIFT;

	return 0;
}

/**
 *  igb_clear_vfta - Clear VLAN filter table
 *  @hw: pointer to the HW structure
 *
 *  Clears the register array which contains the VLAN filter table by
 *  setting all the values to 0.
 **/
void igb_clear_vfta(struct e1000_hw *hw)
{
	u32 offset;

	for (offset = 0; offset < E1000_VLAN_FILTER_TBL_SIZE; offset++) {
		array_wr32(E1000_VFTA, offset, 0);
		wrfl();
	}
}

/**
 *  igb_write_vfta - Write value to VLAN filter table
 *  @hw: pointer to the HW structure
 *  @offset: register offset in VLAN filter table
 *  @value: register value written to VLAN filter table
 *
 *  Writes value at the given offset in the register array which stores
 *  the VLAN filter table.
 **/
void igb_write_vfta(struct e1000_hw *hw, u32 offset, u32 value)
{
	array_wr32(E1000_VFTA, offset, value);
	wrfl();
}

/**
 *  igb_check_alt_mac_addr - Check for alternate MAC addr
 *  @hw: pointer to the HW structure
 *
 *  Checks the nvm for an alternate MAC address.  An alternate MAC address
 *  can be setup by pre-boot software and must be treated like a permanent
 *  address and must override the actual permanent MAC address.  If an
 *  alternate MAC address is fopund it is saved in the hw struct and
 *  prgrammed into RAR0 and the cuntion returns success, otherwise the
 *  fucntion returns an error.
 **/
s32 igb_check_alt_mac_addr(struct e1000_hw *hw)
{
	u32 i;
	s32 ret_val = 0;
	u16 offset, nvm_alt_mac_addr_offset, nvm_data;
	u8 alt_mac_addr[ETH_ALEN];

	ret_val = hw->nvm.ops.read_nvm(hw, NVM_ALT_MAC_ADDR_PTR, 1,
				 &nvm_alt_mac_addr_offset);
	if (ret_val) {
		hw_dbg("NVM Read Error\n");
		goto out;
	}

	if (nvm_alt_mac_addr_offset == 0xFFFF) {
		ret_val = -(E1000_NOT_IMPLEMENTED);
		goto out;
	}

	if (hw->bus.func == E1000_FUNC_1)
		nvm_alt_mac_addr_offset += ETH_ALEN/sizeof(u16);

	for (i = 0; i < ETH_ALEN; i += 2) {
		offset = nvm_alt_mac_addr_offset + (i >> 1);
		ret_val = hw->nvm.ops.read_nvm(hw, offset, 1, &nvm_data);
		if (ret_val) {
			hw_dbg("NVM Read Error\n");
			goto out;
		}

		alt_mac_addr[i] = (u8)(nvm_data & 0xFF);
		alt_mac_addr[i + 1] = (u8)(nvm_data >> 8);
	}

	/* if multicast bit is set, the alternate address will not be used */
	if (alt_mac_addr[0] & 0x01) {
		ret_val = -(E1000_NOT_IMPLEMENTED);
		goto out;
	}

	for (i = 0; i < ETH_ALEN; i++)
		hw->mac.addr[i] = hw->mac.perm_addr[i] = alt_mac_addr[i];

	hw->mac.ops.rar_set(hw, hw->mac.perm_addr, 0);

out:
	return ret_val;
}

/**
 *  igb_rar_set - Set receive address register
 *  @hw: pointer to the HW structure
 *  @addr: pointer to the receive address
 *  @index: receive address array register
 *
 *  Sets the receive address array register at index to the address passed
 *  in by addr.
 **/
void igb_rar_set(struct e1000_hw *hw, u8 *addr, u32 index)
{
	u32 rar_low, rar_high;

	/*
	 * HW expects these in little endian so we reverse the byte order
	 * from network order (big endian) to little endian
	 */
	rar_low = ((u32) addr[0] |
		   ((u32) addr[1] << 8) |
		    ((u32) addr[2] << 16) | ((u32) addr[3] << 24));

	rar_high = ((u32) addr[4] | ((u32) addr[5] << 8));

	if (!hw->mac.disable_av)
		rar_high |= E1000_RAH_AV;

	wr32(E1000_RAL(index), rar_low);
	wr32(E1000_RAH(index), rar_high);
}

/**
 *  igb_mta_set - Set multicast filter table address
 *  @hw: pointer to the HW structure
 *  @hash_value: determines the MTA register and bit to set
 *
 *  The multicast table address is a register array of 32-bit registers.
 *  The hash_value is used to determine what register the bit is in, the
 *  current value is read, the new bit is OR'd in and the new value is
 *  written back into the register.
 **/
void igb_mta_set(struct e1000_hw *hw, u32 hash_value)
{
	u32 hash_bit, hash_reg, mta;

	/*
	 * The MTA is a register array of 32-bit registers. It is
	 * treated like an array of (32*mta_reg_count) bits.  We want to
	 * set bit BitArray[hash_value]. So we figure out what register
	 * the bit is in, read it, OR in the new bit, then write
	 * back the new value.  The (hw->mac.mta_reg_count - 1) serves as a
	 * mask to bits 31:5 of the hash value which gives us the
	 * register we're modifying.  The hash bit within that register
	 * is determined by the lower 5 bits of the hash value.
	 */
	hash_reg = (hash_value >> 5) & (hw->mac.mta_reg_count - 1);
	hash_bit = hash_value & 0x1F;

	mta = array_rd32(E1000_MTA, hash_reg);

	mta |= (1 << hash_bit);

	array_wr32(E1000_MTA, hash_reg, mta);
	wrfl();
}

/**
 *  igb_hash_mc_addr - Generate a multicast hash value
 *  @hw: pointer to the HW structure
 *  @mc_addr: pointer to a multicast address
 *
 *  Generates a multicast address hash value which is used to determine
 *  the multicast filter table array address and new table value.  See
 *  igb_mta_set()
 **/
u32 igb_hash_mc_addr(struct e1000_hw *hw, u8 *mc_addr)
{
	u32 hash_value, hash_mask;
	u8 bit_shift = 0;

	/* Register count multiplied by bits per register */
	hash_mask = (hw->mac.mta_reg_count * 32) - 1;

	/*
	 * For a mc_filter_type of 0, bit_shift is the number of left-shifts
	 * where 0xFF would still fall within the hash mask.
	 */
	while (hash_mask >> bit_shift != 0xFF)
		bit_shift++;

	/*
	 * The portion of the address that is used for the hash table
	 * is determined by the mc_filter_type setting.
	 * The algorithm is such that there is a total of 8 bits of shifting.
	 * The bit_shift for a mc_filter_type of 0 represents the number of
	 * left-shifts where the MSB of mc_addr[5] would still fall within
	 * the hash_mask.  Case 0 does this exactly.  Since there are a total
	 * of 8 bits of shifting, then mc_addr[4] will shift right the
	 * remaining number of bits. Thus 8 - bit_shift.  The rest of the
	 * cases are a variation of this algorithm...essentially raising the
	 * number of bits to shift mc_addr[5] left, while still keeping the
	 * 8-bit shifting total.
	 *
	 * For example, given the following Destination MAC Address and an
	 * mta register count of 128 (thus a 4096-bit vector and 0xFFF mask),
	 * we can see that the bit_shift for case 0 is 4.  These are the hash
	 * values resulting from each mc_filter_type...
	 * [0] [1] [2] [3] [4] [5]
	 * 01  AA  00  12  34  56
	 * LSB                 MSB
	 *
	 * case 0: hash_value = ((0x34 >> 4) | (0x56 << 4)) & 0xFFF = 0x563
	 * case 1: hash_value = ((0x34 >> 3) | (0x56 << 5)) & 0xFFF = 0xAC6
	 * case 2: hash_value = ((0x34 >> 2) | (0x56 << 6)) & 0xFFF = 0x163
	 * case 3: hash_value = ((0x34 >> 0) | (0x56 << 8)) & 0xFFF = 0x634
	 */
	switch (hw->mac.mc_filter_type) {
	default:
	case 0:
		break;
	case 1:
		bit_shift += 1;
		break;
	case 2:
		bit_shift += 2;
		break;
	case 3:
		bit_shift += 4;
		break;
	}

	hash_value = hash_mask & (((mc_addr[4] >> (8 - bit_shift)) |
				  (((u16) mc_addr[5]) << bit_shift)));

	return hash_value;
}

/**
 *  igb_clear_hw_cntrs_base - Clear base hardware counters
 *  @hw: pointer to the HW structure
 *
 *  Clears the base hardware counters by reading the counter registers.
 **/
void igb_clear_hw_cntrs_base(struct e1000_hw *hw)
{
	u32 temp;

	temp = rd32(E1000_CRCERRS);
	temp = rd32(E1000_SYMERRS);
	temp = rd32(E1000_MPC);
	temp = rd32(E1000_SCC);
	temp = rd32(E1000_ECOL);
	temp = rd32(E1000_MCC);
	temp = rd32(E1000_LATECOL);
	temp = rd32(E1000_COLC);
	temp = rd32(E1000_DC);
	temp = rd32(E1000_SEC);
	temp = rd32(E1000_RLEC);
	temp = rd32(E1000_XONRXC);
	temp = rd32(E1000_XONTXC);
	temp = rd32(E1000_XOFFRXC);
	temp = rd32(E1000_XOFFTXC);
	temp = rd32(E1000_FCRUC);
	temp = rd32(E1000_GPRC);
	temp = rd32(E1000_BPRC);
	temp = rd32(E1000_MPRC);
	temp = rd32(E1000_GPTC);
	temp = rd32(E1000_GORCL);
	temp = rd32(E1000_GORCH);
	temp = rd32(E1000_GOTCL);
	temp = rd32(E1000_GOTCH);
	temp = rd32(E1000_RNBC);
	temp = rd32(E1000_RUC);
	temp = rd32(E1000_RFC);
	temp = rd32(E1000_ROC);
	temp = rd32(E1000_RJC);
	temp = rd32(E1000_TORL);
	temp = rd32(E1000_TORH);
	temp = rd32(E1000_TOTL);
	temp = rd32(E1000_TOTH);
	temp = rd32(E1000_TPR);
	temp = rd32(E1000_TPT);
	temp = rd32(E1000_MPTC);
	temp = rd32(E1000_BPTC);
}

/**
 *  igb_check_for_copper_link - Check for link (Copper)
 *  @hw: pointer to the HW structure
 *
 *  Checks to see of the link status of the hardware has changed.  If a
 *  change in link status has been detected, then we read the PHY registers
 *  to get the current speed/duplex if link exists.
 **/
s32 igb_check_for_copper_link(struct e1000_hw *hw)
{
	struct e1000_mac_info *mac = &hw->mac;
	s32 ret_val;
	bool link;

	/*
	 * We only want to go out to the PHY registers to see if Auto-Neg
	 * has completed and/or if our link status has changed.  The
	 * get_link_status flag is set upon receiving a Link Status
	 * Change or Rx Sequence Error interrupt.
	 */
	if (!mac->get_link_status) {
		ret_val = 0;
		goto out;
	}

	/*
	 * First we want to see if the MII Status Register reports
	 * link.  If so, then we want to get the current speed/duplex
	 * of the PHY.
	 */
	ret_val = igb_phy_has_link(hw, 1, 0, &link);
	if (ret_val)
		goto out;

	if (!link)
		goto out; /* No link detected */

	mac->get_link_status = false;

	/*
	 * Check if there was DownShift, must be checked
	 * immediately after link-up
	 */
	igb_check_downshift(hw);

	/*
	 * If we are forcing speed/duplex, then we simply return since
	 * we have already determined whether we have link or not.
	 */
	if (!mac->autoneg) {
		ret_val = -E1000_ERR_CONFIG;
		goto out;
	}

	/*
	 * Auto-Neg is enabled.  Auto Speed Detection takes care
	 * of MAC speed/duplex configuration.  So we only need to
	 * configure Collision Distance in the MAC.
	 */
	igb_config_collision_dist(hw);

	/*
	 * Configure Flow Control now that Auto-Neg has completed.
	 * First, we need to restore the desired flow control
	 * settings because we may have had to re-autoneg with a
	 * different link partner.
	 */
	ret_val = igb_config_fc_after_link_up(hw);
	if (ret_val)
		hw_dbg("Error configuring flow control\n");

out:
	return ret_val;
}

/**
 *  igb_setup_link - Setup flow control and link settings
 *  @hw: pointer to the HW structure
 *
 *  Determines which flow control settings to use, then configures flow
 *  control.  Calls the appropriate media-specific link configuration
 *  function.  Assuming the adapter has a valid link partner, a valid link
 *  should be established.  Assumes the hardware has previously been reset
 *  and the transmitter and receiver are not enabled.
 **/
s32 igb_setup_link(struct e1000_hw *hw)
{
	s32 ret_val = 0;

	/*
	 * In the case of the phy reset being blocked, we already have a link.
	 * We do not need to set it up again.
	 */
	if (igb_check_reset_block(hw))
		goto out;

	ret_val = igb_set_default_fc(hw);
	if (ret_val)
		goto out;

	/*
	 * We want to save off the original Flow Control configuration just
	 * in case we get disconnected and then reconnected into a different
	 * hub or switch with different Flow Control capabilities.
	 */
	hw->fc.original_type = hw->fc.type;

	hw_dbg("After fix-ups FlowControl is now = %x\n", hw->fc.type);

	/* Call the necessary media_type subroutine to configure the link. */
	ret_val = hw->mac.ops.setup_physical_interface(hw);
	if (ret_val)
		goto out;

	/*
	 * Initialize the flow control address, type, and PAUSE timer
	 * registers to their default values.  This is done even if flow
	 * control is disabled, because it does not hurt anything to
	 * initialize these registers.
	 */
	hw_dbg("Initializing the Flow Control address, type and timer regs\n");
	wr32(E1000_FCT, FLOW_CONTROL_TYPE);
	wr32(E1000_FCAH, FLOW_CONTROL_ADDRESS_HIGH);
	wr32(E1000_FCAL, FLOW_CONTROL_ADDRESS_LOW);

	wr32(E1000_FCTTV, hw->fc.pause_time);

	ret_val = igb_set_fc_watermarks(hw);

out:
	return ret_val;
}

/**
 *  igb_config_collision_dist - Configure collision distance
 *  @hw: pointer to the HW structure
 *
 *  Configures the collision distance to the default value and is used
 *  during link setup. Currently no func pointer exists and all
 *  implementations are handled in the generic version of this function.
 **/
void igb_config_collision_dist(struct e1000_hw *hw)
{
	u32 tctl;

	tctl = rd32(E1000_TCTL);

	tctl &= ~E1000_TCTL_COLD;
	tctl |= E1000_COLLISION_DISTANCE << E1000_COLD_SHIFT;

	wr32(E1000_TCTL, tctl);
	wrfl();
}

/**
 *  igb_set_fc_watermarks - Set flow control high/low watermarks
 *  @hw: pointer to the HW structure
 *
 *  Sets the flow control high/low threshold (watermark) registers.  If
 *  flow control XON frame transmission is enabled, then set XON frame
 *  tansmission as well.
 **/
static s32 igb_set_fc_watermarks(struct e1000_hw *hw)
{
	s32 ret_val = 0;
	u32 fcrtl = 0, fcrth = 0;

	/*
	 * Set the flow control receive threshold registers.  Normally,
	 * these registers will be set to a default threshold that may be
	 * adjusted later by the driver's runtime code.  However, if the
	 * ability to transmit pause frames is not enabled, then these
	 * registers will be set to 0.
	 */
	if (hw->fc.type & e1000_fc_tx_pause) {
		/*
		 * We need to set up the Receive Threshold high and low water
		 * marks as well as (optionally) enabling the transmission of
		 * XON frames.
		 */
		fcrtl = hw->fc.low_water;
		if (hw->fc.send_xon)
			fcrtl |= E1000_FCRTL_XONE;

		fcrth = hw->fc.high_water;
	}
	wr32(E1000_FCRTL, fcrtl);
	wr32(E1000_FCRTH, fcrth);

	return ret_val;
}

/**
 *  igb_set_default_fc - Set flow control default values
 *  @hw: pointer to the HW structure
 *
 *  Read the EEPROM for the default values for flow control and store the
 *  values.
 **/
static s32 igb_set_default_fc(struct e1000_hw *hw)
{
	s32 ret_val = 0;
	u16 nvm_data;

	/*
	 * Read and store word 0x0F of the EEPROM. This word contains bits
	 * that determine the hardware's default PAUSE (flow control) mode,
	 * a bit that determines whether the HW defaults to enabling or
	 * disabling auto-negotiation, and the direction of the
	 * SW defined pins. If there is no SW over-ride of the flow
	 * control setting, then the variable hw->fc will
	 * be initialized based on a value in the EEPROM.
	 */
	ret_val = hw->nvm.ops.read_nvm(hw, NVM_INIT_CONTROL2_REG, 1,
				       &nvm_data);

	if (ret_val) {
		hw_dbg("NVM Read Error\n");
		goto out;
	}

	if ((nvm_data & NVM_WORD0F_PAUSE_MASK) == 0)
		hw->fc.type = e1000_fc_none;
	else if ((nvm_data & NVM_WORD0F_PAUSE_MASK) ==
		 NVM_WORD0F_ASM_DIR)
		hw->fc.type = e1000_fc_tx_pause;
	else
		hw->fc.type = e1000_fc_full;

out:
	return ret_val;
}

/**
 *  igb_force_mac_fc - Force the MAC's flow control settings
 *  @hw: pointer to the HW structure
 *
 *  Force the MAC's flow control settings.  Sets the TFCE and RFCE bits in the
 *  device control register to reflect the adapter settings.  TFCE and RFCE
 *  need to be explicitly set by software when a copper PHY is used because
 *  autonegotiation is managed by the PHY rather than the MAC.  Software must
 *  also configure these bits when link is forced on a fiber connection.
 **/
s32 igb_force_mac_fc(struct e1000_hw *hw)
{
	u32 ctrl;
	s32 ret_val = 0;

	ctrl = rd32(E1000_CTRL);

	/*
	 * Because we didn't get link via the internal auto-negotiation
	 * mechanism (we either forced link or we got link via PHY
	 * auto-neg), we have to manually enable/disable transmit an
	 * receive flow control.
	 *
	 * The "Case" statement below enables/disable flow control
	 * according to the "hw->fc.type" parameter.
	 *
	 * The possible values of the "fc" parameter are:
	 *      0:  Flow control is completely disabled
	 *      1:  Rx flow control is enabled (we can receive pause
	 *          frames but not send pause frames).
	 *      2:  Tx flow control is enabled (we can send pause frames
	 *          frames but we do not receive pause frames).
	 *      3:  Both Rx and TX flow control (symmetric) is enabled.
	 *  other:  No other values should be possible at this point.
	 */
	hw_dbg("hw->fc.type = %u\n", hw->fc.type);

	switch (hw->fc.type) {
	case e1000_fc_none:
		ctrl &= (~(E1000_CTRL_TFCE | E1000_CTRL_RFCE));
		break;
	case e1000_fc_rx_pause:
		ctrl &= (~E1000_CTRL_TFCE);
		ctrl |= E1000_CTRL_RFCE;
		break;
	case e1000_fc_tx_pause:
		ctrl &= (~E1000_CTRL_RFCE);
		ctrl |= E1000_CTRL_TFCE;
		break;
	case e1000_fc_full:
		ctrl |= (E1000_CTRL_TFCE | E1000_CTRL_RFCE);
		break;
	default:
		hw_dbg("Flow control param set incorrectly\n");
		ret_val = -E1000_ERR_CONFIG;
		goto out;
	}

	wr32(E1000_CTRL, ctrl);

out:
	return ret_val;
}

/**
 *  igb_config_fc_after_link_up - Configures flow control after link
 *  @hw: pointer to the HW structure
 *
 *  Checks the status of auto-negotiation after link up to ensure that the
 *  speed and duplex were not forced.  If the link needed to be forced, then
 *  flow control needs to be forced also.  If auto-negotiation is enabled
 *  and did not fail, then we configure flow control based on our link
 *  partner.
 **/
s32 igb_config_fc_after_link_up(struct e1000_hw *hw)
{
	struct e1000_mac_info *mac = &hw->mac;
	s32 ret_val = 0;
	u16 mii_status_reg, mii_nway_adv_reg, mii_nway_lp_ability_reg;
	u16 speed, duplex;

	/*
	 * Check for the case where we have fiber media and auto-neg failed
	 * so we had to force link.  In this case, we need to force the
	 * configuration of the MAC to match the "fc" parameter.
	 */
	if (mac->autoneg_failed) {
		if (hw->phy.media_type == e1000_media_type_fiber ||
		    hw->phy.media_type == e1000_media_type_internal_serdes)
			ret_val = igb_force_mac_fc(hw);
	} else {
		if (hw->phy.media_type == e1000_media_type_copper)
			ret_val = igb_force_mac_fc(hw);
	}

	if (ret_val) {
		hw_dbg("Error forcing flow control settings\n");
		goto out;
	}

	/*
	 * Check for the case where we have copper media and auto-neg is
	 * enabled.  In this case, we need to check and see if Auto-Neg
	 * has completed, and if so, how the PHY and link partner has
	 * flow control configured.
	 */
	if ((hw->phy.media_type == e1000_media_type_copper) && mac->autoneg) {
		/*
		 * Read the MII Status Register and check to see if AutoNeg
		 * has completed.  We read this twice because this reg has
		 * some "sticky" (latched) bits.
		 */
		ret_val = hw->phy.ops.read_phy_reg(hw, PHY_STATUS,
						   &mii_status_reg);
		if (ret_val)
			goto out;
		ret_val = hw->phy.ops.read_phy_reg(hw, PHY_STATUS,
						   &mii_status_reg);
		if (ret_val)
			goto out;

		if (!(mii_status_reg & MII_SR_AUTONEG_COMPLETE)) {
			hw_dbg("Copper PHY and Auto Neg "
				 "has not completed.\n");
			goto out;
		}

		/*
		 * The AutoNeg process has completed, so we now need to
		 * read both the Auto Negotiation Advertisement
		 * Register (Address 4) and the Auto_Negotiation Base
		 * Page Ability Register (Address 5) to determine how
		 * flow control was negotiated.
		 */
		ret_val = hw->phy.ops.read_phy_reg(hw, PHY_AUTONEG_ADV,
					    &mii_nway_adv_reg);
		if (ret_val)
			goto out;
		ret_val = hw->phy.ops.read_phy_reg(hw, PHY_LP_ABILITY,
					    &mii_nway_lp_ability_reg);
		if (ret_val)
			goto out;

		/*
		 * Two bits in the Auto Negotiation Advertisement Register
		 * (Address 4) and two bits in the Auto Negotiation Base
		 * Page Ability Register (Address 5) determine flow control
		 * for both the PHY and the link partner.  The following
		 * table, taken out of the IEEE 802.3ab/D6.0 dated March 25,
		 * 1999, describes these PAUSE resolution bits and how flow
		 * control is determined based upon these settings.
		 * NOTE:  DC = Don't Care
		 *
		 *   LOCAL DEVICE  |   LINK PARTNER
		 * PAUSE | ASM_DIR | PAUSE | ASM_DIR | NIC Resolution
		 *-------|---------|-------|---------|--------------------
		 *   0   |    0    |  DC   |   DC    | e1000_fc_none
		 *   0   |    1    |   0   |   DC    | e1000_fc_none
		 *   0   |    1    |   1   |    0    | e1000_fc_none
		 *   0   |    1    |   1   |    1    | e1000_fc_tx_pause
		 *   1   |    0    |   0   |   DC    | e1000_fc_none
		 *   1   |   DC    |   1   |   DC    | e1000_fc_full
		 *   1   |    1    |   0   |    0    | e1000_fc_none
		 *   1   |    1    |   0   |    1    | e1000_fc_rx_pause
		 *
		 * Are both PAUSE bits set to 1?  If so, this implies
		 * Symmetric Flow Control is enabled at both ends.  The
		 * ASM_DIR bits are irrelevant per the spec.
		 *
		 * For Symmetric Flow Control:
		 *
		 *   LOCAL DEVICE  |   LINK PARTNER
		 * PAUSE | ASM_DIR | PAUSE | ASM_DIR | Result
		 *-------|---------|-------|---------|--------------------
		 *   1   |   DC    |   1   |   DC    | E1000_fc_full
		 *
		 */
		if ((mii_nway_adv_reg & NWAY_AR_PAUSE) &&
		    (mii_nway_lp_ability_reg & NWAY_LPAR_PAUSE)) {
			/*
			 * Now we need to check if the user selected RX ONLY
			 * of pause frames.  In this case, we had to advertise
			 * FULL flow control because we could not advertise RX
			 * ONLY. Hence, we must now check to see if we need to
			 * turn OFF  the TRANSMISSION of PAUSE frames.
			 */
			if (hw->fc.original_type == e1000_fc_full) {
				hw->fc.type = e1000_fc_full;
				hw_dbg("Flow Control = FULL.\r\n");
			} else {
				hw->fc.type = e1000_fc_rx_pause;
				hw_dbg("Flow Control = "
				       "RX PAUSE frames only.\r\n");
			}
		}
		/*
		 * For receiving PAUSE frames ONLY.
		 *
		 *   LOCAL DEVICE  |   LINK PARTNER
		 * PAUSE | ASM_DIR | PAUSE | ASM_DIR | Result
		 *-------|---------|-------|---------|--------------------
		 *   0   |    1    |   1   |    1    | e1000_fc_tx_pause
		 */
		else if (!(mii_nway_adv_reg & NWAY_AR_PAUSE) &&
			  (mii_nway_adv_reg & NWAY_AR_ASM_DIR) &&
			  (mii_nway_lp_ability_reg & NWAY_LPAR_PAUSE) &&
			  (mii_nway_lp_ability_reg & NWAY_LPAR_ASM_DIR)) {
			hw->fc.type = e1000_fc_tx_pause;
			hw_dbg("Flow Control = TX PAUSE frames only.\r\n");
		}
		/*
		 * For transmitting PAUSE frames ONLY.
		 *
		 *   LOCAL DEVICE  |   LINK PARTNER
		 * PAUSE | ASM_DIR | PAUSE | ASM_DIR | Result
		 *-------|---------|-------|---------|--------------------
		 *   1   |    1    |   0   |    1    | e1000_fc_rx_pause
		 */
		else if ((mii_nway_adv_reg & NWAY_AR_PAUSE) &&
			 (mii_nway_adv_reg & NWAY_AR_ASM_DIR) &&
			 !(mii_nway_lp_ability_reg & NWAY_LPAR_PAUSE) &&
			 (mii_nway_lp_ability_reg & NWAY_LPAR_ASM_DIR)) {
			hw->fc.type = e1000_fc_rx_pause;
			hw_dbg("Flow Control = RX PAUSE frames only.\r\n");
		}
		/*
		 * Per the IEEE spec, at this point flow control should be
		 * disabled.  However, we want to consider that we could
		 * be connected to a legacy switch that doesn't advertise
		 * desired flow control, but can be forced on the link
		 * partner.  So if we advertised no flow control, that is
		 * what we will resolve to.  If we advertised some kind of
		 * receive capability (Rx Pause Only or Full Flow Control)
		 * and the link partner advertised none, we will configure
		 * ourselves to enable Rx Flow Control only.  We can do
		 * this safely for two reasons:  If the link partner really
		 * didn't want flow control enabled, and we enable Rx, no
		 * harm done since we won't be receiving any PAUSE frames
		 * anyway.  If the intent on the link partner was to have
		 * flow control enabled, then by us enabling RX only, we
		 * can at least receive pause frames and process them.
		 * This is a good idea because in most cases, since we are
		 * predominantly a server NIC, more times than not we will
		 * be asked to delay transmission of packets than asking
		 * our link partner to pause transmission of frames.
		 */
		else if ((hw->fc.original_type == e1000_fc_none ||
			  hw->fc.original_type == e1000_fc_tx_pause) ||
			 hw->fc.strict_ieee) {
			hw->fc.type = e1000_fc_none;
			hw_dbg("Flow Control = NONE.\r\n");
		} else {
			hw->fc.type = e1000_fc_rx_pause;
			hw_dbg("Flow Control = RX PAUSE frames only.\r\n");
		}

		/*
		 * Now we need to do one last check...  If we auto-
		 * negotiated to HALF DUPLEX, flow control should not be
		 * enabled per IEEE 802.3 spec.
		 */
		ret_val = hw->mac.ops.get_speed_and_duplex(hw, &speed, &duplex);
		if (ret_val) {
			hw_dbg("Error getting link speed and duplex\n");
			goto out;
		}

		if (duplex == HALF_DUPLEX)
			hw->fc.type = e1000_fc_none;

		/*
		 * Now we call a subroutine to actually force the MAC
		 * controller to use the correct flow control settings.
		 */
		ret_val = igb_force_mac_fc(hw);
		if (ret_val) {
			hw_dbg("Error forcing flow control settings\n");
			goto out;
		}
	}

out:
	return ret_val;
}

/**
 *  igb_get_speed_and_duplex_copper - Retreive current speed/duplex
 *  @hw: pointer to the HW structure
 *  @speed: stores the current speed
 *  @duplex: stores the current duplex
 *
 *  Read the status register for the current speed/duplex and store the current
 *  speed and duplex for copper connections.
 **/
s32 igb_get_speed_and_duplex_copper(struct e1000_hw *hw, u16 *speed,
				      u16 *duplex)
{
	u32 status;

	status = rd32(E1000_STATUS);
	if (status & E1000_STATUS_SPEED_1000) {
		*speed = SPEED_1000;
		hw_dbg("1000 Mbs, ");
	} else if (status & E1000_STATUS_SPEED_100) {
		*speed = SPEED_100;
		hw_dbg("100 Mbs, ");
	} else {
		*speed = SPEED_10;
		hw_dbg("10 Mbs, ");
	}

	if (status & E1000_STATUS_FD) {
		*duplex = FULL_DUPLEX;
		hw_dbg("Full Duplex\n");
	} else {
		*duplex = HALF_DUPLEX;
		hw_dbg("Half Duplex\n");
	}

	return 0;
}

/**
 *  igb_get_hw_semaphore - Acquire hardware semaphore
 *  @hw: pointer to the HW structure
 *
 *  Acquire the HW semaphore to access the PHY or NVM
 **/
s32 igb_get_hw_semaphore(struct e1000_hw *hw)
{
	u32 swsm;
	s32 ret_val = 0;
	s32 timeout = hw->nvm.word_size + 1;
	s32 i = 0;

	/* Get the SW semaphore */
	while (i < timeout) {
		swsm = rd32(E1000_SWSM);
		if (!(swsm & E1000_SWSM_SMBI))
			break;

		udelay(50);
		i++;
	}

	if (i == timeout) {
		hw_dbg("Driver can't access device - SMBI bit is set.\n");
		ret_val = -E1000_ERR_NVM;
		goto out;
	}

	/* Get the FW semaphore. */
	for (i = 0; i < timeout; i++) {
		swsm = rd32(E1000_SWSM);
		wr32(E1000_SWSM, swsm | E1000_SWSM_SWESMBI);

		/* Semaphore acquired if bit latched */
		if (rd32(E1000_SWSM) & E1000_SWSM_SWESMBI)
			break;

		udelay(50);
	}

	if (i == timeout) {
		/* Release semaphores */
		igb_put_hw_semaphore(hw);
		hw_dbg("Driver can't access the NVM\n");
		ret_val = -E1000_ERR_NVM;
		goto out;
	}

out:
	return ret_val;
}

/**
 *  igb_put_hw_semaphore - Release hardware semaphore
 *  @hw: pointer to the HW structure
 *
 *  Release hardware semaphore used to access the PHY or NVM
 **/
void igb_put_hw_semaphore(struct e1000_hw *hw)
{
	u32 swsm;

	swsm = rd32(E1000_SWSM);

	swsm &= ~(E1000_SWSM_SMBI | E1000_SWSM_SWESMBI);

	wr32(E1000_SWSM, swsm);
}

/**
 *  igb_get_auto_rd_done - Check for auto read completion
 *  @hw: pointer to the HW structure
 *
 *  Check EEPROM for Auto Read done bit.
 **/
s32 igb_get_auto_rd_done(struct e1000_hw *hw)
{
	s32 i = 0;
	s32 ret_val = 0;


	while (i < AUTO_READ_DONE_TIMEOUT) {
		if (rd32(E1000_EECD) & E1000_EECD_AUTO_RD)
			break;
		msleep(1);
		i++;
	}

	if (i == AUTO_READ_DONE_TIMEOUT) {
		hw_dbg("Auto read by HW from NVM has not completed.\n");
		ret_val = -E1000_ERR_RESET;
		goto out;
	}

out:
	return ret_val;
}

/**
 *  igb_valid_led_default - Verify a valid default LED config
 *  @hw: pointer to the HW structure
 *  @data: pointer to the NVM (EEPROM)
 *
 *  Read the EEPROM for the current default LED configuration.  If the
 *  LED configuration is not valid, set to a valid LED configuration.
 **/
static s32 igb_valid_led_default(struct e1000_hw *hw, u16 *data)
{
	s32 ret_val;

	ret_val = hw->nvm.ops.read_nvm(hw, NVM_ID_LED_SETTINGS, 1, data);
	if (ret_val) {
		hw_dbg("NVM Read Error\n");
		goto out;
	}

	if (*data == ID_LED_RESERVED_0000 || *data == ID_LED_RESERVED_FFFF)
		*data = ID_LED_DEFAULT;

out:
	return ret_val;
}

/**
 *  igb_id_led_init -
 *  @hw: pointer to the HW structure
 *
 **/
s32 igb_id_led_init(struct e1000_hw *hw)
{
	struct e1000_mac_info *mac = &hw->mac;
	s32 ret_val;
	const u32 ledctl_mask = 0x000000FF;
	const u32 ledctl_on = E1000_LEDCTL_MODE_LED_ON;
	const u32 ledctl_off = E1000_LEDCTL_MODE_LED_OFF;
	u16 data, i, temp;
	const u16 led_mask = 0x0F;

	ret_val = igb_valid_led_default(hw, &data);
	if (ret_val)
		goto out;

	mac->ledctl_default = rd32(E1000_LEDCTL);
	mac->ledctl_mode1 = mac->ledctl_default;
	mac->ledctl_mode2 = mac->ledctl_default;

	for (i = 0; i < 4; i++) {
		temp = (data >> (i << 2)) & led_mask;
		switch (temp) {
		case ID_LED_ON1_DEF2:
		case ID_LED_ON1_ON2:
		case ID_LED_ON1_OFF2:
			mac->ledctl_mode1 &= ~(ledctl_mask << (i << 3));
			mac->ledctl_mode1 |= ledctl_on << (i << 3);
			break;
		case ID_LED_OFF1_DEF2:
		case ID_LED_OFF1_ON2:
		case ID_LED_OFF1_OFF2:
			mac->ledctl_mode1 &= ~(ledctl_mask << (i << 3));
			mac->ledctl_mode1 |= ledctl_off << (i << 3);
			break;
		default:
			/* Do nothing */
			break;
		}
		switch (temp) {
		case ID_LED_DEF1_ON2:
		case ID_LED_ON1_ON2:
		case ID_LED_OFF1_ON2:
			mac->ledctl_mode2 &= ~(ledctl_mask << (i << 3));
			mac->ledctl_mode2 |= ledctl_on << (i << 3);
			break;
		case ID_LED_DEF1_OFF2:
		case ID_LED_ON1_OFF2:
		case ID_LED_OFF1_OFF2:
			mac->ledctl_mode2 &= ~(ledctl_mask << (i << 3));
			mac->ledctl_mode2 |= ledctl_off << (i << 3);
			break;
		default:
			/* Do nothing */
			break;
		}
	}

out:
	return ret_val;
}

/**
 *  igb_cleanup_led - Set LED config to default operation
 *  @hw: pointer to the HW structure
 *
 *  Remove the current LED configuration and set the LED configuration
 *  to the default value, saved from the EEPROM.
 **/
s32 igb_cleanup_led(struct e1000_hw *hw)
{
	wr32(E1000_LEDCTL, hw->mac.ledctl_default);
	return 0;
}

/**
 *  igb_blink_led - Blink LED
 *  @hw: pointer to the HW structure
 *
 *  Blink the led's which are set to be on.
 **/
s32 igb_blink_led(struct e1000_hw *hw)
{
	u32 ledctl_blink = 0;
	u32 i;

	if (hw->phy.media_type == e1000_media_type_fiber) {
		/* always blink LED0 for PCI-E fiber */
		ledctl_blink = E1000_LEDCTL_LED0_BLINK |
		     (E1000_LEDCTL_MODE_LED_ON << E1000_LEDCTL_LED0_MODE_SHIFT);
	} else {
		/*
		 * set the blink bit for each LED that's "on" (0x0E)
		 * in ledctl_mode2
		 */
		ledctl_blink = hw->mac.ledctl_mode2;
		for (i = 0; i < 4; i++)
			if (((hw->mac.ledctl_mode2 >> (i * 8)) & 0xFF) ==
			    E1000_LEDCTL_MODE_LED_ON)
				ledctl_blink |= (E1000_LEDCTL_LED0_BLINK <<
						 (i * 8));
	}

	wr32(E1000_LEDCTL, ledctl_blink);

	return 0;
}

/**
 *  igb_led_off - Turn LED off
 *  @hw: pointer to the HW structure
 *
 *  Turn LED off.
 **/
s32 igb_led_off(struct e1000_hw *hw)
{
	u32 ctrl;

	switch (hw->phy.media_type) {
	case e1000_media_type_fiber:
		ctrl = rd32(E1000_CTRL);
		ctrl |= E1000_CTRL_SWDPIN0;
		ctrl |= E1000_CTRL_SWDPIO0;
		wr32(E1000_CTRL, ctrl);
		break;
	case e1000_media_type_copper:
		wr32(E1000_LEDCTL, hw->mac.ledctl_mode1);
		break;
	default:
		break;
	}

	return 0;
}

/**
 *  igb_disable_pcie_master - Disables PCI-express master access
 *  @hw: pointer to the HW structure
 *
 *  Returns 0 (0) if successful, else returns -10
 *  (-E1000_ERR_MASTER_REQUESTS_PENDING) if master disable bit has not casued
 *  the master requests to be disabled.
 *
 *  Disables PCI-Express master access and verifies there are no pending
 *  requests.
 **/
s32 igb_disable_pcie_master(struct e1000_hw *hw)
{
	u32 ctrl;
	s32 timeout = MASTER_DISABLE_TIMEOUT;
	s32 ret_val = 0;

	if (hw->bus.type != e1000_bus_type_pci_express)
		goto out;

	ctrl = rd32(E1000_CTRL);
	ctrl |= E1000_CTRL_GIO_MASTER_DISABLE;
	wr32(E1000_CTRL, ctrl);

	while (timeout) {
		if (!(rd32(E1000_STATUS) &
		      E1000_STATUS_GIO_MASTER_ENABLE))
			break;
		udelay(100);
		timeout--;
	}

	if (!timeout) {
		hw_dbg("Master requests are pending.\n");
		ret_val = -E1000_ERR_MASTER_REQUESTS_PENDING;
		goto out;
	}

out:
	return ret_val;
}

/**
 *  igb_reset_adaptive - Reset Adaptive Interframe Spacing
 *  @hw: pointer to the HW structure
 *
 *  Reset the Adaptive Interframe Spacing throttle to default values.
 **/
void igb_reset_adaptive(struct e1000_hw *hw)
{
	struct e1000_mac_info *mac = &hw->mac;

	if (!mac->adaptive_ifs) {
		hw_dbg("Not in Adaptive IFS mode!\n");
		goto out;
	}

	if (!mac->ifs_params_forced) {
		mac->current_ifs_val = 0;
		mac->ifs_min_val = IFS_MIN;
		mac->ifs_max_val = IFS_MAX;
		mac->ifs_step_size = IFS_STEP;
		mac->ifs_ratio = IFS_RATIO;
	}

	mac->in_ifs_mode = false;
	wr32(E1000_AIT, 0);
out:
	return;
}

/**
 *  igb_update_adaptive - Update Adaptive Interframe Spacing
 *  @hw: pointer to the HW structure
 *
 *  Update the Adaptive Interframe Spacing Throttle value based on the
 *  time between transmitted packets and time between collisions.
 **/
void igb_update_adaptive(struct e1000_hw *hw)
{
	struct e1000_mac_info *mac = &hw->mac;

	if (!mac->adaptive_ifs) {
		hw_dbg("Not in Adaptive IFS mode!\n");
		goto out;
	}

	if ((mac->collision_delta * mac->ifs_ratio) > mac->tx_packet_delta) {
		if (mac->tx_packet_delta > MIN_NUM_XMITS) {
			mac->in_ifs_mode = true;
			if (mac->current_ifs_val < mac->ifs_max_val) {
				if (!mac->current_ifs_val)
					mac->current_ifs_val = mac->ifs_min_val;
				else
					mac->current_ifs_val +=
						mac->ifs_step_size;
				wr32(E1000_AIT,
						mac->current_ifs_val);
			}
		}
	} else {
		if (mac->in_ifs_mode &&
		    (mac->tx_packet_delta <= MIN_NUM_XMITS)) {
			mac->current_ifs_val = 0;
			mac->in_ifs_mode = false;
			wr32(E1000_AIT, 0);
		}
	}
out:
	return;
}

/**
 *  igb_validate_mdi_setting - Verify MDI/MDIx settings
 *  @hw: pointer to the HW structure
 *
 *  Verify that when not using auto-negotitation that MDI/MDIx is correctly
 *  set, which is forced to MDI mode only.
 **/
s32 igb_validate_mdi_setting(struct e1000_hw *hw)
{
	s32 ret_val = 0;

	if (!hw->mac.autoneg && (hw->phy.mdix == 0 || hw->phy.mdix == 3)) {
		hw_dbg("Invalid MDI setting detected\n");
		hw->phy.mdix = 1;
		ret_val = -E1000_ERR_CONFIG;
		goto out;
	}

out:
	return ret_val;
}

/**
 *  igb_write_8bit_ctrl_reg - Write a 8bit CTRL register
 *  @hw: pointer to the HW structure
 *  @reg: 32bit register offset such as E1000_SCTL
 *  @offset: register offset to write to
 *  @data: data to write at register offset
 *
 *  Writes an address/data control type register.  There are several of these
 *  and they all have the format address << 8 | data and bit 31 is polled for
 *  completion.
 **/
s32 igb_write_8bit_ctrl_reg(struct e1000_hw *hw, u32 reg,
			      u32 offset, u8 data)
{
	u32 i, regvalue = 0;
	s32 ret_val = 0;

	/* Set up the address and data */
	regvalue = ((u32)data) | (offset << E1000_GEN_CTL_ADDRESS_SHIFT);
	wr32(reg, regvalue);

	/* Poll the ready bit to see if the MDI read completed */
	for (i = 0; i < E1000_GEN_POLL_TIMEOUT; i++) {
		udelay(5);
		regvalue = rd32(reg);
		if (regvalue & E1000_GEN_CTL_READY)
			break;
	}
	if (!(regvalue & E1000_GEN_CTL_READY)) {
		hw_dbg("Reg %08x did not indicate ready\n", reg);
		ret_val = -E1000_ERR_PHY;
		goto out;
	}

out:
	return ret_val;
}

/**
 *  igb_enable_mng_pass_thru - Enable processing of ARP's
 *  @hw: pointer to the HW structure
 *
 *  Verifies the hardware needs to allow ARPs to be processed by the host.
 **/
bool igb_enable_mng_pass_thru(struct e1000_hw *hw)
{
	u32 manc;
	u32 fwsm, factps;
	bool ret_val = false;

	if (!hw->mac.asf_firmware_present)
		goto out;

	manc = rd32(E1000_MANC);

	if (!(manc & E1000_MANC_RCV_TCO_EN) ||
	    !(manc & E1000_MANC_EN_MAC_ADDR_FILTER))
		goto out;

	if (hw->mac.arc_subsystem_valid) {
		fwsm = rd32(E1000_FWSM);
		factps = rd32(E1000_FACTPS);

		if (!(factps & E1000_FACTPS_MNGCG) &&
		    ((fwsm & E1000_FWSM_MODE_MASK) ==
		     (e1000_mng_mode_pt << E1000_FWSM_MODE_SHIFT))) {
			ret_val = true;
			goto out;
		}
	} else {
		if ((manc & E1000_MANC_SMBUS_EN) &&
		    !(manc & E1000_MANC_ASF_EN)) {
			ret_val = true;
			goto out;
		}
	}

out:
	return ret_val;
}
