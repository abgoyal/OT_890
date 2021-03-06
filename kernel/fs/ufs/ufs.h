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

#ifndef _UFS_UFS_H
#define _UFS_UFS_H 1

#define UFS_MAX_GROUP_LOADED 8
#define UFS_CGNO_EMPTY ((unsigned)-1)

struct ufs_sb_private_info;
struct ufs_cg_private_info;
struct ufs_csum;

struct ufs_sb_info {
	struct ufs_sb_private_info * s_uspi;
	struct ufs_csum	* s_csp;
	unsigned s_bytesex;
	unsigned s_flags;
	struct buffer_head ** s_ucg;
	struct ufs_cg_private_info * s_ucpi[UFS_MAX_GROUP_LOADED];
	unsigned s_cgno[UFS_MAX_GROUP_LOADED];
	unsigned short s_cg_loaded;
	unsigned s_mount_opt;
};

struct ufs_inode_info {
	union {
		__fs32	i_data[15];
		__u8	i_symlink[4*15];
		__fs64	u2_i_data[15];
	} i_u1;
	__u32	i_flags;
	__u32	i_shadow;
	__u32	i_unused1;
	__u32	i_unused2;
	__u32	i_oeftflag;
	__u16	i_osync;
	__u64	i_lastfrag;
	__u32   i_dir_start_lookup;
	struct inode vfs_inode;
};

/* mount options */
#define UFS_MOUNT_ONERROR		0x0000000F
#define UFS_MOUNT_ONERROR_PANIC		0x00000001
#define UFS_MOUNT_ONERROR_LOCK		0x00000002
#define UFS_MOUNT_ONERROR_UMOUNT	0x00000004
#define UFS_MOUNT_ONERROR_REPAIR	0x00000008

#define UFS_MOUNT_UFSTYPE		0x0000FFF0
#define UFS_MOUNT_UFSTYPE_OLD		0x00000010
#define UFS_MOUNT_UFSTYPE_44BSD		0x00000020
#define UFS_MOUNT_UFSTYPE_SUN		0x00000040
#define UFS_MOUNT_UFSTYPE_NEXTSTEP	0x00000080
#define UFS_MOUNT_UFSTYPE_NEXTSTEP_CD	0x00000100
#define UFS_MOUNT_UFSTYPE_OPENSTEP	0x00000200
#define UFS_MOUNT_UFSTYPE_SUNx86	0x00000400
#define UFS_MOUNT_UFSTYPE_HP	        0x00000800
#define UFS_MOUNT_UFSTYPE_UFS2		0x00001000
#define UFS_MOUNT_UFSTYPE_SUNOS		0x00002000

#define ufs_clear_opt(o,opt)	o &= ~UFS_MOUNT_##opt
#define ufs_set_opt(o,opt)	o |= UFS_MOUNT_##opt
#define ufs_test_opt(o,opt)	((o) & UFS_MOUNT_##opt)

/*
 * Debug code
 */
#ifdef CONFIG_UFS_DEBUG
#	define UFSD(f, a...)	{					\
		printk ("UFSD (%s, %d): %s:",				\
			__FILE__, __LINE__, __func__);		\
		printk (f, ## a);					\
	}
#else
#	define UFSD(f, a...)	/**/
#endif

/* balloc.c */
extern void ufs_free_fragments (struct inode *, u64, unsigned);
extern void ufs_free_blocks (struct inode *, u64, unsigned);
extern u64 ufs_new_fragments(struct inode *, void *, u64, u64,
			     unsigned, int *, struct page *);

/* cylinder.c */
extern struct ufs_cg_private_info * ufs_load_cylinder (struct super_block *, unsigned);
extern void ufs_put_cylinder (struct super_block *, unsigned);

/* dir.c */
extern const struct inode_operations ufs_dir_inode_operations;
extern int ufs_add_link (struct dentry *, struct inode *);
extern ino_t ufs_inode_by_name(struct inode *, struct dentry *);
extern int ufs_make_empty(struct inode *, struct inode *);
extern struct ufs_dir_entry *ufs_find_entry(struct inode *, struct dentry *, struct page **);
extern int ufs_delete_entry(struct inode *, struct ufs_dir_entry *, struct page *);
extern int ufs_empty_dir (struct inode *);
extern struct ufs_dir_entry *ufs_dotdot(struct inode *, struct page **);
extern void ufs_set_link(struct inode *dir, struct ufs_dir_entry *de,
			 struct page *page, struct inode *inode);

/* file.c */
extern const struct inode_operations ufs_file_inode_operations;
extern const struct file_operations ufs_file_operations;

extern const struct address_space_operations ufs_aops;

/* ialloc.c */
extern void ufs_free_inode (struct inode *inode);
extern struct inode * ufs_new_inode (struct inode *, int);

/* inode.c */
extern struct inode *ufs_iget(struct super_block *, unsigned long);
extern int ufs_write_inode (struct inode *, int);
extern int ufs_sync_inode (struct inode *);
extern void ufs_delete_inode (struct inode *);
extern struct buffer_head * ufs_bread (struct inode *, unsigned, int, int *);
extern int ufs_getfrag_block (struct inode *inode, sector_t fragment, struct buffer_head *bh_result, int create);

/* namei.c */
extern const struct file_operations ufs_dir_operations;

/* super.c */
extern void ufs_warning (struct super_block *, const char *, const char *, ...) __attribute__ ((format (printf, 3, 4)));
extern void ufs_error (struct super_block *, const char *, const char *, ...) __attribute__ ((format (printf, 3, 4)));
extern void ufs_panic (struct super_block *, const char *, const char *, ...) __attribute__ ((format (printf, 3, 4)));

/* symlink.c */
extern const struct inode_operations ufs_fast_symlink_inode_operations;

/* truncate.c */
extern int ufs_truncate (struct inode *, loff_t);

static inline struct ufs_sb_info *UFS_SB(struct super_block *sb)
{
	return sb->s_fs_info;
}

static inline struct ufs_inode_info *UFS_I(struct inode *inode)
{
	return container_of(inode, struct ufs_inode_info, vfs_inode);
}

/*
 * Give cylinder group number for a file system block.
 * Give cylinder group block number for a file system block.
 */
/* #define	ufs_dtog(d)	((d) / uspi->s_fpg) */
static inline u64 ufs_dtog(struct ufs_sb_private_info * uspi, u64 b)
{
	do_div(b, uspi->s_fpg);
	return b;
}
/* #define	ufs_dtogd(d)	((d) % uspi->s_fpg) */
static inline u32 ufs_dtogd(struct ufs_sb_private_info * uspi, u64 b)
{
	return do_div(b, uspi->s_fpg);
}

#endif /* _UFS_UFS_H */
