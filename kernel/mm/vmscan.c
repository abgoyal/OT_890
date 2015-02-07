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
 *  linux/mm/vmscan.c
 *
 *  Copyright (C) 1991, 1992, 1993, 1994  Linus Torvalds
 *
 *  Swap reorganised 29.12.95, Stephen Tweedie.
 *  kswapd added: 7.1.96  sct
 *  Removed kswapd_ctl limits, and swap out as many pages as needed
 *  to bring the system back to freepages.high: 2.4.97, Rik van Riel.
 *  Zone aware kswapd started 02/00, Kanoj Sarcar (kanoj@sgi.com).
 *  Multiqueue VM started 5.8.00, Rik van Riel.
 */

#include <linux/mm.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/kernel_stat.h>
#include <linux/swap.h>
#include <linux/pagemap.h>
#include <linux/init.h>
#include <linux/highmem.h>
#include <linux/vmstat.h>
#include <linux/file.h>
#include <linux/writeback.h>
#include <linux/blkdev.h>
#include <linux/buffer_head.h>	/* for try_to_release_page(),
					buffer_heads_over_limit */
#include <linux/mm_inline.h>
#include <linux/pagevec.h>
#include <linux/backing-dev.h>
#include <linux/rmap.h>
#include <linux/topology.h>
#include <linux/cpu.h>
#include <linux/cpuset.h>
#include <linux/notifier.h>
#include <linux/rwsem.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/memcontrol.h>
#include <linux/delayacct.h>
#include <linux/sysctl.h>

#include <asm/tlbflush.h>
#include <asm/div64.h>

#include <linux/swapops.h>

#include "internal.h"

struct scan_control {
	/* Incremented by the number of inactive pages that were scanned */
	unsigned long nr_scanned;

	/* Number of pages freed so far during a call to shrink_zones() */
	unsigned long nr_reclaimed;

	/* This context's GFP mask */
	gfp_t gfp_mask;

	int may_writepage;

	/* Can pages be swapped as part of reclaim? */
	int may_swap;

	/* This context's SWAP_CLUSTER_MAX. If freeing memory for
	 * suspend, we effectively ignore SWAP_CLUSTER_MAX.
	 * In this context, it doesn't matter that we scan the
	 * whole list at once. */
	int swap_cluster_max;

	int swappiness;

	int all_unreclaimable;

	int order;

	/* Which cgroup do we reclaim from */
	struct mem_cgroup *mem_cgroup;

	/* Pluggable isolate pages callback */
	unsigned long (*isolate_pages)(unsigned long nr, struct list_head *dst,
			unsigned long *scanned, int order, int mode,
			struct zone *z, struct mem_cgroup *mem_cont,
			int active, int file);
};

#define lru_to_page(_head) (list_entry((_head)->prev, struct page, lru))

#ifdef ARCH_HAS_PREFETCH
#define prefetch_prev_lru_page(_page, _base, _field)			\
	do {								\
		if ((_page)->lru.prev != _base) {			\
			struct page *prev;				\
									\
			prev = lru_to_page(&(_page->lru));		\
			prefetch(&prev->_field);			\
		}							\
	} while (0)
#else
#define prefetch_prev_lru_page(_page, _base, _field) do { } while (0)
#endif

#ifdef ARCH_HAS_PREFETCHW
#define prefetchw_prev_lru_page(_page, _base, _field)			\
	do {								\
		if ((_page)->lru.prev != _base) {			\
			struct page *prev;				\
									\
			prev = lru_to_page(&(_page->lru));		\
			prefetchw(&prev->_field);			\
		}							\
	} while (0)
#else
#define prefetchw_prev_lru_page(_page, _base, _field) do { } while (0)
#endif

/*
 * From 0 .. 100.  Higher means more swappy.
 */
int vm_swappiness = 60;
long vm_total_pages;	/* The total number of pages which the VM controls */

static LIST_HEAD(shrinker_list);
static DECLARE_RWSEM(shrinker_rwsem);

#ifdef CONFIG_CGROUP_MEM_RES_CTLR
#define scanning_global_lru(sc)	(!(sc)->mem_cgroup)
#else
#define scanning_global_lru(sc)	(1)
#endif

static struct zone_reclaim_stat *get_reclaim_stat(struct zone *zone,
						  struct scan_control *sc)
{
	if (!scanning_global_lru(sc))
		return mem_cgroup_get_reclaim_stat(sc->mem_cgroup, zone);

	return &zone->reclaim_stat;
}

static unsigned long zone_nr_pages(struct zone *zone, struct scan_control *sc,
				   enum lru_list lru)
{
	if (!scanning_global_lru(sc))
		return mem_cgroup_zone_nr_pages(sc->mem_cgroup, zone, lru);

	return zone_page_state(zone, NR_LRU_BASE + lru);
}


/*
 * Add a shrinker callback to be called from the vm
 */
void register_shrinker(struct shrinker *shrinker)
{
	shrinker->nr = 0;
	down_write(&shrinker_rwsem);
	list_add_tail(&shrinker->list, &shrinker_list);
	up_write(&shrinker_rwsem);
}
EXPORT_SYMBOL(register_shrinker);

/*
 * Remove one
 */
void unregister_shrinker(struct shrinker *shrinker)
{
	down_write(&shrinker_rwsem);
	list_del(&shrinker->list);
	up_write(&shrinker_rwsem);
}
EXPORT_SYMBOL(unregister_shrinker);

#define SHRINK_BATCH 128
/*
 * Call the shrink functions to age shrinkable caches
 *
 * Here we assume it costs one seek to replace a lru page and that it also
 * takes a seek to recreate a cache object.  With this in mind we age equal
 * percentages of the lru and ageable caches.  This should balance the seeks
 * generated by these structures.
 *
 * If the vm encountered mapped pages on the LRU it increase the pressure on
 * slab to avoid swapping.
 *
 * We do weird things to avoid (scanned*seeks*entries) overflowing 32 bits.
 *
 * `lru_pages' represents the number of on-LRU pages in all the zones which
 * are eligible for the caller's allocation attempt.  It is used for balancing
 * slab reclaim versus page reclaim.
 *
 * Returns the number of slab objects which we shrunk.
 */
unsigned long shrink_slab(unsigned long scanned, gfp_t gfp_mask,
			unsigned long lru_pages)
{
	struct shrinker *shrinker;
	unsigned long ret = 0;

	if (scanned == 0)
		scanned = SWAP_CLUSTER_MAX;

	if (!down_read_trylock(&shrinker_rwsem))
		return 1;	/* Assume we'll be able to shrink next time */

	list_for_each_entry(shrinker, &shrinker_list, list) {
		unsigned long long delta;
		unsigned long total_scan;
		unsigned long max_pass = (*shrinker->shrink)(0, gfp_mask);

		delta = (4 * scanned) / shrinker->seeks;
		delta *= max_pass;
		do_div(delta, lru_pages + 1);
		shrinker->nr += delta;
		if (shrinker->nr < 0) {
			printk(KERN_ERR "%s: nr=%ld\n",
					__func__, shrinker->nr);
			shrinker->nr = max_pass;
		}

		/*
		 * Avoid risking looping forever due to too large nr value:
		 * never try to free more than twice the estimate number of
		 * freeable entries.
		 */
		if (shrinker->nr > max_pass * 2)
			shrinker->nr = max_pass * 2;

		total_scan = shrinker->nr;
		shrinker->nr = 0;

		while (total_scan >= SHRINK_BATCH) {
			long this_scan = SHRINK_BATCH;
			int shrink_ret;
			int nr_before;

			nr_before = (*shrinker->shrink)(0, gfp_mask);
			shrink_ret = (*shrinker->shrink)(this_scan, gfp_mask);
			if (shrink_ret == -1)
				break;
			if (shrink_ret < nr_before)
				ret += nr_before - shrink_ret;
			count_vm_events(SLABS_SCANNED, this_scan);
			total_scan -= this_scan;

			cond_resched();
		}

		shrinker->nr += total_scan;
	}
	up_read(&shrinker_rwsem);
	return ret;
}

/* Called without lock on whether page is mapped, so answer is unstable */
static inline int page_mapping_inuse(struct page *page)
{
	struct address_space *mapping;

	/* Page is in somebody's page tables. */
	if (page_mapped(page))
		return 1;

	/* Be more reluctant to reclaim swapcache than pagecache */
	if (PageSwapCache(page))
		return 1;

	mapping = page_mapping(page);
	if (!mapping)
		return 0;

	/* File is mmap'd by somebody? */
	return mapping_mapped(mapping);
}

static inline int is_page_cache_freeable(struct page *page)
{
	return page_count(page) - !!PagePrivate(page) == 2;
}

static int may_write_to_queue(struct backing_dev_info *bdi)
{
	if (current->flags & PF_SWAPWRITE)
		return 1;
	if (!bdi_write_congested(bdi))
		return 1;
	if (bdi == current->backing_dev_info)
		return 1;
	return 0;
}

/*
 * We detected a synchronous write error writing a page out.  Probably
 * -ENOSPC.  We need to propagate that into the address_space for a subsequent
 * fsync(), msync() or close().
 *
 * The tricky part is that after writepage we cannot touch the mapping: nothing
 * prevents it from being freed up.  But we have a ref on the page and once
 * that page is locked, the mapping is pinned.
 *
 * We're allowed to run sleeping lock_page() here because we know the caller has
 * __GFP_FS.
 */
static void handle_write_error(struct address_space *mapping,
				struct page *page, int error)
{
	lock_page(page);
	if (page_mapping(page) == mapping)
		mapping_set_error(mapping, error);
	unlock_page(page);
}

/* Request for sync pageout. */
enum pageout_io {
	PAGEOUT_IO_ASYNC,
	PAGEOUT_IO_SYNC,
};

/* possible outcome of pageout() */
typedef enum {
	/* failed to write page out, page is locked */
	PAGE_KEEP,
	/* move page to the active list, page is locked */
	PAGE_ACTIVATE,
	/* page has been sent to the disk successfully, page is unlocked */
	PAGE_SUCCESS,
	/* page is clean and locked */
	PAGE_CLEAN,
} pageout_t;

/*
 * pageout is called by shrink_page_list() for each dirty page.
 * Calls ->writepage().
 */
