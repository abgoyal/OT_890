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
# Makefile for the Linux/SuperH SH-3 backends.
#

obj-y	:= ex.o probe.o entry.o setup-sh3.o

# CPU subtype setup
obj-$(CONFIG_CPU_SUBTYPE_SH7705)	+= setup-sh7705.o
obj-$(CONFIG_CPU_SUBTYPE_SH7706)	+= setup-sh770x.o
obj-$(CONFIG_CPU_SUBTYPE_SH7707)	+= setup-sh770x.o
obj-$(CONFIG_CPU_SUBTYPE_SH7708)	+= setup-sh770x.o
obj-$(CONFIG_CPU_SUBTYPE_SH7709)	+= setup-sh770x.o
obj-$(CONFIG_CPU_SUBTYPE_SH7710)	+= setup-sh7710.o
obj-$(CONFIG_CPU_SUBTYPE_SH7712)	+= setup-sh7710.o
obj-$(CONFIG_CPU_SUBTYPE_SH7720)	+= setup-sh7720.o
obj-$(CONFIG_CPU_SUBTYPE_SH7721)	+= setup-sh7720.o

# Primary on-chip clocks (common)
clock-$(CONFIG_CPU_SH3)			:= clock-sh3.o
clock-$(CONFIG_CPU_SUBTYPE_SH7705)	:= clock-sh7705.o
clock-$(CONFIG_CPU_SUBTYPE_SH7706)	:= clock-sh7706.o
clock-$(CONFIG_CPU_SUBTYPE_SH7709)	:= clock-sh7709.o
clock-$(CONFIG_CPU_SUBTYPE_SH7710)	:= clock-sh7710.o
clock-$(CONFIG_CPU_SUBTYPE_SH7720)	:= clock-sh7710.o
clock-$(CONFIG_CPU_SUBTYPE_SH7712)	:= clock-sh7712.o

# Pinmux setup
pinmux-$(CONFIG_CPU_SUBTYPE_SH7720)	:= pinmux-sh7720.o

obj-y	+= $(clock-y)
obj-$(CONFIG_GENERIC_GPIO)	+= $(pinmux-y)
