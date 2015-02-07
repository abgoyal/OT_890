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
 *  drivers/s390/net/qeth_core_mpc.c
 *
 *    Copyright IBM Corp. 2007
 *    Author(s): Frank Pavlic <fpavlic@de.ibm.com>,
 *		 Thomas Spatzier <tspat@de.ibm.com>,
 *		 Frank Blaschka <frank.blaschka@de.ibm.com>
 */

#include <linux/module.h>
#include <asm/cio.h>
#include "qeth_core_mpc.h"

unsigned char IDX_ACTIVATE_READ[] = {
	0x00, 0x00, 0x80, 0x00,  0x00, 0x00, 0x00, 0x00,
	0x19, 0x01, 0x01, 0x80,  0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0xc8, 0xc1,
	0xd3, 0xd3, 0xd6, 0xd3,  0xc5, 0x40, 0x00, 0x00,
	0x00, 0x00
};

unsigned char IDX_ACTIVATE_WRITE[] = {
	0x00, 0x00, 0x80, 0x00,  0x00, 0x00, 0x00, 0x00,
	0x15, 0x01, 0x01, 0x80,  0x00, 0x00, 0x00, 0x00,
	0xff, 0xff, 0x00, 0x00,  0x00, 0x00, 0xc8, 0xc1,
	0xd3, 0xd3, 0xd6, 0xd3,  0xc5, 0x40, 0x00, 0x00,
	0x00, 0x00
};

unsigned char CM_ENABLE[] = {
	0x00, 0xe0, 0x00, 0x00,  0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x14,  0x00, 0x00, 0x00, 0x63,
	0x10, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00,
	0x81, 0x7e, 0x00, 0x01,  0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,  0x00, 0x24, 0x00, 0x23,
	0x00, 0x00, 0x23, 0x05,  0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x23,  0x00, 0x00, 0x00, 0x40,
	0x00, 0x0c, 0x41, 0x02,  0x00, 0x17, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x0b, 0x04, 0x01,
	0x7e, 0x04, 0x05, 0x00,  0x01, 0x01, 0x0f,
	0x00,
	0x0c, 0x04, 0x02, 0xff,  0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff
};

unsigned char CM_SETUP[] = {
	0x00, 0xe0, 0x00, 0x00,  0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x14,  0x00, 0x00, 0x00, 0x64,
	0x10, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00,
	0x81, 0x7e, 0x00, 0x01,  0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,  0x00, 0x24, 0x00, 0x24,
	0x00, 0x00, 0x24, 0x05,  0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x24,  0x00, 0x00, 0x00, 0x40,
	0x00, 0x0c, 0x41, 0x04,  0x00, 0x18, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x09, 0x04, 0x04,
	0x05, 0x00, 0x01, 0x01,  0x11,
	0x00, 0x09, 0x04,
	0x05, 0x05, 0x00, 0x00,  0x00, 0x00,
	0x00, 0x06,
	0x04, 0x06, 0xc8, 0x00
};

unsigned char ULP_ENABLE[] = {
	0x00, 0xe0, 0x00, 0x00,  0x00, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x00, 0x14,  0x00, 0x00, 0x00, 0x6b,
	0x10, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00,
	0x41, 0x7e, 0x00, 0x01,  0x00, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00,  0x00, 0x24, 0x00, 0x2b,
	0x00, 0x00, 0x2b, 0x05,  0x20, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x2b,  0x00, 0x00, 0x00, 0x40,
	0x00, 0x0c, 0x41, 0x02,  0x00, 0x1f, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x0b, 0x04, 0x01,
	0x03, 0x04, 0x05, 0x00,  0x01, 0x01, 0x12,
	0x00,
	0x14, 0x04, 0x0a, 0x00,  0x20, 0x00, 0x00, 0xff,
	0xff, 0x00, 0x08, 0xc8,  0xe8, 0xc4, 0xf1, 0xc7,
	0xf1, 0x00, 0x00
};

unsigned char ULP_SETUP[] = {
	0x00, 0xe0, 0x00, 0x00,  0x00, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x14,  0x00, 0x00, 0x00, 0x6c,
	0x10, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00,
	0x41, 0x7e, 0x00, 0x01,  0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x01,  0x00, 0x24, 0x00, 0x2c,
	0x00, 0x00, 0x2c, 0x05,  0x20, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x2c,  0x00, 0x00, 0x00, 0x40,
	0x00, 0x0c, 0x41, 0x04,  0x00, 0x20, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x09, 0x04, 0x04,
	0x05, 0x00, 0x01, 0x01,  0x14,
	0x00, 0x09, 0x04,
	0x05, 0x05, 0x30, 0x01,  0x00, 0x00,
	0x00, 0x06,
	0x04, 0x06, 0x40, 0x00,
	0x00, 0x08, 0x04, 0x0b,
	0x00, 0x00, 0x00, 0x00
};

