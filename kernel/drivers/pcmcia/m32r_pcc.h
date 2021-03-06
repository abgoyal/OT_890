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
 * Copyright (C) 2001 by Hiroyuki Kondo
 */

#define M32R_MAX_PCC	2

/*
 * M32R PC Card Controler
 */
#define M32R_PCC0_BASE        0x00ef7000
#define M32R_PCC1_BASE        0x00ef7020

/*
 * Register offsets
 */
#define PCCR            0x00
#define PCADR           0x04
#define PCMOD           0x08
#define PCIRC           0x0c
#define PCCSIGCR        0x10
#define PCATCR          0x14

/*
 * PCCR
 */
#define PCCR_PCEN       (1UL<<(31-31))

/*
 * PCIRC
 */
#define PCIRC_BWERR     (1UL<<(31-7))
#define PCIRC_CDIN1     (1UL<<(31-14))
#define PCIRC_CDIN2     (1UL<<(31-15))
#define PCIRC_BEIEN     (1UL<<(31-23))
#define PCIRC_CIIEN     (1UL<<(31-30))
#define PCIRC_COIEN     (1UL<<(31-31))

/*
 * PCCSIGCR
 */
#define PCCSIGCR_SEN    (1UL<<(31-3))
#define PCCSIGCR_VEN    (1UL<<(31-7))
#define PCCSIGCR_CRST   (1UL<<(31-15))
#define PCCSIGCR_COCR   (1UL<<(31-31))

/*
 *
 */
#define PCMOD_AS_ATTRIB	(1UL<<(31-19))
#define PCMOD_AS_IO	(1UL<<(31-18))

#define PCMOD_CBSZ	(1UL<<(31-23)) /* set for 8bit */

#define PCMOD_DBEX	(1UL<<(31-31)) /* set for excahnge */

/*
 * M32R PCC Map addr
 */
#define M32R_PCC0_MAPBASE        0x14000000
#define M32R_PCC1_MAPBASE        0x16000000

#define M32R_PCC_MAPMAX		 0x02000000

#define M32R_PCC_MAPSIZE	 0x00001000 /* XXX */
#define M32R_PCC_MAPMASK     	(~(M32R_PCC_MAPMAX-1))
