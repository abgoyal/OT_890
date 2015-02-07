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
 * memory mapped NVRAM
 *
 * (C) Copyright IBM Corp. 2005
 *
 * Authors : Utz Bacher <utz.bacher@de.ibm.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
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

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/types.h>

#include <asm/machdep.h>
#include <asm/nvram.h>
#include <asm/prom.h>

static void __iomem *mmio_nvram_start;
static long mmio_nvram_len;
static DEFINE_SPINLOCK(mmio_nvram_lock);

static ssize_t mmio_nvram_read(char *buf, size_t count, loff_t *index)
{
	unsigned long flags;

	if (*index >= mmio_nvram_len)
		return 0;
	if (*index + count > mmio_nvram_len)
		count = mmio_nvram_len - *index;

	spin_lock_irqsave(&mmio_nvram_lock, flags);

	memcpy_fromio(buf, mmio_nvram_start + *index, count);

	spin_unlock_irqrestore(&mmio_nvram_lock, flags);
	
	*index += count;
	return count;
}

static ssize_t mmio_nvram_write(char *buf, size_t count, loff_t *index)
{
	unsigned long flags;

	if (*index >= mmio_nvram_len)
		return 0;
	if (*index + count > mmio_nvram_len)
		count = mmio_nvram_len - *index;

	spin_lock_irqsave(&mmio_nvram_lock, flags);

	memcpy_toio(mmio_nvram_start + *index, buf, count);

	spin_unlock_irqrestore(&mmio_nvram_lock, flags);
	
	*index += count;
	return count;
}

static ssize_t mmio_nvram_get_size(void)
{
	return mmio_nvram_len;
}

int __init mmio_nvram_init(void)
{
	struct device_node *nvram_node;
	unsigned long nvram_addr;
	struct resource r;
	int ret;

	nvram_node = of_find_node_by_type(NULL, "nvram");
	if (!nvram_node) {
		printk(KERN_WARNING "nvram: no node found in device-tree\n");
		return -ENODEV;
	}

	ret = of_address_to_resource(nvram_node, 0, &r);
	if (ret) {
		printk(KERN_WARNING "nvram: failed to get address (err %d)\n",
		       ret);
		goto out;
	}
	nvram_addr = r.start;
	mmio_nvram_len = r.end - r.start + 1;
	if ( (!mmio_nvram_len) || (!nvram_addr) ) {
		printk(KERN_WARNING "nvram: address or length is 0\n");
		ret = -EIO;
		goto out;
	}

	mmio_nvram_start = ioremap(nvram_addr, mmio_nvram_len);
	if (!mmio_nvram_start) {
		printk(KERN_WARNING "nvram: failed to ioremap\n");
		ret = -ENOMEM;
		goto out;
	}

	printk(KERN_INFO "mmio NVRAM, %luk at 0x%lx mapped to %p\n",
	       mmio_nvram_len >> 10, nvram_addr, mmio_nvram_start);

	ppc_md.nvram_read	= mmio_nvram_read;
	ppc_md.nvram_write	= mmio_nvram_write;
	ppc_md.nvram_size	= mmio_nvram_get_size;

out:
	of_node_put(nvram_node);
	return ret;
}
