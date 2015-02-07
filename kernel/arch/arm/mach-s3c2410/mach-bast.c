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

/* linux/arch/arm/mach-s3c2410/mach-bast.c
 *
 * Copyright (c) 2003-2005,2008 Simtec Electronics
 *   Ben Dooks <ben@simtec.co.uk>
 *
 * http://www.simtec.co.uk/products/EB2410ITX/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/sysdev.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>
#include <linux/dm9000.h>
#include <linux/ata_platform.h>
#include <linux/i2c.h>
#include <linux/io.h>

#include <net/ax88796.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <mach/bast-map.h>
#include <mach/bast-irq.h>
#include <mach/bast-cpld.h>

#include <mach/hardware.h>
#include <asm/irq.h>
#include <asm/mach-types.h>

//#include <asm/debug-ll.h>
#include <plat/regs-serial.h>
#include <mach/regs-gpio.h>
#include <mach/regs-mem.h>
#include <mach/regs-lcd.h>

#include <plat/nand.h>
#include <plat/iic.h>
#include <mach/fb.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>

#include <linux/serial_8250.h>

#include <plat/clock.h>
#include <plat/devs.h>
#include <plat/cpu.h>

#include "usb-simtec.h"
#include "nor-simtec.h"

#define COPYRIGHT ", (c) 2004-2005 Simtec Electronics"

/* macros for virtual address mods for the io space entries */
#define VA_C5(item) ((unsigned long)(item) + BAST_VAM_CS5)
#define VA_C4(item) ((unsigned long)(item) + BAST_VAM_CS4)
#define VA_C3(item) ((unsigned long)(item) + BAST_VAM_CS3)
#define VA_C2(item) ((unsigned long)(item) + BAST_VAM_CS2)

/* macros to modify the physical addresses for io space */

#define PA_CS2(item) (__phys_to_pfn((item) + S3C2410_CS2))
#define PA_CS3(item) (__phys_to_pfn((item) + S3C2410_CS3))
#define PA_CS4(item) (__phys_to_pfn((item) + S3C2410_CS4))
#define PA_CS5(item) (__phys_to_pfn((item) + S3C2410_CS5))

static struct map_desc bast_iodesc[] __initdata = {
  /* ISA IO areas */
  {
	  .virtual	= (u32)S3C24XX_VA_ISA_BYTE,
	  .pfn		= PA_CS2(BAST_PA_ISAIO),
	  .length	= SZ_16M,
	  .type		= MT_DEVICE,
  }, {
	  .virtual	= (u32)S3C24XX_VA_ISA_WORD,
	  .pfn		= PA_CS3(BAST_PA_ISAIO),
	  .length	= SZ_16M,
	  .type		= MT_DEVICE,
  },
  /* bast CPLD control registers, and external interrupt controls */
  {
	  .virtual	= (u32)BAST_VA_CTRL1,
	  .pfn		= __phys_to_pfn(BAST_PA_CTRL1),
	  .length	= SZ_1M,
	  .type		= MT_DEVICE,
  }, {
	  .virtual	= (u32)BAST_VA_CTRL2,
	  .pfn		= __phys_to_pfn(BAST_PA_CTRL2),
	  .length	= SZ_1M,
	  .type		= MT_DEVICE,
  }, {
	  .virtual	= (u32)BAST_VA_CTRL3,
	  .pfn		= __phys_to_pfn(BAST_PA_CTRL3),
	  .length	= SZ_1M,
	  .type		= MT_DEVICE,
  }, {
	  .virtual	= (u32)BAST_VA_CTRL4,
	  .pfn		= __phys_to_pfn(BAST_PA_CTRL4),
	  .length	= SZ_1M,
	  .type		= MT_DEVICE,
  },
  /* PC104 IRQ mux */
  {
	  .virtual	= (u32)BAST_VA_PC104_IRQREQ,
	  .pfn		= __phys_to_pfn(BAST_PA_PC104_IRQREQ),
	  .length	= SZ_1M,
	  .type		= MT_DEVICE,
  }, {
	  .virtual	= (u32)BAST_VA_PC104_IRQRAW,
	  .pfn		= __phys_to_pfn(BAST_PA_PC104_IRQRAW),
	  .length	= SZ_1M,
	  .type		= MT_DEVICE,
  }, {
	  .virtual	= (u32)BAST_VA_PC104_IRQMASK,
	  .pfn		= __phys_to_pfn(BAST_PA_PC104_IRQMASK),
	  .length	= SZ_1M,
	  .type		= MT_DEVICE,
  },

