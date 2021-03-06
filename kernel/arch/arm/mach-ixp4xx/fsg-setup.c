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
 * arch/arm/mach-ixp4xx/fsg-setup.c
 *
 * FSG board-setup
 *
 * Copyright (C) 2008 Rod Whitby <rod@whitby.id.au>
 *
 * based on ixdp425-setup.c:
 *	Copyright (C) 2003-2004 MontaVista Software, Inc.
 * based on nslu2-power.c
 *	Copyright (C) 2005 Tower Technologies
 *
 * Author: Rod Whitby <rod@whitby.id.au>
 * Maintainers: http://www.nslu2-linux.org/
 *
 */

#include <linux/if_ether.h>
#include <linux/irq.h>
#include <linux/serial.h>
#include <linux/serial_8250.h>
#include <linux/leds.h>
#include <linux/reboot.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/io.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/flash.h>
#include <asm/gpio.h>

static struct flash_platform_data fsg_flash_data = {
	.map_name		= "cfi_probe",
	.width			= 2,
};

static struct resource fsg_flash_resource = {
	.flags			= IORESOURCE_MEM,
};

static struct platform_device fsg_flash = {
	.name			= "IXP4XX-Flash",
	.id			= 0,
	.dev = {
		.platform_data	= &fsg_flash_data,
	},
	.num_resources		= 1,
	.resource		= &fsg_flash_resource,
};

static struct i2c_gpio_platform_data fsg_i2c_gpio_data = {
	.sda_pin		= FSG_SDA_PIN,
	.scl_pin		= FSG_SCL_PIN,
};

static struct platform_device fsg_i2c_gpio = {
	.name			= "i2c-gpio",
	.id			= 0,
	.dev = {
		.platform_data	= &fsg_i2c_gpio_data,
	},
};

static struct i2c_board_info __initdata fsg_i2c_board_info [] = {
	{
		I2C_BOARD_INFO("isl1208", 0x6f),
	},
};

static struct resource fsg_uart_resources[] = {
	{
		.start		= IXP4XX_UART1_BASE_PHYS,
		.end		= IXP4XX_UART1_BASE_PHYS + 0x0fff,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= IXP4XX_UART2_BASE_PHYS,
		.end		= IXP4XX_UART2_BASE_PHYS + 0x0fff,
		.flags		= IORESOURCE_MEM,
	}
};

static struct plat_serial8250_port fsg_uart_data[] = {
	{
		.mapbase	= IXP4XX_UART1_BASE_PHYS,
		.membase	= (char *)IXP4XX_UART1_BASE_VIRT + REG_OFFSET,
		.irq		= IRQ_IXP4XX_UART1,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= IXP4XX_UART_XTAL,
	},
	{
		.mapbase	= IXP4XX_UART2_BASE_PHYS,
		.membase	= (char *)IXP4XX_UART2_BASE_VIRT + REG_OFFSET,
		.irq		= IRQ_IXP4XX_UART2,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST,
		.iotype		= UPIO_MEM,
		.regshift	= 2,
		.uartclk	= IXP4XX_UART_XTAL,
	},
	{ }
};

static struct platform_device fsg_uart = {
	.name			= "serial8250",
	.id			= PLAT8250_DEV_PLATFORM,
	.dev = {
		.platform_data	= fsg_uart_data,
	},
	.num_resources		= ARRAY_SIZE(fsg_uart_resources),
	.resource		= fsg_uart_resources,
};

static struct platform_device fsg_leds = {
	.name		= "fsg-led",
	.id		= -1,
};

/* Built-in 10/100 Ethernet MAC interfaces */
static struct eth_plat_info fsg_plat_eth[] = {
	{
		.phy		= 5,
		.rxq		= 3,
		.txreadyq	= 20,
	}, {
		.phy		= 4,
		.rxq		= 4,
		.txreadyq	= 21,
	}
};

static struct platform_device fsg_eth[] = {
	{
		.name			= "ixp4xx_eth",
		.id			= IXP4XX_ETH_NPEB,
		.dev = {
			.platform_data	= fsg_plat_eth,
		},
	}, {
		.name			= "ixp4xx_eth",
		.id			= IXP4XX_ETH_NPEC,
		.dev = {
			.platform_data	= fsg_plat_eth + 1,
		},
	}
};

static struct platform_device *fsg_devices[] __initdata = {
	&fsg_i2c_gpio,
	&fsg_flash,
	&fsg_leds,
	&fsg_eth[0],
	&fsg_eth[1],
};

static irqreturn_t fsg_power_handler(int irq, void *dev_id)
{
	/* Signal init to do the ctrlaltdel action, this will bypass init if
	 * it hasn't started and do a kernel_restart.
	 */
	ctrl_alt_del();

	return IRQ_HANDLED;
}

