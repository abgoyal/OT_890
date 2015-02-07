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
 * linux/arch/arm/mach-pxa/zylonite.c
 *
 * Support for the PXA3xx Development Platform (aka Zylonite)
 *
 * Copyright (C) 2006 Marvell International Ltd.
 *
 * 2007-09-04: eric miao <eric.miao@marvell.com>
 *             rewrite to align with latest kernel
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/pwm_backlight.h>
#include <linux/smc91x.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <mach/hardware.h>
#include <mach/audio.h>
#include <mach/pxafb.h>
#include <mach/zylonite.h>
#include <mach/mmc.h>
#include <mach/ohci.h>
#include <mach/pxa27x_keypad.h>
#include <mach/pxa3xx_nand.h>

#include "devices.h"
#include "generic.h"

#define MAX_SLOTS	3
struct platform_mmc_slot zylonite_mmc_slot[MAX_SLOTS];

int gpio_eth_irq;
int gpio_debug_led1;
int gpio_debug_led2;

int wm9713_irq;

int lcd_id;
int lcd_orientation;

static struct resource smc91x_resources[] = {
	[0] = {
		.start	= ZYLONITE_ETH_PHYS + 0x300,
		.end	= ZYLONITE_ETH_PHYS + 0xfffff,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= -1,	/* for run-time assignment */
		.end	= -1,
		.flags	= IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHEDGE,
	}
};

static struct smc91x_platdata zylonite_smc91x_info = {
	.flags	= SMC91X_USE_8BIT | SMC91X_USE_16BIT |
		  SMC91X_NOWAIT | SMC91X_USE_DMA,
};

static struct platform_device smc91x_device = {
	.name		= "smc91x",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(smc91x_resources),
	.resource	= smc91x_resources,
	.dev		= {
		.platform_data = &zylonite_smc91x_info,
	},
};

#if defined(CONFIG_LEDS_GPIO) || defined(CONFIG_LEDS_GPIO_MODULE)
static struct gpio_led zylonite_debug_leds[] = {
	[0] = {
		.name			= "zylonite:yellow:1",
		.default_trigger	= "heartbeat",
	},
	[1] = {
		.name			= "zylonite:yellow:2",
		.default_trigger	= "default-on",
	},
};

static struct gpio_led_platform_data zylonite_debug_leds_info = {
	.leds		= zylonite_debug_leds,
	.num_leds	= ARRAY_SIZE(zylonite_debug_leds),
};

static struct platform_device zylonite_device_leds = {
	.name		= "leds-gpio",
	.id		= -1,
	.dev		= {
		.platform_data = &zylonite_debug_leds_info,
	}
};

static void __init zylonite_init_leds(void)
{
	zylonite_debug_leds[0].gpio = gpio_debug_led1;
	zylonite_debug_leds[1].gpio = gpio_debug_led2;

	platform_device_register(&zylonite_device_leds);
}
#else
static inline void zylonite_init_leds(void) {}
#endif

#if defined(CONFIG_FB_PXA) || defined(CONFIG_FB_PXA_MODULE)
static struct platform_pwm_backlight_data zylonite_backlight_data = {
	.pwm_id		= 3,
	.max_brightness	= 100,
	.dft_brightness	= 100,
	.pwm_period_ns	= 10000,
};

static struct platform_device zylonite_backlight_device = {
	.name		= "pwm-backlight",
	.dev		= {
		.parent = &pxa27x_device_pwm1.dev,
		.platform_data	= &zylonite_backlight_data,
	},
};

static struct pxafb_mode_info toshiba_ltm035a776c_mode = {
	.pixclock		= 110000,
	.xres			= 240,
	.yres			= 320,
	.bpp			= 16,
	.hsync_len		= 4,
	.left_margin		= 6,
	.right_margin		= 4,
	.vsync_len		= 2,
	.upper_margin		= 2,
	.lower_margin		= 3,
	.sync			= FB_SYNC_VERT_HIGH_ACT,
};

static struct pxafb_mode_info toshiba_ltm04c380k_mode = {
	.pixclock		= 50000,
	.xres			= 640,
	.yres			= 480,
	.bpp			= 16,
	.hsync_len		= 1,
	.left_margin		= 0x9f,
	.right_margin		= 1,
	.vsync_len		= 44,
	.upper_margin		= 0,
	.lower_margin		= 0,
	.sync			= FB_SYNC_HOR_HIGH_ACT|FB_SYNC_VERT_HIGH_ACT,
};

