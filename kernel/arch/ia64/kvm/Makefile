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


#This Make file is to generate asm-offsets.h and build source.
#

#Generate asm-offsets.h for vmm module build
offsets-file := asm-offsets.h

always  := $(offsets-file)
targets := $(offsets-file)
targets += arch/ia64/kvm/asm-offsets.s

# Default sed regexp - multiline due to syntax constraints
define sed-y
	"/^->/{s:^->\([^ ]*\) [\$$#]*\([^ ]*\) \(.*\):#define \1 \2 /* \3 */:; s:->::; p;}"
endef

quiet_cmd_offsets = GEN     $@
define cmd_offsets
	(set -e; \
	 echo "#ifndef __ASM_KVM_OFFSETS_H__"; \
	 echo "#define __ASM_KVM_OFFSETS_H__"; \
	 echo "/*"; \
	 echo " * DO NOT MODIFY."; \
	 echo " *"; \
	 echo " * This file was generated by Makefile"; \
	 echo " *"; \
	 echo " */"; \
	 echo ""; \
	 sed -ne $(sed-y) $<; \
	 echo ""; \
	 echo "#endif" ) > $@
endef

# We use internal rules to avoid the "is up to date" message from make
arch/ia64/kvm/asm-offsets.s: arch/ia64/kvm/asm-offsets.c \
			$(wildcard $(srctree)/arch/ia64/include/asm/*.h)\
			$(wildcard $(srctree)/include/linux/*.h)
	$(call if_changed_dep,cc_s_c)

$(obj)/$(offsets-file): arch/ia64/kvm/asm-offsets.s
	$(call cmd,offsets)

FORCE : $(obj)/$(offsets-file)

#
# Makefile for Kernel-based Virtual Machine module
#

EXTRA_CFLAGS += -Ivirt/kvm -Iarch/ia64/kvm/
EXTRA_AFLAGS += -Ivirt/kvm -Iarch/ia64/kvm/

common-objs = $(addprefix ../../../virt/kvm/, kvm_main.o ioapic.o \
		coalesced_mmio.o irq_comm.o)

ifeq ($(CONFIG_IOMMU_API),y)
common-objs += $(addprefix ../../../virt/kvm/, iommu.o)
endif

kvm-objs := $(common-objs) kvm-ia64.o kvm_fw.o
obj-$(CONFIG_KVM) += kvm.o

CFLAGS_vcpu.o += -mfixed-range=f2-f5,f12-f127
kvm-intel-objs = vmm.o vmm_ivt.o trampoline.o vcpu.o optvfault.o mmio.o \
	vtlb.o process.o kvm_lib.o
#Add link memcpy and memset to avoid possible structure assignment error
kvm-intel-objs += memcpy.o memset.o
obj-$(CONFIG_KVM_INTEL) += kvm-intel.o
