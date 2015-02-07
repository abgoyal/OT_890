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
 * include/asm-xtensa/dma-mapping.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2003 - 2005 Tensilica Inc.
 */

#ifndef _XTENSA_DMA_MAPPING_H
#define _XTENSA_DMA_MAPPING_H

#include <asm/cache.h>
#include <asm/io.h>
#include <linux/mm.h>
#include <linux/scatterlist.h>

/*
 * DMA-consistent mapping functions.
 */

extern void *consistent_alloc(int, size_t, dma_addr_t, unsigned long);
extern void consistent_free(void*, size_t, dma_addr_t);
extern void consistent_sync(void*, size_t, int);

#define dma_alloc_noncoherent(d, s, h, f) dma_alloc_coherent(d, s, h, f)
#define dma_free_noncoherent(d, s, v, h) dma_free_coherent(d, s, v, h)

void *dma_alloc_coherent(struct device *dev, size_t size,
			   dma_addr_t *dma_handle, gfp_t flag);

void dma_free_coherent(struct device *dev, size_t size,
			 void *vaddr, dma_addr_t dma_handle);

static inline dma_addr_t
dma_map_single(struct device *dev, void *ptr, size_t size,
	       enum dma_data_direction direction)
{
	BUG_ON(direction == DMA_NONE);
	consistent_sync(ptr, size, direction);
	return virt_to_phys(ptr);
}

static inline void
dma_unmap_single(struct device *dev, dma_addr_t dma_addr, size_t size,
		 enum dma_data_direction direction)
{
	BUG_ON(direction == DMA_NONE);
}

static inline int
dma_map_sg(struct device *dev, struct scatterlist *sg, int nents,
	   enum dma_data_direction direction)
{
	int i;

	BUG_ON(direction == DMA_NONE);

	for (i = 0; i < nents; i++, sg++ ) {
		BUG_ON(!sg_page(sg));

		sg->dma_address = sg_phys(sg);
		consistent_sync(sg_virt(sg), sg->length, direction);
	}

	return nents;
}

static inline dma_addr_t
dma_map_page(struct device *dev, struct page *page, unsigned long offset,
	     size_t size, enum dma_data_direction direction)
{
	BUG_ON(direction == DMA_NONE);
	return (dma_addr_t)(page_to_pfn(page)) * PAGE_SIZE + offset;
}

static inline void
dma_unmap_page(struct device *dev, dma_addr_t dma_address, size_t size,
	       enum dma_data_direction direction)
{
	BUG_ON(direction == DMA_NONE);
}


static inline void
dma_unmap_sg(struct device *dev, struct scatterlist *sg, int nhwentries,
	     enum dma_data_direction direction)
{
	BUG_ON(direction == DMA_NONE);
}

static inline void
dma_sync_single_for_cpu(struct device *dev, dma_addr_t dma_handle, size_t size,
		enum dma_data_direction direction)
{
	consistent_sync((void *)bus_to_virt(dma_handle), size, direction);
}

static inline void
dma_sync_single_for_device(struct device *dev, dma_addr_t dma_handle, size_t size,
		enum dma_data_direction direction)
{
	consistent_sync((void *)bus_to_virt(dma_handle), size, direction);
}

static inline void
dma_sync_single_range_for_cpu(struct device *dev, dma_addr_t dma_handle,
		      unsigned long offset, size_t size,
		      enum dma_data_direction direction)
{

	consistent_sync((void *)bus_to_virt(dma_handle)+offset,size,direction);
}

static inline void
dma_sync_single_range_for_device(struct device *dev, dma_addr_t dma_handle,
		      unsigned long offset, size_t size,
		      enum dma_data_direction direction)
{

	consistent_sync((void *)bus_to_virt(dma_handle)+offset,size,direction);
}
static inline void
dma_sync_sg_for_cpu(struct device *dev, struct scatterlist *sg, int nelems,
		 enum dma_data_direction dir)
{
	int i;
	for (i = 0; i < nelems; i++, sg++)
		consistent_sync(sg_virt(sg), sg->length, dir);
}

static inline void
dma_sync_sg_for_device(struct device *dev, struct scatterlist *sg, int nelems,
		 enum dma_data_direction dir)
{
	int i;
	for (i = 0; i < nelems; i++, sg++)
		consistent_sync(sg_virt(sg), sg->length, dir);
}
static inline int
dma_mapping_error(struct device *dev, dma_addr_t dma_addr)
{
	return 0;
}

static inline int
dma_supported(struct device *dev, u64 mask)
{
	return 1;
}

static inline int
dma_set_mask(struct device *dev, u64 mask)
{
	if(!dev->dma_mask || !dma_supported(dev, mask))
		return -EIO;

	*dev->dma_mask = mask;

	return 0;
}

static inline int
dma_get_cache_alignment(void)
{
	return L1_CACHE_BYTES;
}

#define dma_is_consistent(d, h)	(1)

static inline void
dma_cache_sync(struct device *dev, void *vaddr, size_t size,
	       enum dma_data_direction direction)
{
	consistent_sync(vaddr, size, direction);
}

#endif	/* _XTENSA_DMA_MAPPING_H */