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
 * Copyright (c) 2007 Mellanox Technologies. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <linux/errno.h>
#include <linux/if_ether.h>

#include <linux/mlx4/cmd.h>

#include "mlx4.h"

#define MLX4_MAC_VALID		(1ull << 63)
#define MLX4_MAC_MASK		0xffffffffffffULL

#define MLX4_VLAN_VALID		(1u << 31)
#define MLX4_VLAN_MASK		0xfff

void mlx4_init_mac_table(struct mlx4_dev *dev, struct mlx4_mac_table *table)
{
	int i;

	mutex_init(&table->mutex);
	for (i = 0; i < MLX4_MAX_MAC_NUM; i++) {
		table->entries[i] = 0;
		table->refs[i]	 = 0;
	}
	table->max   = 1 << dev->caps.log_num_macs;
	table->total = 0;
}

void mlx4_init_vlan_table(struct mlx4_dev *dev, struct mlx4_vlan_table *table)
{
	int i;

	mutex_init(&table->mutex);
	for (i = 0; i < MLX4_MAX_VLAN_NUM; i++) {
		table->entries[i] = 0;
		table->refs[i]	 = 0;
	}
	table->max   = 1 << dev->caps.log_num_vlans;
	table->total = 0;
}

static int mlx4_set_port_mac_table(struct mlx4_dev *dev, u8 port,
				   __be64 *entries)
{
	struct mlx4_cmd_mailbox *mailbox;
	u32 in_mod;
	int err;

	mailbox = mlx4_alloc_cmd_mailbox(dev);
	if (IS_ERR(mailbox))
		return PTR_ERR(mailbox);

	memcpy(mailbox->buf, entries, MLX4_MAC_TABLE_SIZE);

	in_mod = MLX4_SET_PORT_MAC_TABLE << 8 | port;
	err = mlx4_cmd(dev, mailbox->dma, in_mod, 1, MLX4_CMD_SET_PORT,
		       MLX4_CMD_TIME_CLASS_B);

	mlx4_free_cmd_mailbox(dev, mailbox);
	return err;
}

int mlx4_register_mac(struct mlx4_dev *dev, u8 port, u64 mac, int *index)
{
	struct mlx4_mac_table *table = &mlx4_priv(dev)->port[port].mac_table;
	int i, err = 0;
	int free = -1;

	mlx4_dbg(dev, "Registering MAC: 0x%llx\n", (unsigned long long) mac);
	mutex_lock(&table->mutex);
	for (i = 0; i < MLX4_MAX_MAC_NUM - 1; i++) {
		if (free < 0 && !table->refs[i]) {
			free = i;
			continue;
		}

		if (mac == (MLX4_MAC_MASK & be64_to_cpu(table->entries[i]))) {
			/* MAC already registered, increase refernce count */
			*index = i;
			++table->refs[i];
			goto out;
		}
	}
	mlx4_dbg(dev, "Free MAC index is %d\n", free);

	if (table->total == table->max) {
		/* No free mac entries */
		err = -ENOSPC;
		goto out;
	}

	/* Register new MAC */
	table->refs[free] = 1;
	table->entries[free] = cpu_to_be64(mac | MLX4_MAC_VALID);

	err = mlx4_set_port_mac_table(dev, port, table->entries);
	if (unlikely(err)) {
		mlx4_err(dev, "Failed adding MAC: 0x%llx\n", (unsigned long long) mac);
		table->refs[free] = 0;
		table->entries[free] = 0;
		goto out;
	}

	*index = free;
	++table->total;
out:
	mutex_unlock(&table->mutex);
	return err;
}
EXPORT_SYMBOL_GPL(mlx4_register_mac);

void mlx4_unregister_mac(struct mlx4_dev *dev, u8 port, int index)
{
	struct mlx4_mac_table *table = &mlx4_priv(dev)->port[port].mac_table;

	mutex_lock(&table->mutex);
	if (!table->refs[index]) {
		mlx4_warn(dev, "No MAC entry for index %d\n", index);
		goto out;
	}
	if (--table->refs[index]) {
		mlx4_warn(dev, "Have more references for index %d,"
			  "no need to modify MAC table\n", index);
		goto out;
	}
	table->entries[index] = 0;
	mlx4_set_port_mac_table(dev, port, table->entries);
	--table->total;
out:
	mutex_unlock(&table->mutex);
}
EXPORT_SYMBOL_GPL(mlx4_unregister_mac);

static int mlx4_set_port_vlan_table(struct mlx4_dev *dev, u8 port,
				    __be32 *entries)
{
	struct mlx4_cmd_mailbox *mailbox;
	u32 in_mod;
	int err;

	mailbox = mlx4_alloc_cmd_mailbox(dev);
	if (IS_ERR(mailbox))
		return PTR_ERR(mailbox);

	memcpy(mailbox->buf, entries, MLX4_VLAN_TABLE_SIZE);
	in_mod = MLX4_SET_PORT_VLAN_TABLE << 8 | port;
	err = mlx4_cmd(dev, mailbox->dma, in_mod, 1, MLX4_CMD_SET_PORT,
		       MLX4_CMD_TIME_CLASS_B);

	mlx4_free_cmd_mailbox(dev, mailbox);

	return err;
}

