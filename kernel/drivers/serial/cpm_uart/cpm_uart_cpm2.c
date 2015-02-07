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
 *  linux/drivers/serial/cpm_uart_cpm2.c
 *
 *  Driver for CPM (SCC/SMC) serial ports; CPM2 definitions
 *
 *  Maintainer: Kumar Gala (galak@kernel.crashing.org) (CPM2)
 *              Pantelis Antoniou (panto@intracom.gr) (CPM1)
 *
 *  Copyright (C) 2004 Freescale Semiconductor, Inc.
 *            (C) 2004 Intracom, S.A.
 *            (C) 2006 MontaVista Software, Inc.
 *		Vitaly Bordug <vbordug@ru.mvista.com>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <linux/module.h>
#include <linux/tty.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/serial.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/device.h>
#include <linux/bootmem.h>
#include <linux/dma-mapping.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/fs_pd.h>
#include <asm/prom.h>

#include <linux/serial_core.h>
#include <linux/kernel.h>

#include "cpm_uart.h"

/**************************************************************/

void cpm_line_cr_cmd(struct uart_cpm_port *port, int cmd)
{
	cpm_command(port->command, cmd);
}

void __iomem *cpm_uart_map_pram(struct uart_cpm_port *port,
				struct device_node *np)
{
	void __iomem *pram;
	unsigned long offset;
	struct resource res;
	unsigned long len;

	/* Don't remap parameter RAM if it has already been initialized
	 * during console setup.
	 */
	if (IS_SMC(port) && port->smcup)
		return port->smcup;
	else if (!IS_SMC(port) && port->sccup)
		return port->sccup;

	if (of_address_to_resource(np, 1, &res))
		return NULL;

	len = 1 + res.end - res.start;
	pram = ioremap(res.start, len);
	if (!pram)
		return NULL;

	if (!IS_SMC(port))
		return pram;

	if (len != 2) {
		printk(KERN_WARNING "cpm_uart[%d]: device tree references "
			"SMC pram, using boot loader/wrapper pram mapping. "
			"Please fix your device tree to reference the pram "
			"base register instead.\n",
			port->port.line);
		return pram;
	}

	offset = cpm_dpalloc(PROFF_SMC_SIZE, 64);
	out_be16(pram, offset);
	iounmap(pram);
	return cpm_muram_addr(offset);
}

void cpm_uart_unmap_pram(struct uart_cpm_port *port, void __iomem *pram)
{
	if (!IS_SMC(port))
		iounmap(pram);
}

/*
 * Allocate DP-Ram and memory buffers. We need to allocate a transmit and
 * receive buffer descriptors from dual port ram, and a character
 * buffer area from host mem. If we are allocating for the console we need
 * to do it from bootmem
 */
int cpm_uart_allocbuf(struct uart_cpm_port *pinfo, unsigned int is_con)
{
	int dpmemsz, memsz;
	u8 __iomem *dp_mem;
	unsigned long dp_offset;
	u8 *mem_addr;
	dma_addr_t dma_addr = 0;

	pr_debug("CPM uart[%d]:allocbuf\n", pinfo->port.line);

	dpmemsz = sizeof(cbd_t) * (pinfo->rx_nrfifos + pinfo->tx_nrfifos);
	dp_offset = cpm_dpalloc(dpmemsz, 8);
	if (IS_ERR_VALUE(dp_offset)) {
		printk(KERN_ERR
		       "cpm_uart_cpm.c: could not allocate buffer descriptors\n");
		return -ENOMEM;
	}

	dp_mem = cpm_dpram_addr(dp_offset);

	memsz = L1_CACHE_ALIGN(pinfo->rx_nrfifos * pinfo->rx_fifosize) +
	    L1_CACHE_ALIGN(pinfo->tx_nrfifos * pinfo->tx_fifosize);
	if (is_con) {
		mem_addr = alloc_bootmem(memsz);
		dma_addr = virt_to_bus(mem_addr);
	}
	else
		mem_addr = dma_alloc_coherent(pinfo->port.dev, memsz, &dma_addr,
					      GFP_KERNEL);

	if (mem_addr == NULL) {
		cpm_dpfree(dp_offset);
		printk(KERN_ERR
		       "cpm_uart_cpm.c: could not allocate coherent memory\n");
		return -ENOMEM;
	}

	pinfo->dp_addr = dp_offset;
	pinfo->mem_addr = mem_addr;
	pinfo->dma_addr = dma_addr;
	pinfo->mem_size = memsz;

	pinfo->rx_buf = mem_addr;
	pinfo->tx_buf = pinfo->rx_buf + L1_CACHE_ALIGN(pinfo->rx_nrfifos
						       * pinfo->rx_fifosize);

	pinfo->rx_bd_base = (cbd_t __iomem *)dp_mem;
	pinfo->tx_bd_base = pinfo->rx_bd_base + pinfo->rx_nrfifos;

	return 0;
}

void cpm_uart_freebuf(struct uart_cpm_port *pinfo)
{
	dma_free_coherent(pinfo->port.dev, L1_CACHE_ALIGN(pinfo->rx_nrfifos *
							  pinfo->rx_fifosize) +
			  L1_CACHE_ALIGN(pinfo->tx_nrfifos *
					 pinfo->tx_fifosize), (void __force *)pinfo->mem_addr,
			  pinfo->dma_addr);

	cpm_dpfree(pinfo->dp_addr);
}
