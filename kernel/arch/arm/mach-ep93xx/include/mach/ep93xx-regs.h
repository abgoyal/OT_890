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
 * arch/arm/mach-ep93xx/include/mach/ep93xx-regs.h
 */

#ifndef __ASM_ARCH_EP93XX_REGS_H
#define __ASM_ARCH_EP93XX_REGS_H

/*
 * EP93xx Physical Memory Map:
 *
 * The ASDO pin is sampled at system reset to select a synchronous or
 * asynchronous boot configuration.  When ASDO is "1" (i.e. pulled-up)
 * the synchronous boot mode is selected.  When ASDO is "0" (i.e
 * pulled-down) the asynchronous boot mode is selected.
 *
 * In synchronous boot mode nSDCE3 is decoded starting at physical address
 * 0x00000000 and nCS0 is decoded starting at 0xf0000000.  For asynchronous
 * boot mode they are swapped with nCS0 decoded at 0x00000000 ann nSDCE3
 * decoded at 0xf0000000.
 *
 * There is known errata for the EP93xx dealing with External Memory
 * Configurations.  Please refer to "AN273: EP93xx Silicon Rev E Design
 * Guidelines" for more information.  This document can be found at:
 *
 *	http://www.cirrus.com/en/pubs/appNote/AN273REV4.pdf
 */

#define EP93XX_CS0_PHYS_BASE_ASYNC	0x00000000	/* ASDO Pin = 0 */
#define EP93XX_SDCE3_PHYS_BASE_SYNC	0x00000000	/* ASDO Pin = 1 */
#define EP93XX_CS1_PHYS_BASE		0x10000000
#define EP93XX_CS2_PHYS_BASE		0x20000000
#define EP93XX_CS3_PHYS_BASE		0x30000000
#define EP93XX_PCMCIA_PHYS_BASE		0x40000000
#define EP93XX_CS6_PHYS_BASE		0x60000000
#define EP93XX_CS7_PHYS_BASE		0x70000000
#define EP93XX_SDCE0_PHYS_BASE		0xc0000000
#define EP93XX_SDCE1_PHYS_BASE		0xd0000000
#define EP93XX_SDCE2_PHYS_BASE		0xe0000000
#define EP93XX_SDCE3_PHYS_BASE_ASYNC	0xf0000000	/* ASDO Pin = 0 */
#define EP93XX_CS0_PHYS_BASE_SYNC	0xf0000000	/* ASDO Pin = 1 */

/*
 * EP93xx linux memory map:
 *
 * virt		phys		size
 * fe800000			5M		per-platform mappings
 * fed00000	80800000	2M		APB
 * fef00000	80000000	1M		AHB
 */

#define EP93XX_AHB_PHYS_BASE		0x80000000
#define EP93XX_AHB_VIRT_BASE		0xfef00000
#define EP93XX_AHB_SIZE			0x00100000

#define EP93XX_APB_PHYS_BASE		0x80800000
#define EP93XX_APB_VIRT_BASE		0xfed00000
#define EP93XX_APB_SIZE			0x00200000


/* AHB peripherals */
#define EP93XX_DMA_BASE			(EP93XX_AHB_VIRT_BASE + 0x00000000)

#define EP93XX_ETHERNET_BASE		(EP93XX_AHB_VIRT_BASE + 0x00010000)
#define EP93XX_ETHERNET_PHYS_BASE	(EP93XX_AHB_PHYS_BASE + 0x00010000)

#define EP93XX_USB_BASE			(EP93XX_AHB_VIRT_BASE + 0x00020000)
#define EP93XX_USB_PHYS_BASE		(EP93XX_AHB_PHYS_BASE + 0x00020000)

#define EP93XX_RASTER_BASE		(EP93XX_AHB_VIRT_BASE + 0x00030000)

#define EP93XX_GRAPHICS_ACCEL_BASE	(EP93XX_AHB_VIRT_BASE + 0x00040000)

#define EP93XX_SDRAM_CONTROLLER_BASE	(EP93XX_AHB_VIRT_BASE + 0x00060000)

#define EP93XX_PCMCIA_CONTROLLER_BASE	(EP93XX_AHB_VIRT_BASE + 0x00080000)

#define EP93XX_BOOT_ROM_BASE		(EP93XX_AHB_VIRT_BASE + 0x00090000)

#define EP93XX_IDE_BASE			(EP93XX_AHB_VIRT_BASE + 0x000a0000)

#define EP93XX_VIC1_BASE		(EP93XX_AHB_VIRT_BASE + 0x000b0000)

#define EP93XX_VIC2_BASE		(EP93XX_AHB_VIRT_BASE + 0x000c0000)


/* APB peripherals */
#define EP93XX_TIMER_BASE		(EP93XX_APB_VIRT_BASE + 0x00010000)
#define EP93XX_TIMER_REG(x)		(EP93XX_TIMER_BASE + (x))
#define EP93XX_TIMER1_LOAD		EP93XX_TIMER_REG(0x00)
#define EP93XX_TIMER1_VALUE		EP93XX_TIMER_REG(0x04)
#define EP93XX_TIMER1_CONTROL		EP93XX_TIMER_REG(0x08)
#define EP93XX_TIMER1_CLEAR		EP93XX_TIMER_REG(0x0c)
#define EP93XX_TIMER2_LOAD		EP93XX_TIMER_REG(0x20)
#define EP93XX_TIMER2_VALUE		EP93XX_TIMER_REG(0x24)
#define EP93XX_TIMER2_CONTROL		EP93XX_TIMER_REG(0x28)
#define EP93XX_TIMER2_CLEAR		EP93XX_TIMER_REG(0x2c)
#define EP93XX_TIMER4_VALUE_LOW		EP93XX_TIMER_REG(0x60)
#define EP93XX_TIMER4_VALUE_HIGH	EP93XX_TIMER_REG(0x64)
#define EP93XX_TIMER3_LOAD		EP93XX_TIMER_REG(0x80)
#define EP93XX_TIMER3_VALUE		EP93XX_TIMER_REG(0x84)
#define EP93XX_TIMER3_CONTROL		EP93XX_TIMER_REG(0x88)
#define EP93XX_TIMER3_CLEAR		EP93XX_TIMER_REG(0x8c)