  /* peripheral space... one for each of fast/slow/byte/16bit */
  /* note, ide is only decoded in word space, even though some registers
   * are only 8bit */

  /* slow, byte */
  { VA_C2(BAST_VA_ISAIO),   PA_CS2(BAST_PA_ISAIO),    SZ_16M, MT_DEVICE },
  { VA_C2(BAST_VA_ISAMEM),  PA_CS2(BAST_PA_ISAMEM),   SZ_16M, MT_DEVICE },
  { VA_C2(BAST_VA_SUPERIO), PA_CS2(BAST_PA_SUPERIO),  SZ_1M,  MT_DEVICE },

  /* slow, word */
  { VA_C3(BAST_VA_ISAIO),   PA_CS3(BAST_PA_ISAIO),    SZ_16M, MT_DEVICE },
  { VA_C3(BAST_VA_ISAMEM),  PA_CS3(BAST_PA_ISAMEM),   SZ_16M, MT_DEVICE },
  { VA_C3(BAST_VA_SUPERIO), PA_CS3(BAST_PA_SUPERIO),  SZ_1M,  MT_DEVICE },

  /* fast, byte */
  { VA_C4(BAST_VA_ISAIO),   PA_CS4(BAST_PA_ISAIO),    SZ_16M, MT_DEVICE },
  { VA_C4(BAST_VA_ISAMEM),  PA_CS4(BAST_PA_ISAMEM),   SZ_16M, MT_DEVICE },
  { VA_C4(BAST_VA_SUPERIO), PA_CS4(BAST_PA_SUPERIO),  SZ_1M,  MT_DEVICE },

  /* fast, word */
  { VA_C5(BAST_VA_ISAIO),   PA_CS5(BAST_PA_ISAIO),    SZ_16M, MT_DEVICE },
  { VA_C5(BAST_VA_ISAMEM),  PA_CS5(BAST_PA_ISAMEM),   SZ_16M, MT_DEVICE },
  { VA_C5(BAST_VA_SUPERIO), PA_CS5(BAST_PA_SUPERIO),  SZ_1M,  MT_DEVICE },
};

#define UCON S3C2410_UCON_DEFAULT | S3C2410_UCON_UCLK
#define ULCON S3C2410_LCON_CS8 | S3C2410_LCON_PNONE | S3C2410_LCON_STOPB
#define UFCON S3C2410_UFCON_RXTRIG8 | S3C2410_UFCON_FIFOMODE

static struct s3c24xx_uart_clksrc bast_serial_clocks[] = {
	[0] = {
		.name		= "uclk",
		.divisor	= 1,
		.min_baud	= 0,
		.max_baud	= 0,
	},
	[1] = {
		.name		= "pclk",
		.divisor	= 1,
		.min_baud	= 0,
		.max_baud	= 0,
	}
};


static struct s3c2410_uartcfg bast_uartcfgs[] __initdata = {
	[0] = {
		.hwport	     = 0,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
		.clocks	     = bast_serial_clocks,
		.clocks_size = ARRAY_SIZE(bast_serial_clocks),
	},
	[1] = {
		.hwport	     = 1,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
		.clocks	     = bast_serial_clocks,
		.clocks_size = ARRAY_SIZE(bast_serial_clocks),
	},
	/* port 2 is not actually used */
	[2] = {
		.hwport	     = 2,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
		.clocks	     = bast_serial_clocks,
		.clocks_size = ARRAY_SIZE(bast_serial_clocks),
	}
};

/* NAND Flash on BAST board */

#ifdef CONFIG_PM
static int bast_pm_suspend(struct sys_device *sd, pm_message_t state)
{
	/* ensure that an nRESET is not generated on resume. */
	s3c2410_gpio_setpin(S3C2410_GPA21, 1);
	s3c2410_gpio_cfgpin(S3C2410_GPA21, S3C2410_GPA21_OUT);

	return 0;
}

