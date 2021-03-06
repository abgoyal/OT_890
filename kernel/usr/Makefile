# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.

# MediaTek Inc. (C) 2010. All rights reserved.
#
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
# RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
# AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.


#
# kbuild file for usr/ - including initramfs image
#

klibcdirs:;
PHONY += klibcdirs


# Generate builtin.o based on initramfs_data.o
obj-$(CONFIG_BLK_DEV_INITRD) := initramfs_data.o

# initramfs_data.o contains the initramfs_data.cpio.gz image.
# The image is included using .incbin, a dependency which is not
# tracked automatically.
$(obj)/initramfs_data.o: $(obj)/initramfs_data.cpio.gz FORCE

#####
# Generate the initramfs cpio archive

hostprogs-y := gen_init_cpio
initramfs   := $(CONFIG_SHELL) $(srctree)/scripts/gen_initramfs_list.sh
ramfs-input := $(if $(filter-out "",$(CONFIG_INITRAMFS_SOURCE)), \
			$(shell echo $(CONFIG_INITRAMFS_SOURCE)),-d)
ramfs-args  := \
        $(if $(CONFIG_INITRAMFS_ROOT_UID), -u $(CONFIG_INITRAMFS_ROOT_UID)) \
        $(if $(CONFIG_INITRAMFS_ROOT_GID), -g $(CONFIG_INITRAMFS_ROOT_GID))

# .initramfs_data.cpio.gz.d is used to identify all files included
# in initramfs and to detect if any files are added/removed.
# Removed files are identified by directory timestamp being updated
# The dependency list is generated by gen_initramfs.sh -l
ifneq ($(wildcard $(obj)/.initramfs_data.cpio.gz.d),)
	include $(obj)/.initramfs_data.cpio.gz.d
endif

quiet_cmd_initfs = GEN     $@
      cmd_initfs = $(initramfs) -o $@ $(ramfs-args) $(ramfs-input)

targets := initramfs_data.cpio.gz
# do not try to update files included in initramfs
$(deps_initramfs): ;

$(deps_initramfs): klibcdirs
# We rebuild initramfs_data.cpio.gz if:
# 1) Any included file is newer then initramfs_data.cpio.gz
# 2) There are changes in which files are included (added or deleted)
# 3) If gen_init_cpio are newer than initramfs_data.cpio.gz
# 4) arguments to gen_initramfs.sh changes
$(obj)/initramfs_data.cpio.gz: $(obj)/gen_init_cpio $(deps_initramfs) klibcdirs
	$(Q)$(initramfs) -l $(ramfs-input) > $(obj)/.initramfs_data.cpio.gz.d
	$(call if_changed,initfs)

