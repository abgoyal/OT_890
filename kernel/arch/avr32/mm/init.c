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
 * Copyright (C) 2004-2006 Atmel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/init.h>
#include <linux/mmzone.h>
#include <linux/module.h>
#include <linux/bootmem.h>
#include <linux/pagemap.h>
#include <linux/nodemask.h>

#include <asm/page.h>
#include <asm/mmu_context.h>
#include <asm/tlb.h>
#include <asm/io.h>
#include <asm/dma.h>
#include <asm/setup.h>
#include <asm/sections.h>

#define __page_aligned	__attribute__((section(".data.page_aligned")))

DEFINE_PER_CPU(struct mmu_gather, mmu_gathers);

pgd_t swapper_pg_dir[PTRS_PER_PGD] __page_aligned;

struct page *empty_zero_page;
EXPORT_SYMBOL(empty_zero_page);

/*
 * Cache of MMU context last used.
 */
unsigned long mmu_context_cache = NO_CONTEXT;

/*
 * paging_init() sets up the page tables
 *
 * This routine also unmaps the page at virtual kernel address 0, so
 * that we can trap those pesky NULL-reference errors in the kernel.
 */
void __init paging_init(void)
{
	extern unsigned long _evba;
	void *zero_page;
	int nid;

	/*
	 * Make sure we can handle exceptions before enabling
	 * paging. Not that we should ever _get_ any exceptions this
	 * early, but you never know...
	 */
	printk("Exception vectors start at %p\n", &_evba);
	sysreg_write(EVBA, (unsigned long)&_evba);

	/*
	 * Since we are ready to handle exceptions now, we should let
	 * the CPU generate them...
	 */
	__asm__ __volatile__ ("csrf %0" : : "i"(SR_EM_BIT));

	/*
	 * Allocate the zero page. The allocator will panic if it
	 * can't satisfy the request, so no need to check.
	 */
	zero_page = alloc_bootmem_low_pages_node(NODE_DATA(0),
						 PAGE_SIZE);

	sysreg_write(PTBR, (unsigned long)swapper_pg_dir);
	enable_mmu();
	printk ("CPU: Paging enabled\n");

	for_each_online_node(nid) {
		pg_data_t *pgdat = NODE_DATA(nid);
		unsigned long zones_size[MAX_NR_ZONES];
		unsigned long low, start_pfn;

		start_pfn = pgdat->bdata->node_min_pfn;
		low = pgdat->bdata->node_low_pfn;

		memset(zones_size, 0, sizeof(zones_size));
		zones_size[ZONE_NORMAL] = low - start_pfn;

		printk("Node %u: start_pfn = 0x%lx, low = 0x%lx\n",
		       nid, start_pfn, low);

		free_area_init_node(nid, zones_size, start_pfn, NULL);

		printk("Node %u: mem_map starts at %p\n",
		       pgdat->node_id, pgdat->node_mem_map);
	}

	mem_map = NODE_DATA(0)->node_mem_map;

	empty_zero_page = virt_to_page(zero_page);
	flush_dcache_page(empty_zero_page);
}

void __init mem_init(void)
{
	int codesize, reservedpages, datasize, initsize;
	int nid, i;

	reservedpages = 0;
	high_memory = NULL;

	/* this will put all low memory onto the freelists */
	for_each_online_node(nid) {
		pg_data_t *pgdat = NODE_DATA(nid);
		unsigned long node_pages = 0;
		void *node_high_memory;

		num_physpages += pgdat->node_present_pages;

		if (pgdat->node_spanned_pages != 0)
			node_pages = free_all_bootmem_node(pgdat);

		totalram_pages += node_pages;

		for (i = 0; i < node_pages; i++)
			if (PageReserved(pgdat->node_mem_map + i))
				reservedpages++;

		node_high_memory = (void *)((pgdat->node_start_pfn
					     + pgdat->node_spanned_pages)
					    << PAGE_SHIFT);
		if (node_high_memory > high_memory)
			high_memory = node_high_memory;
	}

	max_mapnr = MAP_NR(high_memory);

	codesize = (unsigned long)_etext - (unsigned long)_text;
	datasize = (unsigned long)_edata - (unsigned long)_data;
	initsize = (unsigned long)__init_end - (unsigned long)__init_begin;

	printk ("Memory: %luk/%luk available (%dk kernel code, "
		"%dk reserved, %dk data, %dk init)\n",
		(unsigned long)nr_free_pages() << (PAGE_SHIFT - 10),
		totalram_pages << (PAGE_SHIFT - 10),
		codesize >> 10,
		reservedpages << (PAGE_SHIFT - 10),
		datasize >> 10,
		initsize >> 10);
}

static inline void free_area(unsigned long addr, unsigned long end, char *s)
{
	unsigned int size = (end - addr) >> 10;

	for (; addr < end; addr += PAGE_SIZE) {
		struct page *page = virt_to_page(addr);
		ClearPageReserved(page);
		init_page_count(page);
		free_page(addr);
		totalram_pages++;
	}

	if (size && s)
		printk(KERN_INFO "Freeing %s memory: %dK (%lx - %lx)\n",
		       s, size, end - (size << 10), end);
}

void free_initmem(void)
{
	free_area((unsigned long)__init_begin, (unsigned long)__init_end,
		  "init");
}

#ifdef CONFIG_BLK_DEV_INITRD

void free_initrd_mem(unsigned long start, unsigned long end)
{
	free_area(start, end, "initrd");
}

#endif
