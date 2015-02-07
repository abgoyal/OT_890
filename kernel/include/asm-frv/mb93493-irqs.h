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

/* mb93493-irqs.h: MB93493 companion chip IRQs
 *
 * Copyright (C) 2004 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _ASM_MB93493_IRQS_H
#define _ASM_MB93493_IRQS_H

#include <asm/irq.h>

#ifndef __ASSEMBLY__

/* IRQ IDs presented to drivers */
enum {
	IRQ_MB93493_VDC			= IRQ_BASE_MB93493 + 0,
	IRQ_MB93493_VCC			= IRQ_BASE_MB93493 + 1,
	IRQ_MB93493_AUDIO_OUT		= IRQ_BASE_MB93493 + 2,
	IRQ_MB93493_I2C_0		= IRQ_BASE_MB93493 + 3,
	IRQ_MB93493_I2C_1		= IRQ_BASE_MB93493 + 4,
	IRQ_MB93493_USB			= IRQ_BASE_MB93493 + 5,
	IRQ_MB93493_LOCAL_BUS		= IRQ_BASE_MB93493 + 7,
	IRQ_MB93493_PCMCIA		= IRQ_BASE_MB93493 + 8,
	IRQ_MB93493_GPIO		= IRQ_BASE_MB93493 + 9,
	IRQ_MB93493_AUDIO_IN		= IRQ_BASE_MB93493 + 10,
};

/* IRQ multiplexor mappings */
#define ROUTE_VIA_IRQ0	0	/* route IRQ by way of CPU external IRQ 0 */
#define ROUTE_VIA_IRQ1	1	/* route IRQ by way of CPU external IRQ 1 */

#define IRQ_MB93493_VDC_ROUTE		ROUTE_VIA_IRQ0
#define IRQ_MB93493_VCC_ROUTE		ROUTE_VIA_IRQ1
#define IRQ_MB93493_AUDIO_OUT_ROUTE	ROUTE_VIA_IRQ1
#define IRQ_MB93493_I2C_0_ROUTE		ROUTE_VIA_IRQ1
#define IRQ_MB93493_I2C_1_ROUTE		ROUTE_VIA_IRQ1
#define IRQ_MB93493_USB_ROUTE		ROUTE_VIA_IRQ1
#define IRQ_MB93493_LOCAL_BUS_ROUTE	ROUTE_VIA_IRQ1
#define IRQ_MB93493_PCMCIA_ROUTE	ROUTE_VIA_IRQ1
#define IRQ_MB93493_GPIO_ROUTE		ROUTE_VIA_IRQ1
#define IRQ_MB93493_AUDIO_IN_ROUTE	ROUTE_VIA_IRQ1

#endif /* !__ASSEMBLY__ */

#endif /* _ASM_MB93493_IRQS_H */
