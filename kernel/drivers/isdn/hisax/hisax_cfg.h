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

/* $Id$
 * define of the basic HiSax configuration structures
 * and pcmcia interface
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 *
 */

#define ISDN_CTYPE_16_0			1
#define ISDN_CTYPE_8_0			2
#define ISDN_CTYPE_16_3			3
#define ISDN_CTYPE_PNP			4
#define ISDN_CTYPE_A1			5
#define ISDN_CTYPE_ELSA			6
#define ISDN_CTYPE_ELSA_PNP		7
#define ISDN_CTYPE_TELESPCMCIA		8
#define ISDN_CTYPE_IX1MICROR2		9
#define ISDN_CTYPE_ELSA_PCMCIA		10
#define ISDN_CTYPE_DIEHLDIVA		11
#define ISDN_CTYPE_ASUSCOM		12
#define ISDN_CTYPE_TELEINT		13
#define ISDN_CTYPE_TELES3C		14
#define ISDN_CTYPE_SEDLBAUER		15
#define ISDN_CTYPE_SPORTSTER		16
#define ISDN_CTYPE_MIC			17
#define ISDN_CTYPE_ELSA_PCI		18
#define ISDN_CTYPE_COMPAQ_ISA		19
#define ISDN_CTYPE_NETJET_S		20
#define ISDN_CTYPE_TELESPCI		21
#define ISDN_CTYPE_SEDLBAUER_PCMCIA	22
#define ISDN_CTYPE_AMD7930		23
#define ISDN_CTYPE_NICCY		24
#define ISDN_CTYPE_S0BOX		25
#define ISDN_CTYPE_A1_PCMCIA		26
#define ISDN_CTYPE_FRITZPCI		27
#define ISDN_CTYPE_SEDLBAUER_FAX	28
#define ISDN_CTYPE_ISURF		29
#define ISDN_CTYPE_ACERP10		30
#define ISDN_CTYPE_HSTSAPHIR		31
#define	ISDN_CTYPE_BKM_A4T		32
#define	ISDN_CTYPE_SCT_QUADRO		33
#define ISDN_CTYPE_GAZEL		34
#define ISDN_CTYPE_HFC_PCI		35
#define ISDN_CTYPE_W6692		36
#define ISDN_CTYPE_HFC_SX		37
#define ISDN_CTYPE_NETJET_U		38
#define ISDN_CTYPE_HFC_SP_PCMCIA	39
#define ISDN_CTYPE_DYNAMIC		40
#define ISDN_CTYPE_ENTERNOW		41
#define ISDN_CTYPE_COUNT		41

typedef struct IsdnCardState	IsdnCardState_t;
typedef struct IsdnCard		IsdnCard_t;

struct IsdnCard {
	int		typ;
	int 		protocol;	/* EDSS1, 1TR6 or NI1 */
	unsigned long	para[4];
	IsdnCardState_t	*cs;
};

typedef int (*hisax_setup_func_t)(struct IsdnCard *card);

extern void	HiSax_closecard(int);
extern int	hisax_init_pcmcia(void *, int *, IsdnCard_t *);
