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

#ifndef __ASM_SH_DMA_MAPPING_H
#define __ASM_SH_DMA_MAPPING_H

#include <linux/mm.h>
#include <linux/scatterlist.h>
#include <asm/cacheflush.h>
#include <asm/io.h>
#include <asm-generic/dma-coherent.h>

extern struct bus_type pci_bus_type;

#define dma_supported(dev, mask)	(1)

static inline int dma_set_mask(struct device *dev, u64 mask)
{
	if (!dev->dma_mask || !dma_supported(dev, mask))
		return -EIO;

	*dev->dma_mask = mask;

	return 0;
}

void *dma_alloc_coherent(struct device *dev, size_t size,
			 dma_addr_t *dma_handle, gfp_t flag);

void dma_free_coherent(struct device *dev, size_t size,
		       void *vaddr, dma_addr_t dma_handle);

void dma_cache_sync(struct device *dev, void *vaddr, size_t size,
		    enum dma_data_direction dir);

#define dma_alloc_noncoherent(d, s, h, f) dma_alloc_coherent(d, s, h, f)
#define dma_free_noncoherent(d, s, v, h) dma_free_coherent(d, s, v, h)
#define dma_is_consistent(d, h) (1)

static inline dma_addr_t dma_map_single(struct device *dev,
					void *ptr, size_t size,
					enum dma_data_direction dir)
{
#if defined(CONFIG_PCI) && !defined(CONFIG_SH_PCIDMA_NONCOHERENT)
	if (dev->bus == &pci_bus_type)
		return virt_to_phys(ptr);
#endif
	dma_cache_sync(dev, ptr, size, dir);

	return virt_to_phys(ptr);
}

#define dma_unmap_single(dev, addr, size, dir)	do { } while (0)

static inline int dma_map_sg(struct device *dev, struct scatterlist *sg,
			     int nents, enum dma_data_direction dir)
{
	int i;

	for (i = 0; i < nents; i++) {
#if !defined(CONFIG_PCI) || defined(CONFIG_SH_PCIDMA_NONCOHERENT)
		dma_cache_sync(dev, sg_virt(&sg[i]), sg[i].length, dir);
#endif
		sg[i].dma_address = sg_phys(&sg[i]);
	}

	return nents;
}

#define dma_unmap_sg(dev, sg, nents, dir)	do { } while (0)

static inline dma_addr_t dma_map_page(struct device *dev, struct page *page,
				      unsigned long offset, size_t size,
				      enum dma_data_direction dir)
{
	return dma_map_single(dev, page_address(page) + offset, size, dir);
}

static inline void dma_unmap_page(struct device *dev, dma_addr_t dma_address,
				  size_t size, enum dma_data_direction dir)
{
	dma_unmap_single(dev, dma_address, size, dir);
}

static inline void dma_sync_single(struct device *dev, dma_addr_t dma_handle,
				   size_t size, enum dma_data_direction dir)
{
#if defined(CONFIG_PCI) && !defined(CONFIG_SH_PCIDMA_NONCOHERENT)
	if (dev->bus == &pci_bus_type)
		return;
#endif
	dma_cache_sync(dev, phys_to_virt(dma_handle), size, dir);
}

static inline void dma_sync_single_range(struct device *dev,
					 dma_addr_t dma_handle,
					 unsigned long offset, size_t size,
					 enum dma_data_direction dir)
{
#if defined(CONFIG_PCI) && !defined(CONFIG_SH_PCIDMA_NONCOHERENT)
	if (dev->bus == &pci_bus_type)
		return;
#endif
	dma_cache_sync(dev, phys_to_virt(dma_handle) + offset, size, dir);
}

static inline void dma_sync_sg(struct device *dev, struct scatterlist *sg,
			       int nelems, enum dma_data_direction dir)
{
	int i;

	for (i = 0; i < nelems; i++) {
#if !defined(CONFIG_PCI) || defined(CONFIG_SH_PCIDMA_NONCOHERENT)
		dma_cache_sync(dev, sg_virt(&sg[i]), sg[i].length, dir);
#endif
		sg[i].dma_address = sg_phys(&sg[i]);
	}
}

static inline void dma_sync_single_for_cpu(struct device *dev,
					   dma_addr_t dma_handle, size_t size,
					   enum dma_data_direction dir)
{
	dma_sync_single(dev, dma_handle, size, dir);
}

static inline void dma_sync_single_for_device(struct device *dev,
					      dma_addr_t dma_handle,
					      size_t size,
					      enum dma_data_direction dir)
{
	dma_sync_single(dev, dma_handle, size, dir);
}

static inline void dma_sync_single_range_for_cpu(struct device *dev,
						 dma_addr_t dma_handle,
						 unsigned long offset,
						 size_t size,
						 enum dma_data_direction direction)
{
	dma_sync_single_for_cpu(dev, dma_handle+offset, size, direction);
}

static inline void dma_sync_single_range_for_device(struct device *dev,
						    dma_addr_t dma_handle,
						    unsigned long offset,
						    size_t size,
						    enum dma_data_direction direction)
{
	dma_sync_single_for_device(dev, dma_handle+offset, size, direction);
}


static inline void dma_sync_sg_for_cpu(struct device *dev,
				       struct scatterlist *sg, int nelems,
				       enum dma_data_direction dir)
{
	dma_sync_sg(dev, sg, nelems, dir);
}

static inline void dma_sync_sg_for_device(struct device *dev,
					  struct scatterlist *sg, int nelems,
					  enum dma_data_direction dir)
{
	dma_sync_sg(dev, sg, nelems, dir);
}


static inline int dma_get_cache_alignment(void)
{
	/*
	 * Each processor family will define its own L1_CACHE_SHIFT,
	 * L1_CACHE_BYTES wraps to this, so this is always safe.
	 */
	return L1_CACHE_BYTES;
}

static inline int dma_mapping_error(struct device *dev, dma_addr_t dma_addr)
{
	return dma_addr == 0;
}

#define ARCH_HAS_DMA_DECLARE_COHERENT_MEMORY

extern int
dma_declare_coherent_memory(struct device *dev, dma_addr_t bus_addr,
			    dma_addr_t device_addr, size_t size, int flags);

extern void
dma_release_declared_memory(struct device *dev);

extern void *
dma_mark_declared_memory_occupied(struct device *dev,
				  dma_addr_t device_addr, size_t size);

#endif /* __ASM_SH_DMA_MAPPING_H */
