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

#ifndef __ASM_MIPS_PROM_H
#define __ASM_MIPS_PROM_H

#define PROM_RESET		0
#define PROM_EXEC		1
#define PROM_RESTART		2
#define PROM_REINIT		3
#define PROM_REBOOT		4
#define PROM_AUTOBOOT		5
#define PROM_OPEN		6
#define PROM_READ		7
#define PROM_WRITE		8
#define PROM_IOCTL		9
#define PROM_CLOSE		10
#define PROM_GETCHAR		11
#define PROM_PUTCHAR		12
#define PROM_SHOWCHAR		13	/* XXX */
#define PROM_GETS		14	/* XXX */
#define PROM_PUTS		15	/* XXX */
#define PROM_PRINTF		16	/* XXX */

/* What are these for? */
#define PROM_INITPROTO		17	/* XXX */
#define PROM_PROTOENABLE	18	/* XXX */
#define PROM_PROTODISABLE	19	/* XXX */
#define PROM_GETPKT		20	/* XXX */
#define PROM_PUTPKT		21	/* XXX */

/* More PROM shit.  Probably has to do with VME RMW cycles??? */
#define PROM_ORW_RMW		22	/* XXX */
#define PROM_ORH_RMW		23	/* XXX */
#define PROM_ORB_RMW		24	/* XXX */
#define PROM_ANDW_RMW		25	/* XXX */
#define PROM_ANDH_RMW		26	/* XXX */
#define PROM_ANDB_RMW		27	/* XXX */

/* Cache handling stuff */
#define PROM_FLUSHCACHE		28	/* XXX */
#define PROM_CLEARCACHE		29	/* XXX */

/* Libc alike stuff */
#define PROM_SETJMP		30	/* XXX */
#define PROM_LONGJMP		31	/* XXX */
#define PROM_BEVUTLB		32	/* XXX */
#define PROM_GETENV		33	/* XXX */
#define PROM_SETENV		34	/* XXX */
#define PROM_ATOB		35	/* XXX */
#define PROM_STRCMP		36	/* XXX */
#define PROM_STRLEN		37	/* XXX */
#define PROM_STRCPY		38	/* XXX */
#define PROM_STRCAT		39	/* XXX */

/* Misc stuff */
#define PROM_PARSER		40	/* XXX */
#define PROM_RANGE		41	/* XXX */
#define PROM_ARGVIZE		42	/* XXX */
#define PROM_HELP		43	/* XXX */

/* Entry points for some PROM commands */
#define PROM_DUMPCMD		44	/* XXX */
#define PROM_SETENVCMD		45	/* XXX */
#define PROM_UNSETENVCMD	46	/* XXX */
#define PROM_PRINTENVCMD	47	/* XXX */
#define PROM_BEVEXCEPT		48	/* XXX */
#define PROM_ENABLECMD		49	/* XXX */
#define PROM_DISABLECMD		50	/* XXX */

#define PROM_CLEARNOFAULT	51	/* XXX */
#define PROM_NOTIMPLEMENT	52	/* XXX */

#define PROM_NV_GET		53	/* XXX */
#define PROM_NV_SET		54	/* XXX */

extern char *prom_getenv(char *);

#endif /* __ASM_MIPS_PROM_H */