static pageout_t pageout(struct page *page, struct address_space *mapping,
						enum pageout_io sync_writeback)
{
	/*
	 * If the page is dirty, only perform writeback if that write
	 * will be non-blocking.  To prevent this allocation from being
	 * stalled by pagecache activity.  But note that there may be
	 * stalls if we need to run get_block().  We could test
	 * PagePrivate for that.
	 *
	 * If this process is currently in generic_file_write() against
	 * this page's queue, we can perform writeback even if that
	 * will block.
	 *
	 * If the page is swapcache, write it back even if that would
	 * block, for some throttling. This happens by accident, because
	 * swap_backing_dev_info is bust: it doesn't reflect the
	 * congestion state of the swapdevs.  Easy to fix, if needed.
	 * See swapfile.c:page_queue_congested().
	 */
	if (!is_page_cache_freeable(page))
		return PAGE_KEEP;
	if (!mapping) {
		/*
		 * Some data journaling orphaned pages can have
		 * page->mapping == NULL while being dirty with clean buffers.
		 */
		if (PagePrivate(page)) {
			if (try_to_free_buffers(page)) {
				ClearPageDirty(page);
				printk("%s: orphaned page\n", __func__);
				return PAGE_CLEAN;
			}
		}
		return PAGE_KEEP;
	}
	if (mapping->a_ops->writepage == NULL)
		return PAGE_ACTIVATE;
	if (!may_write_to_queue(mapping->backing_dev_info))
		return PAGE_KEEP;

	if (clear_page_dirty_for_io(page)) {
		int res;
		struct writeback_control wbc = {
			.sync_mode = WB_SYNC_NONE,
			.nr_to_write = SWAP_CLUSTER_MAX,
			.range_start = 0,
			.range_end = LLONG_MAX,
			.nonblocking = 1,
			.for_reclaim = 1,
		};

		SetPageReclaim(page);
		res = mapping->a_ops->writepage(page, &wbc);
		if (res < 0)
			handle_write_error(mapping, page, res);
		if (res == AOP_WRITEPAGE_ACTIVATE) {
			ClearPageReclaim(page);
			return PAGE_ACTIVATE;
		}

		/*
		 * Wait on writeback if requested to. This happens when
		 * direct reclaiming a large contiguous area and the
		 * first attempt to free a range of pages fails.
		 */
		if (PageWriteback(page) && sync_writeback == PAGEOUT_IO_SYNC)
			wait_on_page_writeback(page);

		if (!PageWriteback(page)) {
			/* synchronous write or broken a_ops? */
			ClearPageReclaim(page);
		}
		inc_zone_page_state(page, NR_VMSCAN_WRITE);
		return PAGE_SUCCESS;
	}

	return PAGE_CLEAN;
}

/*
 * Same as remove_mapping, but if the page is removed from the mapping, it
 * gets returned with a refcount of 0.
 */
static int __remove_mapping(struct address_space *mapping, struct page *page)
{
	BUG_ON(!PageLocked(page));
	BUG_ON(mapping != page_mapping(page));

	spin_lock_irq(&mapping->tree_lock);
	/*
	 * The non racy check for a busy page.
	 *
	 * Must be careful with the order of the tests. When someone has
	 * a ref to the page, it may be possible that they dirty it then
	 * drop the reference. So if PageDirty is tested before page_count
	 * here, then the following race may occur:
	 *
	 * get_user_pages(&page);
	 * [user mapping goes away]
	 * write_to(page);
	 *				!PageDirty(page)    [good]
	 * SetPageDirty(page);
	 * put_page(page);
	 *				!page_count(page)   [good, discard it]
	 *
	 * [oops, our write_to data is lost]
	 *
	 * Reversing the order of the tests ensures such a situation cannot
	 * escape unnoticed. The smp_rmb is needed to ensure the page->flags
	 * load is not satisfied before that of page->_count.
	 *
	 * Note that if SetPageDirty is always performed via set_page_dirty,
	 * and thus under tree_lock, then this ordering is not required.
	 */
	if (!page_freeze_refs(page, 2))
		goto cannot_free;
	/* note: atomic_cmpxchg in page_freeze_refs provides the smp_rmb */
	if (unlikely(PageDirty(page))) {
		page_unfreeze_refs(page, 2);
		goto cannot_free;
	}

	if (PageSwapCache(page)) {
		swp_entry_t swap = { .val = page_private(page) };
		__delete_from_swap_cache(page);
		spin_unlock_irq(&mapping->tree_lock);
		swap_free(swap);
	} else {
		__remove_from_page_cache(page);
		spin_unlock_irq(&mapping->tree_lock);
	}

	return 1;

cannot_free:
	spin_unlock_irq(&mapping->tree_lock);
	return 0;
}

/*
 * Attempt to detach a locked page from its ->mapping.  If it is dirty or if
 * someone else has a ref on the page, abort and return 0.  If it was
 * successfully detached, return 1.  Assumes the caller has a single ref on
 * this page.
 */
int remove_mapping(struct address_space *mapping, struct page *page)
{
	if (__remove_mapping(mapping, page)) {
		/*
		 * Unfreezing the refcount with 1 rather than 2 effectively
		 * drops the pagecache ref for us without requiring another
		 * atomic operation.
		 */
		page_unfreeze_refs(page, 1);
		return 1;
	}
	return 0;
}

/**
 * putback_lru_page - put previously isolated page onto appropriate LRU list
 * @page: page to be put back to appropriate lru list
 *
 * Add previously isolated @page to appropriate LRU list.
 * Page may still be unevictable for other reasons.
 *
 * lru_lock must not be held, interrupts must be enabled.
 */
#ifdef CONFIG_UNEVICTABLE_LRU
void putback_lru_page(struct page *page)
{
	int lru;
	int active = !!TestClearPageActive(page);
	int was_unevictable = PageUnevictable(page);

	VM_BUG_ON(PageLRU(page));

redo:
	ClearPageUnevictable(page);

	if (page_evictable(page, NULL)) {
		/*
		 * For evictable pages, we can use the cache.
		 * In event of a race, worst case is we end up with an
		 * unevictable page on [in]active list.
		 * We know how to handle that.
		 */
		lru = active + page_is_file_cache(page);
		lru_cache_add_lru(page, lru);
	} else {
		/*
		 * Put unevictable pages directly on zone's unevictable
		 * list.
		 */
		lru = LRU_UNEVICTABLE;
		add_page_to_unevictable_list(page);
	}

	/*
	 * page's status can change while we move it among lru. If an evictable
	 * page is on unevictable list, it never be freed. To avoid that,
	 * check after we added it to the list, again.
	 */
	if (lru == LRU_UNEVICTABLE && page_evictable(page, NULL)) {
		if (!isolate_lru_page(page)) {
			put_page(page);
			goto redo;
		}
		/* This means someone else dropped this page from LRU
		 * So, it will be freed or putback to LRU again. There is
		 * nothing to do here.
		 */
	}

	if (was_unevictable && lru != LRU_UNEVICTABLE)
		count_vm_event(UNEVICTABLE_PGRESCUED);
	else if (!was_unevictable && lru == LRU_UNEVICTABLE)
		count_vm_event(UNEVICTABLE_PGCULLED);

	put_page(page);		/* drop ref from isolate */
}

#else /* CONFIG_UNEVICTABLE_LRU */

void putback_lru_page(struct page *page)
{
	int lru;
	VM_BUG_ON(PageLRU(page));

	lru = !!TestClearPageActive(page) + page_is_file_cache(page);
	lru_cache_add_lru(page, lru);
	put_page(page);
}
#endif /* CONFIG_UNEVICTABLE_LRU */


/*
 * shrink_page_list() returns the number of reclaimed pages
 */
static unsigned long shrink_page_list(struct list_head *page_list,
					struct scan_control *sc,
					enum pageout_io sync_writeback)
{
	LIST_HEAD(ret_pages);
	struct pagevec freed_pvec;
	int pgactivate = 0;
	unsigned long nr_reclaimed = 0;

	cond_resched();

	pagevec_init(&freed_pvec, 1);
	while (!list_empty(page_list)) {
		struct address_space *mapping;
		struct page *page;
		int may_enter_fs;
		int referenced;

		cond_resched();

		page = lru_to_page(page_list);
		list_del(&page->lru);

		if (!trylock_page(page))
			goto keep;

		VM_BUG_ON(PageActive(page));

		sc->nr_scanned++;

		if (unlikely(!page_evictable(page, NULL)))
			goto cull_mlocked;

		if (!sc->may_swap && page_mapped(page))
			goto keep_locked;

		/* Double the slab pressure for mapped and swapcache pages */
		if (page_mapped(page) || PageSwapCache(page))
			sc->nr_scanned++;

		may_enter_fs = (sc->gfp_mask & __GFP_FS) ||
			(PageSwapCache(page) && (sc->gfp_mask & __GFP_IO));

		if (PageWriteback(page)) {
			/*
			 * Synchronous reclaim is performed in two passes,
			 * first an asynchronous pass over the list to
			 * start parallel writeback, and a second synchronous
			 * pass to wait for the IO to complete.  Wait here
			 * for any page for which writeback has already
			 * started.
			 */
			if (sync_writeback == PAGEOUT_IO_SYNC && may_enter_fs)
				wait_on_page_writeback(page);
			else
				goto keep_locked;
		}

		referenced = page_referenced(page, 1, sc->mem_cgroup);
		/* In active use or really unfreeable?  Activate it. */
		if (sc->order <= PAGE_ALLOC_COSTLY_ORDER &&
					referenced && page_mapping_inuse(page))
			goto activate_locked;

		/*
		 * Anonymous process memory has backing store?
		 * Try to allocate it some swap space here.
		 */
		if (PageAnon(page) && !PageSwapCache(page)) {
			if (!(sc->gfp_mask & __GFP_IO))
				goto keep_locked;
			if (!add_to_swap(page))
				goto activate_locked;
			may_enter_fs = 1;
		}

		mapping = page_mapping(page);

		/*
		 * The page is mapped into the page tables of one or more
		 * processes. Try to unmap it here.
		 */
		if (page_mapped(page) && mapping) {
			switch (try_to_unmap(page, 0)) {
			case SWAP_FAIL:
				goto activate_locked;
			case SWAP_AGAIN:
				goto keep_locked;
			case SWAP_MLOCK:
				goto cull_mlocked;
			case SWAP_SUCCESS:
				; /* try to free the page below */
			}
		}

		if (PageDirty(page)) {
			if (sc->order <= PAGE_ALLOC_COSTLY_ORDER && referenced)
				goto keep_locked;
			if (!may_enter_fs)
				goto keep_locked;
			if (!sc->may_writepage)
				goto keep_locked;

			/* Page is dirty, try to write it out here */
			switch (pageout(page, mapping, sync_writeback)) {
			case PAGE_KEEP:
				goto keep_locked;
			case PAGE_ACTIVATE:
				goto activate_locked;
			case PAGE_SUCCESS:
				if (PageWriteback(page) || PageDirty(page))
					goto keep;
				/*
				 * A synchronous write - probably a ramdisk.  Go
				 * ahead and try to reclaim the page.
				 */
				if (!trylock_page(page))
					goto keep;
				if (PageDirty(page) || PageWriteback(page))
					goto keep_locked;
				mapping = page_mapping(page);
			case PAGE_CLEAN:
				; /* try to free the page below */
			}
		}

		/*
		 * If the page has buffers, try to free the buffer mappings
		 * associated with this page. If we succeed we try to free
		 * the page as well.
		 *
		 * We do this even if the page is PageDirty().
		 * try_to_release_page() does not perform I/O, but it is
		 * possible for a page to have PageDirty set, but it is actually
		 * clean (all its buffers are clean).  This happens if the
		 * buffers were written out directly, with submit_bh(). ext3
		 * will do this, as well as the blockdev mapping.
		 * try_to_release_page() will discover that cleanness and will
		 * drop the buffers and mark the page clean - it can be freed.
		 *
		 * Rarely, pages can have buffers and no ->mapping.  These are
		 * the pages which were not successfully invalidated in
		 * truncate_complete_page().  We try to drop those buffers here
		 * and if that worked, and the page is no longer mapped into
		 * process address space (page_count == 1) it can be freed.
		 * Otherwise, leave the page on the LRU so it is swappable.
		 */
		if (PagePrivate(page)) {
			if (!try_to_release_page(page, sc->gfp_mask))
				goto activate_locked;
			if (!mapping && page_count(page) == 1) {
				unlock_page(page);
				if (put_page_testzero(page))
					goto free_it;
				else {
					/*
					 * rare race with speculative reference.
					 * the speculative reference will free
					 * this page shortly, so we may
					 * increment nr_reclaimed here (and
					 * leave it off the LRU).
					 */
					nr_reclaimed++;
					continue;
				}
			}
		}

		if (!mapping || !__remove_mapping(mapping, page))
			goto keep_locked;

		/*
		 * At this point, we have no other references and there is
		 * no way to pick any more up (removed from LRU, removed
		 * from pagecache). Can use non-atomic bitops now (and
		 * we obviously don't have to worry about waking up a process
		 * waiting on the page lock, because there are no references.
		 */
		__clear_page_locked(page);
free_it:
		nr_reclaimed++;
		if (!pagevec_add(&freed_pvec, page)) {
			__pagevec_free(&freed_pvec);
			pagevec_reinit(&freed_pvec);
		}
		continue;

cull_mlocked:
		if (PageSwapCache(page))
			try_to_free_swap(page);
		unlock_page(page);
		putback_lru_page(page);
		continue;

activate_locked:
		/* Not a candidate for swapping, so reclaim swap space. */
		if (PageSwapCache(page) && vm_swap_full())
			try_to_free_swap(page);
		VM_BUG_ON(PageActive(page));
		SetPageActive(page);
		pgactivate++;
keep_locked:
		unlock_page(page);
keep:
		list_add(&page->lru, &ret_pages);
		VM_BUG_ON(PageLRU(page) || PageUnevictable(page));
	}
	list_splice(&ret_pages, page_list);
	if (pagevec_count(&freed_pvec))
		__pagevec_free(&freed_pvec);
	count_vm_events(PGACTIVATE, pgactivate);
	return nr_reclaimed;
}

