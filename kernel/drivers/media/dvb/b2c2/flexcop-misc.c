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
 * This file is part of linux driver the digital TV devices equipped with B2C2 FlexcopII(b)/III
 *
 * flexcop-misc.c - miscellaneous functions.
 *
 * see flexcop.c for copyright information.
 */
#include "flexcop.h"

void flexcop_determine_revision(struct flexcop_device *fc)
{
	flexcop_ibi_value v = fc->read_ibi_reg(fc,misc_204);

	switch (v.misc_204.Rev_N_sig_revision_hi) {
		case 0x2:
			deb_info("found a FlexCopII.\n");
			fc->rev = FLEXCOP_II;
			break;
		case 0x3:
			deb_info("found a FlexCopIIb.\n");
			fc->rev = FLEXCOP_IIB;
			break;
		case 0x0:
			deb_info("found a FlexCopIII.\n");
			fc->rev = FLEXCOP_III;
			break;
		default:
			err("unkown FlexCop Revision: %x. Please report the linux-dvb@linuxtv.org.",v.misc_204.Rev_N_sig_revision_hi);
			break;
	}

	if ((fc->has_32_hw_pid_filter = v.misc_204.Rev_N_sig_caps))
		deb_info("this FlexCop has the additional 32 hardware pid filter.\n");
	else
		deb_info("this FlexCop has only the 6 basic main hardware pid filter.\n");
	/* bus parts have to decide if hw pid filtering is used or not. */
}

static const char *flexcop_revision_names[] = {
	"Unkown chip",
	"FlexCopII",
	"FlexCopIIb",
	"FlexCopIII",
};

static const char *flexcop_device_names[] = {
	"Unkown device",
	"Air2PC/AirStar 2 DVB-T",
	"Air2PC/AirStar 2 ATSC 1st generation",
	"Air2PC/AirStar 2 ATSC 2nd generation",
	"Sky2PC/SkyStar 2 DVB-S",
	"Sky2PC/SkyStar 2 DVB-S (old version)",
	"Cable2PC/CableStar 2 DVB-C",
	"Air2PC/AirStar 2 ATSC 3rd generation (HD5000)",
	"Sky2PC/SkyStar 2 DVB-S rev 2.7a/u",
	"Sky2PC/SkyStar 2 DVB-S rev 2.8",
};

static const char *flexcop_bus_names[] = {
	"USB",
	"PCI",
};

void flexcop_device_name(struct flexcop_device *fc,const char *prefix,const
		char *suffix)
{
	info("%s '%s' at the '%s' bus controlled by a '%s' %s",prefix,
			flexcop_device_names[fc->dev_type],flexcop_bus_names[fc->bus_type],
			flexcop_revision_names[fc->rev],suffix);
}

void flexcop_dump_reg(struct flexcop_device *fc, flexcop_ibi_register reg, int num)
{
	flexcop_ibi_value v;
	int i;
	for (i = 0; i < num; i++) {
		v = fc->read_ibi_reg(fc,reg+4*i);
		deb_rdump("0x%03x: %08x, ",reg+4*i, v.raw);
	}
	deb_rdump("\n");
}
EXPORT_SYMBOL(flexcop_dump_reg);
