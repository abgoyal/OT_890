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

#ifndef _ASM_SPARC64_DMA_MAPPING_H
#define _ASM_SPARC64_DMA_MAPPING_H

#include <linux/scatterlist.h>
#include <linux/mm.h>

#define DMA_ERROR_CODE	(~(dma_addr_t)0x0)

struct dma_ops {
	void *(*alloc_coherent)(struct device *dev, size_t size,
				dma_addr_t *dma_handle, gfp_t flag);
	void (*free_coherent)(struct device *dev, size_t size,
			      void *cpu_addr, dma_addr_t dma_handle);
	dma_addr_t (*map_single)(struct device *dev, void *cpu_addr,
				 size_t size,
				 enum dma_data_direction direction);
	void (*unmap_single)(struct device *dev, dma_addr_t dma_addr,
			     size_t size,
			     enum dma_data_direction direction);
	int (*map_sg)(struct device *dev, struct scatterlist *sg, int nents,
		      enum dma_data_direction direction);
	void (*unmap_sg)(struct device *dev, struct scatterlist *sg,
			 int nhwentries,
			 enum dma_data_direction direction);
	void (*sync_single_for_cpu)(struct device *dev,
				    dma_addr_t dma_handle, size_t size,
				    enum dma_data_direction direction);
	void (*sync_sg_for_cpu)(struct device *dev, struct scatterlist *sg,
				int nelems,
				enum dma_data_direction direction);
};
extern const struct dma_ops *dma_ops;

extern int dma_supported(struct device *dev, u64 mask);
extern int dma_set_mask(struct device *dev, u64 dma_mask);

static inline void *dma_alloc_coherent(struct device *dev, size_t size,
				       dma_addr_t *dma_handle, gfp_t flag)
{
	return dma_ops->alloc_coherent(dev, size, dma_handle, flag);
}

static inline void dma_free_coherent(struct device *dev, size_t size,
				     void *cpu_addr, dma_addr_t dma_handle)
{
	dma_ops->free_coherent(dev, size, cpu_addr, dma_handle);
}

static inline dma_addr_t dma_map_single(struct device *dev, void *cpu_addr,
					size_t size,
					enum dma_data_direction direction)
{
	return dma_ops->map_single(dev, cpu_addr, size, direction);
}

static inline void dma_unmap_single(struct device *dev, dma_addr_t dma_addr,
				    size_t size,
				    enum dma_data_direction direction)
{
	dma_ops->unmap_single(dev, dma_addr, size, direction);
}

static inline dma_addr_t dma_map_page(struct device *dev, struct page *page,
				      unsigned long offset, size_t size,
				      enum dma_data_direction direction)
{
	return dma_ops->map_single(dev, page_address(page) + offset,
				   size, direction);
}

static inline void dma_unmap_page(struct device *dev, dma_addr_t dma_address,
				  size_t size,
				  enum dma_data_direction direction)
{
	dma_ops->unmap_single(dev, dma_address, size, direction);
}

static inline int dma_map_sg(struct device *dev, struct scatterlist *sg,
			     int nents, enum dma_data_direction direction)
{
	return dma_ops->map_sg(dev, sg, nents, direction);
}

static inline void dma_unmap_sg(struct device *dev, struct scatterlist *sg,
				int nents, enum dma_data_direction direction)
{
	dma_ops->unmap_sg(dev, sg, nents, direction);
}

static inline void dma_sync_single_for_cpu(struct device *dev,
					   dma_addr_t dma_handle, size_t size,
					   enum dma_data_direction direction)
{
	dma_ops->sync_single_for_cpu(dev, dma_handle, size, direction);
}

static inline void dma_sync_single_for_device(struct device *dev,
					      dma_addr_t dma_handle,
					      size_t size,
					      enum dma_data_direction direction)
{
	/* No flushing needed to sync cpu writes to the device.  */
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
	/* No flushing needed to sync cpu writes to the device.  */
}


static inline void dma_sync_sg_for_cpu(struct device *dev,
				       struct scatterlist *sg, int nelems,
				       enum dma_data_direction direction)
{
	dma_ops->sync_sg_for_cpu(dev, sg, nelems, direction);
}

static inline void dma_sync_sg_for_device(struct device *dev,
					  struct scatterlist *sg, int nelems,
					  enum dma_data_direction direction)
{
	/* No flushing needed to sync cpu writes to the device.  */
}

static inline int dma_mapping_error(struct device *dev, dma_addr_t dma_addr)
{
	return (dma_addr == DMA_ERROR_CODE);
}

static inline int dma_get_cache_alignment(void)
{
	/* no easy way to get cache size on all processors, so return
	 * the maximum possible, to be safe */
	return (1 << INTERNODE_CACHE_SHIFT);
}

#define dma_alloc_noncoherent(d, s, h, f) dma_alloc_coherent(d, s, h, f)
#define dma_free_noncoherent(d, s, v, h) dma_free_coherent(d, s, v, h)
#define dma_is_consistent(d, h)	(1)

#endif /* _ASM_SPARC64_DMA_MAPPING_H */
