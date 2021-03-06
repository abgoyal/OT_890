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
 * Copyright (C) 2001-2002 Sistina Software (UK) Limited.
 *
 * This file is released under the GPL.
 */

#ifndef DM_SNAPSHOT_H
#define DM_SNAPSHOT_H

#include <linux/device-mapper.h>
#include "dm-exception-store.h"
#include "dm-bio-list.h"
#include <linux/blkdev.h>
#include <linux/workqueue.h>

struct exception_table {
	uint32_t hash_mask;
	unsigned hash_shift;
	struct list_head *table;
};

#define DM_TRACKED_CHUNK_HASH_SIZE	16
#define DM_TRACKED_CHUNK_HASH(x)	((unsigned long)(x) & \
					 (DM_TRACKED_CHUNK_HASH_SIZE - 1))

struct dm_snapshot {
	struct rw_semaphore lock;
	struct dm_target *ti;

	struct dm_dev *origin;
	struct dm_dev *cow;

	/* List of snapshots per Origin */
	struct list_head list;

	/* Size of data blocks saved - must be a power of 2 */
	chunk_t chunk_size;
	chunk_t chunk_mask;
	chunk_t chunk_shift;

	/* You can't use a snapshot if this is 0 (e.g. if full) */
	int valid;

	/* Origin writes don't trigger exceptions until this is set */
	int active;

	/* Used for display of table */
	char type;

	mempool_t *pending_pool;

	atomic_t pending_exceptions_count;

	struct exception_table pending;
	struct exception_table complete;

	/*
	 * pe_lock protects all pending_exception operations and access
	 * as well as the snapshot_bios list.
	 */
	spinlock_t pe_lock;

	/* The on disk metadata handler */
	struct dm_exception_store store;

	struct dm_kcopyd_client *kcopyd_client;

	/* Queue of snapshot writes for ksnapd to flush */
	struct bio_list queued_bios;
	struct work_struct queued_bios_work;

	/* Chunks with outstanding reads */
	mempool_t *tracked_chunk_pool;
	spinlock_t tracked_chunk_lock;
	struct hlist_head tracked_chunk_hash[DM_TRACKED_CHUNK_HASH_SIZE];
};

/*
 * Return the number of sectors in the device.
 */
static inline sector_t get_dev_size(struct block_device *bdev)
{
	return bdev->bd_inode->i_size >> SECTOR_SHIFT;
}

static inline chunk_t sector_to_chunk(struct dm_snapshot *s, sector_t sector)
{
	return (sector & ~s->chunk_mask) >> s->chunk_shift;
}

static inline sector_t chunk_to_sector(struct dm_snapshot *s, chunk_t chunk)
{
	return chunk << s->chunk_shift;
}

static inline int bdev_equal(struct block_device *lhs, struct block_device *rhs)
{
	/*
	 * There is only ever one instance of a particular block
	 * device so we can compare pointers safely.
	 */
	return lhs == rhs;
}

#endif
