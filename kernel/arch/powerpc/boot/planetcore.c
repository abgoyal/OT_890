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
 * PlanetCore configuration data support functions
 *
 * Author: Scott Wood <scottwood@freescale.com>
 *
 * Copyright (c) 2007 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include "stdio.h"
#include "stdlib.h"
#include "ops.h"
#include "planetcore.h"
#include "io.h"

/* PlanetCore passes information to the OS in the form of
 * a table of key=value strings, separated by newlines.
 *
 * The list is terminated by an empty string (i.e. two
 * consecutive newlines).
 *
 * To make it easier to parse, we first convert all the
 * newlines into null bytes.
 */

void planetcore_prepare_table(char *table)
{
	do {
		if (*table == '\n')
			*table = 0;

		table++;
	} while (*(table - 1) || *table != '\n');

	*table = 0;
}

const char *planetcore_get_key(const char *table, const char *key)
{
	int keylen = strlen(key);

	do {
		if (!strncmp(table, key, keylen) && table[keylen] == '=')
			return table + keylen + 1;

		table += strlen(table) + 1;
	} while (strlen(table) != 0);

	return NULL;
}

int planetcore_get_decimal(const char *table, const char *key, u64 *val)
{
	const char *str = planetcore_get_key(table, key);
	if (!str)
		return 0;

	*val = strtoull(str, NULL, 10);
	return 1;
}

int planetcore_get_hex(const char *table, const char *key, u64 *val)
{
	const char *str = planetcore_get_key(table, key);
	if (!str)
		return 0;

	*val = strtoull(str, NULL, 16);
	return 1;
}

static u64 mac_table[4] = {
	0x000000000000,
	0x000000800000,
	0x000000400000,
	0x000000c00000,
};

void planetcore_set_mac_addrs(const char *table)
{
	u8 addr[4][6];
	u64 int_addr;
	u32 i;
	int j;

	if (!planetcore_get_hex(table, PLANETCORE_KEY_MAC_ADDR, &int_addr))
		return;

	for (i = 0; i < 4; i++) {
		u64 this_dev_addr = (int_addr & ~0x000000c00000) |
		                    mac_table[i];

		for (j = 5; j >= 0; j--) {
			addr[i][j] = this_dev_addr & 0xff;
			this_dev_addr >>= 8;
		}

		dt_fixup_mac_address(i, addr[i]);
	}
}

static char prop_buf[MAX_PROP_LEN];

void planetcore_set_stdout_path(const char *table)
{
	char *path;
	const char *label;
	void *node, *chosen;

	label = planetcore_get_key(table, PLANETCORE_KEY_SERIAL_PORT);
	if (!label)
		return;

	node = find_node_by_prop_value_str(NULL, "linux,planetcore-label",
	                                   label);
	if (!node)
		return;

	path = get_path(node, prop_buf, MAX_PROP_LEN);
	if (!path)
		return;

	chosen = finddevice("/chosen");
	if (!chosen)
		chosen = create_node(NULL, "chosen");
	if (!chosen)
		return;

	setprop_str(chosen, "linux,stdout-path", path);
}

void planetcore_set_serial_speed(const char *table)
{
	void *chosen, *stdout;
	u64 baud;
	u32 baud32;
	int len;

	chosen = finddevice("/chosen");
	if (!chosen)
		return;

	len = getprop(chosen, "linux,stdout-path", prop_buf, MAX_PROP_LEN);
	if (len <= 0)
		return;

	stdout = finddevice(prop_buf);
	if (!stdout) {
		printf("planetcore_set_serial_speed: "
		       "Bad /chosen/linux,stdout-path.\r\n");

		return;
	}

	if (!planetcore_get_decimal(table, PLANETCORE_KEY_SERIAL_BAUD,
	                            &baud)) {
		printf("planetcore_set_serial_speed: No SB tag.\r\n");
		return;
	}

	baud32 = baud;
	setprop(stdout, "current-speed", &baud32, 4);
}
