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
 * linux/include/asm-mips/txx9/generic.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */
#ifndef __ASM_TXX9_GENERIC_H
#define __ASM_TXX9_GENERIC_H

#include <linux/init.h>
#include <linux/ioport.h>	/* for struct resource */

extern struct resource txx9_ce_res[];
#define TXX9_CE(n)	(unsigned long)(txx9_ce_res[(n)].start)
extern unsigned int txx9_pcode;
extern char txx9_pcode_str[8];
void txx9_reg_res_init(unsigned int pcode, unsigned long base,
		       unsigned long size);

extern unsigned int txx9_master_clock;
extern unsigned int txx9_cpu_clock;
extern unsigned int txx9_gbus_clock;
#define TXX9_IMCLK	(txx9_gbus_clock / 2)

extern int txx9_ccfg_toeon;
struct uart_port;
int early_serial_txx9_setup(struct uart_port *port);

struct pci_dev;
struct txx9_board_vec {
	const char *system;
	void (*prom_init)(void);
	void (*mem_setup)(void);
	void (*irq_setup)(void);
	void (*time_init)(void);
	void (*arch_init)(void);
	void (*device_init)(void);
#ifdef CONFIG_PCI
	int (*pci_map_irq)(const struct pci_dev *dev, u8 slot, u8 pin);
#endif
};
extern struct txx9_board_vec *txx9_board_vec;
extern int (*txx9_irq_dispatch)(int pending);
char *prom_getcmdline(void);
const char *prom_getenv(const char *name);
void txx9_wdt_init(unsigned long base);
void txx9_wdt_now(unsigned long base);
void txx9_spi_init(int busid, unsigned long base, int irq);
void txx9_ethaddr_init(unsigned int id, unsigned char *ethaddr);
void txx9_sio_init(unsigned long baseaddr, int irq,
		   unsigned int line, unsigned int sclk, int nocts);
void prom_putchar(char c);
#ifdef CONFIG_EARLY_PRINTK
extern void (*txx9_prom_putchar)(char c);
void txx9_sio_putchar_init(unsigned long baseaddr);
#else
static inline void txx9_sio_putchar_init(unsigned long baseaddr)
{
}
#endif

struct physmap_flash_data;
void txx9_physmap_flash_init(int no, unsigned long addr, unsigned long size,
			     const struct physmap_flash_data *pdata);

/* 8 bit version of __fls(): find first bit set (returns 0..7) */
static inline unsigned int __fls8(unsigned char x)
{
	int r = 7;

	if (!(x & 0xf0)) {
		r -= 4;
		x <<= 4;
	}
	if (!(x & 0xc0)) {
		r -= 2;
		x <<= 2;
	}
	if (!(x & 0x80))
		r -= 1;
	return r;
}

void txx9_iocled_init(unsigned long baseaddr,
		      int basenum, unsigned int num, int lowactive,
		      const char *color, char **deftriggers);

/* 7SEG LED */
void txx9_7segled_init(unsigned int num,
		       void (*putc)(unsigned int pos, unsigned char val));
int txx9_7segled_putc(unsigned int pos, char c);

#endif /* __ASM_TXX9_GENERIC_H */