/* LRU Isolation modes. */
#define ISOLATE_INACTIVE 0	/* Isolate inactive pages. */
#define ISOLATE_ACTIVE 1	/* Isolate active pages. */
#define ISOLATE_BOTH 2		/* Isolate both active and inactive pages. */

/*
 * Attempt to remove the specified page from its LRU.  Only take this page
 * if it is of the appropriate PageActive status.  Pages which are being
 * freed elsewhere are also ignored.
 *
 * page:	page to consider
 * mode:	one of the LRU isolation modes defined above
 *
 * returns 0 on success, -ve errno on failure.
 */
int __isolate_lru_page(struct page *page, int mode, int file)
{
	int ret = -EINVAL;

	/* Only take pages on the LRU. */
	if (!PageLRU(page))
		return ret;

	/*
	 * When checking the active state, we need to be sure we are
	 * dealing with comparible boolean values.  Take the logical not
	 * of each.
	 */
	if (mode != ISOLATE_BOTH && (!PageActive(page) != !mode))
		return ret;

	if (mode != ISOLATE_BOTH && (!page_is_file_cache(page) != !file))
		return ret;

	/*
	 * When this function is being called for lumpy reclaim, we
	 * initially look into all LRU pages, active, inactive and
	 * unevictable; only give shrink_page_list evictable pages.
	 */
	if (PageUnevictable(page))
		return ret;

	ret = -EBUSY;

	if (likely(get_page_unless_zero(page))) {
		/*
		 * Be careful not to clear PageLRU until after we're
		 * sure the page is not being freed elsewhere -- the
		 * page release code relies on it.
		 */
		ClearPageLRU(page);
		ret = 0;
		mem_cgroup_del_lru(page);
	}

	return ret;
}

/*
 * zone->lru_lock is heavily contended.  Some of the functions that
 * shrink the lists perform better by taking out a batch of pages
 * and working on them outside the LRU lock.
 *
 * For pagecache intensive workloads, this function is the hottest
 * spot in the kernel (apart from copy_*_user functions).
 *
 * Appropriate locks must be held before calling this function.
 *
 * @nr_to_scan:	The number of pages to look through on the list.
 * @src:	The LRU list to pull pages off.
 * @dst:	The temp list to put pages on to.
 * @scanned:	The number of pages that were scanned.
 * @order:	The caller's attempted allocation order
 * @mode:	One of the LRU isolation modes
 * @file:	True [1] if isolating file [!anon] pages
 *
 * returns how many pages were moved onto *@dst.
 */
static unsigned long isolate_lru_pages(unsigned long nr_to_scan,
		struct list_head *src, struct list_head *dst,
		unsigned long *scanned, int order, int mode, int file)
{
	unsigned long nr_taken = 0;
	unsigned long scan;

	for (scan = 0; scan < nr_to_scan && !list_empty(src); scan++) {
		struct page *page;
		unsigned long pfn;
		unsigned long end_pfn;
		unsigned long page_pfn;
		int zone_id;

		page = lru_to_page(src);
		prefetchw_prev_lru_page(page, src, flags);

		VM_BUG_ON(!PageLRU(page));

		switch (__isolate_lru_page(page, mode, file)) {
		case 0:
			list_move(&page->lru, dst);
			nr_taken++;
			break;

		case -EBUSY:
			/* else it is being freed elsewhere */
			list_move(&page->lru, src);
			continue;

		default:
			BUG();
		}

		if (!order)
			continue;

		/*
		 * Attempt to take all pages in the order aligned region
		 * surrounding the tag page.  Only take those pages of
		 * the same active state as that tag page.  We may safely
		 * round the target page pfn down to the requested order
		 * as the mem_map is guarenteed valid out to MAX_ORDER,
		 * where that page is in a different zone we will detect
		 * it from its zone id and abort this block scan.
		 */
		zone_id = page_zone_id(page);
		page_pfn = page_to_pfn(page);
		pfn = page_pfn & ~((1 << order) - 1);
		end_pfn = pfn + (1 << order);
		for (; pfn < end_pfn; pfn++) {
			struct page *cursor_page;

			/* The target page is in the block, ignore it. */
			if (unlikely(pfn == page_pfn))
				continue;

			/* Avoid holes within the zone. */
			if (unlikely(!pfn_valid_within(pfn)))
				break;

			cursor_page = pfn_to_page(pfn);

			/* Check that we have not crossed a zone boundary. */
			if (unlikely(page_zone_id(cursor_page) != zone_id))
				continue;
			switch (__isolate_lru_page(cursor_page, mode, file)) {
			case 0:
				list_move(&cursor_page->lru, dst);
				nr_taken++;
				scan++;
				break;

			case -EBUSY:
				/* else it is being freed elsewhere */
				list_move(&cursor_page->lru, src);
			default:
				break;	/* ! on LRU or wrong list */
			}
		}
	}

	*scanned = scan;
	return nr_taken;
}

static unsigned long isolate_pages_global(unsigned long nr,
					struct list_head *dst,
					unsigned long *scanned, int order,
					int mode, struct zone *z,
					struct mem_cgroup *mem_cont,
					int active, int file)
{
	int lru = LRU_BASE;
	if (active)
		lru += LRU_ACTIVE;
	if (file)
		lru += LRU_FILE;
	return isolate_lru_pages(nr, &z->lru[lru].list, dst, scanned, order,
								mode, !!file);
}

/*
 * clear_active_flags() is a helper for shrink_active_list(), clearing
 * any active bits from the pages in the list.
 */
static unsigned long clear_active_flags(struct list_head *page_list,
					unsigned int *count)
{
	int nr_active = 0;
	int lru;
	struct page *page;

	list_for_each_entry(page, page_list, lru) {
		lru = page_is_file_cache(page);
		if (PageActive(page)) {
			lru += LRU_ACTIVE;
			ClearPageActive(page);
			nr_active++;
		}
		count[lru]++;
	}

	return nr_active;
}

/**
 * isolate_lru_page - tries to isolate a page from its LRU list
 * @page: page to isolate from its LRU list
 *
 * Isolates a @page from an LRU list, clears PageLRU and adjusts the
 * vmstat statistic corresponding to whatever LRU list the page was on.
 *
 * Returns 0 if the page was removed from an LRU list.
 * Returns -EBUSY if the page was not on an LRU list.
 *
 * The returned page will have PageLRU() cleared.  If it was found on
 * the active list, it will have PageActive set.  If it was found on
 * the unevictable list, it will have the PageUnevictable bit set. That flag
 * may need to be cleared by the caller before letting the page go.
 *
 * The vmstat statistic corresponding to the list on which the page was
 * found will be decremented.
 *
 * Restrictions:
 * (1) Must be called with an elevated refcount on the page. This is a
 *     fundamentnal difference from isolate_lru_pages (which is called
 *     without a stable reference).
 * (2) the lru_lock must not be held.
 * (3) interrupts must be enabled.
 */
int isolate_lru_page(struct page *page)
{
	int ret = -EBUSY;

	if (PageLRU(page)) {
		struct zone *zone = page_zone(page);

		spin_lock_irq(&zone->lru_lock);
		if (PageLRU(page) && get_page_unless_zero(page)) {
			int lru = page_lru(page);
			ret = 0;
			ClearPageLRU(page);

			del_page_from_lru_list(zone, page, lru);
		}
		spin_unlock_irq(&zone->lru_lock);
	}
	return ret;
}

/*
 * shrink_inactive_list() is a helper for shrink_zone().  It returns the number
 * of reclaimed pages
 */