unsigned char DM_ACT[] = {
	0x00, 0xe0, 0x00, 0x00,  0x00, 0x00, 0x00, 0x05,
	0x00, 0x00, 0x00, 0x14,  0x00, 0x00, 0x00, 0x55,
	0x10, 0x00, 0x00, 0x01,
	0x00, 0x00, 0x00, 0x00,
	0x41, 0x7e, 0x00, 0x01,  0x00, 0x00, 0x00, 0x03,
	0x00, 0x00, 0x00, 0x02,  0x00, 0x24, 0x00, 0x15,
	0x00, 0x00, 0x2c, 0x05,  0x20, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x15,  0x00, 0x00, 0x00, 0x40,
	0x00, 0x0c, 0x43, 0x60,  0x00, 0x09, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x09, 0x04, 0x04,
	0x05, 0x40, 0x01, 0x01,  0x00
};

unsigned char IPA_PDU_HEADER[] = {
	0x00, 0xe0, 0x00, 0x00,  0x77, 0x77, 0x77, 0x77,
	0x00, 0x00, 0x00, 0x14,  0x00, 0x00,
		(IPA_PDU_HEADER_SIZE+sizeof(struct qeth_ipa_cmd)) / 256,
		(IPA_PDU_HEADER_SIZE+sizeof(struct qeth_ipa_cmd)) % 256,
	0x10, 0x00, 0x00, 0x01,  0x00, 0x00, 0x00, 0x00,
	0xc1, 0x03, 0x00, 0x01,  0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,  0x00, 0x24,
		sizeof(struct qeth_ipa_cmd) / 256,
		sizeof(struct qeth_ipa_cmd) % 256,
	0x00,
		sizeof(struct qeth_ipa_cmd) / 256,
		sizeof(struct qeth_ipa_cmd) % 256,
	0x05,
	0x77, 0x77, 0x77, 0x77,
	0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
	0x01, 0x00,
		sizeof(struct qeth_ipa_cmd) / 256,
		sizeof(struct qeth_ipa_cmd) % 256,
	0x00, 0x00, 0x00, 0x40,
};
EXPORT_SYMBOL_GPL(IPA_PDU_HEADER);

unsigned char WRITE_CCW[] = {
	0x01, CCW_FLAG_SLI, 0, 0,
	0, 0, 0, 0
};

unsigned char READ_CCW[] = {
	0x02, CCW_FLAG_SLI, 0, 0,
	0, 0, 0, 0
};


struct ipa_rc_msg {
	enum qeth_ipa_return_codes rc;
	char *msg;
};

static struct ipa_rc_msg qeth_ipa_rc_msg[] = {
	{IPA_RC_SUCCESS,		"success"},
	{IPA_RC_NOTSUPP,		"Command not supported"},
	{IPA_RC_IP_TABLE_FULL,		"Add Addr IP Table Full - ipv6"},
	{IPA_RC_UNKNOWN_ERROR,		"IPA command failed - reason unknown"},
	{IPA_RC_UNSUPPORTED_COMMAND,	"Command not supported"},
	{IPA_RC_DUP_IPV6_REMOTE, "ipv6 address already registered remote"},
	{IPA_RC_DUP_IPV6_HOME,		"ipv6 address already registered"},
	{IPA_RC_UNREGISTERED_ADDR,	"Address not registered"},
	{IPA_RC_NO_ID_AVAILABLE,	"No identifiers available"},
	{IPA_RC_ID_NOT_FOUND,		"Identifier not found"},
	{IPA_RC_INVALID_IP_VERSION,	"IP version incorrect"},
	{IPA_RC_LAN_FRAME_MISMATCH,	"LAN and frame mismatch"},
	{IPA_RC_L2_UNSUPPORTED_CMD,	"Unsupported layer 2 command"},
	{IPA_RC_L2_DUP_MAC,		"Duplicate MAC address"},
	{IPA_RC_L2_ADDR_TABLE_FULL,	"Layer2 address table full"},
	{IPA_RC_L2_DUP_LAYER3_MAC,	"Duplicate with layer 3 MAC"},
	{IPA_RC_L2_GMAC_NOT_FOUND,	"GMAC not found"},
	{IPA_RC_L2_MAC_NOT_FOUND,	"L2 mac address not found"},
	{IPA_RC_L2_INVALID_VLAN_ID,	"L2 invalid vlan id"},
	{IPA_RC_L2_DUP_VLAN_ID,		"L2 duplicate vlan id"},
	{IPA_RC_L2_VLAN_ID_NOT_FOUND,	"L2 vlan id not found"},
	{IPA_RC_DATA_MISMATCH,		"Data field mismatch (v4/v6 mixed)"},
	{IPA_RC_INVALID_MTU_SIZE,	"Invalid MTU size"},
	{IPA_RC_INVALID_LANTYPE,	"Invalid LAN type"},
	{IPA_RC_INVALID_LANNUM,		"Invalid LAN num"},
	{IPA_RC_DUPLICATE_IP_ADDRESS,	"Address already registered"},
	{IPA_RC_IP_ADDR_TABLE_FULL,	"IP address table full"},
	{IPA_RC_LAN_PORT_STATE_ERROR,	"LAN port state error"},
	{IPA_RC_SETIP_NO_STARTLAN,	"Setip no startlan received"},
	{IPA_RC_SETIP_ALREADY_RECEIVED,	"Setip already received"},
	{IPA_RC_IP_ADDR_ALREADY_USED,	"IP address already in use on LAN"},
	{IPA_RC_MC_ADDR_NOT_FOUND,	"Multicast address not found"},
	{IPA_RC_SETIP_INVALID_VERSION,	"SETIP invalid IP version"},
	{IPA_RC_UNSUPPORTED_SUBCMD,	"Unsupported assist subcommand"},
	{IPA_RC_ARP_ASSIST_NO_ENABLE,	"Only partial success, no enable"},
	{IPA_RC_PRIMARY_ALREADY_DEFINED, "Primary already defined"},
	{IPA_RC_SECOND_ALREADY_DEFINED,	"Secondary already defined"},
	{IPA_RC_INVALID_SETRTG_INDICATOR, "Invalid SETRTG indicator"},
	{IPA_RC_MC_ADDR_ALREADY_DEFINED, "Multicast address already defined"},
	{IPA_RC_LAN_OFFLINE,		"STRTLAN_LAN_DISABLED - LAN offline"},
	{IPA_RC_INVALID_IP_VERSION2,	"Invalid IP version"},
	{IPA_RC_FFFF,			"Unknown Error"}
};



