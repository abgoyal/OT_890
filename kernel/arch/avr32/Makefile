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
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 2004-2006 Atmel Corporation.

# Default target when executing plain make
.PHONY: all
all: uImage vmlinux.elf

KBUILD_DEFCONFIG	:= atstk1002_defconfig

KBUILD_CFLAGS	+= -pipe -fno-builtin -mno-pic
KBUILD_AFLAGS	+= -mrelax -mno-pic
CFLAGS_MODULE	+= -mno-relax
LDFLAGS_vmlinux	+= --relax

cpuflags-$(CONFIG_PLATFORM_AT32AP)	+= -march=ap

KBUILD_CFLAGS	+= $(cpuflags-y)
KBUILD_AFLAGS	+= $(cpuflags-y)

CHECKFLAGS	+= -D__avr32__ -D__BIG_ENDIAN

machine-$(CONFIG_PLATFORM_AT32AP) := at32ap
machdirs	:= $(patsubst %,arch/avr32/mach-%/, $(machine-y))

KBUILD_CPPFLAGS	+= $(patsubst %,-I$(srctree)/%include,$(machdirs))

head-$(CONFIG_LOADER_U_BOOT)		+= arch/avr32/boot/u-boot/head.o
head-y					+= arch/avr32/kernel/head.o
core-y					+= $(machdirs)
core-$(CONFIG_BOARD_ATSTK1000)		+= arch/avr32/boards/atstk1000/
core-$(CONFIG_BOARD_ATNGW100)		+= arch/avr32/boards/atngw100/
core-$(CONFIG_BOARD_HAMMERHEAD)		+= arch/avr32/boards/hammerhead/
core-$(CONFIG_BOARD_FAVR_32)		+= arch/avr32/boards/favr-32/
core-$(CONFIG_BOARD_MIMC200)		+= arch/avr32/boards/mimc200/
core-$(CONFIG_LOADER_U_BOOT)		+= arch/avr32/boot/u-boot/
core-y					+= arch/avr32/kernel/
core-y					+= arch/avr32/mm/
drivers-$(CONFIG_OPROFILE)		+= arch/avr32/oprofile/
libs-y					+= arch/avr32/lib/

CLEAN_FILES += include/asm-avr32/.arch include/asm-avr32/arch

BOOT_TARGETS := vmlinux.elf vmlinux.bin uImage uImage.srec

.PHONY: $(BOOT_TARGETS) install

boot := arch/$(ARCH)/boot/images

             KBUILD_IMAGE := $(boot)/uImage
vmlinux.elf: KBUILD_IMAGE := $(boot)/vmlinux.elf
vmlinux.cso: KBUILD_IMAGE := $(boot)/vmlinux.cso
uImage.srec: KBUILD_IMAGE := $(boot)/uImage.srec
uImage:      KBUILD_IMAGE := $(boot)/uImage

quiet_cmd_listing = LST     $@
      cmd_listing = avr32-linux-objdump $(OBJDUMPFLAGS) -lS $< > $@
quiet_cmd_disasm  = DIS     $@
      cmd_disasm  = avr32-linux-objdump $(OBJDUMPFLAGS) -d $< > $@

vmlinux.elf vmlinux.bin uImage.srec uImage vmlinux.cso: vmlinux
	$(Q)$(MAKE) $(build)=$(boot) $(boot)/$@

install: vmlinux
	$(Q)$(MAKE) $(build)=$(boot) BOOTIMAGE=$(KBUILD_IMAGE) $@

vmlinux.s: vmlinux
	$(call if_changed,disasm)

vmlinux.lst: vmlinux
	$(call if_changed,listing)

CLEAN_FILES += vmlinux.s vmlinux.lst

archclean:
	$(Q)$(MAKE) $(clean)=$(boot)

define archhelp
  @echo '* vmlinux.elf		- ELF image with load address 0'
  @echo '  vmlinux.cso		- PathFinder CSO image'
  @echo '* uImage		- Create a bootable image for U-Boot'
endef