static unsigned long shrink_inactive_list(unsigned long max_scan,
			struct zone *zone, struct scan_control *sc,
			int priority, int file)
{
	LIST_HEAD(page_list);
	struct pagevec pvec;
	unsigned long nr_scanned = 0;
	unsigned long nr_reclaimed = 0;
	struct zone_reclaim_stat *reclaim_stat = get_reclaim_stat(zone, sc);

	pagevec_init(&pvec, 1);

	lru_add_drain();
	spin_lock_irq(&zone->lru_lock);
	do {
		struct page *page;
		unsigned long nr_taken;
		unsigned long nr_scan;
		unsigned long nr_freed;
		unsigned long nr_active;
		unsigned int count[NR_LRU_LISTS] = { 0, };
		int mode = ISOLATE_INACTIVE;

		/*
		 * If we need a large contiguous chunk of memory, or have
		 * trouble getting a small set of contiguous pages, we
		 * will reclaim both active and inactive pages.
		 *
		 * We use the same threshold as pageout congestion_wait below.
		 */
		if (sc->order > PAGE_ALLOC_COSTLY_ORDER)
			mode = ISOLATE_BOTH;
		else if (sc->order && priority < DEF_PRIORITY - 2)
			mode = ISOLATE_BOTH;

		nr_taken = sc->isolate_pages(sc->swap_cluster_max,
			     &page_list, &nr_scan, sc->order, mode,
				zone, sc->mem_cgroup, 0, file);
		nr_active = clear_active_flags(&page_list, count);
		__count_vm_events(PGDEACTIVATE, nr_active);

		__mod_zone_page_state(zone, NR_ACTIVE_FILE,
						-count[LRU_ACTIVE_FILE]);
		__mod_zone_page_state(zone, NR_INACTIVE_FILE,
						-count[LRU_INACTIVE_FILE]);
		__mod_zone_page_state(zone, NR_ACTIVE_ANON,
						-count[LRU_ACTIVE_ANON]);
		__mod_zone_page_state(zone, NR_INACTIVE_ANON,
						-count[LRU_INACTIVE_ANON]);

		if (scanning_global_lru(sc))
			zone->pages_scanned += nr_scan;

		reclaim_stat->recent_scanned[0] += count[LRU_INACTIVE_ANON];
		reclaim_stat->recent_scanned[0] += count[LRU_ACTIVE_ANON];
		reclaim_stat->recent_scanned[1] += count[LRU_INACTIVE_FILE];
		reclaim_stat->recent_scanned[1] += count[LRU_ACTIVE_FILE];

		spin_unlock_irq(&zone->lru_lock);

		nr_scanned += nr_scan;
		nr_freed = shrink_page_list(&page_list, sc, PAGEOUT_IO_ASYNC);

		/*
		 * If we are direct reclaiming for contiguous pages and we do
		 * not reclaim everything in the list, try again and wait
		 * for IO to complete. This will stall high-order allocations
		 * but that should be acceptable to the caller
		 */
		if (nr_freed < nr_taken && !current_is_kswapd() &&
					sc->order > PAGE_ALLOC_COSTLY_ORDER) {
			congestion_wait(WRITE, HZ/10);

			/*
			 * The attempt at page out may have made some
			 * of the pages active, mark them inactive again.
			 */
			nr_active = clear_active_flags(&page_list, count);
			count_vm_events(PGDEACTIVATE, nr_active);

			nr_freed += shrink_page_list(&page_list, sc,
							PAGEOUT_IO_SYNC);
		}

		nr_reclaimed += nr_freed;
		local_irq_disable();
		if (current_is_kswapd()) {
			__count_zone_vm_events(PGSCAN_KSWAPD, zone, nr_scan);
			__count_vm_events(KSWAPD_STEAL, nr_freed);
		} else if (scanning_global_lru(sc))
			__count_zone_vm_events(PGSCAN_DIRECT, zone, nr_scan);

		__count_zone_vm_events(PGSTEAL, zone, nr_freed);

		if (nr_taken == 0)
			goto done;

		spin_lock(&zone->lru_lock);
		/*
		 * Put back any unfreeable pages.
		 */
		while (!list_empty(&page_list)) {
			int lru;
			page = lru_to_page(&page_list);
			VM_BUG_ON(PageLRU(page));
			list_del(&page->lru);
			if (unlikely(!page_evictable(page, NULL))) {
				spin_unlock_irq(&zone->lru_lock);
				putback_lru_page(page);
				spin_lock_irq(&zone->lru_lock);
				continue;
			}
			SetPageLRU(page);
			lru = page_lru(page);
			add_page_to_lru_list(zone, page, lru);
			if (PageActive(page)) {
				int file = !!page_is_file_cache(page);
				reclaim_stat->recent_rotated[file]++;
			}
			if (!pagevec_add(&pvec, page)) {
				spin_unlock_irq(&zone->lru_lock);
				__pagevec_release(&pvec);
				spin_lock_irq(&zone->lru_lock);
			}
		}
  	} while (nr_scanned < max_scan);
	spin_unlock(&zone->lru_lock);
done:
	local_irq_enable();
	pagevec_release(&pvec);
	return nr_reclaimed;
}

/*
 * We are about to scan this zone at a certain priority level.  If that priority
 * level is smaller (ie: more urgent) than the previous priority, then note
 * that priority level within the zone.  This is done so that when the next
 * process comes in to scan this zone, it will immediately start out at this
 * priority level rather than having to build up its own scanning priority.
 * Here, this priority affects only the reclaim-mapped threshold.
 */
static inline void note_zone_scanning_priority(struct zone *zone, int priority)
{
	if (priority < zone->prev_priority)
		zone->prev_priority = priority;
}

/*
 * This moves pages from the active list to the inactive list.
 *
 * We move them the other way if the page is referenced by one or more
 * processes, from rmap.
 *
 * If the pages are mostly unmapped, the processing is fast and it is
 * appropriate to hold zone->lru_lock across the whole operation.  But if
 * the pages are mapped, the processing is slow (page_referenced()) so we
 * should drop zone->lru_lock around each page.  It's impossible to balance
 * this, so instead we remove the pages from the LRU while processing them.
 * It is safe to rely on PG_active against the non-LRU pages in here because
 * nobody will play with that bit on a non-LRU page.
 *
 * The downside is that we have to touch page->_count against each page.
 * But we had to alter page->flags anyway.
 */


static void shrink_active_list(unsigned long nr_pages, struct zone *zone,
			struct scan_control *sc, int priority, int file)
{
	unsigned long pgmoved;
	int pgdeactivate = 0;
	unsigned long pgscanned;
	LIST_HEAD(l_hold);	/* The pages which were snipped off */
	LIST_HEAD(l_inactive);
	struct page *page;
	struct pagevec pvec;
	enum lru_list lru;
	struct zone_reclaim_stat *reclaim_stat = get_reclaim_stat(zone, sc);

	lru_add_drain();
	spin_lock_irq(&zone->lru_lock);
	pgmoved = sc->isolate_pages(nr_pages, &l_hold, &pgscanned, sc->order,
					ISOLATE_ACTIVE, zone,
					sc->mem_cgroup, 1, file);
	/*
	 * zone->pages_scanned is used for detect zone's oom
	 * mem_cgroup remembers nr_scan by itself.
	 */
	if (scanning_global_lru(sc)) {
		zone->pages_scanned += pgscanned;
	}
	reclaim_stat->recent_scanned[!!file] += pgmoved;

	if (file)
		__mod_zone_page_state(zone, NR_ACTIVE_FILE, -pgmoved);
	else
		__mod_zone_page_state(zone, NR_ACTIVE_ANON, -pgmoved);
	spin_unlock_irq(&zone->lru_lock);

	pgmoved = 0;
	while (!list_empty(&l_hold)) {
		cond_resched();
		page = lru_to_page(&l_hold);
		list_del(&page->lru);

		if (unlikely(!page_evictable(page, NULL))) {
			putback_lru_page(page);
			continue;
		}

		/* page_referenced clears PageReferenced */
		if (page_mapping_inuse(page) &&
		    page_referenced(page, 0, sc->mem_cgroup))
			pgmoved++;

		list_add(&page->lru, &l_inactive);
	}

	/*
	 * Move the pages to the [file or anon] inactive list.
	 */
	pagevec_init(&pvec, 1);
	lru = LRU_BASE + file * LRU_FILE;

	spin_lock_irq(&zone->lru_lock);
	/*
	 * Count referenced pages from currently used mappings as
	 * rotated, even though they are moved to the inactive list.
	 * This helps balance scan pressure between file and anonymous
	 * pages in get_scan_ratio.
	 */
	reclaim_stat->recent_rotated[!!file] += pgmoved;

	pgmoved = 0;
	while (!list_empty(&l_inactive)) {
		page = lru_to_page(&l_inactive);
		prefetchw_prev_lru_page(page, &l_inactive, flags);
		VM_BUG_ON(PageLRU(page));
		SetPageLRU(page);
		VM_BUG_ON(!PageActive(page));
		ClearPageActive(page);

		list_move(&page->lru, &zone->lru[lru].list);
		mem_cgroup_add_lru_list(page, lru);
		pgmoved++;
		if (!pagevec_add(&pvec, page)) {
			__mod_zone_page_state(zone, NR_LRU_BASE + lru, pgmoved);
			spin_unlock_irq(&zone->lru_lock);
			pgdeactivate += pgmoved;
			pgmoved = 0;
			if (buffer_heads_over_limit)
				pagevec_strip(&pvec);
			__pagevec_release(&pvec);
			spin_lock_irq(&zone->lru_lock);
		}
	}
	__mod_zone_page_state(zone, NR_LRU_BASE + lru, pgmoved);
	pgdeactivate += pgmoved;
	if (buffer_heads_over_limit) {
		spin_unlock_irq(&zone->lru_lock);
		pagevec_strip(&pvec);
		spin_lock_irq(&zone->lru_lock);
	}
	__count_zone_vm_events(PGREFILL, zone, pgscanned);
	__count_vm_events(PGDEACTIVATE, pgdeactivate);
	spin_unlock_irq(&zone->lru_lock);
	if (vm_swap_full())
		pagevec_swap_free(&pvec);

	pagevec_release(&pvec);
}

static int inactive_anon_is_low_global(struct zone *zone)
{
	unsigned long active, inactive;

	active = zone_page_state(zone, NR_ACTIVE_ANON);
	inactive = zone_page_state(zone, NR_INACTIVE_ANON);

	if (inactive * zone->inactive_ratio < active)
		return 1;

	return 0;
}

/**
 * inactive_anon_is_low - check if anonymous pages need to be deactivated
 * @zone: zone to check
 * @sc:   scan control of this context
 *
 * Returns true if the zone does not have enough inactive anon pages,
 * meaning some active anon pages need to be deactivated.
 */
static int inactive_anon_is_low(struct zone *zone, struct scan_control *sc)
{
	int low;

	if (scanning_global_lru(sc))
		low = inactive_anon_is_low_global(zone);
	else
		low = mem_cgroup_inactive_anon_is_low(sc->mem_cgroup);
	return low;
}

static unsigned long shrink_list(enum lru_list lru, unsigned long nr_to_scan,
	struct zone *zone, struct scan_control *sc, int priority)
{
	int file = is_file_lru(lru);

	if (lru == LRU_ACTIVE_FILE) {
		shrink_active_list(nr_to_scan, zone, sc, priority, file);
		return 0;
	}

	if (lru == LRU_ACTIVE_ANON && inactive_anon_is_low(zone, sc)) {
		shrink_active_list(nr_to_scan, zone, sc, priority, file);
		return 0;
	}
	return shrink_inactive_list(nr_to_scan, zone, sc, priority, file);
}

