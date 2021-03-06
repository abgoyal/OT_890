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
 *  linux/fs/hfsplus/ioctl.c
 *
 * Copyright (C) 2003
 * Ethan Benson <erbenson@alaska.net>
 * partially derived from linux/fs/ext2/ioctl.c
 * Copyright (C) 1993, 1994, 1995
 * Remy Card (card@masi.ibp.fr)
 * Laboratoire MASI - Institut Blaise Pascal
 * Universite Pierre et Marie Curie (Paris VI)
 *
 * hfsplus ioctls
 */

#include <linux/capability.h>
#include <linux/fs.h>
#include <linux/mount.h>
#include <linux/sched.h>
#include <linux/xattr.h>
#include <asm/uaccess.h>
#include "hfsplus_fs.h"

int hfsplus_ioctl(struct inode *inode, struct file *filp, unsigned int cmd,
		  unsigned long arg)
{
	unsigned int flags;

	switch (cmd) {
	case HFSPLUS_IOC_EXT2_GETFLAGS:
		flags = 0;
		if (HFSPLUS_I(inode).rootflags & HFSPLUS_FLG_IMMUTABLE)
			flags |= FS_IMMUTABLE_FL; /* EXT2_IMMUTABLE_FL */
		if (HFSPLUS_I(inode).rootflags & HFSPLUS_FLG_APPEND)
			flags |= FS_APPEND_FL; /* EXT2_APPEND_FL */
		if (HFSPLUS_I(inode).userflags & HFSPLUS_FLG_NODUMP)
			flags |= FS_NODUMP_FL; /* EXT2_NODUMP_FL */
		return put_user(flags, (int __user *)arg);
	case HFSPLUS_IOC_EXT2_SETFLAGS: {
		int err = 0;
		err = mnt_want_write(filp->f_path.mnt);
		if (err)
			return err;

		if (!is_owner_or_cap(inode)) {
			err = -EACCES;
			goto setflags_out;
		}
		if (get_user(flags, (int __user *)arg)) {
			err = -EFAULT;
			goto setflags_out;
		}
		if (flags & (FS_IMMUTABLE_FL|FS_APPEND_FL) ||
		    HFSPLUS_I(inode).rootflags & (HFSPLUS_FLG_IMMUTABLE|HFSPLUS_FLG_APPEND)) {
			if (!capable(CAP_LINUX_IMMUTABLE)) {
				err = -EPERM;
				goto setflags_out;
			}
		}

		/* don't silently ignore unsupported ext2 flags */
		if (flags & ~(FS_IMMUTABLE_FL|FS_APPEND_FL|FS_NODUMP_FL)) {
			err = -EOPNOTSUPP;
			goto setflags_out;
		}
		if (flags & FS_IMMUTABLE_FL) { /* EXT2_IMMUTABLE_FL */
			inode->i_flags |= S_IMMUTABLE;
			HFSPLUS_I(inode).rootflags |= HFSPLUS_FLG_IMMUTABLE;
		} else {
			inode->i_flags &= ~S_IMMUTABLE;
			HFSPLUS_I(inode).rootflags &= ~HFSPLUS_FLG_IMMUTABLE;
		}
		if (flags & FS_APPEND_FL) { /* EXT2_APPEND_FL */
			inode->i_flags |= S_APPEND;
			HFSPLUS_I(inode).rootflags |= HFSPLUS_FLG_APPEND;
		} else {
			inode->i_flags &= ~S_APPEND;
			HFSPLUS_I(inode).rootflags &= ~HFSPLUS_FLG_APPEND;
		}
		if (flags & FS_NODUMP_FL) /* EXT2_NODUMP_FL */
			HFSPLUS_I(inode).userflags |= HFSPLUS_FLG_NODUMP;
		else
			HFSPLUS_I(inode).userflags &= ~HFSPLUS_FLG_NODUMP;

		inode->i_ctime = CURRENT_TIME_SEC;
		mark_inode_dirty(inode);
setflags_out:
		mnt_drop_write(filp->f_path.mnt);
		return err;
	}
	default:
		return -ENOTTY;
	}
}

