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

#ifndef WM8785_H_INCLUDED
#define WM8785_H_INCLUDED

#define WM8785_R0	0
#define WM8785_R1	1
#define WM8785_R2	2
#define WM8785_R7	7

/* R0 */
#define WM8785_MCR_MASK		0x007
#define WM8785_MCR_SLAVE	0x000
#define WM8785_MCR_MASTER_128	0x001
#define WM8785_MCR_MASTER_192	0x002
#define WM8785_MCR_MASTER_256	0x003
#define WM8785_MCR_MASTER_384	0x004
#define WM8785_MCR_MASTER_512	0x005
#define WM8785_MCR_MASTER_768	0x006
#define WM8785_OSR_MASK		0x018
#define WM8785_OSR_SINGLE	0x000
#define WM8785_OSR_DOUBLE	0x008
#define WM8785_OSR_QUAD		0x010
#define WM8785_FORMAT_MASK	0x060
#define WM8785_FORMAT_RJUST	0x000
#define WM8785_FORMAT_LJUST	0x020
#define WM8785_FORMAT_I2S	0x040
#define WM8785_FORMAT_DSP	0x060
/* R1 */
#define WM8785_WL_MASK		0x003
#define WM8785_WL_16		0x000
#define WM8785_WL_20		0x001
#define WM8785_WL_24		0x002
#define WM8785_WL_32		0x003
#define WM8785_LRP		0x004
#define WM8785_BCLKINV		0x008
#define WM8785_LRSWAP		0x010
#define WM8785_DEVNO_MASK	0x0e0
/* R2 */
#define WM8785_HPFR		0x001
#define WM8785_HPFL		0x002
#define WM8785_SDODIS		0x004
#define WM8785_PWRDNR		0x008
#define WM8785_PWRDNL		0x010
#define WM8785_TDM_MASK		0x1c0

#endif