/*
 * Determine how aggressively the anon and file LRU lists should be
 * scanned.  The relative value of each set of LRU lists is determined
 * by looking at the fraction of the pages scanned we did rotate back
 * onto the active list instead of evict.
 *
 * percent[0] specifies how much pressure to put on ram/swap backed
 * memory, while percent[1] determines pressure on the file LRUs.
 */
static void get_scan_ratio(struct zone *zone, struct scan_control *sc,
					unsigned long *percent)
{
	unsigned long anon, file, free;
	unsigned long anon_prio, file_prio;
	unsigned long ap, fp;
	struct zone_reclaim_stat *reclaim_stat = get_reclaim_stat(zone, sc);

	/* If we have no swap space, do not bother scanning anon pages. */
	if (nr_swap_pages <= 0) {
		percent[0] = 0;
		percent[1] = 100;
		return;
	}

	anon  = zone_nr_pages(zone, sc, LRU_ACTIVE_ANON) +
		zone_nr_pages(zone, sc, LRU_INACTIVE_ANON);
	file  = zone_nr_pages(zone, sc, LRU_ACTIVE_FILE) +
		zone_nr_pages(zone, sc, LRU_INACTIVE_FILE);

	if (scanning_global_lru(sc)) {
		free  = zone_page_state(zone, NR_FREE_PAGES);
		/* If we have very few page cache pages,
		   force-scan anon pages. */
		if (unlikely(file + free <= zone->pages_high)) {
			percent[0] = 100;
			percent[1] = 0;
			return;
		}
	}

	/*
	 * OK, so we have swap space and a fair amount of page cache
	 * pages.  We use the recently rotated / recently scanned
	 * ratios to determine how valuable each cache is.
	 *
	 * Because workloads change over time (and to avoid overflow)
	 * we keep these statistics as a floating average, which ends
	 * up weighing recent references more than old ones.
	 *
	 * anon in [0], file in [1]
	 */
	if (unlikely(reclaim_stat->recent_scanned[0] > anon / 4)) {
		spin_lock_irq(&zone->lru_lock);
		reclaim_stat->recent_scanned[0] /= 2;
		reclaim_stat->recent_rotated[0] /= 2;
		spin_unlock_irq(&zone->lru_lock);
	}

	if (unlikely(reclaim_stat->recent_scanned[1] > file / 4)) {
		spin_lock_irq(&zone->lru_lock);
		reclaim_stat->recent_scanned[1] /= 2;
		reclaim_stat->recent_rotated[1] /= 2;
		spin_unlock_irq(&zone->lru_lock);
	}

	/*
	 * With swappiness at 100, anonymous and file have the same priority.
	 * This scanning priority is essentially the inverse of IO cost.
	 */
	anon_prio = sc->swappiness;
	file_prio = 200 - sc->swappiness;

	/*
	 * The amount of pressure on anon vs file pages is inversely
	 * proportional to the fraction of recently scanned pages on
	 * each list that were recently referenced and in active use.
	 */
	ap = (anon_prio + 1) * (reclaim_stat->recent_scanned[0] + 1);
	ap /= reclaim_stat->recent_rotated[0] + 1;

	fp = (file_prio + 1) * (reclaim_stat->recent_scanned[1] + 1);
	fp /= reclaim_stat->recent_rotated[1] + 1;

	/* Normalize to percentages */
	percent[0] = 100 * ap / (ap + fp + 1);
	percent[1] = 100 - percent[0];
}


/*
 * This is a basic per-zone page freer.  Used by both kswapd and direct reclaim.
 */
static void shrink_zone(int priority, struct zone *zone,
				struct scan_control *sc)
{
	unsigned long nr[NR_LRU_LISTS];
	unsigned long nr_to_scan;
	unsigned long percent[2];	/* anon @ 0; file @ 1 */
	enum lru_list l;
	unsigned long nr_reclaimed = sc->nr_reclaimed;
	unsigned long swap_cluster_max = sc->swap_cluster_max;

	get_scan_ratio(zone, sc, percent);

	for_each_evictable_lru(l) {
		int file = is_file_lru(l);
		int scan;

		scan = zone_nr_pages(zone, sc, l);
		if (priority) {
			scan >>= priority;
			scan = (scan * percent[file]) / 100;
		}
		if (scanning_global_lru(sc)) {
			zone->lru[l].nr_scan += scan;
			nr[l] = zone->lru[l].nr_scan;
			if (nr[l] >= swap_cluster_max)
				zone->lru[l].nr_scan = 0;
			else
				nr[l] = 0;
		} else
			nr[l] = scan;
	}

	while (nr[LRU_INACTIVE_ANON] || nr[LRU_ACTIVE_FILE] ||
					nr[LRU_INACTIVE_FILE]) {
		for_each_evictable_lru(l) {
			if (nr[l]) {
				nr_to_scan = min(nr[l], swap_cluster_max);
				nr[l] -= nr_to_scan;

				nr_reclaimed += shrink_list(l, nr_to_scan,
							    zone, sc, priority);
			}
		}
		/*
		 * On large memory systems, scan >> priority can become
		 * really large. This is fine for the starting priority;
		 * we want to put equal scanning pressure on each zone.
		 * However, if the VM has a harder time of freeing pages,
		 * with multiple processes reclaiming pages, the total
		 * freeing target can get unreasonably large.
		 */
		if (nr_reclaimed > swap_cluster_max &&
			priority < DEF_PRIORITY && !current_is_kswapd())
			break;
	}

	sc->nr_reclaimed = nr_reclaimed;

	/*
	 * Even if we did not try to evict anon pages at all, we want to
	 * rebalance the anon lru active/inactive ratio.
	 */
	if (inactive_anon_is_low(zone, sc))
		shrink_active_list(SWAP_CLUSTER_MAX, zone, sc, priority, 0);

	throttle_vm_writeout(sc->gfp_mask);
}

/*
 * This is the direct reclaim path, for page-allocating processes.  We only
 * try to reclaim pages from zones which will satisfy the caller's allocation
 * request.
 *
 * We reclaim from a zone even if that zone is over pages_high.  Because:
 * a) The caller may be trying to free *extra* pages to satisfy a higher-order
 *    allocation or
 * b) The zones may be over pages_high but they must go *over* pages_high to
 *    satisfy the `incremental min' zone defense algorithm.
 *
 * If a zone is deemed to be full of pinned pages then just give it a light
 * scan then give up on it.
 */
static void shrink_zones(int priority, struct zonelist *zonelist,
					struct scan_control *sc)
{
	enum zone_type high_zoneidx = gfp_zone(sc->gfp_mask);
	struct zoneref *z;
	struct zone *zone;

	sc->all_unreclaimable = 1;
	for_each_zone_zonelist(zone, z, zonelist, high_zoneidx) {
		if (!populated_zone(zone))
			continue;
		/*
		 * Take care memory controller reclaiming has small influence
		 * to global LRU.
		 */
		if (scanning_global_lru(sc)) {
			if (!cpuset_zone_allowed_hardwall(zone, GFP_KERNEL))
				continue;
			note_zone_scanning_priority(zone, priority);

			if (zone_is_all_unreclaimable(zone) &&
						priority != DEF_PRIORITY)
				continue;	/* Let kswapd poll it */
			sc->all_unreclaimable = 0;
		} else {
			/*
			 * Ignore cpuset limitation here. We just want to reduce
			 * # of used pages by us regardless of memory shortage.
			 */
			sc->all_unreclaimable = 0;
			mem_cgroup_note_reclaim_priority(sc->mem_cgroup,
							priority);
		}

		shrink_zone(priority, zone, sc);
	}
}

/*
 * This is the main entry point to direct page reclaim.
 *
 * If a full scan of the inactive list fails to free enough memory then we
 * are "out of memory" and something needs to be killed.
 *
 * If the caller is !__GFP_FS then the probability of a failure is reasonably
 * high - the zone may be full of dirty or under-writeback pages, which this
 * caller can't do much about.  We kick pdflush and take explicit naps in the
 * hope that some of these pages can be written.  But if the allocating task
 * holds filesystem locks which prevent writeout this might not work, and the
 * allocation attempt will fail.
 *
 * returns:	0, if no pages reclaimed
 * 		else, the number of pages reclaimed
 */
static unsigned long do_try_to_free_pages(struct zonelist *zonelist,
					struct scan_control *sc)
{
	int priority;
	unsigned long ret = 0;
	unsigned long total_scanned = 0;
	struct reclaim_state *reclaim_state = current->reclaim_state;
	unsigned long lru_pages = 0;
	struct zoneref *z;
	struct zone *zone;
	enum zone_type high_zoneidx = gfp_zone(sc->gfp_mask);

	delayacct_freepages_start();

	if (scanning_global_lru(sc))
		count_vm_event(ALLOCSTALL);
	/*
	 * mem_cgroup will not do shrink_slab.
	 */
	if (scanning_global_lru(sc)) {
		for_each_zone_zonelist(zone, z, zonelist, high_zoneidx) {

			if (!cpuset_zone_allowed_hardwall(zone, GFP_KERNEL))
				continue;

			lru_pages += zone_lru_pages(zone);
		}
	}

	for (priority = DEF_PRIORITY; priority >= 0; priority--) {
		sc->nr_scanned = 0;
		if (!priority)
			disable_swap_token();
		shrink_zones(priority, zonelist, sc);
		/*
		 * Don't shrink slabs when reclaiming memory from
		 * over limit cgroups
		 */
		if (scanning_global_lru(sc)) {
			shrink_slab(sc->nr_scanned, sc->gfp_mask, lru_pages);
			if (reclaim_state) {
				sc->nr_reclaimed += reclaim_state->reclaimed_slab;
				reclaim_state->reclaimed_slab = 0;
			}
		}
		total_scanned += sc->nr_scanned;
		if (sc->nr_reclaimed >= sc->swap_cluster_max) {
			ret = sc->nr_reclaimed;
			goto out;
		}

		/*
		 * Try to write back as many pages as we just scanned.  This
		 * tends to cause slow streaming writers to write data to the
		 * disk smoothly, at the dirtying rate, which is nice.   But
		 * that's undesirable in laptop mode, where we *want* lumpy
		 * writeout.  So in laptop mode, write out the whole world.
		 */
		if (total_scanned > sc->swap_cluster_max +
					sc->swap_cluster_max / 2) {
			wakeup_pdflush(laptop_mode ? 0 : total_scanned);
			sc->may_writepage = 1;
		}

		/* Take a nap, wait for some writeback to complete */
		if (sc->nr_scanned && priority < DEF_PRIORITY - 2)
			congestion_wait(WRITE, HZ/10);
	}
	/* top priority shrink_zones still had more to do? don't OOM, then */
	if (!sc->all_unreclaimable && scanning_global_lru(sc))
		ret = sc->nr_reclaimed;
out:
	/*
	 * Now that we've scanned all the zones at this priority level, note
	 * that level within the zone so that the next thread which performs
	 * scanning of this zone will immediately start out at this priority
	 * level.  This affects only the decision whether or not to bring
	 * mapped pages onto the inactive list.
	 */
	if (priority < 0)
		priority = 0;