char *qeth_get_ipa_msg(enum qeth_ipa_return_codes rc)
{
	int x = 0;
	qeth_ipa_rc_msg[sizeof(qeth_ipa_rc_msg) /
			sizeof(struct ipa_rc_msg) - 1].rc = rc;
	while (qeth_ipa_rc_msg[x].rc != rc)
		x++;
	return qeth_ipa_rc_msg[x].msg;
}


struct ipa_cmd_names {
	enum qeth_ipa_cmds cmd;
	char *name;
};

static struct ipa_cmd_names qeth_ipa_cmd_names[] = {
	{IPA_CMD_STARTLAN,	"startlan"},
	{IPA_CMD_STOPLAN,	"stoplan"},
	{IPA_CMD_SETVMAC,	"setvmac"},
	{IPA_CMD_DELVMAC,	"delvmac"},
	{IPA_CMD_SETGMAC,	"setgmac"},
	{IPA_CMD_DELGMAC,	"delgmac"},
	{IPA_CMD_SETVLAN,	"setvlan"},
	{IPA_CMD_DELVLAN,	"delvlan"},
	{IPA_CMD_SETCCID,	"setccid"},
	{IPA_CMD_DELCCID,	"delccid"},
	{IPA_CMD_MODCCID,	"modccid"},
	{IPA_CMD_SETIP,		"setip"},
	{IPA_CMD_QIPASSIST,	"qipassist"},
	{IPA_CMD_SETASSPARMS,	"setassparms"},
	{IPA_CMD_SETIPM,	"setipm"},
	{IPA_CMD_DELIPM,	"delipm"},
	{IPA_CMD_SETRTG,	"setrtg"},
	{IPA_CMD_DELIP,		"delip"},
	{IPA_CMD_SETADAPTERPARMS, "setadapterparms"},
	{IPA_CMD_SET_DIAG_ASS,	"set_diag_ass"},
	{IPA_CMD_CREATE_ADDR,	"create_addr"},
	{IPA_CMD_DESTROY_ADDR,	"destroy_addr"},
	{IPA_CMD_REGISTER_LOCAL_ADDR,	"register_local_addr"},
	{IPA_CMD_UNREGISTER_LOCAL_ADDR,	"unregister_local_addr"},
	{IPA_CMD_UNKNOWN,	"unknown"},
};

char *qeth_get_ipa_cmd_name(enum qeth_ipa_cmds cmd)
{
	int x = 0;
	qeth_ipa_cmd_names[
		sizeof(qeth_ipa_cmd_names) /
			sizeof(struct ipa_cmd_names)-1].cmd = cmd;
	while (qeth_ipa_cmd_names[x].cmd != cmd)
		x++;
	return qeth_ipa_cmd_names[x].name;
}
