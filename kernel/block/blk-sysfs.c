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
 * Functions related to sysfs handling
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/bio.h>
#include <linux/blkdev.h>
#include <linux/blktrace_api.h>

#include "blk.h"

struct queue_sysfs_entry {
	struct attribute attr;
	ssize_t (*show)(struct request_queue *, char *);
	ssize_t (*store)(struct request_queue *, const char *, size_t);
};

static ssize_t
queue_var_show(unsigned int var, char *page)
{
	return sprintf(page, "%d\n", var);
}

static ssize_t
queue_var_store(unsigned long *var, const char *page, size_t count)
{
	char *p = (char *) page;

	*var = simple_strtoul(p, &p, 10);
	return count;
}

static ssize_t queue_requests_show(struct request_queue *q, char *page)
{
	return queue_var_show(q->nr_requests, (page));
}

static ssize_t
queue_requests_store(struct request_queue *q, const char *page, size_t count)
{
	struct request_list *rl = &q->rq;
	unsigned long nr;
	int ret = queue_var_store(&nr, page, count);
	if (nr < BLKDEV_MIN_RQ)
		nr = BLKDEV_MIN_RQ;

	spin_lock_irq(q->queue_lock);
	q->nr_requests = nr;
	blk_queue_congestion_threshold(q);

	if (rl->count[READ] >= queue_congestion_on_threshold(q))
		blk_set_queue_congested(q, READ);
	else if (rl->count[READ] < queue_congestion_off_threshold(q))
		blk_clear_queue_congested(q, READ);

	if (rl->count[WRITE] >= queue_congestion_on_threshold(q))
		blk_set_queue_congested(q, WRITE);
	else if (rl->count[WRITE] < queue_congestion_off_threshold(q))
		blk_clear_queue_congested(q, WRITE);

	if (rl->count[READ] >= q->nr_requests) {
		blk_set_queue_full(q, READ);
	} else if (rl->count[READ]+1 <= q->nr_requests) {
		blk_clear_queue_full(q, READ);
		wake_up(&rl->wait[READ]);
	}

	if (rl->count[WRITE] >= q->nr_requests) {
		blk_set_queue_full(q, WRITE);
	} else if (rl->count[WRITE]+1 <= q->nr_requests) {
		blk_clear_queue_full(q, WRITE);
		wake_up(&rl->wait[WRITE]);
	}
	spin_unlock_irq(q->queue_lock);
	return ret;
}

static ssize_t queue_ra_show(struct request_queue *q, char *page)
{
	int ra_kb = q->backing_dev_info.ra_pages << (PAGE_CACHE_SHIFT - 10);

	return queue_var_show(ra_kb, (page));
}

static ssize_t
queue_ra_store(struct request_queue *q, const char *page, size_t count)
{
	unsigned long ra_kb;
	ssize_t ret = queue_var_store(&ra_kb, page, count);

	q->backing_dev_info.ra_pages = ra_kb >> (PAGE_CACHE_SHIFT - 10);

	return ret;
}

static ssize_t queue_max_sectors_show(struct request_queue *q, char *page)
{
	int max_sectors_kb = q->max_sectors >> 1;

	return queue_var_show(max_sectors_kb, (page));
}

static ssize_t queue_hw_sector_size_show(struct request_queue *q, char *page)
{
	return queue_var_show(q->hardsect_size, page);
}

static ssize_t
queue_max_sectors_store(struct request_queue *q, const char *page, size_t count)
{
	unsigned long max_sectors_kb,
			max_hw_sectors_kb = q->max_hw_sectors >> 1,
			page_kb = 1 << (PAGE_CACHE_SHIFT - 10);
	ssize_t ret = queue_var_store(&max_sectors_kb, page, count);

	if (max_sectors_kb > max_hw_sectors_kb || max_sectors_kb < page_kb)
		return -EINVAL;

	spin_lock_irq(q->queue_lock);
	q->max_sectors = max_sectors_kb << 1;
	spin_unlock_irq(q->queue_lock);

	return ret;
}

static ssize_t queue_max_hw_sectors_show(struct request_queue *q, char *page)
{
	int max_hw_sectors_kb = q->max_hw_sectors >> 1;

	return queue_var_show(max_hw_sectors_kb, (page));
}

static ssize_t queue_nonrot_show(struct request_queue *q, char *page)
{
	return queue_var_show(!blk_queue_nonrot(q), page);
}