int hfsplus_setxattr(struct dentry *dentry, const char *name,
		     const void *value, size_t size, int flags)
{
	struct inode *inode = dentry->d_inode;
	struct hfs_find_data fd;
	hfsplus_cat_entry entry;
	struct hfsplus_cat_file *file;
	int res;

	if (!S_ISREG(inode->i_mode) || HFSPLUS_IS_RSRC(inode))
		return -EOPNOTSUPP;

	res = hfs_find_init(HFSPLUS_SB(inode->i_sb).cat_tree, &fd);
	if (res)
		return res;
	res = hfsplus_find_cat(inode->i_sb, inode->i_ino, &fd);
	if (res)
		goto out;
	hfs_bnode_read(fd.bnode, &entry, fd.entryoffset,
			sizeof(struct hfsplus_cat_file));
	file = &entry.file;

	if (!strcmp(name, "hfs.type")) {
		if (size == 4)
			memcpy(&file->user_info.fdType, value, 4);
		else
			res = -ERANGE;
	} else if (!strcmp(name, "hfs.creator")) {
		if (size == 4)
			memcpy(&file->user_info.fdCreator, value, 4);
		else
			res = -ERANGE;
	} else
		res = -EOPNOTSUPP;
	if (!res)
		hfs_bnode_write(fd.bnode, &entry, fd.entryoffset,
				sizeof(struct hfsplus_cat_file));
out:
	hfs_find_exit(&fd);
	return res;
}

ssize_t hfsplus_getxattr(struct dentry *dentry, const char *name,
			 void *value, size_t size)
{
	struct inode *inode = dentry->d_inode;
	struct hfs_find_data fd;
	hfsplus_cat_entry entry;
	struct hfsplus_cat_file *file;
	ssize_t res = 0;

	if (!S_ISREG(inode->i_mode) || HFSPLUS_IS_RSRC(inode))
		return -EOPNOTSUPP;

	if (size) {
		res = hfs_find_init(HFSPLUS_SB(inode->i_sb).cat_tree, &fd);
		if (res)
			return res;
		res = hfsplus_find_cat(inode->i_sb, inode->i_ino, &fd);
		if (res)
			goto out;
		hfs_bnode_read(fd.bnode, &entry, fd.entryoffset,
				sizeof(struct hfsplus_cat_file));
	}
	file = &entry.file;

	if (!strcmp(name, "hfs.type")) {
		if (size >= 4) {
			memcpy(value, &file->user_info.fdType, 4);
			res = 4;
		} else
			res = size ? -ERANGE : 4;
	} else if (!strcmp(name, "hfs.creator")) {
		if (size >= 4) {
			memcpy(value, &file->user_info.fdCreator, 4);
			res = 4;
		} else
			res = size ? -ERANGE : 4;
	} else
		res = -ENODATA;
out:
	if (size)
		hfs_find_exit(&fd);
	return res;
}

#define HFSPLUS_ATTRLIST_SIZE (sizeof("hfs.creator")+sizeof("hfs.type"))

ssize_t hfsplus_listxattr(struct dentry *dentry, char *buffer, size_t size)
{
	struct inode *inode = dentry->d_inode;

	if (!S_ISREG(inode->i_mode) || HFSPLUS_IS_RSRC(inode))
		return -EOPNOTSUPP;

	if (!buffer || !size)
		return HFSPLUS_ATTRLIST_SIZE;
	if (size < HFSPLUS_ATTRLIST_SIZE)
		return -ERANGE;
	strcpy(buffer, "hfs.type");
	strcpy(buffer + sizeof("hfs.type"), "hfs.creator");

	return HFSPLUS_ATTRLIST_SIZE;
}