static struct pxafb_mach_info zylonite_toshiba_lcd_info = {
	.num_modes      	= 1,
	.lcd_conn		= LCD_COLOR_TFT_16BPP | LCD_PCLK_EDGE_FALL,
};

static struct pxafb_mode_info sharp_ls037_modes[] = {
	[0] = {
		.pixclock	= 158000,
		.xres		= 240,
		.yres		= 320,
		.bpp		= 16,
		.hsync_len	= 4,
		.left_margin	= 39,
		.right_margin	= 39,
		.vsync_len	= 1,
		.upper_margin	= 2,
		.lower_margin	= 3,
		.sync		= 0,
	},
	[1] = {
		.pixclock	= 39700,
		.xres		= 480,
		.yres		= 640,
		.bpp		= 16,
		.hsync_len	= 8,
		.left_margin	= 81,
		.right_margin	= 81,
		.vsync_len	= 1,
		.upper_margin	= 2,
		.lower_margin	= 7,
		.sync		= 0,
	},
};

static struct pxafb_mach_info zylonite_sharp_lcd_info = {
	.modes			= sharp_ls037_modes,
	.num_modes		= 2,
	.lcd_conn		= LCD_COLOR_TFT_16BPP | LCD_PCLK_EDGE_FALL,
};

static void __init zylonite_init_lcd(void)
{
	platform_device_register(&zylonite_backlight_device);

	if (lcd_id & 0x20) {
		set_pxa_fb_info(&zylonite_sharp_lcd_info);
		return;
	}

	/* legacy LCD panels, it would be handy here if LCD panel type can
	 * be decided at run-time
	 */
	if (1)
		zylonite_toshiba_lcd_info.modes = &toshiba_ltm035a776c_mode;
	else
		zylonite_toshiba_lcd_info.modes = &toshiba_ltm04c380k_mode;

	set_pxa_fb_info(&zylonite_toshiba_lcd_info);
}
#else
static inline void zylonite_init_lcd(void) {}
#endif

#if defined(CONFIG_MMC)
static int zylonite_mci_ro(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);

	return gpio_get_value(zylonite_mmc_slot[pdev->id].gpio_wp);
}

static int zylonite_mci_init(struct device *dev,
			     irq_handler_t zylonite_detect_int,
			     void *data)
{
	struct platform_device *pdev = to_platform_device(dev);
	int err, cd_irq, gpio_cd, gpio_wp;

	cd_irq = gpio_to_irq(zylonite_mmc_slot[pdev->id].gpio_cd);
	gpio_cd = zylonite_mmc_slot[pdev->id].gpio_cd;
	gpio_wp = zylonite_mmc_slot[pdev->id].gpio_wp;

	/*
	 * setup GPIO for Zylonite MMC controller
	 */
	err = gpio_request(gpio_cd, "mmc card detect");
	if (err)
		goto err_request_cd;
	gpio_direction_input(gpio_cd);

	err = gpio_request(gpio_wp, "mmc write protect");
	if (err)
		goto err_request_wp;
	gpio_direction_input(gpio_wp);

	err = request_irq(cd_irq, zylonite_detect_int,
			  IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
			  "MMC card detect", data);
	if (err) {
		printk(KERN_ERR "%s: MMC/SD/SDIO: "
				"can't request card detect IRQ\n", __func__);
		goto err_request_irq;
	}

	return 0;

err_request_irq:
	gpio_free(gpio_wp);
err_request_wp:
	gpio_free(gpio_cd);
err_request_cd:
	return err;
}

static void zylonite_mci_exit(struct device *dev, void *data)
{
	struct platform_device *pdev = to_platform_device(dev);
	int cd_irq, gpio_cd, gpio_wp;

	cd_irq = gpio_to_irq(zylonite_mmc_slot[pdev->id].gpio_cd);
	gpio_cd = zylonite_mmc_slot[pdev->id].gpio_cd;
	gpio_wp = zylonite_mmc_slot[pdev->id].gpio_wp;

	free_irq(cd_irq, data);
	gpio_free(gpio_cd);
	gpio_free(gpio_wp);
}

