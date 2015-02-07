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

/***********************license start***************
 * Author: Cavium Networks
 *
 * Contact: support@caviumnetworks.com
 * This file is part of the OCTEON SDK
 *
 * Copyright (c) 2003-2008 Cavium Networks
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, Version 2, as
 * published by the Free Software Foundation.
 *
 * This file is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * or visit http://www.gnu.org/licenses/.
 *
 * This file may also be available under a different license from Cavium.
 * Contact Cavium Networks for more information
 ***********************license end**************************************/

/*
 * File defining checks for different Octeon features.
 */

#ifndef __OCTEON_FEATURE_H__
#define __OCTEON_FEATURE_H__

enum octeon_feature {
	/*
	 * Octeon models in the CN5XXX family and higher support
	 * atomic add instructions to memory (saa/saad).
	 */
	OCTEON_FEATURE_SAAD,
	/* Does this Octeon support the ZIP offload engine? */
	OCTEON_FEATURE_ZIP,
	/* Does this Octeon support crypto acceleration using COP2? */
	OCTEON_FEATURE_CRYPTO,
	/* Does this Octeon support PCI express? */
	OCTEON_FEATURE_PCIE,
	/* Some Octeon models support internal memory for storing
	 * cryptographic keys */
	OCTEON_FEATURE_KEY_MEMORY,
	/* Octeon has a LED controller for banks of external LEDs */
	OCTEON_FEATURE_LED_CONTROLLER,
	/* Octeon has a trace buffer */
	OCTEON_FEATURE_TRA,
	/* Octeon has a management port */
	OCTEON_FEATURE_MGMT_PORT,
	/* Octeon has a raid unit */
	OCTEON_FEATURE_RAID,
	/* Octeon has a builtin USB */
	OCTEON_FEATURE_USB,
};

static inline int cvmx_fuse_read(int fuse);

/**
 * Determine if the current Octeon supports a specific feature. These
 * checks have been optimized to be fairly quick, but they should still
 * be kept out of fast path code.
 *
 * @feature: Feature to check for. This should always be a constant so the
 *                compiler can remove the switch statement through optimization.
 *
 * Returns Non zero if the feature exists. Zero if the feature does not
 *         exist.
 */
static inline int octeon_has_feature(enum octeon_feature feature)
{
	switch (feature) {
	case OCTEON_FEATURE_SAAD:
		return !OCTEON_IS_MODEL(OCTEON_CN3XXX);

	case OCTEON_FEATURE_ZIP:
		if (OCTEON_IS_MODEL(OCTEON_CN30XX)
		    || OCTEON_IS_MODEL(OCTEON_CN50XX)
		    || OCTEON_IS_MODEL(OCTEON_CN52XX))
			return 0;
		else if (OCTEON_IS_MODEL(OCTEON_CN38XX_PASS1))
			return 1;
		else
			return !cvmx_fuse_read(121);

	case OCTEON_FEATURE_CRYPTO:
		return !cvmx_fuse_read(90);

	case OCTEON_FEATURE_PCIE:
		return OCTEON_IS_MODEL(OCTEON_CN56XX)
			|| OCTEON_IS_MODEL(OCTEON_CN52XX);

	case OCTEON_FEATURE_KEY_MEMORY:
	case OCTEON_FEATURE_LED_CONTROLLER:
		return OCTEON_IS_MODEL(OCTEON_CN38XX)
			|| OCTEON_IS_MODEL(OCTEON_CN58XX)
			|| OCTEON_IS_MODEL(OCTEON_CN56XX);
	case OCTEON_FEATURE_TRA:
		return !(OCTEON_IS_MODEL(OCTEON_CN30XX)
			 || OCTEON_IS_MODEL(OCTEON_CN50XX));
	case OCTEON_FEATURE_MGMT_PORT:
		return OCTEON_IS_MODEL(OCTEON_CN56XX)
			|| OCTEON_IS_MODEL(OCTEON_CN52XX);
	case OCTEON_FEATURE_RAID:
		return OCTEON_IS_MODEL(OCTEON_CN56XX)
			|| OCTEON_IS_MODEL(OCTEON_CN52XX);
	case OCTEON_FEATURE_USB:
		return !(OCTEON_IS_MODEL(OCTEON_CN38XX)
			 || OCTEON_IS_MODEL(OCTEON_CN58XX));
	}
	return 0;
}

#endif /* __OCTEON_FEATURE_H__ */