	if (scanning_global_lru(sc)) {
		for_each_zone_zonelist(zone, z, zonelist, high_zoneidx) {

			if (!cpuset_zone_allowed_hardwall(zone, GFP_KERNEL))
				continue;

			zone->prev_priority = priority;
		}
	} else
		mem_cgroup_record_reclaim_priority(sc->mem_cgroup, priority);

	delayacct_freepages_end();

	return ret;
}

unsigned long try_to_free_pages(struct zonelist *zonelist, int order,
								gfp_t gfp_mask)
{
	struct scan_control sc = {
		.gfp_mask = gfp_mask,
		.may_writepage = !laptop_mode,
		.swap_cluster_max = SWAP_CLUSTER_MAX,
		.may_swap = 1,
		.swappiness = vm_swappiness,
		.order = order,
		.mem_cgroup = NULL,
		.isolate_pages = isolate_pages_global,
	};

	return do_try_to_free_pages(zonelist, &sc);
}

#ifdef CONFIG_CGROUP_MEM_RES_CTLR

unsigned long try_to_free_mem_cgroup_pages(struct mem_cgroup *mem_cont,
					   gfp_t gfp_mask,
					   bool noswap,
					   unsigned int swappiness)
{
	struct scan_control sc = {
		.may_writepage = !laptop_mode,
		.may_swap = 1,
		.swap_cluster_max = SWAP_CLUSTER_MAX,
		.swappiness = swappiness,
		.order = 0,
		.mem_cgroup = mem_cont,
		.isolate_pages = mem_cgroup_isolate_pages,
	};
	struct zonelist *zonelist;

	if (noswap)
		sc.may_swap = 0;

	sc.gfp_mask = (gfp_mask & GFP_RECLAIM_MASK) |
			(GFP_HIGHUSER_MOVABLE & ~GFP_RECLAIM_MASK);
	zonelist = NODE_DATA(numa_node_id())->node_zonelists;
	return do_try_to_free_pages(zonelist, &sc);
}
#endif

/*
 * For kswapd, balance_pgdat() will work across all this node's zones until
 * they are all at pages_high.
 *
 * Returns the number of pages which were actually freed.
 *
 * There is special handling here for zones which are full of pinned pages.
 * This can happen if the pages are all mlocked, or if they are all used by
 * device drivers (say, ZONE_DMA).  Or if they are all in use by hugetlb.
 * What we do is to detect the case where all pages in the zone have been
 * scanned twice and there has been zero successful reclaim.  Mark the zone as
 * dead and from now on, only perform a short scan.  Basically we're polling
 * the zone for when the problem goes away.
 *
 * kswapd scans the zones in the highmem->normal->dma direction.  It skips
 * zones which have free_pages > pages_high, but once a zone is found to have
 * free_pages <= pages_high, we scan that zone and the lower zones regardless
 * of the number of free pages in the lower zones.  This interoperates with
 * the page allocator fallback scheme to ensure that aging of pages is balanced
 * across the zones.
 */
static unsigned long balance_pgdat(pg_data_t *pgdat, int order)
{
	int all_zones_ok;
	int priority;
	int i;
	unsigned long total_scanned;
	struct reclaim_state *reclaim_state = current->reclaim_state;
	struct scan_control sc = {
		.gfp_mask = GFP_KERNEL,
		.may_swap = 1,
		.swap_cluster_max = SWAP_CLUSTER_MAX,
		.swappiness = vm_swappiness,
		.order = order,
		.mem_cgroup = NULL,
		.isolate_pages = isolate_pages_global,
	};
	/*
	 * temp_priority is used to remember the scanning priority at which
	 * this zone was successfully refilled to free_pages == pages_high.
	 */
	int temp_priority[MAX_NR_ZONES];

loop_again:
	total_scanned = 0;
	sc.nr_reclaimed = 0;
	sc.may_writepage = !laptop_mode;
	count_vm_event(PAGEOUTRUN);

	for (i = 0; i < pgdat->nr_zones; i++)
		temp_priority[i] = DEF_PRIORITY;

	for (priority = DEF_PRIORITY; priority >= 0; priority--) {
		int end_zone = 0;	/* Inclusive.  0 = ZONE_DMA */
		unsigned long lru_pages = 0;

		/* The swap token gets in the way of swapout... */
		if (!priority)
			disable_swap_token();

		all_zones_ok = 1;

		/*
		 * Scan in the highmem->dma direction for the highest
		 * zone which needs scanning
		 */
		for (i = pgdat->nr_zones - 1; i >= 0; i--) {
			struct zone *zone = pgdat->node_zones + i;

			if (!populated_zone(zone))
				continue;

			if (zone_is_all_unreclaimable(zone) &&
			    priority != DEF_PRIORITY)
				continue;

			/*
			 * Do some background aging of the anon list, to give
			 * pages a chance to be referenced before reclaiming.
			 */
			if (inactive_anon_is_low(zone, &sc))
				shrink_active_list(SWAP_CLUSTER_MAX, zone,
							&sc, priority, 0);

			if (!zone_watermark_ok(zone, order, zone->pages_high,
					       0, 0)) {
				end_zone = i;
				break;
			}
		}
		if (i < 0)
			goto out;

		for (i = 0; i <= end_zone; i++) {
			struct zone *zone = pgdat->node_zones + i;

			lru_pages += zone_lru_pages(zone);
		}

		/*
		 * Now scan the zone in the dma->highmem direction, stopping
		 * at the last zone which needs scanning.
		 *
		 * We do this because the page allocator works in the opposite
		 * direction.  This prevents the page allocator from allocating
		 * pages behind kswapd's direction of progress, which would
		 * cause too much scanning of the lower zones.
		 */
		for (i = 0; i <= end_zone; i++) {
			struct zone *zone = pgdat->node_zones + i;
			int nr_slab;

			if (!populated_zone(zone))
				continue;

			if (zone_is_all_unreclaimable(zone) &&
					priority != DEF_PRIORITY)
				continue;

			if (!zone_watermark_ok(zone, order, zone->pages_high,
					       end_zone, 0))
				all_zones_ok = 0;
			temp_priority[i] = priority;
			sc.nr_scanned = 0;
			note_zone_scanning_priority(zone, priority);
			/*
			 * We put equal pressure on every zone, unless one
			 * zone has way too many pages free already.
			 */
			if (!zone_watermark_ok(zone, order, 8*zone->pages_high,
						end_zone, 0))
				shrink_zone(priority, zone, &sc);
			reclaim_state->reclaimed_slab = 0;
			nr_slab = shrink_slab(sc.nr_scanned, GFP_KERNEL,
						lru_pages);
			sc.nr_reclaimed += reclaim_state->reclaimed_slab;
			total_scanned += sc.nr_scanned;
			if (zone_is_all_unreclaimable(zone))
				continue;
			if (nr_slab == 0 && zone->pages_scanned >=
						(zone_lru_pages(zone) * 6))
					zone_set_flag(zone,
						      ZONE_ALL_UNRECLAIMABLE);
			/*
			 * If we've done a decent amount of scanning and
			 * the reclaim ratio is low, start doing writepage
			 * even in laptop mode
			 */
			if (total_scanned > SWAP_CLUSTER_MAX * 2 &&
			    total_scanned > sc.nr_reclaimed + sc.nr_reclaimed / 2)
				sc.may_writepage = 1;
		}
		if (all_zones_ok)
			break;		/* kswapd: all done */
		/*
		 * OK, kswapd is getting into trouble.  Take a nap, then take
		 * another pass across the zones.
		 */
		if (total_scanned && priority < DEF_PRIORITY - 2)
			congestion_wait(WRITE, HZ/10);

		/*
		 * We do this so kswapd doesn't build up large priorities for
		 * example when it is freeing in parallel with allocators. It
		 * matches the direct reclaim path behaviour in terms of impact
		 * on zone->*_priority.
		 */
		if (sc.nr_reclaimed >= SWAP_CLUSTER_MAX)
			break;
	}
out:
	/*
	 * Note within each zone the priority level at which this zone was
	 * brought into a happy state.  So that the next thread which scans this
	 * zone will start out at that priority level.
	 */
	for (i = 0; i < pgdat->nr_zones; i++) {
		struct zone *zone = pgdat->node_zones + i;

		zone->prev_priority = temp_priority[i];
	}
	if (!all_zones_ok) {
		cond_resched();

		try_to_freeze();

		/*
		 * Fragmentation may mean that the system cannot be
		 * rebalanced for high-order allocations in all zones.
		 * At this point, if nr_reclaimed < SWAP_CLUSTER_MAX,
		 * it means the zones have been fully scanned and are still
		 * not balanced. For high-order allocations, there is
		 * little point trying all over again as kswapd may
		 * infinite loop.
		 *
		 * Instead, recheck all watermarks at order-0 as they
		 * are the most important. If watermarks are ok, kswapd will go
		 * back to sleep. High-order users can still perform direct
		 * reclaim if they wish.
		 */
		if (sc.nr_reclaimed < SWAP_CLUSTER_MAX)
			order = sc.order = 0;

		goto loop_again;
	}

	return sc.nr_reclaimed;
}

/*
 * The background pageout daemon, started as a kernel thread
 * from the init process.
 *
 * This basically trickles out pages so that we have _some_
 * free memory available even if there is no other activity
 * that frees anything up. This is needed for things like routing
 * etc, where we otherwise might have all activity going on in
 * asynchronous contexts that cannot page things out.
 *
 * If there are applications that are active memory-allocators
 * (most normal use), this basically shouldn't matter.
 */
static int kswapd(void *p)
{
	unsigned long order;
	pg_data_t *pgdat = (pg_data_t*)p;
	struct task_struct *tsk = current;
	DEFINE_WAIT(wait);
	struct reclaim_state reclaim_state = {
		.reclaimed_slab = 0,
	};
	node_to_cpumask_ptr(cpumask, pgdat->node_id);

	if (!cpumask_empty(cpumask))
		set_cpus_allowed_ptr(tsk, cpumask);
	current->reclaim_state = &reclaim_state;

	/*
	 * Tell the memory management that we're a "memory allocator",
	 * and that if we need more memory we should get access to it
	 * regardless (see "__alloc_pages()"). "kswapd" should
	 * never get caught in the normal page freeing logic.
	 *
	 * (Kswapd normally doesn't need memory anyway, but sometimes
	 * you need a small amount of memory in order to be able to
	 * page out something else, and this flag essentially protects
	 * us from recursively trying to free more memory as we're
	 * trying to free the first piece of memory in the first place).
	 */
	tsk->flags |= PF_MEMALLOC | PF_SWAPWRITE | PF_KSWAPD;
	set_freezable();

	order = 0;
	for ( ; ; ) {
		unsigned long new_order;

		prepare_to_wait(&pgdat->kswapd_wait, &wait, TASK_INTERRUPTIBLE);
		new_order = pgdat->kswapd_max_order;
		pgdat->kswapd_max_order = 0;
		if (order < new_order) {
			/*
			 * Don't sleep if someone wants a larger 'order'
			 * allocation
			 */
			order = new_order;
		} else {
			if (!freezing(current))
				schedule();

			order = pgdat->kswapd_max_order;
		}
		finish_wait(&pgdat->kswapd_wait, &wait);

		if (!try_to_freeze()) {
			/* We can speed up thawing tasks if we don't call
			 * balance_pgdat after returning from the refrigerator
			 */
			balance_pgdat(pgdat, order);
		}
	}
	return 0;
}