static struct pxamci_platform_data zylonite_mci_platform_data = {
	.detect_delay	= 20,
	.ocr_mask	= MMC_VDD_32_33|MMC_VDD_33_34,
	.init 		= zylonite_mci_init,
	.exit		= zylonite_mci_exit,
	.get_ro		= zylonite_mci_ro,
};

static struct pxamci_platform_data zylonite_mci2_platform_data = {
	.detect_delay	= 20,
	.ocr_mask	= MMC_VDD_32_33|MMC_VDD_33_34,
};

static void __init zylonite_init_mmc(void)
{
	pxa_set_mci_info(&zylonite_mci_platform_data);
	pxa3xx_set_mci2_info(&zylonite_mci2_platform_data);
	if (cpu_is_pxa310())
		pxa3xx_set_mci3_info(&zylonite_mci_platform_data);
}
#else
static inline void zylonite_init_mmc(void) {}
#endif

#if defined(CONFIG_KEYBOARD_PXA27x) || defined(CONFIG_KEYBOARD_PXA27x_MODULE)
static unsigned int zylonite_matrix_key_map[] = {
	/* KEY(row, col, key_code) */
	KEY(0, 0, KEY_A), KEY(0, 1, KEY_B), KEY(0, 2, KEY_C), KEY(0, 5, KEY_D),
	KEY(1, 0, KEY_E), KEY(1, 1, KEY_F), KEY(1, 2, KEY_G), KEY(1, 5, KEY_H),
	KEY(2, 0, KEY_I), KEY(2, 1, KEY_J), KEY(2, 2, KEY_K), KEY(2, 5, KEY_L),
	KEY(3, 0, KEY_M), KEY(3, 1, KEY_N), KEY(3, 2, KEY_O), KEY(3, 5, KEY_P),
	KEY(5, 0, KEY_Q), KEY(5, 1, KEY_R), KEY(5, 2, KEY_S), KEY(5, 5, KEY_T),
	KEY(6, 0, KEY_U), KEY(6, 1, KEY_V), KEY(6, 2, KEY_W), KEY(6, 5, KEY_X),
	KEY(7, 1, KEY_Y), KEY(7, 2, KEY_Z),

	KEY(4, 4, KEY_0), KEY(1, 3, KEY_1), KEY(4, 1, KEY_2), KEY(1, 4, KEY_3),
	KEY(2, 3, KEY_4), KEY(4, 2, KEY_5), KEY(2, 4, KEY_6), KEY(3, 3, KEY_7),
	KEY(4, 3, KEY_8), KEY(3, 4, KEY_9),

	KEY(4, 5, KEY_SPACE),
	KEY(5, 3, KEY_KPASTERISK), 	/* * */
	KEY(5, 4, KEY_KPDOT), 		/* #" */

	KEY(0, 7, KEY_UP),
	KEY(1, 7, KEY_DOWN),
	KEY(2, 7, KEY_LEFT),
	KEY(3, 7, KEY_RIGHT),
	KEY(2, 6, KEY_HOME),
	KEY(3, 6, KEY_END),
	KEY(6, 4, KEY_DELETE),
	KEY(6, 6, KEY_BACK),
	KEY(6, 3, KEY_CAPSLOCK),	/* KEY_LEFTSHIFT), */

	KEY(4, 6, KEY_ENTER),		/* scroll push */
	KEY(5, 7, KEY_ENTER),		/* keypad action */

	KEY(0, 4, KEY_EMAIL),
	KEY(5, 6, KEY_SEND),
	KEY(4, 0, KEY_CALENDAR),
	KEY(7, 6, KEY_RECORD),
	KEY(6, 7, KEY_VOLUMEUP),
	KEY(7, 7, KEY_VOLUMEDOWN),

	KEY(0, 6, KEY_F22),	/* soft1 */
	KEY(1, 6, KEY_F23),	/* soft2 */
	KEY(0, 3, KEY_AUX),	/* contact */
};

static struct pxa27x_keypad_platform_data zylonite_keypad_info = {
	.matrix_key_rows	= 8,
	.matrix_key_cols	= 8,
	.matrix_key_map		= zylonite_matrix_key_map,
	.matrix_key_map_size	= ARRAY_SIZE(zylonite_matrix_key_map),

