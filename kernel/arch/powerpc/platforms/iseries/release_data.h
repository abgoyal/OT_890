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
 * Copyright (C) 2001  Mike Corrigan IBM Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */
#ifndef _ISERIES_RELEASE_DATA_H
#define _ISERIES_RELEASE_DATA_H

/*
 * This control block contains the critical information about the
 * release so that it can be changed in the future (ie, the virtual
 * address of the OS's NACA).
 */
#include <asm/types.h>
#include "naca.h"

/*
 * When we IPL a secondary partition, we will check if if the
 * secondary xMinPlicVrmIndex > the primary xVrmIndex.
 * If it is then this tells PLIC that this secondary is not
 * supported running on this "old" of a level of PLIC.
 *
 * Likewise, we will compare the primary xMinSlicVrmIndex to
 * the secondary xVrmIndex.
 * If the primary xMinSlicVrmDelta > secondary xVrmDelta then we
 * know that this PLIC does not support running an OS "that old".
 */

#define	HVREL_TAGSINACTIVE	0x8000
#define HVREL_32BIT		0x4000
#define HVREL_NOSHAREDPROCS	0x2000
#define HVREL_NOHMT		0x1000

struct HvReleaseData {
	u32	xDesc;		/* Descriptor "HvRD" ebcdic	x00-x03 */
	u16	xSize;		/* Size of this control block	x04-x05 */
	u16	xVpdAreasPtrOffset; /* Offset in NACA of ItVpdAreas x06-x07 */
	struct  naca_struct	*xSlicNacaAddr; /* Virt addr of SLIC NACA x08-x0F */
	u32	xMsNucDataOffset; /* Offset of Linux Mapping Data x10-x13 */
	u32	xRsvd1;		/* Reserved			x14-x17 */
	u16	xFlags;
	u16	xVrmIndex;	/* VRM Index of OS image	x1A-x1B */
	u16	xMinSupportedPlicVrmIndex; /* Min PLIC level  (soft) x1C-x1D */
	u16	xMinCompatablePlicVrmIndex; /* Min PLIC levelP (hard) x1E-x1F */
	char	xVrmName[12];	/* Displayable name		x20-x2B */
	char	xRsvd3[20];	/* Reserved			x2C-x3F */
};

extern const struct HvReleaseData	hvReleaseData;

#endif /* _ISERIES_RELEASE_DATA_H */