static int bast_pm_resume(struct sys_device *sd)
{
	s3c2410_gpio_cfgpin(S3C2410_GPA21, S3C2410_GPA21_nRSTOUT);
	return 0;
}

#else
#define bast_pm_suspend NULL
#define bast_pm_resume NULL
#endif

static struct sysdev_class bast_pm_sysclass = {
	.name		= "mach-bast",
	.suspend	= bast_pm_suspend,
	.resume		= bast_pm_resume,
};

static struct sys_device bast_pm_sysdev = {
	.cls		= &bast_pm_sysclass,
};

static int smartmedia_map[] = { 0 };
static int chip0_map[] = { 1 };
static int chip1_map[] = { 2 };
static int chip2_map[] = { 3 };

static struct mtd_partition bast_default_nand_part[] = {
	[0] = {
		.name	= "Boot Agent",
		.size	= SZ_16K,
		.offset	= 0,
	},
	[1] = {
		.name	= "/boot",
		.size	= SZ_4M - SZ_16K,
		.offset	= SZ_16K,
	},
	[2] = {
		.name	= "user",
		.offset	= SZ_4M,
		.size	= MTDPART_SIZ_FULL,
	}
};

/* the bast has 4 selectable slots for nand-flash, the three
 * on-board chip areas, as well as the external SmartMedia
 * slot.
 *
 * Note, there is no current hot-plug support for the SmartMedia
 * socket.
*/

static struct s3c2410_nand_set bast_nand_sets[] = {
	[0] = {
		.name		= "SmartMedia",
		.nr_chips	= 1,
		.nr_map		= smartmedia_map,
		.nr_partitions	= ARRAY_SIZE(bast_default_nand_part),
		.partitions	= bast_default_nand_part,
	},
	[1] = {
		.name		= "chip0",
		.nr_chips	= 1,
		.nr_map		= chip0_map,
		.nr_partitions	= ARRAY_SIZE(bast_default_nand_part),
		.partitions	= bast_default_nand_part,
	},
	[2] = {
		.name		= "chip1",
		.nr_chips	= 1,
		.nr_map		= chip1_map,
		.nr_partitions	= ARRAY_SIZE(bast_default_nand_part),
		.partitions	= bast_default_nand_part,
	},
	[3] = {
		.name		= "chip2",
		.nr_chips	= 1,
		.nr_map		= chip2_map,
		.nr_partitions	= ARRAY_SIZE(bast_default_nand_part),
		.partitions	= bast_default_nand_part,
	}
};

static void bast_nand_select(struct s3c2410_nand_set *set, int slot)
{
	unsigned int tmp;

	slot = set->nr_map[slot] & 3;

	pr_debug("bast_nand: selecting slot %d (set %p,%p)\n",
		 slot, set, set->nr_map);

	tmp = __raw_readb(BAST_VA_CTRL2);
	tmp &= BAST_CPLD_CTLR2_IDERST;
	tmp |= slot;
	tmp |= BAST_CPLD_CTRL2_WNAND;

	pr_debug("bast_nand: ctrl2 now %02x\n", tmp);

	__raw_writeb(tmp, BAST_VA_CTRL2);
}

static struct s3c2410_platform_nand bast_nand_info = {
	.tacls		= 30,
	.twrph0		= 60,
	.twrph1		= 60,
	.nr_sets	= ARRAY_SIZE(bast_nand_sets),
	.sets		= bast_nand_sets,
	.select_chip	= bast_nand_select,
};

/* DM9000 */

static struct resource bast_dm9k_resource[] = {
	[0] = {
		.start = S3C2410_CS5 + BAST_PA_DM9000,
		.end   = S3C2410_CS5 + BAST_PA_DM9000 + 3,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = S3C2410_CS5 + BAST_PA_DM9000 + 0x40,
		.end   = S3C2410_CS5 + BAST_PA_DM9000 + 0x40 + 0x3f,
		.flags = IORESOURCE_MEM,
	},
	[2] = {
		.start = IRQ_DM9000,
		.end   = IRQ_DM9000,
		.flags = IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL,
	}

};

/* for the moment we limit ourselves to 16bit IO until some
 * better IO routines can be written and tested
*/

static struct dm9000_plat_data bast_dm9k_platdata = {
	.flags		= DM9000_PLATF_16BITONLY,
};

