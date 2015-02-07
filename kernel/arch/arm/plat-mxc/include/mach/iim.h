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
 * Copyright 2004-2007 Freescale Semiconductor, Inc. All Rights Reserved.
 * Copyright 2008 Juergen Beisert, kernel@pengutronix.de
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 */

#ifndef __ASM_ARCH_MXC_IIM_H__
#define __ASM_ARCH_MXC_IIM_H__

/* Register offsets */
#define MXC_IIMSTAT             0x0000
#define MXC_IIMSTATM            0x0004
#define MXC_IIMERR              0x0008
#define MXC_IIMEMASK            0x000C
#define MXC_IIMFCTL             0x0010
#define MXC_IIMUA               0x0014
#define MXC_IIMLA               0x0018
#define MXC_IIMSDAT             0x001C
#define MXC_IIMPREV             0x0020
#define MXC_IIMSREV             0x0024
#define MXC_IIMPRG_P            0x0028
#define MXC_IIMSCS0             0x002C
#define MXC_IIMSCS1             0x0030
#define MXC_IIMSCS2             0x0034
#define MXC_IIMSCS3             0x0038
#define MXC_IIMFBAC0            0x0800
#define MXC_IIMJAC              0x0804
#define MXC_IIMHWV1             0x0808
#define MXC_IIMHWV2             0x080C
#define MXC_IIMHAB0             0x0810
#define MXC_IIMHAB1             0x0814
/* Definitions for i.MX27 TO2 */
#define MXC_IIMMAC              0x0814
#define MXC_IIMPREV_FUSE        0x0818
#define MXC_IIMSREV_FUSE        0x081C
#define MXC_IIMSJC_CHALL_0      0x0820
#define MXC_IIMSJC_CHALL_7      0x083C
#define MXC_IIMFB0UC17          0x0840
#define MXC_IIMFB0UC255         0x0BFC
#define MXC_IIMFBAC1            0x0C00
/* Definitions for i.MX27 TO2 */
#define MXC_IIMSUID             0x0C04
#define MXC_IIMKEY0             0x0C04
#define MXC_IIMKEY20            0x0C54
#define MXC_IIMSJC_RESP_0       0x0C58
#define MXC_IIMSJC_RESP_7       0x0C74
#define MXC_IIMFB1UC30          0x0C78
#define MXC_IIMFB1UC255         0x0FFC

/* Bit definitions */

#define MXC_IIMHWV1_WLOCK               (0x1 << 7)
#define MXC_IIMHWV1_MCU_ENDIAN          (0x1 << 6)
#define MXC_IIMHWV1_DSP_ENDIAN          (0x1 << 5)
#define MXC_IIMHWV1_BOOT_INT            (0x1 << 4)
#define MXC_IIMHWV1_SCC_DISABLE         (0x1 << 3)
#define MXC_IIMHWV1_HANTRO_DISABLE      (0x1 << 2)
#define MXC_IIMHWV1_MEMSTICK_DIS        (0x1 << 1)

#define MXC_IIMHWV2_WLOCK               (0x1 << 7)
#define MXC_IIMHWV2_BP_SDMA             (0x1 << 6)
#define MXC_IIMHWV2_SCM_DCM             (0x1 << 5)

#endif /* __ASM_ARCH_MXC_IIM_H__ */