static ssize_t queue_nonrot_store(struct request_queue *q, const char *page,
				  size_t count)
{
	unsigned long nm;
	ssize_t ret = queue_var_store(&nm, page, count);

	spin_lock_irq(q->queue_lock);
	if (nm)
		queue_flag_clear(QUEUE_FLAG_NONROT, q);
	else
		queue_flag_set(QUEUE_FLAG_NONROT, q);
	spin_unlock_irq(q->queue_lock);

	return ret;
}

static ssize_t queue_nomerges_show(struct request_queue *q, char *page)
{
	return queue_var_show(blk_queue_nomerges(q), page);
}

static ssize_t queue_nomerges_store(struct request_queue *q, const char *page,
				    size_t count)
{
	unsigned long nm;
	ssize_t ret = queue_var_store(&nm, page, count);

	spin_lock_irq(q->queue_lock);
	if (nm)
		queue_flag_set(QUEUE_FLAG_NOMERGES, q);
	else
		queue_flag_clear(QUEUE_FLAG_NOMERGES, q);
	spin_unlock_irq(q->queue_lock);

	return ret;
}

static ssize_t queue_rq_affinity_show(struct request_queue *q, char *page)
{
	unsigned int set = test_bit(QUEUE_FLAG_SAME_COMP, &q->queue_flags);

	return queue_var_show(set != 0, page);
}

static ssize_t
queue_rq_affinity_store(struct request_queue *q, const char *page, size_t count)
{
	ssize_t ret = -EINVAL;
#if defined(CONFIG_USE_GENERIC_SMP_HELPERS)
	unsigned long val;

	ret = queue_var_store(&val, page, count);
	spin_lock_irq(q->queue_lock);
	if (val)
		queue_flag_set(QUEUE_FLAG_SAME_COMP, q);
	else
		queue_flag_clear(QUEUE_FLAG_SAME_COMP,  q);
	spin_unlock_irq(q->queue_lock);
#endif
	return ret;
}

static ssize_t queue_iostats_show(struct request_queue *q, char *page)
{
	return queue_var_show(blk_queue_io_stat(q), page);
}

static ssize_t queue_iostats_store(struct request_queue *q, const char *page,
				   size_t count)
{
	unsigned long stats;
	ssize_t ret = queue_var_store(&stats, page, count);

	spin_lock_irq(q->queue_lock);
	if (stats)
		queue_flag_set(QUEUE_FLAG_IO_STAT, q);
	else
		queue_flag_clear(QUEUE_FLAG_IO_STAT, q);
	spin_unlock_irq(q->queue_lock);

	return ret;
}

static struct queue_sysfs_entry queue_requests_entry = {
	.attr = {.name = "nr_requests", .mode = S_IRUGO | S_IWUSR },
	.show = queue_requests_show,
	.store = queue_requests_store,
};

static struct queue_sysfs_entry queue_ra_entry = {
	.attr = {.name = "read_ahead_kb", .mode = S_IRUGO | S_IWUSR },
	.show = queue_ra_show,
	.store = queue_ra_store,
};

static struct queue_sysfs_entry queue_max_sectors_entry = {
	.attr = {.name = "max_sectors_kb", .mode = S_IRUGO | S_IWUSR },
	.show = queue_max_sectors_show,
	.store = queue_max_sectors_store,
};

static struct queue_sysfs_entry queue_max_hw_sectors_entry = {
	.attr = {.name = "max_hw_sectors_kb", .mode = S_IRUGO },
	.show = queue_max_hw_sectors_show,
};

static struct queue_sysfs_entry queue_iosched_entry = {
	.attr = {.name = "scheduler", .mode = S_IRUGO | S_IWUSR },
	.show = elv_iosched_show,
	.store = elv_iosched_store,
};

static struct queue_sysfs_entry queue_hw_sector_size_entry = {
	.attr = {.name = "hw_sector_size", .mode = S_IRUGO },
	.show = queue_hw_sector_size_show,
};

static struct queue_sysfs_entry queue_nonrot_entry = {
	.attr = {.name = "rotational", .mode = S_IRUGO | S_IWUSR },
	.show = queue_nonrot_show,
	.store = queue_nonrot_store,
};

static struct queue_sysfs_entry queue_nomerges_entry = {
	.attr = {.name = "nomerges", .mode = S_IRUGO | S_IWUSR },
	.show = queue_nomerges_show,
	.store = queue_nomerges_store,
};

static struct queue_sysfs_entry queue_rq_affinity_entry = {
	.attr = {.name = "rq_affinity", .mode = S_IRUGO | S_IWUSR },
	.show = queue_rq_affinity_show,
	.store = queue_rq_affinity_store,
};

