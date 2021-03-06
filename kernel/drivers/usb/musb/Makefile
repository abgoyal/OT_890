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
# for USB OTG silicon based on Mentor Graphics INVENTRA designs
#

musb_hdrc-objs := musb_core.o

obj-$(CONFIG_USB_MUSB_HDRC)	+= musb_hdrc.o

ifeq ($(CONFIG_ARCH_DAVINCI),y)
	musb_hdrc-objs	+= davinci.o
endif

ifeq ($(CONFIG_USB_TUSB6010),y)
	musb_hdrc-objs	+= tusb6010.o
endif

ifeq ($(CONFIG_ARCH_OMAP2430),y)
	musb_hdrc-objs	+= omap2430.o
endif

ifeq ($(CONFIG_ARCH_OMAP3430),y)
	musb_hdrc-objs	+= omap2430.o
endif

ifeq ($(CONFIG_BF54x),y)
	musb_hdrc-objs	+= blackfin.o
endif

ifeq ($(CONFIG_BF52x),y)
	musb_hdrc-objs	+= blackfin.o
endif

ifeq ($(CONFIG_USB_GADGET_MUSB_HDRC),y)
	musb_hdrc-objs		+= musb_gadget_ep0.o musb_gadget.o
endif

ifeq ($(CONFIG_USB_MUSB_HDRC_HCD),y)
	musb_hdrc-objs		+= musb_virthub.o musb_host.o
endif

# the kconfig must guarantee that only one of the
# possible I/O schemes will be enabled at a time ...
# PIO only, or DMA (several potential schemes).
# though PIO is always there to back up DMA, and for ep0

ifneq ($(CONFIG_MUSB_PIO_ONLY),y)

  ifeq ($(CONFIG_USB_INVENTRA_DMA),y)
    musb_hdrc-objs		+= musbhsdma.o

  else
    ifeq ($(CONFIG_USB_TI_CPPI_DMA),y)
      musb_hdrc-objs		+= cppi_dma.o

    else
      ifeq ($(CONFIG_USB_TUSB_OMAP_DMA),y)
        musb_hdrc-objs		+= tusb6010_omap.o

      endif
    endif
  endif
endif


################################################################################

# FIXME remove all these extra "-DMUSB_* things, stick to CONFIG_*

ifeq ($(CONFIG_USB_INVENTRA_MUSB_HAS_AHB_ID),y)
	EXTRA_CFLAGS += -DMUSB_AHB_ID
endif

# Debugging

ifeq ($(CONFIG_USB_MUSB_DEBUG),y)
	EXTRA_CFLAGS += -DDEBUG
endif
