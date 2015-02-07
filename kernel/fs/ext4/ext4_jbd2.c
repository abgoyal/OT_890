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
 * Interface between ext4 and JBD
 */

#include "ext4_jbd2.h"

int __ext4_journal_get_undo_access(const char *where, handle_t *handle,
				struct buffer_head *bh)
{
	int err = 0;

	if (ext4_handle_valid(handle)) {
		err = jbd2_journal_get_undo_access(handle, bh);
		if (err)
			ext4_journal_abort_handle(where, __func__, bh,
						  handle, err);
	}
	return err;
}

int __ext4_journal_get_write_access(const char *where, handle_t *handle,
				struct buffer_head *bh)
{
	int err = 0;

	if (ext4_handle_valid(handle)) {
		err = jbd2_journal_get_write_access(handle, bh);
		if (err)
			ext4_journal_abort_handle(where, __func__, bh,
						  handle, err);
	}
	return err;
}

int __ext4_journal_forget(const char *where, handle_t *handle,
				struct buffer_head *bh)
{
	int err = 0;

	if (ext4_handle_valid(handle)) {
		err = jbd2_journal_forget(handle, bh);
		if (err)
			ext4_journal_abort_handle(where, __func__, bh,
						  handle, err);
	}
	return err;
}

int __ext4_journal_revoke(const char *where, handle_t *handle,
				ext4_fsblk_t blocknr, struct buffer_head *bh)
{
	int err = 0;

	if (ext4_handle_valid(handle)) {
		err = jbd2_journal_revoke(handle, blocknr, bh);
		if (err)
			ext4_journal_abort_handle(where, __func__, bh,
						  handle, err);
	}
	return err;
}

int __ext4_journal_get_create_access(const char *where,
				handle_t *handle, struct buffer_head *bh)
{
	int err = 0;

	if (ext4_handle_valid(handle)) {
		err = jbd2_journal_get_create_access(handle, bh);
		if (err)
			ext4_journal_abort_handle(where, __func__, bh,
						  handle, err);
	}
	return err;
}

int __ext4_handle_dirty_metadata(const char *where, handle_t *handle,
				 struct inode *inode, struct buffer_head *bh)
{
	int err = 0;

	if (ext4_handle_valid(handle)) {
		err = jbd2_journal_dirty_metadata(handle, bh);
		if (err)
			ext4_journal_abort_handle(where, __func__, bh,
						  handle, err);
	} else {
		mark_buffer_dirty(bh);
		if (inode && inode_needs_sync(inode)) {
			sync_dirty_buffer(bh);
			if (buffer_req(bh) && !buffer_uptodate(bh)) {
				ext4_error(inode->i_sb, __func__,
					   "IO error syncing inode, "
					   "inode=%lu, block=%llu",
					   inode->i_ino,
					   (unsigned long long) bh->b_blocknr);
				err = -EIO;
			}
		}
	}
	return err;
}