/*
 * A zone is low on free memory, so wake its kswapd task to service it.
 */
void wakeup_kswapd(struct zone *zone, int order)
{
	pg_data_t *pgdat;

	if (!populated_zone(zone))
		return;

	pgdat = zone->zone_pgdat;
	if (zone_watermark_ok(zone, order, zone->pages_low, 0, 0))
		return;
	if (pgdat->kswapd_max_order < order)
		pgdat->kswapd_max_order = order;
	if (!cpuset_zone_allowed_hardwall(zone, GFP_KERNEL))
		return;
	if (!waitqueue_active(&pgdat->kswapd_wait))
		return;
	wake_up_interruptible(&pgdat->kswapd_wait);
}

unsigned long global_lru_pages(void)
{
	return global_page_state(NR_ACTIVE_ANON)
		+ global_page_state(NR_ACTIVE_FILE)
		+ global_page_state(NR_INACTIVE_ANON)
		+ global_page_state(NR_INACTIVE_FILE);
}

#ifdef CONFIG_PM
/*
 * Helper function for shrink_all_memory().  Tries to reclaim 'nr_pages' pages
 * from LRU lists system-wide, for given pass and priority, and returns the
 * number of reclaimed pages
 *
 * For pass > 3 we also try to shrink the LRU lists that contain a few pages
 */
static unsigned long shrink_all_zones(unsigned long nr_pages, int prio,
				      int pass, struct scan_control *sc)
{
	struct zone *zone;
	unsigned long ret = 0;

	for_each_zone(zone) {
		enum lru_list l;

		if (!populated_zone(zone))
			continue;
		if (zone_is_all_unreclaimable(zone) && prio != DEF_PRIORITY)
			continue;

		for_each_evictable_lru(l) {
			enum zone_stat_item ls = NR_LRU_BASE + l;
			unsigned long lru_pages = zone_page_state(zone, ls);

			/* For pass = 0, we don't shrink the active list */
			if (pass == 0 && (l == LRU_ACTIVE_ANON ||
						l == LRU_ACTIVE_FILE))
				continue;

			zone->lru[l].nr_scan += (lru_pages >> prio) + 1;
			if (zone->lru[l].nr_scan >= nr_pages || pass > 3) {
				unsigned long nr_to_scan;

				zone->lru[l].nr_scan = 0;
				nr_to_scan = min(nr_pages, lru_pages);
				ret += shrink_list(l, nr_to_scan, zone,
								sc, prio);
				if (ret >= nr_pages)
					return ret;
			}
		}
	}
	return ret;
}

/*
 * Try to free `nr_pages' of memory, system-wide, and return the number of
 * freed pages.
 *
 * Rather than trying to age LRUs the aim is to preserve the overall
 * LRU order by reclaiming preferentially
 * inactive > active > active referenced > active mapped
 */
unsigned long shrink_all_memory(unsigned long nr_pages)
{
	unsigned long lru_pages, nr_slab;
	unsigned long ret = 0;
	int pass;
	struct reclaim_state reclaim_state;
	struct scan_control sc = {
		.gfp_mask = GFP_KERNEL,
		.may_swap = 0,
		.swap_cluster_max = nr_pages,
		.may_writepage = 1,
		.isolate_pages = isolate_pages_global,
	};

	current->reclaim_state = &reclaim_state;

	lru_pages = global_lru_pages();
	nr_slab = global_page_state(NR_SLAB_RECLAIMABLE);
	/* If slab caches are huge, it's better to hit them first */
	while (nr_slab >= lru_pages) {
		reclaim_state.reclaimed_slab = 0;
		shrink_slab(nr_pages, sc.gfp_mask, lru_pages);
		if (!reclaim_state.reclaimed_slab)
			break;

		ret += reclaim_state.reclaimed_slab;
		if (ret >= nr_pages)
			goto out;

		nr_slab -= reclaim_state.reclaimed_slab;
	}

	/*
	 * We try to shrink LRUs in 5 passes:
	 * 0 = Reclaim from inactive_list only
	 * 1 = Reclaim from active list but don't reclaim mapped
	 * 2 = 2nd pass of type 1
	 * 3 = Reclaim mapped (normal reclaim)
	 * 4 = 2nd pass of type 3
	 */
	for (pass = 0; pass < 5; pass++) {
		int prio;

		/* Force reclaiming mapped pages in the passes #3 and #4 */
		if (pass > 2)
			sc.may_swap = 1;

		for (prio = DEF_PRIORITY; prio >= 0; prio--) {
			unsigned long nr_to_scan = nr_pages - ret;

			sc.nr_scanned = 0;
			ret += shrink_all_zones(nr_to_scan, prio, pass, &sc);
			if (ret >= nr_pages)
				goto out;

			reclaim_state.reclaimed_slab = 0;
			shrink_slab(sc.nr_scanned, sc.gfp_mask,
					global_lru_pages());
			ret += reclaim_state.reclaimed_slab;
			if (ret >= nr_pages)
				goto out;

			if (sc.nr_scanned && prio < DEF_PRIORITY - 2)
				congestion_wait(WRITE, HZ / 10);
		}
	}

	/*
	 * If ret = 0, we could not shrink LRUs, but there may be something
	 * in slab caches
	 */
	if (!ret) {
		do {
			reclaim_state.reclaimed_slab = 0;
			shrink_slab(nr_pages, sc.gfp_mask, global_lru_pages());
			ret += reclaim_state.reclaimed_slab;
		} while (ret < nr_pages && reclaim_state.reclaimed_slab > 0);
	}

out:
	current->reclaim_state = NULL;

	return ret;
}
#endif

/* It's optimal to keep kswapds on the same CPUs as their memory, but
   not required for correctness.  So if the last cpu in a node goes
   away, we get changed to run anywhere: as the first one comes back,
   restore their cpu bindings. */
static int __devinit cpu_callback(struct notifier_block *nfb,
				  unsigned long action, void *hcpu)
{
	int nid;

	if (action == CPU_ONLINE || action == CPU_ONLINE_FROZEN) {
		for_each_node_state(nid, N_HIGH_MEMORY) {
			pg_data_t *pgdat = NODE_DATA(nid);
			node_to_cpumask_ptr(mask, pgdat->node_id);

			if (cpumask_any_and(cpu_online_mask, mask) < nr_cpu_ids)
				/* One of our CPUs online: restore mask */
				set_cpus_allowed_ptr(pgdat->kswapd, mask);
		}
	}
	return NOTIFY_OK;
}

/*
 * This kswapd start function will be called by init and node-hot-add.
 * On node-hot-add, kswapd will moved to proper cpus if cpus are hot-added.
 */
int kswapd_run(int nid)
{
	pg_data_t *pgdat = NODE_DATA(nid);
	int ret = 0;

	if (pgdat->kswapd)
		return 0;

	pgdat->kswapd = kthread_run(kswapd, pgdat, "kswapd%d", nid);
	if (IS_ERR(pgdat->kswapd)) {
		/* failure at boot is fatal */
		BUG_ON(system_state == SYSTEM_BOOTING);
		printk("Failed to start kswapd on node %d\n",nid);
		ret = -1;
	}
	return ret;
}

static int __init kswapd_init(void)
{
	int nid;

	swap_setup();
	for_each_node_state(nid, N_HIGH_MEMORY)
 		kswapd_run(nid);
	hotcpu_notifier(cpu_callback, 0);
	return 0;
}

module_init(kswapd_init)

#ifdef CONFIG_NUMA
/*
 * Zone reclaim mode
 *
 * If non-zero call zone_reclaim when the number of free pages falls below
 * the watermarks.
 */
int zone_reclaim_mode __read_mostly;

#define RECLAIM_OFF 0
#define RECLAIM_ZONE (1<<0)	/* Run shrink_inactive_list on the zone */
#define RECLAIM_WRITE (1<<1)	/* Writeout pages during reclaim */
#define RECLAIM_SWAP (1<<2)	/* Swap pages out during reclaim */

/*
 * Priority for ZONE_RECLAIM. This determines the fraction of pages
 * of a node considered for each zone_reclaim. 4 scans 1/16th of
 * a zone.
 */
#define ZONE_RECLAIM_PRIORITY 4

/*
 * Percentage of pages in a zone that must be unmapped for zone_reclaim to
 * occur.
 */
int sysctl_min_unmapped_ratio = 1;

/*
 * If the number of slab pages in a zone grows beyond this percentage then
 * slab reclaim needs to occur.
 */
int sysctl_min_slab_ratio = 5;

/*
 * Try to free up some pages from this zone through reclaim.
 */
static int __zone_reclaim(struct zone *zone, gfp_t gfp_mask, unsigned int order)
{
	/* Minimum pages needed in order to stay on node */
	const unsigned long nr_pages = 1 << order;
	struct task_struct *p = current;
	struct reclaim_state reclaim_state;
	int priority;
	struct scan_control sc = {
		.may_writepage = !!(zone_reclaim_mode & RECLAIM_WRITE),
		.may_swap = !!(zone_reclaim_mode & RECLAIM_SWAP),
		.swap_cluster_max = max_t(unsigned long, nr_pages,
					SWAP_CLUSTER_MAX),
		.gfp_mask = gfp_mask,
		.swappiness = vm_swappiness,
		.isolate_pages = isolate_pages_global,
	};
	unsigned long slab_reclaimable;

	disable_swap_token();
	cond_resched();
	/*
	 * We need to be able to allocate from the reserves for RECLAIM_SWAP
	 * and we also need to be able to write out pages for RECLAIM_WRITE
	 * and RECLAIM_SWAP.
	 */
	p->flags |= PF_MEMALLOC | PF_SWAPWRITE;
	reclaim_state.reclaimed_slab = 0;
	p->reclaim_state = &reclaim_state;

	if (zone_page_state(zone, NR_FILE_PAGES) -
		zone_page_state(zone, NR_FILE_MAPPED) >
		zone->min_unmapped_pages) {
		/*
		 * Free memory by calling shrink zone with increasing
		 * priorities until we have enough memory freed.
		 */
		priority = ZONE_RECLAIM_PRIORITY;
		do {
			note_zone_scanning_priority(zone, priority);
			shrink_zone(priority, zone, &sc);
			priority--;
		} while (priority >= 0 && sc.nr_reclaimed < nr_pages);
	}

	slab_reclaimable = zone_page_state(zone, NR_SLAB_RECLAIMABLE);
	if (slab_reclaimable > zone->min_slab_pages) {
		/*
		 * shrink_slab() does not currently allow us to determine how
		 * many pages were freed in this zone. So we take the current
		 * number of slab pages and shake the slab until it is reduced
		 * by the same nr_pages that we used for reclaiming unmapped
		 * pages.
		 *
		 * Note that shrink_slab will free memory on all zones and may
		 * take a long time.
		 */
		while (shrink_slab(sc.nr_scanned, gfp_mask, order) &&
			zone_page_state(zone, NR_SLAB_RECLAIMABLE) >
				slab_reclaimable - nr_pages)
			;

		/*
		 * Update nr_reclaimed by the number of slab pages we
		 * reclaimed from this zone.
		 */
		sc.nr_reclaimed += slab_reclaimable -
			zone_page_state(zone, NR_SLAB_RECLAIMABLE);
	}

	p->reclaim_state = NULL;
	current->flags &= ~(PF_MEMALLOC | PF_SWAPWRITE);
	return sc.nr_reclaimed >= nr_pages;
}

