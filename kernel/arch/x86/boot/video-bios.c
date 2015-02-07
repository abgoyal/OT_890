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

/* -*- linux-c -*- ------------------------------------------------------- *
 *
 *   Copyright (C) 1991, 1992 Linus Torvalds
 *   Copyright 2007 rPath, Inc. - All Rights Reserved
 *
 *   This file is part of the Linux kernel, and is made available under
 *   the terms of the GNU General Public License version 2.
 *
 * ----------------------------------------------------------------------- */

/*
 * Standard video BIOS modes
 *
 * We have two options for this; silent and scanned.
 */

#include "boot.h"
#include "video.h"

static __videocard video_bios;

/* Set a conventional BIOS mode */
static int set_bios_mode(u8 mode);

static int bios_set_mode(struct mode_info *mi)
{
	return set_bios_mode(mi->mode - VIDEO_FIRST_BIOS);
}

static int set_bios_mode(u8 mode)
{
	u16 ax;
	u8 new_mode;

	ax = mode;		/* AH=0x00 Set Video Mode */
	asm volatile(INT10
		     : "+a" (ax)
		     : : "ebx", "ecx", "edx", "esi", "edi");

	ax = 0x0f00;		/* Get Current Video Mode */
	asm volatile(INT10
		     : "+a" (ax)
		     : : "ebx", "ecx", "edx", "esi", "edi");

	do_restore = 1;		/* Assume video contents were lost */
	new_mode = ax & 0x7f;	/* Not all BIOSes are clean with the top bit */

	if (new_mode == mode)
		return 0;	/* Mode change OK */

#ifndef _WAKEUP
	if (new_mode != boot_params.screen_info.orig_video_mode) {
		/* Mode setting failed, but we didn't end up where we
		   started.  That's bad.  Try to revert to the original
		   video mode. */
		ax = boot_params.screen_info.orig_video_mode;
		asm volatile(INT10
			     : "+a" (ax)
			     : : "ebx", "ecx", "edx", "esi", "edi");
	}
#endif
	return -1;
}

static int bios_probe(void)
{
	u8 mode;
#ifdef _WAKEUP
	u8 saved_mode = 0x03;
#else
	u8 saved_mode = boot_params.screen_info.orig_video_mode;
#endif
	u16 crtc;
	struct mode_info *mi;
	int nmodes = 0;

	if (adapter != ADAPTER_EGA && adapter != ADAPTER_VGA)
		return 0;

	set_fs(0);
	crtc = vga_crtc();

	video_bios.modes = GET_HEAP(struct mode_info, 0);

	for (mode = 0x14; mode <= 0x7f; mode++) {
		if (!heap_free(sizeof(struct mode_info)))
			break;

		if (mode_defined(VIDEO_FIRST_BIOS+mode))
			continue;

		if (set_bios_mode(mode))
			continue;

		/* Try to verify that it's a text mode. */

		/* Attribute Controller: make graphics controller disabled */
		if (in_idx(0x3c0, 0x10) & 0x01)
			continue;

		/* Graphics Controller: verify Alpha addressing enabled */
		if (in_idx(0x3ce, 0x06) & 0x01)
			continue;

		/* CRTC cursor location low should be zero(?) */
		if (in_idx(crtc, 0x0f))
			continue;

		mi = GET_HEAP(struct mode_info, 1);
		mi->mode = VIDEO_FIRST_BIOS+mode;
		mi->depth = 0;	/* text */
		mi->x = rdfs16(0x44a);
		mi->y = rdfs8(0x484)+1;
		nmodes++;
	}

	set_bios_mode(saved_mode);

	return nmodes;
}

static __videocard video_bios =
{
	.card_name	= "BIOS",
	.probe		= bios_probe,
	.set_mode	= bios_set_mode,
	.unsafe		= 1,
	.xmode_first	= VIDEO_FIRST_BIOS,
	.xmode_n	= 0x80,
};