static struct queue_sysfs_entry queue_iostats_entry = {
	.attr = {.name = "iostats", .mode = S_IRUGO | S_IWUSR },
	.show = queue_iostats_show,
	.store = queue_iostats_store,
};

static struct attribute *default_attrs[] = {
	&queue_requests_entry.attr,
	&queue_ra_entry.attr,
	&queue_max_hw_sectors_entry.attr,
	&queue_max_sectors_entry.attr,
	&queue_iosched_entry.attr,
	&queue_hw_sector_size_entry.attr,
	&queue_nonrot_entry.attr,
	&queue_nomerges_entry.attr,
	&queue_rq_affinity_entry.attr,
	&queue_iostats_entry.attr,
	NULL,
};

#define to_queue(atr) container_of((atr), struct queue_sysfs_entry, attr)

static ssize_t
queue_attr_show(struct kobject *kobj, struct attribute *attr, char *page)
{
	struct queue_sysfs_entry *entry = to_queue(attr);
	struct request_queue *q =
		container_of(kobj, struct request_queue, kobj);
	ssize_t res;

	if (!entry->show)
		return -EIO;
	mutex_lock(&q->sysfs_lock);
	if (test_bit(QUEUE_FLAG_DEAD, &q->queue_flags)) {
		mutex_unlock(&q->sysfs_lock);
		return -ENOENT;
	}
	res = entry->show(q, page);
	mutex_unlock(&q->sysfs_lock);
	return res;
}

static ssize_t
queue_attr_store(struct kobject *kobj, struct attribute *attr,
		    const char *page, size_t length)
{
	struct queue_sysfs_entry *entry = to_queue(attr);
	struct request_queue *q;
	ssize_t res;

	if (!entry->store)
		return -EIO;

	q = container_of(kobj, struct request_queue, kobj);
	mutex_lock(&q->sysfs_lock);
	if (test_bit(QUEUE_FLAG_DEAD, &q->queue_flags)) {
		mutex_unlock(&q->sysfs_lock);
		return -ENOENT;
	}
	res = entry->store(q, page, length);
	mutex_unlock(&q->sysfs_lock);
	return res;
}

/**
 * blk_cleanup_queue: - release a &struct request_queue when it is no longer needed
 * @kobj:    the kobj belonging of the request queue to be released
 *
 * Description:
 *     blk_cleanup_queue is the pair to blk_init_queue() or
 *     blk_queue_make_request().  It should be called when a request queue is
 *     being released; typically when a block device is being de-registered.
 *     Currently, its primary task it to free all the &struct request
 *     structures that were allocated to the queue and the queue itself.
 *
 * Caveat:
 *     Hopefully the low level driver will have finished any
 *     outstanding requests first...
 **/
static void blk_release_queue(struct kobject *kobj)
{
	struct request_queue *q =
		container_of(kobj, struct request_queue, kobj);
	struct request_list *rl = &q->rq;

	blk_sync_queue(q);

	if (rl->rq_pool)
		mempool_destroy(rl->rq_pool);

	if (q->queue_tags)
		__blk_queue_free_tags(q);

	blk_trace_shutdown(q);

	bdi_destroy(&q->backing_dev_info);
	kmem_cache_free(blk_requestq_cachep, q);
}

static struct sysfs_ops queue_sysfs_ops = {
	.show	= queue_attr_show,
	.store	= queue_attr_store,
};

struct kobj_type blk_queue_ktype = {
	.sysfs_ops	= &queue_sysfs_ops,
	.default_attrs	= default_attrs,
	.release	= blk_release_queue,
};

int blk_register_queue(struct gendisk *disk)
{
	int ret;

	struct request_queue *q = disk->queue;

	if (WARN_ON(!q))
		return -ENXIO;

	if (!q->request_fn)
		return 0;

	ret = kobject_add(&q->kobj, kobject_get(&disk_to_dev(disk)->kobj),
			  "%s", "queue");
	if (ret < 0)
		return ret;

	kobject_uevent(&q->kobj, KOBJ_ADD);

	ret = elv_register_queue(q);
	if (ret) {
		kobject_uevent(&q->kobj, KOBJ_REMOVE);
		kobject_del(&q->kobj);
		return ret;
	}

	return 0;
}

void blk_unregister_queue(struct gendisk *disk)
{
	struct request_queue *q = disk->queue;

	if (WARN_ON(!q))
		return;

	if (q->request_fn) {
		elv_unregister_queue(q);

		kobject_uevent(&q->kobj, KOBJ_REMOVE);
		kobject_del(&q->kobj);
		kobject_put(&disk_to_dev(disk)->kobj);
	}
}
