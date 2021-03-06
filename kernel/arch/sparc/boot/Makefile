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


# Makefile for the Sparc boot stuff.
#
# Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
# Copyright (C) 1997,1998 Jakub Jelinek (jj@ultra.linux.cz)

ROOT_IMG	:= /usr/src/root.img
ELFTOAOUT	:= elftoaout

hostprogs-y	:= piggyback_32 piggyback_64 btfixupprep
targets		:= tftpboot.img btfix.o btfix.S image zImage vmlinux.aout
clean-files	:= System.map

quiet_cmd_elftoaout	= ELFTOAOUT $@
      cmd_elftoaout	= $(ELFTOAOUT) $(obj)/image -o $@

ifeq ($(CONFIG_SPARC32),y)
quiet_cmd_piggy		= PIGGY   $@
      cmd_piggy		= $(obj)/piggyback_32 $@ $(obj)/System.map $(ROOT_IMG)
quiet_cmd_btfix		= BTFIX   $@
      cmd_btfix		= $(OBJDUMP) -x vmlinux | $(obj)/btfixupprep > $@
quiet_cmd_sysmap        = SYSMAP  $(obj)/System.map
      cmd_sysmap        = $(CONFIG_SHELL) $(srctree)/scripts/mksysmap
quiet_cmd_image = LD      $@
      cmd_image = $(LD) $(LDFLAGS) $(EXTRA_LDFLAGS) $(LDFLAGS_$(@F)) -o $@
quiet_cmd_strip = STRIP   $@
      cmd_strip = $(STRIP) -R .comment -R .note -K sun4u_init -K _end -K _start $(obj)/image -o $@


define rule_image
	$(if $($(quiet)cmd_image),               \
	  echo '  $($(quiet)cmd_image)' &&)      \
	  $(cmd_image);                          \
	$(if $($(quiet)cmd_sysmap),              \
	  echo '  $($(quiet)cmd_sysmap)' &&)  \
	$(cmd_sysmap) $@ $(obj)/System.map;      \
	if [ $$? -ne 0 ]; then                   \
		rm -f $@;                        \
		/bin/false;                      \
	fi;                                      \
	echo 'cmd_$@ := $(cmd_image)' > $(@D)/.$(@F).cmd
endef

BTOBJS := $(patsubst %/, %/built-in.o, $(VMLINUX_INIT))
BTLIBS := $(patsubst %/, %/built-in.o, $(VMLINUX_MAIN))
LDFLAGS_image := -T arch/sparc/kernel/vmlinux.lds $(BTOBJS) \
                  --start-group $(BTLIBS) --end-group \
                  $(kallsyms.o) $(obj)/btfix.o

# Link the final image including btfixup'ed symbols.
# This is a replacement for the link done in the top-level Makefile.
# Note: No dependency on the prerequisite files since that would require
# make to try check if they are updated - and due to changes
# in gcc options (path for example) this would result in
# these files being recompiled for each build.
$(obj)/image: $(obj)/btfix.o FORCE
	$(call if_changed_rule,image)

$(obj)/zImage: $(obj)/image
	$(call if_changed,strip)

$(obj)/tftpboot.img: $(obj)/piggyback $(obj)/System.map $(obj)/image FORCE
	$(call if_changed,elftoaout)
	$(call if_changed,piggy)

$(obj)/btfix.S: $(obj)/btfixupprep vmlinux FORCE
	$(call if_changed,btfix)

endif

ifeq ($(CONFIG_SPARC64),y)
quiet_cmd_piggy     = PIGGY   $@
      cmd_piggy     = $(obj)/piggyback_64 $@ System.map $(ROOT_IMG)
quiet_cmd_strip     = STRIP   $@
      cmd_strip     = $(STRIP) -R .comment -R .note -K sun4u_init -K _end -K _start vmlinux -o $@


# Actual linking
$(obj)/image: vmlinux FORCE
	$(call if_changed,strip)
	@echo '  kernel: $@ is ready'

$(obj)/tftpboot.img: vmlinux $(obj)/piggyback_64 System.map $(ROOT_IMG) FORCE
	$(call if_changed,elftoaout)
	$(call if_changed,piggy)
	@echo '  kernel: $@ is ready'

$(obj)/vmlinux.aout: vmlinux FORCE
	$(call if_changed,elftoaout)
	@echo '  kernel: $@ is ready'
endif

