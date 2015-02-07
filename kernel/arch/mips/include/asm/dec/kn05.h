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
 *	include/asm-mips/dec/kn05.h
 *
 *	DECstation/DECsystem 5000/260 (4max+ or KN05), 5000/150 (4min
 *	or KN04-BA), Personal DECstation/DECsystem 5000/50 (4maxine or
 *	KN04-CA) and DECsystem 5900/260 (KN05) R4k CPU card MB ASIC
 *	definitions.
 *
 *	Copyright (C) 2002, 2003, 2005, 2008  Maciej W. Rozycki
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 *
 *	WARNING!  All this information is pure guesswork based on the
 *	ROM.  It is provided here in hope it will give someone some
 *	food for thought.  No documentation for the KN05 nor the KN04
 *	module has been located so far.
 */
#ifndef __ASM_MIPS_DEC_KN05_H
#define __ASM_MIPS_DEC_KN05_H

#include <asm/dec/ioasic_addrs.h>

/*
 * The oncard MB (Memory Buffer) ASIC provides an additional address
 * decoder.  Certain address ranges within the "high" 16 slots are
 * passed to the I/O ASIC's decoder like with the KN03 or KN02-BA/CA.
 * Others are handled locally.  "Low" slots are always passed.
 */
#define KN4K_SLOT_BASE	0x1fc00000

#define KN4K_MB_ROM	(0*IOASIC_SLOT_SIZE)	/* KN05/KN04 card ROM */
#define KN4K_IOCTL	(1*IOASIC_SLOT_SIZE)	/* I/O ASIC */
#define KN4K_ESAR	(2*IOASIC_SLOT_SIZE)	/* LANCE MAC address chip */
#define KN4K_LANCE	(3*IOASIC_SLOT_SIZE)	/* LANCE Ethernet */
#define KN4K_MB_INT	(4*IOASIC_SLOT_SIZE)	/* MB interrupt register */
#define KN4K_MB_EA	(5*IOASIC_SLOT_SIZE)	/* MB error address? */
#define KN4K_MB_EC	(6*IOASIC_SLOT_SIZE)	/* MB error ??? */
#define KN4K_MB_CSR	(7*IOASIC_SLOT_SIZE)	/* MB control & status */
#define KN4K_RES_08	(8*IOASIC_SLOT_SIZE)	/* unused? */
#define KN4K_RES_09	(9*IOASIC_SLOT_SIZE)	/* unused? */
#define KN4K_RES_10	(10*IOASIC_SLOT_SIZE)	/* unused? */
#define KN4K_RES_11	(11*IOASIC_SLOT_SIZE)	/* unused? */
#define KN4K_SCSI	(12*IOASIC_SLOT_SIZE)	/* ASC SCSI */
#define KN4K_RES_13	(13*IOASIC_SLOT_SIZE)	/* unused? */
#define KN4K_RES_14	(14*IOASIC_SLOT_SIZE)	/* unused? */
#define KN4K_RES_15	(15*IOASIC_SLOT_SIZE)	/* unused? */

/*
 * Bits for the MB interrupt register.
 * The register appears read-only.
 */
#define KN4K_MB_INT_TC		(1<<0)		/* TURBOchannel? */
#define KN4K_MB_INT_RTC		(1<<1)		/* RTC? */
#define KN4K_MB_INT_MT		(1<<3)		/* I/O ASIC cascade */

/*
 * Bits for the MB control & status register.
 * Set to 0x00bf8001 for KN05 and to 0x003f8000 for KN04 by the firmware.
 */
#define KN4K_MB_CSR_PF		(1<<0)		/* PreFetching enable? */
#define KN4K_MB_CSR_F		(1<<1)		/* ??? */
#define KN4K_MB_CSR_ECC		(0xff<<2)	/* ??? */
#define KN4K_MB_CSR_OD		(1<<10)		/* ??? */
#define KN4K_MB_CSR_CP		(1<<11)		/* ??? */
#define KN4K_MB_CSR_UNC		(1<<12)		/* ??? */
#define KN4K_MB_CSR_IM		(1<<13)		/* ??? */
#define KN4K_MB_CSR_NC		(1<<14)		/* ??? */
#define KN4K_MB_CSR_EE		(1<<15)		/* (bus) Exception Enable? */
#define KN4K_MB_CSR_MSK		(0x1f<<16)	/* CPU Int[4:0] mask */
#define KN4K_MB_CSR_FW		(1<<21)		/* ??? */
#define KN4K_MB_CSR_W		(1<<31)		/* ??? */

#endif /* __ASM_MIPS_DEC_KN05_H */
