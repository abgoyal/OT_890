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
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1992 - 1997, 2000-2005 Silicon Graphics, Inc. All rights reserved.
 */
#ifndef _ASM_IA64_SN_XTALK_HUBDEV_H
#define _ASM_IA64_SN_XTALK_HUBDEV_H

#include "xtalk/xwidgetdev.h"

#define HUB_WIDGET_ID_MAX 0xf
#define DEV_PER_WIDGET (2*2*8)
#define IIO_ITTE_WIDGET_BITS    4       /* size of widget field */
#define IIO_ITTE_WIDGET_MASK    ((1<<IIO_ITTE_WIDGET_BITS)-1)
#define IIO_ITTE_WIDGET_SHIFT   8

#define IIO_ITTE_WIDGET(itte)	\
	(((itte) >> IIO_ITTE_WIDGET_SHIFT) & IIO_ITTE_WIDGET_MASK)

/*
 * Use the top big window as a surrogate for the first small window
 */
#define SWIN0_BIGWIN            HUB_NUM_BIG_WINDOW
#define IIO_NUM_ITTES   7
#define HUB_NUM_BIG_WINDOW      (IIO_NUM_ITTES - 1)

/* This struct is shared between the PROM and the kernel.
 * Changes to this struct will require corresponding changes to the kernel.
 */
struct sn_flush_device_common {
	int sfdl_bus;
	int sfdl_slot;
	int sfdl_pin;
	struct common_bar_list {
		unsigned long start;
		unsigned long end;
	} sfdl_bar_list[6];
	unsigned long sfdl_force_int_addr;
	unsigned long sfdl_flush_value;
	volatile unsigned long *sfdl_flush_addr;
	u32 sfdl_persistent_busnum;
	u32 sfdl_persistent_segment;
	struct pcibus_info *sfdl_pcibus_info;
};

/* This struct is kernel only and is not used by the PROM */
struct sn_flush_device_kernel {
	spinlock_t sfdl_flush_lock;
	struct sn_flush_device_common *common;
};

/* 01/16/06 This struct is the old PROM/kernel struct and needs to be included
 * for older official PROMs to function on the new kernel base.  This struct
 * will be removed when the next official PROM release occurs. */

struct sn_flush_device_war {
	struct sn_flush_device_common common;
	u32 filler; /* older PROMs expect the default size of a spinlock_t */
};

/*
 * **widget_p - Used as an array[wid_num][device] of sn_flush_device_kernel.
 */
struct sn_flush_nasid_entry  {
	struct sn_flush_device_kernel **widget_p; // Used as an array of wid_num
	u64 iio_itte[8];
};

struct hubdev_info {
	geoid_t				hdi_geoid;
	short				hdi_nasid;
	short				hdi_peer_nasid;   /* Dual Porting Peer */

	struct sn_flush_nasid_entry	hdi_flush_nasid_list;
	struct xwidget_info		hdi_xwidget_info[HUB_WIDGET_ID_MAX + 1];


	void				*hdi_nodepda;
	void				*hdi_node_vertex;
	u32				max_segment_number;
	u32				max_pcibus_number;
};

extern void hubdev_init_node(nodepda_t *, cnodeid_t);
extern void hub_error_init(struct hubdev_info *);
extern void ice_error_init(struct hubdev_info *);


#endif /* _ASM_IA64_SN_XTALK_HUBDEV_H */
