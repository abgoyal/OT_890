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

/****************************************************************************
 * Driver for Solarflare Solarstorm network controllers and boards
 * Copyright 2007-2008 Solarflare Communications Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, incorporated herein by reference.
 */

#ifndef EFX_ENUM_H
#define EFX_ENUM_H

/**
 * enum efx_loopback_mode - loopback modes
 * @LOOPBACK_NONE: no loopback
 * @LOOPBACK_GMAC: loopback within GMAC at unspecified level
 * @LOOPBACK_XGMII: loopback within XMAC at XGMII level
 * @LOOPBACK_XGXS: loopback within XMAC at XGXS level
 * @LOOPBACK_XAUI: loopback within XMAC at XAUI level
 * @LOOPBACK_GPHY: loopback within 1G PHY at unspecified level
 * @LOOPBACK_PHYXS: loopback within 10G PHY at PHYXS level
 * @LOOPBACK_PCS: loopback within 10G PHY at PCS level
 * @LOOPBACK_PMAPMD: loopback within 10G PHY at PMAPMD level
 * @LOOPBACK_NETWORK: reflecting loopback (even further than furthest!)
 */
/* Please keep in order and up-to-date w.r.t the following two #defines */
enum efx_loopback_mode {
	LOOPBACK_NONE = 0,
	LOOPBACK_GMAC = 1,
	LOOPBACK_XGMII = 2,
	LOOPBACK_XGXS = 3,
	LOOPBACK_XAUI = 4,
	LOOPBACK_GPHY = 5,
	LOOPBACK_PHYXS = 6,
	LOOPBACK_PCS = 7,
	LOOPBACK_PMAPMD = 8,
	LOOPBACK_NETWORK = 9,
	LOOPBACK_MAX
};

#define LOOPBACK_TEST_MAX LOOPBACK_PMAPMD

extern const char *efx_loopback_mode_names[];
#define LOOPBACK_MODE_NAME(mode)			\
	STRING_TABLE_LOOKUP(mode, efx_loopback_mode)
#define LOOPBACK_MODE(efx)				\
	LOOPBACK_MODE_NAME(efx->loopback_mode)

/* These loopbacks occur within the controller */
#define LOOPBACKS_INTERNAL ((1 << LOOPBACK_GMAC) |     \
			    (1 << LOOPBACK_XGMII)|     \
			    (1 << LOOPBACK_XGXS) |     \
			    (1 << LOOPBACK_XAUI))

#define LOOPBACK_MASK(_efx)			\
	(1 << (_efx)->loopback_mode)

#define LOOPBACK_INTERNAL(_efx)				\
	(!!(LOOPBACKS_INTERNAL & LOOPBACK_MASK(_efx)))

#define LOOPBACK_CHANGED(_from, _to, _mask)				\
	(!!((LOOPBACK_MASK(_from) ^ LOOPBACK_MASK(_to)) & (_mask)))

#define LOOPBACK_OUT_OF(_from, _to, _mask)				\
	((LOOPBACK_MASK(_from) & (_mask)) && !(LOOPBACK_MASK(_to) & (_mask)))

/*****************************************************************************/

/**
 * enum reset_type - reset types
 *
 * %RESET_TYPE_INVSIBLE, %RESET_TYPE_ALL, %RESET_TYPE_WORLD and
 * %RESET_TYPE_DISABLE specify the method/scope of the reset.  The
 * other valuesspecify reasons, which efx_schedule_reset() will choose
 * a method for.
 *
 * @RESET_TYPE_INVISIBLE: don't reset the PHYs or interrupts
 * @RESET_TYPE_ALL: reset everything but PCI core blocks
 * @RESET_TYPE_WORLD: reset everything, save & restore PCI config
 * @RESET_TYPE_DISABLE: disable NIC
 * @RESET_TYPE_TX_WATCHDOG: reset due to TX watchdog
 * @RESET_TYPE_INT_ERROR: reset due to internal error
 * @RESET_TYPE_RX_RECOVERY: reset to recover from RX datapath errors
 * @RESET_TYPE_RX_DESC_FETCH: pcie error during rx descriptor fetch
 * @RESET_TYPE_TX_DESC_FETCH: pcie error during tx descriptor fetch
 * @RESET_TYPE_TX_SKIP: hardware completed empty tx descriptors
 */
enum reset_type {
	RESET_TYPE_NONE = -1,
	RESET_TYPE_INVISIBLE = 0,
	RESET_TYPE_ALL = 1,
	RESET_TYPE_WORLD = 2,
	RESET_TYPE_DISABLE = 3,
	RESET_TYPE_MAX_METHOD,
	RESET_TYPE_TX_WATCHDOG,
	RESET_TYPE_INT_ERROR,
	RESET_TYPE_RX_RECOVERY,
	RESET_TYPE_RX_DESC_FETCH,
	RESET_TYPE_TX_DESC_FETCH,
	RESET_TYPE_TX_SKIP,
	RESET_TYPE_MAX,
};

#endif /* EFX_ENUM_H */
