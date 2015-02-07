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
 * arch/arm/mach-ixp4xx/include/mach/irqs.h 
 *
 * IRQ definitions for IXP4XX based systems
 *
 * Copyright (C) 2002 Intel Corporation.
 * Copyright (C) 2003 MontaVista Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef _ARCH_IXP4XX_IRQS_H_
#define _ARCH_IXP4XX_IRQS_H_


#define IRQ_IXP4XX_NPEA		0
#define IRQ_IXP4XX_NPEB		1
#define IRQ_IXP4XX_NPEC		2
#define IRQ_IXP4XX_QM1		3
#define IRQ_IXP4XX_QM2		4
#define IRQ_IXP4XX_TIMER1	5
#define IRQ_IXP4XX_GPIO0	6
#define IRQ_IXP4XX_GPIO1	7
#define IRQ_IXP4XX_PCI_INT	8
#define IRQ_IXP4XX_PCI_DMA1	9
#define IRQ_IXP4XX_PCI_DMA2	10
#define IRQ_IXP4XX_TIMER2	11
#define IRQ_IXP4XX_USB		12
#define IRQ_IXP4XX_UART2	13
#define IRQ_IXP4XX_TIMESTAMP	14
#define IRQ_IXP4XX_UART1	15
#define IRQ_IXP4XX_WDOG		16
#define IRQ_IXP4XX_AHB_PMU	17
#define IRQ_IXP4XX_XSCALE_PMU	18
#define IRQ_IXP4XX_GPIO2	19
#define IRQ_IXP4XX_GPIO3	20
#define IRQ_IXP4XX_GPIO4	21
#define IRQ_IXP4XX_GPIO5	22
#define IRQ_IXP4XX_GPIO6	23
#define IRQ_IXP4XX_GPIO7	24
#define IRQ_IXP4XX_GPIO8	25
#define IRQ_IXP4XX_GPIO9	26
#define IRQ_IXP4XX_GPIO10	27
#define IRQ_IXP4XX_GPIO11	28
#define IRQ_IXP4XX_GPIO12	29
#define IRQ_IXP4XX_SW_INT1	30
#define IRQ_IXP4XX_SW_INT2	31
#define IRQ_IXP4XX_USB_HOST	32
#define IRQ_IXP4XX_I2C		33
#define IRQ_IXP4XX_SSP		34
#define IRQ_IXP4XX_TSYNC	35
#define IRQ_IXP4XX_EAU_DONE	36
#define IRQ_IXP4XX_SHA_DONE	37
#define IRQ_IXP4XX_SWCP_PE	58
#define IRQ_IXP4XX_QM_PE	60
#define IRQ_IXP4XX_MCU_ECC	61
#define IRQ_IXP4XX_EXP_PE	62

/*
 * Only first 32 sources are valid if running on IXP42x systems
 */
#if defined(CONFIG_CPU_IXP46X) || defined(CONFIG_CPU_IXP43X)
#define NR_IRQS			64
#else
#define NR_IRQS			32
#endif

#define	XSCALE_PMU_IRQ		(IRQ_IXP4XX_XSCALE_PMU)

/*
 * IXDP425 board IRQs
 */
#define	IRQ_IXDP425_PCI_INTA	IRQ_IXP4XX_GPIO11
#define	IRQ_IXDP425_PCI_INTB	IRQ_IXP4XX_GPIO10
#define	IRQ_IXDP425_PCI_INTC	IRQ_IXP4XX_GPIO9
#define	IRQ_IXDP425_PCI_INTD	IRQ_IXP4XX_GPIO8

/*
 * Gateworks Avila board IRQs
 */
#define	IRQ_AVILA_PCI_INTA	IRQ_IXP4XX_GPIO11
#define	IRQ_AVILA_PCI_INTB	IRQ_IXP4XX_GPIO10
#define	IRQ_AVILA_PCI_INTC	IRQ_IXP4XX_GPIO9
#define	IRQ_AVILA_PCI_INTD	IRQ_IXP4XX_GPIO8


/*
 * PrPMC1100 Board IRQs
 */
#define	IRQ_PRPMC1100_PCI_INTA	IRQ_IXP4XX_GPIO11
#define	IRQ_PRPMC1100_PCI_INTB	IRQ_IXP4XX_GPIO10
#define	IRQ_PRPMC1100_PCI_INTC	IRQ_IXP4XX_GPIO9
#define	IRQ_PRPMC1100_PCI_INTD	IRQ_IXP4XX_GPIO8

/*
 * ADI Coyote Board IRQs
 */
#define	IRQ_COYOTE_PCI_SLOT0	IRQ_IXP4XX_GPIO6
#define	IRQ_COYOTE_PCI_SLOT1	IRQ_IXP4XX_GPIO11
#define	IRQ_COYOTE_IDE		IRQ_IXP4XX_GPIO5

/*
 * NSLU2 board IRQs
 */
#define        IRQ_NSLU2_PCI_INTA      IRQ_IXP4XX_GPIO11
#define        IRQ_NSLU2_PCI_INTB      IRQ_IXP4XX_GPIO10
#define        IRQ_NSLU2_PCI_INTC      IRQ_IXP4XX_GPIO9

/*
 * NAS100D board IRQs
 */
#define        IRQ_NAS100D_PCI_INTA    IRQ_IXP4XX_GPIO11
#define        IRQ_NAS100D_PCI_INTB    IRQ_IXP4XX_GPIO10
#define        IRQ_NAS100D_PCI_INTC    IRQ_IXP4XX_GPIO9
#define        IRQ_NAS100D_PCI_INTD    IRQ_IXP4XX_GPIO8
#define        IRQ_NAS100D_PCI_INTE    IRQ_IXP4XX_GPIO7

/*
 * D-Link DSM-G600 RevA board IRQs
 */
#define        IRQ_DSMG600_PCI_INTA    IRQ_IXP4XX_GPIO11
#define        IRQ_DSMG600_PCI_INTB    IRQ_IXP4XX_GPIO10
#define        IRQ_DSMG600_PCI_INTC    IRQ_IXP4XX_GPIO9
#define        IRQ_DSMG600_PCI_INTD    IRQ_IXP4XX_GPIO8
#define        IRQ_DSMG600_PCI_INTE    IRQ_IXP4XX_GPIO7
#define        IRQ_DSMG600_PCI_INTF    IRQ_IXP4XX_GPIO6

/*
 * Freecom FSG-3 Board IRQs
 */
#define        IRQ_FSG_PCI_INTA        IRQ_IXP4XX_GPIO6
#define        IRQ_FSG_PCI_INTB        IRQ_IXP4XX_GPIO7
#define        IRQ_FSG_PCI_INTC        IRQ_IXP4XX_GPIO5

#endif