int mlx4_register_vlan(struct mlx4_dev *dev, u8 port, u16 vlan, int *index)
{
	struct mlx4_vlan_table *table = &mlx4_priv(dev)->port[port].vlan_table;
	int i, err = 0;
	int free = -1;

	mutex_lock(&table->mutex);
	for (i = MLX4_VLAN_REGULAR; i < MLX4_MAX_VLAN_NUM; i++) {
		if (free < 0 && (table->refs[i] == 0)) {
			free = i;
			continue;
		}

		if (table->refs[i] &&
		    (vlan == (MLX4_VLAN_MASK &
			      be32_to_cpu(table->entries[i])))) {
			/* Vlan already registered, increase refernce count */
			*index = i;
			++table->refs[i];
			goto out;
		}
	}

	if (table->total == table->max) {
		/* No free vlan entries */
		err = -ENOSPC;
		goto out;
	}

	/* Register new MAC */
	table->refs[free] = 1;
	table->entries[free] = cpu_to_be32(vlan | MLX4_VLAN_VALID);

	err = mlx4_set_port_vlan_table(dev, port, table->entries);
	if (unlikely(err)) {
		mlx4_warn(dev, "Failed adding vlan: %u\n", vlan);
		table->refs[free] = 0;
		table->entries[free] = 0;
		goto out;
	}

	*index = free;
	++table->total;
out:
	mutex_unlock(&table->mutex);
	return err;
}
EXPORT_SYMBOL_GPL(mlx4_register_vlan);

void mlx4_unregister_vlan(struct mlx4_dev *dev, u8 port, int index)
{
	struct mlx4_vlan_table *table = &mlx4_priv(dev)->port[port].vlan_table;

	if (index < MLX4_VLAN_REGULAR) {
		mlx4_warn(dev, "Trying to free special vlan index %d\n", index);
		return;
	}

	mutex_lock(&table->mutex);
	if (!table->refs[index]) {
		mlx4_warn(dev, "No vlan entry for index %d\n", index);
		goto out;
	}
	if (--table->refs[index]) {
		mlx4_dbg(dev, "Have more references for index %d,"
			 "no need to modify vlan table\n", index);
		goto out;
	}
	table->entries[index] = 0;
	mlx4_set_port_vlan_table(dev, port, table->entries);
	--table->total;
out:
	mutex_unlock(&table->mutex);
}
EXPORT_SYMBOL_GPL(mlx4_unregister_vlan);

int mlx4_get_port_ib_caps(struct mlx4_dev *dev, u8 port, __be32 *caps)
{
	struct mlx4_cmd_mailbox *inmailbox, *outmailbox;
	u8 *inbuf, *outbuf;
	int err;

	inmailbox = mlx4_alloc_cmd_mailbox(dev);
	if (IS_ERR(inmailbox))
		return PTR_ERR(inmailbox);

	outmailbox = mlx4_alloc_cmd_mailbox(dev);
	if (IS_ERR(outmailbox)) {
		mlx4_free_cmd_mailbox(dev, inmailbox);
		return PTR_ERR(outmailbox);
	}

	inbuf = inmailbox->buf;
	outbuf = outmailbox->buf;
	memset(inbuf, 0, 256);
	memset(outbuf, 0, 256);
	inbuf[0] = 1;
	inbuf[1] = 1;
	inbuf[2] = 1;
	inbuf[3] = 1;
	*(__be16 *) (&inbuf[16]) = cpu_to_be16(0x0015);
	*(__be32 *) (&inbuf[20]) = cpu_to_be32(port);

	err = mlx4_cmd_box(dev, inmailbox->dma, outmailbox->dma, port, 3,
			   MLX4_CMD_MAD_IFC, MLX4_CMD_TIME_CLASS_C);
	if (!err)
		*caps = *(__be32 *) (outbuf + 84);
	mlx4_free_cmd_mailbox(dev, inmailbox);
	mlx4_free_cmd_mailbox(dev, outmailbox);
	return err;
}

int mlx4_SET_PORT(struct mlx4_dev *dev, u8 port)
{
	struct mlx4_cmd_mailbox *mailbox;
	int err;
	u8 is_eth = dev->caps.port_type[port] == MLX4_PORT_TYPE_ETH;

	mailbox = mlx4_alloc_cmd_mailbox(dev);
	if (IS_ERR(mailbox))
		return PTR_ERR(mailbox);

	memset(mailbox->buf, 0, 256);
	if (is_eth) {
		((u8 *) mailbox->buf)[3] = 6;
		((__be16 *) mailbox->buf)[4] = cpu_to_be16(1 << 15);
		((__be16 *) mailbox->buf)[6] = cpu_to_be16(1 << 15);
	} else
		((__be32 *) mailbox->buf)[1] = dev->caps.ib_port_def_cap[port];
	err = mlx4_cmd(dev, mailbox->dma, port, is_eth, MLX4_CMD_SET_PORT,
		       MLX4_CMD_TIME_CLASS_B);

	mlx4_free_cmd_mailbox(dev, mailbox);
	return err;
}
