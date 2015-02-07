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

#ifndef __ASM_SH_CPU_SH5_IRQ_H
#define __ASM_SH_CPU_SH5_IRQ_H

/*
 * include/asm-sh/cpu-sh5/irq.h
 *
 * Copyright (C) 2000, 2001  Paolo Alberelli
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */


/*
 * Encoded IRQs are not considered worth to be supported.
 * Main reason is that there's no per-encoded-interrupt
 * enable/disable mechanism (as there was in SH3/4).
 * An all enabled/all disabled is worth only if there's
 * a cascaded IC to disable/enable/ack on. Until such
 * IC is available there's no such support.
 *
 * Presumably Encoded IRQs may use extra IRQs beyond 64,
 * below. Some logic must be added to cope with IRQ_IRL?
 * in an exclusive way.
 *
 * Priorities are set at Platform level, when IRQ_IRL0-3
 * are set to 0 Encoding is allowed. Otherwise it's not
 * allowed.
 */

/* Independent IRQs */
#define IRQ_IRL0	0
#define IRQ_IRL1	1
#define IRQ_IRL2	2
#define IRQ_IRL3	3

#define IRQ_INTA	4
#define IRQ_INTB	5
#define IRQ_INTC	6
#define IRQ_INTD	7

#define IRQ_SERR	12
#define IRQ_ERR		13
#define IRQ_PWR3	14
#define IRQ_PWR2	15
#define IRQ_PWR1	16
#define IRQ_PWR0	17

#define IRQ_DMTE0	18
#define IRQ_DMTE1	19
#define IRQ_DMTE2	20
#define IRQ_DMTE3	21
#define IRQ_DAERR	22

#define IRQ_TUNI0	32
#define IRQ_TUNI1	33
#define IRQ_TUNI2	34
#define IRQ_TICPI2	35

#define IRQ_ATI		36
#define IRQ_PRI		37
#define IRQ_CUI		38

#define IRQ_ERI		39
#define IRQ_RXI		40
#define IRQ_BRI		41
#define IRQ_TXI		42

#define IRQ_ITI		63

#define NR_INTC_IRQS	64

#ifdef CONFIG_SH_CAYMAN
#define NR_EXT_IRQS     32
#define START_EXT_IRQS  64

/* PCI bus 2 uses encoded external interrupts on the Cayman board */
#define IRQ_P2INTA      (START_EXT_IRQS + (3*8) + 0)
#define IRQ_P2INTB      (START_EXT_IRQS + (3*8) + 1)
#define IRQ_P2INTC      (START_EXT_IRQS + (3*8) + 2)
#define IRQ_P2INTD      (START_EXT_IRQS + (3*8) + 3)

#define I8042_KBD_IRQ	(START_EXT_IRQS + 2)
#define I8042_AUX_IRQ	(START_EXT_IRQS + 6)

#define IRQ_CFCARD	(START_EXT_IRQS + 7)
#define IRQ_PCMCIA	(0)

#else
#define NR_EXT_IRQS	0
#endif

/* Default IRQs, fixed */
#define TIMER_IRQ	IRQ_TUNI0
#define RTC_IRQ		IRQ_CUI

/* Default Priorities, Platform may choose differently */
#define	NO_PRIORITY	0	/* Disabled */
#define TIMER_PRIORITY	2
#define RTC_PRIORITY	TIMER_PRIORITY
#define SCIF_PRIORITY	3
#define INTD_PRIORITY	3
#define	IRL3_PRIORITY	4
#define INTC_PRIORITY	6
#define	IRL2_PRIORITY	7
#define INTB_PRIORITY	9
#define	IRL1_PRIORITY	10
#define INTA_PRIORITY	12
#define	IRL0_PRIORITY	13
#define TOP_PRIORITY	15

extern int intc_evt_to_irq[(0xE20/0x20)+1];
int intc_irq_describe(char* p, int irq);
extern int platform_int_priority[NR_INTC_IRQS];

#endif /* __ASM_SH_CPU_SH5_IRQ_H */
