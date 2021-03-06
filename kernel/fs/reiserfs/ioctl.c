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
 * Copyright 2000 by Hans Reiser, licensing governed by reiserfs/README
 */

#include <linux/capability.h>
#include <linux/fs.h>
#include <linux/mount.h>
#include <linux/reiserfs_fs.h>
#include <linux/time.h>
#include <asm/uaccess.h>
#include <linux/pagemap.h>
#include <linux/smp_lock.h>
#include <linux/compat.h>

/*
** reiserfs_ioctl - handler for ioctl for inode
** supported commands:
**  1) REISERFS_IOC_UNPACK - try to unpack tail from direct item into indirect
**                           and prevent packing file (argument arg has to be non-zero)
**  2) REISERFS_IOC_[GS]ETFLAGS, REISERFS_IOC_[GS]ETVERSION
**  3) That's all for a while ...
*/
int reiserfs_ioctl(struct inode *inode, struct file *filp, unsigned int cmd,
		   unsigned long arg)
{
	unsigned int flags;
	int err = 0;

	switch (cmd) {
	case REISERFS_IOC_UNPACK:
		if (S_ISREG(inode->i_mode)) {
			if (arg)
				return reiserfs_unpack(inode, filp);
			else
				return 0;
		} else
			return -ENOTTY;
		/* following two cases are taken from fs/ext2/ioctl.c by Remy
		   Card (card@masi.ibp.fr) */
	case REISERFS_IOC_GETFLAGS:
		if (!reiserfs_attrs(inode->i_sb))
			return -ENOTTY;

		flags = REISERFS_I(inode)->i_attrs;
		i_attrs_to_sd_attrs(inode, (__u16 *) & flags);
		return put_user(flags, (int __user *)arg);
	case REISERFS_IOC_SETFLAGS:{
			if (!reiserfs_attrs(inode->i_sb))
				return -ENOTTY;

			err = mnt_want_write(filp->f_path.mnt);
			if (err)
				return err;

			if (!is_owner_or_cap(inode)) {
				err = -EPERM;
				goto setflags_out;
			}
			if (get_user(flags, (int __user *)arg)) {
				err = -EFAULT;
				goto setflags_out;
			}
			/*
			 * Is it quota file? Do not allow user to mess with it
			 */
			if (IS_NOQUOTA(inode)) {
				err = -EPERM;
				goto setflags_out;
			}
			if (((flags ^ REISERFS_I(inode)->
			      i_attrs) & (REISERFS_IMMUTABLE_FL |
					  REISERFS_APPEND_FL))
			    && !capable(CAP_LINUX_IMMUTABLE)) {
				err = -EPERM;
				goto setflags_out;
			}
			if ((flags & REISERFS_NOTAIL_FL) &&
			    S_ISREG(inode->i_mode)) {
				int result;

				result = reiserfs_unpack(inode, filp);
				if (result) {
					err = result;
					goto setflags_out;
				}
			}
			sd_attrs_to_i_attrs(flags, inode);
			REISERFS_I(inode)->i_attrs = flags;
			inode->i_ctime = CURRENT_TIME_SEC;
			mark_inode_dirty(inode);
setflags_out:
			mnt_drop_write(filp->f_path.mnt);
			return err;
		}
	case REISERFS_IOC_GETVERSION:
		return put_user(inode->i_generation, (int __user *)arg);
	case REISERFS_IOC_SETVERSION:
		if (!is_owner_or_cap(inode))
			return -EPERM;
		err = mnt_want_write(filp->f_path.mnt);
		if (err)
			return err;
		if (get_user(inode->i_generation, (int __user *)arg)) {
			err = -EFAULT;
			goto setversion_out;
		}
		inode->i_ctime = CURRENT_TIME_SEC;
		mark_inode_dirty(inode);
setversion_out:
		mnt_drop_write(filp->f_path.mnt);
		return err;
	default:
		return -ENOTTY;
	}
}

#ifdef CONFIG_COMPAT
long reiserfs_compat_ioctl(struct file *file, unsigned int cmd,
				unsigned long arg)
{
	struct inode *inode = file->f_path.dentry->d_inode;
	int ret;

	/* These are just misnamed, they actually get/put from/to user an int */
	switch (cmd) {
	case REISERFS_IOC32_UNPACK:
		cmd = REISERFS_IOC_UNPACK;
		break;
	case REISERFS_IOC32_GETFLAGS:
		cmd = REISERFS_IOC_GETFLAGS;
		break;
	case REISERFS_IOC32_SETFLAGS:
		cmd = REISERFS_IOC_SETFLAGS;
		break;
	case REISERFS_IOC32_GETVERSION:
		cmd = REISERFS_IOC_GETVERSION;
		break;
	case REISERFS_IOC32_SETVERSION:
		cmd = REISERFS_IOC_SETVERSION;
		break;
	default:
		return -ENOIOCTLCMD;
	}
	lock_kernel();
	ret = reiserfs_ioctl(inode, file, cmd, (unsigned long) compat_ptr(arg));
	unlock_kernel();
	return ret;
}
#endif

int reiserfs_commit_write(struct file *f, struct page *page,
			  unsigned from, unsigned to);
int reiserfs_prepare_write(struct file *f, struct page *page,
			   unsigned from, unsigned to);
/*
** reiserfs_unpack
** Function try to convert tail from direct item into indirect.
** It set up nopack attribute in the REISERFS_I(inode)->nopack
*/
int reiserfs_unpack(struct inode *inode, struct file *filp)
{
	int retval = 0;
	int index;
	struct page *page;
	struct address_space *mapping;
	unsigned long write_from;
	unsigned long blocksize = inode->i_sb->s_blocksize;

	if (inode->i_size == 0) {
		REISERFS_I(inode)->i_flags |= i_nopack_mask;
		return 0;
	}
	/* ioctl already done */
	if (REISERFS_I(inode)->i_flags & i_nopack_mask) {
		return 0;
	}

	/* we need to make sure nobody is changing the file size beneath
	 ** us
	 */
	mutex_lock(&inode->i_mutex);
	reiserfs_write_lock(inode->i_sb);

	write_from = inode->i_size & (blocksize - 1);
	/* if we are on a block boundary, we are already unpacked.  */
	if (write_from == 0) {
		REISERFS_I(inode)->i_flags |= i_nopack_mask;
		goto out;
	}

	/* we unpack by finding the page with the tail, and calling
	 ** reiserfs_prepare_write on that page.  This will force a 
	 ** reiserfs_get_block to unpack the tail for us.
	 */
	index = inode->i_size >> PAGE_CACHE_SHIFT;
	mapping = inode->i_mapping;
	page = grab_cache_page(mapping, index);
	retval = -ENOMEM;
	if (!page) {
		goto out;
	}
	retval = reiserfs_prepare_write(NULL, page, write_from, write_from);
	if (retval)
		goto out_unlock;

	/* conversion can change page contents, must flush */
	flush_dcache_page(page);
	retval = reiserfs_commit_write(NULL, page, write_from, write_from);
	REISERFS_I(inode)->i_flags |= i_nopack_mask;

      out_unlock:
	unlock_page(page);
	page_cache_release(page);

      out:
	mutex_unlock(&inode->i_mutex);
	reiserfs_write_unlock(inode->i_sb);
	return retval;
}
