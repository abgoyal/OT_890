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
# This file is included by the global makefile so that you can add your own
# architecture-specific flags and dependencies.
#
# Copyright (C) 2002 - 2007 Jeff Dike (jdike@{addtoit,linux.intel}.com)
# Licensed under the GPL
#

ARCH_DIR := arch/um
OS := $(shell uname -s)
# We require bash because the vmlinux link and loader script cpp use bash
# features.
SHELL := /bin/bash

filechk_gen_header = $<

core-y			+= $(ARCH_DIR)/kernel/		\
			   $(ARCH_DIR)/drivers/		\
			   $(ARCH_DIR)/os-$(OS)/

MODE_INCLUDE	+= -I$(srctree)/$(ARCH_DIR)/include/shared/skas

include $(srctree)/$(ARCH_DIR)/Makefile-skas

SHARED_HEADERS	:= $(ARCH_DIR)/include/shared
ARCH_INCLUDE	:= -I$(srctree)/$(SHARED_HEADERS)
ARCH_INCLUDE	+= -I$(srctree)/$(ARCH_DIR)/sys-$(SUBARCH)/shared
ifneq ($(KBUILD_SRC),)
ARCH_INCLUDE	+= -I$(SHARED_HEADERS)
endif
KBUILD_CPPFLAGS += -I$(srctree)/$(ARCH_DIR)/sys-$(SUBARCH)

# -Dvmap=kernel_vmap prevents anything from referencing the libpcap.o symbol so
# named - it's a common symbol in libpcap, so we get a binary which crashes.
#
# Same things for in6addr_loopback and mktime - found in libc. For these two we
# only get link-time error, luckily.
#
# These apply to USER_CFLAGS to.

KBUILD_CFLAGS += $(CFLAGS) $(CFLAGS-y) -D__arch_um__ -DSUBARCH=\"$(SUBARCH)\" \
	$(ARCH_INCLUDE) $(MODE_INCLUDE) -Dvmap=kernel_vmap	\
	-Din6addr_loopback=kernel_in6addr_loopback \
	-Din6addr_any=kernel_in6addr_any

KBUILD_AFLAGS += $(ARCH_INCLUDE)

USER_CFLAGS = $(patsubst $(KERNEL_DEFINES),,$(patsubst -D__KERNEL__,,\
	$(patsubst -I%,,$(KBUILD_CFLAGS)))) $(ARCH_INCLUDE) $(MODE_INCLUDE) \
	$(filter -I%,$(CFLAGS)) -D_FILE_OFFSET_BITS=64

include $(srctree)/$(ARCH_DIR)/Makefile-$(SUBARCH)

#This will adjust *FLAGS accordingly to the platform.
include $(srctree)/$(ARCH_DIR)/Makefile-os-$(OS)

KBUILD_CPPFLAGS += -I$(srctree)/arch/$(HEADER_ARCH)/include

# -Derrno=kernel_errno - This turns all kernel references to errno into
# kernel_errno to separate them from the libc errno.  This allows -fno-common
# in KBUILD_CFLAGS.  Otherwise, it would cause ld to complain about the two different
# errnos.
# These apply to kernelspace only.
#
# strip leading and trailing whitespace to make the USER_CFLAGS removal of these
# defines more robust

KERNEL_DEFINES = $(strip -Derrno=kernel_errno -Dsigprocmask=kernel_sigprocmask \
			 -Dmktime=kernel_mktime $(ARCH_KERNEL_DEFINES))
KBUILD_CFLAGS += $(KERNEL_DEFINES)

PHONY += linux

all: linux

linux: vmlinux
	@echo '  LINK $@'
	$(Q)ln -f $< $@

define archhelp
  echo '* linux		- Binary kernel image (./linux) - for backward'
  echo '		   compatibility only, this creates a hard link to the'
  echo '		   real kernel binary, the "vmlinux" binary you'
  echo '		   find in the kernel root.'
endef

KBUILD_KCONFIG := arch/um/Kconfig.$(HEADER_ARCH)

archprepare: $(SHARED_HEADERS)/user_constants.h
archprepare: $(SHARED_HEADERS)/kern_constants.h

LINK-$(CONFIG_LD_SCRIPT_STATIC) += -static
LINK-$(CONFIG_LD_SCRIPT_DYN) += -Wl,-rpath,/lib

CFLAGS_NO_HARDENING := $(call cc-option, -fno-PIC,) $(call cc-option, -fno-pic,) \
	$(call cc-option, -fno-stack-protector,) \
	$(call cc-option, -fno-stack-protector-all,)

CONFIG_KERNEL_STACK_ORDER ?= 2
STACK_SIZE := $(shell echo $$[ 4096 * (1 << $(CONFIG_KERNEL_STACK_ORDER)) ] )

CPPFLAGS_vmlinux.lds = -U$(SUBARCH) -DSTART=$(START) -DELF_ARCH=$(ELF_ARCH) \
	-DELF_FORMAT="$(ELF_FORMAT)" -DKERNEL_STACK_SIZE=$(STACK_SIZE)

# The wrappers will select whether using "malloc" or the kernel allocator.
LINK_WRAPS = -Wl,--wrap,malloc -Wl,--wrap,free -Wl,--wrap,calloc

LD_FLAGS_CMDLINE = $(foreach opt,$(LDFLAGS),-Wl,$(opt))

CFLAGS_vmlinux := $(LINK-y) $(LINK_WRAPS) $(LD_FLAGS_CMDLINE)
define cmd_vmlinux__
	$(CC) $(CFLAGS_vmlinux) -o $@ \
	-Wl,-T,$(vmlinux-lds) $(vmlinux-init) \
	-Wl,--start-group $(vmlinux-main) -Wl,--end-group \
	-lutil \
	$(filter-out $(vmlinux-lds) $(vmlinux-init) $(vmlinux-main) vmlinux.o \
	FORCE ,$^) ; rm -f linux
endef

# When cleaning we don't include .config, so we don't include
# TT or skas makefiles and don't clean skas_ptregs.h.
CLEAN_FILES += linux x.i gmon.out \
	$(SHARED_HEADERS)/user_constants.h \
	$(SHARED_HEADERS)/kern_constants.h

archclean:
	@find . \( -name '*.bb' -o -name '*.bbg' -o -name '*.da' \
		-o -name '*.gcov' \) -type f -print | xargs rm -f

# Generated files

$(ARCH_DIR)/sys-$(SUBARCH)/user-offsets.s: FORCE
	$(Q)$(MAKE) $(build)=$(ARCH_DIR)/sys-$(SUBARCH) $@

define filechk_gen-asm-offsets
        (set -e; \
         echo "/*"; \
         echo " * DO NOT MODIFY."; \
         echo " *"; \
         echo " * This file was generated by arch/$(ARCH)/Makefile"; \
         echo " *"; \
         echo " */"; \
         echo ""; \
         sed -ne "/^->/{s:^->\([^ ]*\) [\$$#]*\([^ ]*\) \(.*\):#define \1 \2 /* \3 */:; s:->::; p;}"; \
         echo ""; )
endef

$(SHARED_HEADERS)/user_constants.h: $(ARCH_DIR)/sys-$(SUBARCH)/user-offsets.s
	$(call filechk,gen-asm-offsets)

$(SHARED_HEADERS)/kern_constants.h:
	$(Q)mkdir -p $(dir $@)
	$(Q)echo '#include "../../../../include/asm/asm-offsets.h"' >$@

export SUBARCH USER_CFLAGS CFLAGS_NO_HARDENING OS HEADER_ARCH DEV_NULL_PATH
