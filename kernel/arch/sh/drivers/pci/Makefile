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
# Makefile for the PCI specific kernel interface routines under Linux.
#

obj-y					+= pci.o
obj-$(CONFIG_PCI_AUTO)			+= pci-auto.o

obj-$(CONFIG_CPU_SUBTYPE_SH7751)	+= pci-sh7751.o ops-sh4.o
obj-$(CONFIG_CPU_SUBTYPE_SH7751R)	+= pci-sh7751.o ops-sh4.o
obj-$(CONFIG_CPU_SUBTYPE_SH7763)	+= pci-sh7780.o ops-sh4.o
obj-$(CONFIG_CPU_SUBTYPE_SH7780)	+= pci-sh7780.o ops-sh4.o
obj-$(CONFIG_CPU_SUBTYPE_SH7785)	+= pci-sh7780.o ops-sh4.o
obj-$(CONFIG_CPU_SH5)			+= pci-sh5.o ops-sh5.o

obj-$(CONFIG_SH_DREAMCAST)		+= ops-dreamcast.o fixups-dreamcast.o
obj-$(CONFIG_SH_SECUREEDGE5410)		+= ops-snapgear.o
obj-$(CONFIG_SH_RTS7751R2D)		+= ops-rts7751r2d.o fixups-rts7751r2d.o
obj-$(CONFIG_SH_SH03)			+= ops-sh03.o fixups-sh03.o
obj-$(CONFIG_SH_HIGHLANDER)		+= ops-r7780rp.o fixups-r7780rp.o
obj-$(CONFIG_SH_SDK7780)		+= ops-sdk7780.o fixups-sdk7780.o
obj-$(CONFIG_SH_TITAN)			+= ops-titan.o
obj-$(CONFIG_SH_LANDISK)		+= ops-landisk.o
obj-$(CONFIG_SH_LBOX_RE2)		+= ops-lboxre2.o fixups-lboxre2.o
obj-$(CONFIG_SH_7780_SOLUTION_ENGINE)	+= ops-se7780.o fixups-se7780.o
obj-$(CONFIG_SH_CAYMAN)			+= ops-cayman.o
obj-$(CONFIG_SH_SH7785LCR)		+= ops-sh7785lcr.o fixups-sh7785lcr.o