int zone_reclaim(struct zone *zone, gfp_t gfp_mask, unsigned int order)
{
	int node_id;
	int ret;

	/*
	 * Zone reclaim reclaims unmapped file backed pages and
	 * slab pages if we are over the defined limits.
	 *
	 * A small portion of unmapped file backed pages is needed for
	 * file I/O otherwise pages read by file I/O will be immediately
	 * thrown out if the zone is overallocated. So we do not reclaim
	 * if less than a specified percentage of the zone is used by
	 * unmapped file backed pages.
	 */
	if (zone_page_state(zone, NR_FILE_PAGES) -
	    zone_page_state(zone, NR_FILE_MAPPED) <= zone->min_unmapped_pages
	    && zone_page_state(zone, NR_SLAB_RECLAIMABLE)
			<= zone->min_slab_pages)
		return 0;

	if (zone_is_all_unreclaimable(zone))
		return 0;

	/*
	 * Do not scan if the allocation should not be delayed.
	 */
	if (!(gfp_mask & __GFP_WAIT) || (current->flags & PF_MEMALLOC))
			return 0;

	/*
	 * Only run zone reclaim on the local zone or on zones that do not
	 * have associated processors. This will favor the local processor
	 * over remote processors and spread off node memory allocations
	 * as wide as possible.
	 */
	node_id = zone_to_nid(zone);
	if (node_state(node_id, N_CPU) && node_id != numa_node_id())
		return 0;

	if (zone_test_and_set_flag(zone, ZONE_RECLAIM_LOCKED))
		return 0;
	ret = __zone_reclaim(zone, gfp_mask, order);
	zone_clear_flag(zone, ZONE_RECLAIM_LOCKED);

	return ret;
}
#endif

#ifdef CONFIG_UNEVICTABLE_LRU
/*
 * page_evictable - test whether a page is evictable
 * @page: the page to test
 * @vma: the VMA in which the page is or will be mapped, may be NULL
 *
 * Test whether page is evictable--i.e., should be placed on active/inactive
 * lists vs unevictable list.  The vma argument is !NULL when called from the
 * fault path to determine how to instantate a new page.
 *
 * Reasons page might not be evictable:
 * (1) page's mapping marked unevictable
 * (2) page is part of an mlocked VMA
 *
 */
int page_evictable(struct page *page, struct vm_area_struct *vma)
{

	if (mapping_unevictable(page_mapping(page)))
		return 0;

	if (PageMlocked(page) || (vma && is_mlocked_vma(vma, page)))
		return 0;

	return 1;
}

/**
 * check_move_unevictable_page - check page for evictability and move to appropriate zone lru list
 * @page: page to check evictability and move to appropriate lru list
 * @zone: zone page is in
 *
 * Checks a page for evictability and moves the page to the appropriate
 * zone lru list.
 *
 * Restrictions: zone->lru_lock must be held, page must be on LRU and must
 * have PageUnevictable set.
 */
static void check_move_unevictable_page(struct page *page, struct zone *zone)
{
	VM_BUG_ON(PageActive(page));

retry:
	ClearPageUnevictable(page);
	if (page_evictable(page, NULL)) {
		enum lru_list l = LRU_INACTIVE_ANON + page_is_file_cache(page);

		__dec_zone_state(zone, NR_UNEVICTABLE);
		list_move(&page->lru, &zone->lru[l].list);
		mem_cgroup_move_lists(page, LRU_UNEVICTABLE, l);
		__inc_zone_state(zone, NR_INACTIVE_ANON + l);
		__count_vm_event(UNEVICTABLE_PGRESCUED);
	} else {
		/*
		 * rotate unevictable list
		 */
		SetPageUnevictable(page);
		list_move(&page->lru, &zone->lru[LRU_UNEVICTABLE].list);
		mem_cgroup_rotate_lru_list(page, LRU_UNEVICTABLE);
		if (page_evictable(page, NULL))
			goto retry;
	}
}

/**
 * scan_mapping_unevictable_pages - scan an address space for evictable pages
 * @mapping: struct address_space to scan for evictable pages
 *
 * Scan all pages in mapping.  Check unevictable pages for
 * evictability and move them to the appropriate zone lru list.
 */
void scan_mapping_unevictable_pages(struct address_space *mapping)
{
	pgoff_t next = 0;
	pgoff_t end   = (i_size_read(mapping->host) + PAGE_CACHE_SIZE - 1) >>
			 PAGE_CACHE_SHIFT;
	struct zone *zone;
	struct pagevec pvec;

	if (mapping->nrpages == 0)
		return;

	pagevec_init(&pvec, 0);
	while (next < end &&
		pagevec_lookup(&pvec, mapping, next, PAGEVEC_SIZE)) {
		int i;
		int pg_scanned = 0;

		zone = NULL;

		for (i = 0; i < pagevec_count(&pvec); i++) {
			struct page *page = pvec.pages[i];
			pgoff_t page_index = page->index;
			struct zone *pagezone = page_zone(page);

			pg_scanned++;
			if (page_index > next)
				next = page_index;
			next++;

			if (pagezone != zone) {
				if (zone)
					spin_unlock_irq(&zone->lru_lock);
				zone = pagezone;
				spin_lock_irq(&zone->lru_lock);
			}

			if (PageLRU(page) && PageUnevictable(page))
				check_move_unevictable_page(page, zone);
		}
		if (zone)
			spin_unlock_irq(&zone->lru_lock);
		pagevec_release(&pvec);

		count_vm_events(UNEVICTABLE_PGSCANNED, pg_scanned);
	}

}

/**
 * scan_zone_unevictable_pages - check unevictable list for evictable pages
 * @zone - zone of which to scan the unevictable list
 *
 * Scan @zone's unevictable LRU lists to check for pages that have become
 * evictable.  Move those that have to @zone's inactive list where they
 * become candidates for reclaim, unless shrink_inactive_zone() decides
 * to reactivate them.  Pages that are still unevictable are rotated
 * back onto @zone's unevictable list.
 */
#define SCAN_UNEVICTABLE_BATCH_SIZE 16UL /* arbitrary lock hold batch size */
static void scan_zone_unevictable_pages(struct zone *zone)
{
	struct list_head *l_unevictable = &zone->lru[LRU_UNEVICTABLE].list;
	unsigned long scan;
	unsigned long nr_to_scan = zone_page_state(zone, NR_UNEVICTABLE);

	while (nr_to_scan > 0) {
		unsigned long batch_size = min(nr_to_scan,
						SCAN_UNEVICTABLE_BATCH_SIZE);

		spin_lock_irq(&zone->lru_lock);
		for (scan = 0;  scan < batch_size; scan++) {
			struct page *page = lru_to_page(l_unevictable);

			if (!trylock_page(page))
				continue;

			prefetchw_prev_lru_page(page, l_unevictable, flags);

			if (likely(PageLRU(page) && PageUnevictable(page)))
				check_move_unevictable_page(page, zone);

			unlock_page(page);
		}
		spin_unlock_irq(&zone->lru_lock);

		nr_to_scan -= batch_size;
	}
}


/**
 * scan_all_zones_unevictable_pages - scan all unevictable lists for evictable pages
 *
 * A really big hammer:  scan all zones' unevictable LRU lists to check for
 * pages that have become evictable.  Move those back to the zones'
 * inactive list where they become candidates for reclaim.
 * This occurs when, e.g., we have unswappable pages on the unevictable lists,
 * and we add swap to the system.  As such, it runs in the context of a task
 * that has possibly/probably made some previously unevictable pages
 * evictable.
 */
static void scan_all_zones_unevictable_pages(void)
{
	struct zone *zone;

	for_each_zone(zone) {
		scan_zone_unevictable_pages(zone);
	}
}

/*
 * scan_unevictable_pages [vm] sysctl handler.  On demand re-scan of
 * all nodes' unevictable lists for evictable pages
 */
unsigned long scan_unevictable_pages;

int scan_unevictable_handler(struct ctl_table *table, int write,
			   struct file *file, void __user *buffer,
			   size_t *length, loff_t *ppos)
{
	proc_doulongvec_minmax(table, write, file, buffer, length, ppos);

	if (write && *(unsigned long *)table->data)
		scan_all_zones_unevictable_pages();

	scan_unevictable_pages = 0;
	return 0;
}

/*
 * per node 'scan_unevictable_pages' attribute.  On demand re-scan of
 * a specified node's per zone unevictable lists for evictable pages.
 */

static ssize_t read_scan_unevictable_node(struct sys_device *dev,
					  struct sysdev_attribute *attr,
					  char *buf)
{
	return sprintf(buf, "0\n");	/* always zero; should fit... */
}

static ssize_t write_scan_unevictable_node(struct sys_device *dev,
					   struct sysdev_attribute *attr,
					const char *buf, size_t count)
{
	struct zone *node_zones = NODE_DATA(dev->id)->node_zones;
	struct zone *zone;
	unsigned long res;
	unsigned long req = strict_strtoul(buf, 10, &res);

	if (!req)
		return 1;	/* zero is no-op */

	for (zone = node_zones; zone - node_zones < MAX_NR_ZONES; ++zone) {
		if (!populated_zone(zone))
			continue;
		scan_zone_unevictable_pages(zone);
	}
	return 1;
}


static SYSDEV_ATTR(scan_unevictable_pages, S_IRUGO | S_IWUSR,
			read_scan_unevictable_node,
			write_scan_unevictable_node);

int scan_unevictable_register_node(struct node *node)
{
	return sysdev_create_file(&node->sysdev, &attr_scan_unevictable_pages);
}

void scan_unevictable_unregister_node(struct node *node)
{
	sysdev_remove_file(&node->sysdev, &attr_scan_unevictable_pages);
}

#endif
