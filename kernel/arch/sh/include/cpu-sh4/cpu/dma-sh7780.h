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

#ifndef __ASM_SH_CPU_SH4_DMA_SH7780_H
#define __ASM_SH_CPU_SH4_DMA_SH7780_H

#define REQ_HE	0x000000C0
#define REQ_H	0x00000080
#define REQ_LE	0x00000040
#define TM_BURST 0x0000020
#define TS_8	0x00000000
#define TS_16	0x00000008
#define TS_32	0x00000010
#define TS_16BLK	0x00000018
#define TS_32BLK	0x00100000

/*
 * The SuperH DMAC supports a number of transmit sizes, we list them here,
 * with their respective values as they appear in the CHCR registers.
 *
 * Defaults to a 64-bit transfer size.
 */
enum {
	XMIT_SZ_8BIT,
	XMIT_SZ_16BIT,
	XMIT_SZ_32BIT,
	XMIT_SZ_128BIT,
	XMIT_SZ_256BIT,
};

/*
 * The DMA count is defined as the number of bytes to transfer.
 */
static unsigned int ts_shift[] __maybe_unused = {
	[XMIT_SZ_8BIT]		= 0,
	[XMIT_SZ_16BIT]		= 1,
	[XMIT_SZ_32BIT]		= 2,
	[XMIT_SZ_128BIT]	= 4,
	[XMIT_SZ_256BIT]	= 5,
};

#endif /* __ASM_SH_CPU_SH4_DMA_SH7780_H */
