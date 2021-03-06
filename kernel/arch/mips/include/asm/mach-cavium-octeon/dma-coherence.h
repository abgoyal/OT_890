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
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2006  Ralf Baechle <ralf@linux-mips.org>
 *
 *
 * Similar to mach-generic/dma-coherence.h except
 * plat_device_is_coherent hard coded to return 1.
 *
 */
#ifndef __ASM_MACH_CAVIUM_OCTEON_DMA_COHERENCE_H
#define __ASM_MACH_CAVIUM_OCTEON_DMA_COHERENCE_H

struct device;

dma_addr_t octeon_map_dma_mem(struct device *, void *, size_t);
void octeon_unmap_dma_mem(struct device *, dma_addr_t);

static inline dma_addr_t plat_map_dma_mem(struct device *dev, void *addr,
	size_t size)
{
	return octeon_map_dma_mem(dev, addr, size);
}

static inline dma_addr_t plat_map_dma_mem_page(struct device *dev,
	struct page *page)
{
	return octeon_map_dma_mem(dev, page_address(page), PAGE_SIZE);
}

static inline unsigned long plat_dma_addr_to_phys(dma_addr_t dma_addr)
{
	return dma_addr;
}

static inline void plat_unmap_dma_mem(struct device *dev, dma_addr_t dma_addr)
{
	octeon_unmap_dma_mem(dev, dma_addr);
}

static inline int plat_dma_supported(struct device *dev, u64 mask)
{
	return 1;
}

static inline void plat_extra_sync_for_device(struct device *dev)
{
	mb();
}

static inline int plat_device_is_coherent(struct device *dev)
{
	return 1;
}

static inline int plat_dma_mapping_error(struct device *dev,
					 dma_addr_t dma_addr)
{
	return dma_addr == -1;
}

#endif /* __ASM_MACH_CAVIUM_OCTEON_DMA_COHERENCE_H */