static struct platform_device bast_device_dm9k = {
	.name		= "dm9000",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(bast_dm9k_resource),
	.resource	= bast_dm9k_resource,
	.dev		= {
		.platform_data = &bast_dm9k_platdata,
	}
};

/* serial devices */

#define SERIAL_BASE  (S3C2410_CS2 + BAST_PA_SUPERIO)
#define SERIAL_FLAGS (UPF_BOOT_AUTOCONF | UPF_IOREMAP | UPF_SHARE_IRQ)
#define SERIAL_CLK   (1843200)

static struct plat_serial8250_port bast_sio_data[] = {
	[0] = {
		.mapbase	= SERIAL_BASE + 0x2f8,
		.irq		= IRQ_PCSERIAL1,
		.flags		= SERIAL_FLAGS,
		.iotype		= UPIO_MEM,
		.regshift	= 0,
		.uartclk	= SERIAL_CLK,
	},
	[1] = {
		.mapbase	= SERIAL_BASE + 0x3f8,
		.irq		= IRQ_PCSERIAL2,
		.flags		= SERIAL_FLAGS,
		.iotype		= UPIO_MEM,
		.regshift	= 0,
		.uartclk	= SERIAL_CLK,
	},
	{ }
};

static struct platform_device bast_sio = {
	.name			= "serial8250",
	.id			= PLAT8250_DEV_PLATFORM,
	.dev			= {
		.platform_data	= &bast_sio_data,
	},
};

/* we have devices on the bus which cannot work much over the
 * standard 100KHz i2c bus frequency
*/

static struct s3c2410_platform_i2c __initdata bast_i2c_info = {
	.flags		= 0,
	.slave_addr	= 0x10,
	.bus_freq	= 100*1000,
	.max_freq	= 130*1000,
};

/* Asix AX88796 10/100 ethernet controller */

static struct ax_plat_data bast_asix_platdata = {
	.flags		= AXFLG_MAC_FROMDEV,
	.wordlength	= 2,
	.dcr_val	= 0x48,
	.rcr_val	= 0x40,
};

static struct resource bast_asix_resource[] = {
	[0] = {
		.start = S3C2410_CS5 + BAST_PA_ASIXNET,
		.end   = S3C2410_CS5 + BAST_PA_ASIXNET + (0x18 * 0x20) - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = S3C2410_CS5 + BAST_PA_ASIXNET + (0x1f * 0x20),
		.end   = S3C2410_CS5 + BAST_PA_ASIXNET + (0x1f * 0x20),
		.flags = IORESOURCE_MEM,
	},
	[2] = {
		.start = IRQ_ASIX,
		.end   = IRQ_ASIX,
		.flags = IORESOURCE_IRQ
	}
};

static struct platform_device bast_device_asix = {
	.name		= "ax88796",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(bast_asix_resource),
	.resource	= bast_asix_resource,
	.dev		= {
		.platform_data = &bast_asix_platdata
	}
};

/* Asix AX88796 10/100 ethernet controller parallel port */

static struct resource bast_asixpp_resource[] = {
	[0] = {
		.start = S3C2410_CS5 + BAST_PA_ASIXNET + (0x18 * 0x20),
		.end   = S3C2410_CS5 + BAST_PA_ASIXNET + (0x1b * 0x20) - 1,
		.flags = IORESOURCE_MEM,
	}
};

static struct platform_device bast_device_axpp = {
	.name		= "ax88796-pp",
	.id		= 0,
	.num_resources	= ARRAY_SIZE(bast_asixpp_resource),
	.resource	= bast_asixpp_resource,
};

/* LCD/VGA controller */

static struct s3c2410fb_display __initdata bast_lcd_info[] = {
	{
		.type		= S3C2410_LCDCON1_TFT,
		.width		= 640,
		.height		= 480,

		.pixclock	= 33333,
		.xres		= 640,
		.yres		= 480,
		.bpp		= 4,
		.left_margin	= 40,
		.right_margin	= 20,
		.hsync_len	= 88,
		.upper_margin	= 30,
		.lower_margin	= 32,
		.vsync_len	= 3,

