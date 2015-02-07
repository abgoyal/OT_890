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
 * Derived from include/asm-x86/mach-summit/mach_mpparse.h
 *          and include/asm-x86/mach-default/bios_ebda.h
 *
 * Author: Laurent Vivier <Laurent.Vivier@bull.net>
 */

#ifndef _ASM_X86_RIO_H
#define _ASM_X86_RIO_H

#define RIO_TABLE_VERSION	3

struct rio_table_hdr {
	u8 version;		/* Version number of this data structure  */
	u8 num_scal_dev;	/* # of Scalability devices               */
	u8 num_rio_dev;		/* # of RIO I/O devices                   */
} __attribute__((packed));

struct scal_detail {
	u8 node_id;		/* Scalability Node ID                    */
	u32 CBAR;		/* Address of 1MB register space          */
	u8 port0node;		/* Node ID port connected to: 0xFF=None   */
	u8 port0port;		/* Port num port connected to: 0,1,2, or  */
				/* 0xFF=None                              */
	u8 port1node;		/* Node ID port connected to: 0xFF = None */
	u8 port1port;		/* Port num port connected to: 0,1,2, or  */
				/* 0xFF=None                              */
	u8 port2node;		/* Node ID port connected to: 0xFF = None */
	u8 port2port;		/* Port num port connected to: 0,1,2, or  */
				/* 0xFF=None                              */
	u8 chassis_num;		/* 1 based Chassis number (1 = boot node) */
} __attribute__((packed));

struct rio_detail {
	u8 node_id;		/* RIO Node ID                            */
	u32 BBAR;		/* Address of 1MB register space          */
	u8 type;		/* Type of device                         */
	u8 owner_id;		/* Node ID of Hurricane that owns this    */
				/* node                                   */
	u8 port0node;		/* Node ID port connected to: 0xFF=None   */
	u8 port0port;		/* Port num port connected to: 0,1,2, or  */
				/* 0xFF=None                              */
	u8 port1node;		/* Node ID port connected to: 0xFF=None   */
	u8 port1port;		/* Port num port connected to: 0,1,2, or  */
				/* 0xFF=None                              */
	u8 first_slot;		/* Lowest slot number below this Calgary  */
	u8 status;		/* Bit 0 = 1 : the XAPIC is used          */
				/*       = 0 : the XAPIC is not used, ie: */
				/*            ints fwded to another XAPIC */
				/*           Bits1:7 Reserved             */
	u8 WP_index;		/* instance index - lower ones have       */
				/*     lower slot numbers/PCI bus numbers */
	u8 chassis_num;		/* 1 based Chassis number                 */
} __attribute__((packed));

enum {
	HURR_SCALABILTY	= 0,	/* Hurricane Scalability info */
	HURR_RIOIB	= 2,	/* Hurricane RIOIB info       */
	COMPAT_CALGARY	= 4,	/* Compatibility Calgary      */
	ALT_CALGARY	= 5,	/* Second Planar Calgary      */
};

#endif /* _ASM_X86_RIO_H */