	.enable_rotary0		= 1,
	.rotary0_up_key		= KEY_UP,
	.rotary0_down_key	= KEY_DOWN,

	.debounce_interval	= 30,
};

static void __init zylonite_init_keypad(void)
{
	pxa_set_keypad_info(&zylonite_keypad_info);
}
#else
static inline void zylonite_init_keypad(void) {}
#endif

#if defined(CONFIG_MTD_NAND_PXA3xx) || defined(CONFIG_MTD_NAND_PXA3xx_MODULE)
static struct mtd_partition zylonite_nand_partitions[] = {
	[0] = {
		.name        = "Bootloader",
		.offset      = 0,
		.size        = 0x060000,
		.mask_flags  = MTD_WRITEABLE, /* force read-only */
	},
	[1] = {
		.name        = "Kernel",
		.offset      = 0x060000,
		.size        = 0x200000,
		.mask_flags  = MTD_WRITEABLE, /* force read-only */
	},
	[2] = {
		.name        = "Filesystem",
		.offset      = 0x0260000,
		.size        = 0x3000000,     /* 48M - rootfs */
	},
	[3] = {
		.name        = "MassStorage",
		.offset      = 0x3260000,
		.size        = 0x3d40000,
	},
	[4] = {
		.name        = "BBT",
		.offset      = 0x6FA0000,
		.size        = 0x80000,
		.mask_flags  = MTD_WRITEABLE,  /* force read-only */
	},
	/* NOTE: we reserve some blocks at the end of the NAND flash for
	 * bad block management, and the max number of relocation blocks
	 * differs on different platforms. Please take care with it when
	 * defining the partition table.
	 */
};

static struct pxa3xx_nand_platform_data zylonite_nand_info = {
	.enable_arbiter	= 1,
	.parts		= zylonite_nand_partitions,
	.nr_parts	= ARRAY_SIZE(zylonite_nand_partitions),
};

static void __init zylonite_init_nand(void)
{
	pxa3xx_set_nand_info(&zylonite_nand_info);
}
#else
static inline void zylonite_init_nand(void) {}
#endif /* CONFIG_MTD_NAND_PXA3xx || CONFIG_MTD_NAND_PXA3xx_MODULE */

#if defined(CONFIG_USB_OHCI_HCD) || defined(CONFIG_USB_OHCI_HCD_MODULE)
static struct pxaohci_platform_data zylonite_ohci_info = {
	.port_mode	= PMM_PERPORT_MODE,
	.flags		= ENABLE_PORT1 | ENABLE_PORT2 |
			  POWER_CONTROL_LOW | POWER_SENSE_LOW,
};

static void __init zylonite_init_ohci(void)
{
	pxa_set_ohci_info(&zylonite_ohci_info);
}
#else
static inline void zylonite_init_ohci(void) {}
#endif /* CONFIG_USB_OHCI_HCD || CONFIG_USB_OHCI_HCD_MODULE */

static void __init zylonite_init(void)
{
	/* board-processor specific initialization */
	zylonite_pxa300_init();
	zylonite_pxa320_init();

	/*
	 * Note: We depend that the bootloader set
	 * the correct value to MSC register for SMC91x.
	 */
	smc91x_resources[1].start = gpio_to_irq(gpio_eth_irq);
	smc91x_resources[1].end   = gpio_to_irq(gpio_eth_irq);
	platform_device_register(&smc91x_device);

	pxa_set_ac97_info(NULL);
	zylonite_init_lcd();
	zylonite_init_mmc();
	zylonite_init_keypad();
	zylonite_init_nand();
	zylonite_init_leds();
	zylonite_init_ohci();
}

MACHINE_START(ZYLONITE, "PXA3xx Platform Development Kit (aka Zylonite)")
	.phys_io	= 0x40000000,
	.boot_params	= 0xa0000100,
	.io_pg_offst	= (io_p2v(0x40000000) >> 18) & 0xfffc,
	.map_io		= pxa_map_io,
	.init_irq	= pxa3xx_init_irq,
	.timer		= &pxa_timer,
	.init_machine	= zylonite_init,
MACHINE_END