		.lcdcon5	= 0x00014b02,
	},
	{
		.type		= S3C2410_LCDCON1_TFT,
		.width		= 640,
		.height		= 480,

		.pixclock	= 33333,
		.xres		= 640,
		.yres		= 480,
		.bpp		= 8,
		.left_margin	= 40,
		.right_margin	= 20,
		.hsync_len	= 88,
		.upper_margin	= 30,
		.lower_margin	= 32,
		.vsync_len	= 3,

		.lcdcon5	= 0x00014b02,
	},
	{
		.type		= S3C2410_LCDCON1_TFT,
		.width		= 640,
		.height		= 480,

		.pixclock	= 33333,
		.xres		= 640,
		.yres		= 480,
		.bpp		= 16,
		.left_margin	= 40,
		.right_margin	= 20,
		.hsync_len	= 88,
		.upper_margin	= 30,
		.lower_margin	= 32,
		.vsync_len	= 3,

		.lcdcon5	= 0x00014b02,
	},
};

/* LCD/VGA controller */

static struct s3c2410fb_mach_info __initdata bast_fb_info = {

	.displays = bast_lcd_info,
	.num_displays = ARRAY_SIZE(bast_lcd_info),
	.default_display = 1,
};

/* I2C devices fitted. */

static struct i2c_board_info bast_i2c_devs[] __initdata = {
	{
		I2C_BOARD_INFO("tlv320aic23", 0x1a),
	}, {
		I2C_BOARD_INFO("simtec-pmu", 0x6b),
	}, {
		I2C_BOARD_INFO("ch7013", 0x75),
	},
};

/* Standard BAST devices */

static struct platform_device *bast_devices[] __initdata = {
	&s3c_device_usb,
	&s3c_device_lcd,
	&s3c_device_wdt,
	&s3c_device_i2c0,
 	&s3c_device_rtc,
	&s3c_device_nand,
	&bast_device_dm9k,
	&bast_device_asix,
	&bast_device_axpp,
	&bast_sio,
};

static struct clk *bast_clocks[] __initdata = {
	&s3c24xx_dclk0,
	&s3c24xx_dclk1,
	&s3c24xx_clkout0,
	&s3c24xx_clkout1,
	&s3c24xx_uclk,
};

static void __init bast_map_io(void)
{
	/* initialise the clocks */

	s3c24xx_dclk0.parent = &clk_upll;
	s3c24xx_dclk0.rate   = 12*1000*1000;

	s3c24xx_dclk1.parent = &clk_upll;
	s3c24xx_dclk1.rate   = 24*1000*1000;

	s3c24xx_clkout0.parent  = &s3c24xx_dclk0;
	s3c24xx_clkout1.parent  = &s3c24xx_dclk1;

	s3c24xx_uclk.parent  = &s3c24xx_clkout1;

	s3c24xx_register_clocks(bast_clocks, ARRAY_SIZE(bast_clocks));

	s3c_device_nand.dev.platform_data = &bast_nand_info;

	s3c_i2c0_set_platdata(&bast_i2c_info);

	s3c24xx_init_io(bast_iodesc, ARRAY_SIZE(bast_iodesc));
	s3c24xx_init_clocks(0);
	s3c24xx_init_uarts(bast_uartcfgs, ARRAY_SIZE(bast_uartcfgs));

	usb_simtec_init();
}

static void __init bast_init(void)
{
	sysdev_class_register(&bast_pm_sysclass);
	sysdev_register(&bast_pm_sysdev);

	s3c24xx_fb_set_platdata(&bast_fb_info);
	platform_add_devices(bast_devices, ARRAY_SIZE(bast_devices));

	i2c_register_board_info(0, bast_i2c_devs,
				ARRAY_SIZE(bast_i2c_devs));

	nor_simtec_init();
}

MACHINE_START(BAST, "Simtec-BAST")
	/* Maintainer: Ben Dooks <ben@simtec.co.uk> */
	.phys_io	= S3C2410_PA_UART,
	.io_pg_offst	= (((u32)S3C24XX_VA_UART) >> 18) & 0xfffc,
	.boot_params	= S3C2410_SDRAM_PA + 0x100,
	.map_io		= bast_map_io,
	.init_irq	= s3c24xx_init_irq,
	.init_machine	= bast_init,
	.timer		= &s3c24xx_timer,
MACHINE_END