static irqreturn_t fsg_reset_handler(int irq, void *dev_id)
{
	/* This is the paper-clip reset which does an emergency reboot. */
	printk(KERN_INFO "Restarting system.\n");
	machine_restart(NULL);

	/* This should never be reached. */
	return IRQ_HANDLED;
}

static void __init fsg_init(void)
{
	uint8_t __iomem *f;

	ixp4xx_sys_init();

	fsg_flash_resource.start = IXP4XX_EXP_BUS_BASE(0);
	fsg_flash_resource.end =
		IXP4XX_EXP_BUS_BASE(0) + ixp4xx_exp_bus_size - 1;

	*IXP4XX_EXP_CS0 |= IXP4XX_FLASH_WRITABLE;
	*IXP4XX_EXP_CS1 = *IXP4XX_EXP_CS0;

	/* Configure CS2 for operation, 8bit and writable */
	*IXP4XX_EXP_CS2 = 0xbfff0002;

	i2c_register_board_info(0, fsg_i2c_board_info,
				ARRAY_SIZE(fsg_i2c_board_info));

	/* This is only useful on a modified machine, but it is valuable
	 * to have it first in order to see debug messages, and so that
	 * it does *not* get removed if platform_add_devices fails!
	 */
	(void)platform_device_register(&fsg_uart);

	platform_add_devices(fsg_devices, ARRAY_SIZE(fsg_devices));

	if (request_irq(gpio_to_irq(FSG_RB_GPIO), &fsg_reset_handler,
			IRQF_DISABLED | IRQF_TRIGGER_LOW,
			"FSG reset button", NULL) < 0) {

		printk(KERN_DEBUG "Reset Button IRQ %d not available\n",
			gpio_to_irq(FSG_RB_GPIO));
	}

	if (request_irq(gpio_to_irq(FSG_SB_GPIO), &fsg_power_handler,
			IRQF_DISABLED | IRQF_TRIGGER_LOW,
			"FSG power button", NULL) < 0) {

		printk(KERN_DEBUG "Power Button IRQ %d not available\n",
			gpio_to_irq(FSG_SB_GPIO));
	}

	/*
	 * Map in a portion of the flash and read the MAC addresses.
	 * Since it is stored in BE in the flash itself, we need to
	 * byteswap it if we're in LE mode.
	 */
	f = ioremap(IXP4XX_EXP_BUS_BASE(0), 0x400000);
	if (f) {
#ifdef __ARMEB__
		int i;
		for (i = 0; i < 6; i++) {
			fsg_plat_eth[0].hwaddr[i] = readb(f + 0x3C0422 + i);
			fsg_plat_eth[1].hwaddr[i] = readb(f + 0x3C043B + i);
		}
#else

		/*
		  Endian-swapped reads from unaligned addresses are
		  required to extract the two MACs from the big-endian
		  Redboot config area in flash.
		*/

		fsg_plat_eth[0].hwaddr[0] = readb(f + 0x3C0421);
		fsg_plat_eth[0].hwaddr[1] = readb(f + 0x3C0420);
		fsg_plat_eth[0].hwaddr[2] = readb(f + 0x3C0427);
		fsg_plat_eth[0].hwaddr[3] = readb(f + 0x3C0426);
		fsg_plat_eth[0].hwaddr[4] = readb(f + 0x3C0425);
		fsg_plat_eth[0].hwaddr[5] = readb(f + 0x3C0424);

		fsg_plat_eth[1].hwaddr[0] = readb(f + 0x3C0439);
		fsg_plat_eth[1].hwaddr[1] = readb(f + 0x3C043F);
		fsg_plat_eth[1].hwaddr[2] = readb(f + 0x3C043E);
		fsg_plat_eth[1].hwaddr[3] = readb(f + 0x3C043D);
		fsg_plat_eth[1].hwaddr[4] = readb(f + 0x3C043C);
		fsg_plat_eth[1].hwaddr[5] = readb(f + 0x3C0443);
#endif
		iounmap(f);
	}
	printk(KERN_INFO "FSG: Using MAC address %pM for port 0\n",
	       fsg_plat_eth[0].hwaddr);
	printk(KERN_INFO "FSG: Using MAC address %pM for port 1\n",
	       fsg_plat_eth[1].hwaddr);

}

MACHINE_START(FSG, "Freecom FSG-3")
	/* Maintainer: www.nslu2-linux.org */
	.phys_io	= IXP4XX_PERIPHERAL_BASE_PHYS,
	.io_pg_offst	= ((IXP4XX_PERIPHERAL_BASE_VIRT) >> 18) & 0xfffc,
	.map_io		= ixp4xx_map_io,
	.init_irq	= ixp4xx_init_irq,
	.timer		= &ixp4xx_timer,
	.boot_params	= 0x0100,
	.init_machine	= fsg_init,
MACHINE_END