#define EP93XX_I2S_BASE			(EP93XX_APB_VIRT_BASE + 0x00020000)

#define EP93XX_SECURITY_BASE		(EP93XX_APB_VIRT_BASE + 0x00030000)

#define EP93XX_GPIO_BASE		(EP93XX_APB_VIRT_BASE + 0x00040000)
#define EP93XX_GPIO_REG(x)		(EP93XX_GPIO_BASE + (x))
#define EP93XX_GPIO_F_INT_TYPE1		EP93XX_GPIO_REG(0x4c)
#define EP93XX_GPIO_F_INT_TYPE2		EP93XX_GPIO_REG(0x50)
#define EP93XX_GPIO_F_INT_ACK		EP93XX_GPIO_REG(0x54)
#define EP93XX_GPIO_F_INT_ENABLE	EP93XX_GPIO_REG(0x58)
#define EP93XX_GPIO_F_INT_STATUS	EP93XX_GPIO_REG(0x5c)
#define EP93XX_GPIO_A_INT_TYPE1		EP93XX_GPIO_REG(0x90)
#define EP93XX_GPIO_A_INT_TYPE2		EP93XX_GPIO_REG(0x94)
#define EP93XX_GPIO_A_INT_ACK		EP93XX_GPIO_REG(0x98)
#define EP93XX_GPIO_A_INT_ENABLE	EP93XX_GPIO_REG(0x9c)
#define EP93XX_GPIO_A_INT_STATUS	EP93XX_GPIO_REG(0xa0)
#define EP93XX_GPIO_B_INT_TYPE1		EP93XX_GPIO_REG(0xac)
#define EP93XX_GPIO_B_INT_TYPE2		EP93XX_GPIO_REG(0xb0)
#define EP93XX_GPIO_B_INT_ACK		EP93XX_GPIO_REG(0xb4)
#define EP93XX_GPIO_B_INT_ENABLE	EP93XX_GPIO_REG(0xb8)
#define EP93XX_GPIO_B_INT_STATUS	EP93XX_GPIO_REG(0xbc)

#define EP93XX_AAC_BASE			(EP93XX_APB_VIRT_BASE + 0x00080000)

#define EP93XX_SPI_BASE			(EP93XX_APB_VIRT_BASE + 0x000a0000)

#define EP93XX_IRDA_BASE		(EP93XX_APB_VIRT_BASE + 0x000b0000)

#define EP93XX_UART1_BASE		(EP93XX_APB_VIRT_BASE + 0x000c0000)
#define EP93XX_UART1_PHYS_BASE		(EP93XX_APB_PHYS_BASE + 0x000c0000)

#define EP93XX_UART2_BASE		(EP93XX_APB_VIRT_BASE + 0x000d0000)
#define EP93XX_UART2_PHYS_BASE		(EP93XX_APB_PHYS_BASE + 0x000d0000)

#define EP93XX_UART3_BASE		(EP93XX_APB_VIRT_BASE + 0x000e0000)
#define EP93XX_UART3_PHYS_BASE		(EP93XX_APB_PHYS_BASE + 0x000e0000)

#define EP93XX_KEY_MATRIX_BASE		(EP93XX_APB_VIRT_BASE + 0x000f0000)

#define EP93XX_ADC_BASE			(EP93XX_APB_VIRT_BASE + 0x00100000)
#define EP93XX_TOUCHSCREEN_BASE		(EP93XX_APB_VIRT_BASE + 0x00100000)

#define EP93XX_PWM_BASE			(EP93XX_APB_VIRT_BASE + 0x00110000)

#define EP93XX_RTC_BASE			(EP93XX_APB_VIRT_BASE + 0x00120000)

#define EP93XX_SYSCON_BASE		(EP93XX_APB_VIRT_BASE + 0x00130000)
#define EP93XX_SYSCON_REG(x)		(EP93XX_SYSCON_BASE + (x))
#define EP93XX_SYSCON_POWER_STATE	EP93XX_SYSCON_REG(0x00)
#define EP93XX_SYSCON_CLOCK_CONTROL	EP93XX_SYSCON_REG(0x04)
#define EP93XX_SYSCON_CLOCK_UARTBAUD	0x20000000
#define EP93XX_SYSCON_CLOCK_USH_EN	0x10000000
#define EP93XX_SYSCON_HALT		EP93XX_SYSCON_REG(0x08)
#define EP93XX_SYSCON_STANDBY		EP93XX_SYSCON_REG(0x0c)
#define EP93XX_SYSCON_CLOCK_SET1	EP93XX_SYSCON_REG(0x20)
#define EP93XX_SYSCON_CLOCK_SET2	EP93XX_SYSCON_REG(0x24)
#define EP93XX_SYSCON_DEVICE_CONFIG	EP93XX_SYSCON_REG(0x80)
#define EP93XX_SYSCON_DEVICE_CONFIG_CRUNCH_ENABLE	0x00800000
#define EP93XX_SYSCON_SWLOCK		EP93XX_SYSCON_REG(0xc0)

#define EP93XX_WATCHDOG_BASE		(EP93XX_APB_VIRT_BASE + 0x00140000)


#endif