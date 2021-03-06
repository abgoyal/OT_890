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
 * include/linux/serial.h
 *
 * Copyright (C) 1992 by Theodore Ts'o.
 * 
 * Redistribution of this file is permitted under the terms of the GNU 
 * Public License (GPL)
 */

#ifndef _LINUX_SERIAL_H
#define _LINUX_SERIAL_H

#include <linux/types.h>

#ifdef __KERNEL__
#include <asm/page.h>

/*
 * Counters of the input lines (CTS, DSR, RI, CD) interrupts
 */

struct async_icount {
	__u32	cts, dsr, rng, dcd, tx, rx;
	__u32	frame, parity, overrun, brk;
	__u32	buf_overrun;
};

/*
 * The size of the serial xmit buffer is 1 page, or 4096 bytes
 */
#define SERIAL_XMIT_SIZE PAGE_SIZE

#endif

struct serial_struct {
	int	type;
	int	line;
	unsigned int	port;
	int	irq;
	int	flags;
	int	xmit_fifo_size;
	int	custom_divisor;
	int	baud_base;
	unsigned short	close_delay;
	char	io_type;
	char	reserved_char[1];
	int	hub6;
	unsigned short	closing_wait; /* time to wait before closing */
	unsigned short	closing_wait2; /* no longer used... */
	unsigned char	*iomem_base;
	unsigned short	iomem_reg_shift;
	unsigned int	port_high;
	unsigned long	iomap_base;	/* cookie passed into ioremap */
};

/*
 * For the close wait times, 0 means wait forever for serial port to
 * flush its output.  65535 means don't wait at all.
 */
#define ASYNC_CLOSING_WAIT_INF	0
#define ASYNC_CLOSING_WAIT_NONE	65535

/*
 * These are the supported serial types.
 */
#define PORT_UNKNOWN	0
#define PORT_8250	1
#define PORT_16450	2
#define PORT_16550	3
#define PORT_16550A	4
#define PORT_CIRRUS     5	/* usurped by cyclades.c */
#define PORT_16650	6
#define PORT_16650V2	7
#define PORT_16750	8
#define PORT_STARTECH	9	/* usurped by cyclades.c */
#define PORT_16C950	10	/* Oxford Semiconductor */
#define PORT_16654	11
#define PORT_16850	12
#define PORT_RSA	13	/* RSA-DV II/S card */
#define PORT_MAX	13

#define SERIAL_IO_PORT	0
#define SERIAL_IO_HUB6	1
#define SERIAL_IO_MEM	2

struct serial_uart_config {
	char	*name;
	int	dfl_xmit_fifo_size;
	int	flags;
};

#define UART_CLEAR_FIFO		0x01
#define UART_USE_FIFO		0x02
#define UART_STARTECH		0x04
#define UART_NATSEMI		0x08

/*
 * Definitions for async_struct (and serial_struct) flags field
 */
#define ASYNC_HUP_NOTIFY 0x0001 /* Notify getty on hangups and closes 
				   on the callout port */
#define ASYNC_FOURPORT  0x0002	/* Set OU1, OUT2 per AST Fourport settings */
#define ASYNC_SAK	0x0004	/* Secure Attention Key (Orange book) */
#define ASYNC_SPLIT_TERMIOS 0x0008 /* Separate termios for dialin/callout */

#define ASYNC_SPD_MASK	0x1030
#define ASYNC_SPD_HI	0x0010	/* Use 56000 instead of 38400 bps */

#define ASYNC_SPD_VHI	0x0020  /* Use 115200 instead of 38400 bps */
#define ASYNC_SPD_CUST	0x0030  /* Use user-specified divisor */

#define ASYNC_SKIP_TEST	0x0040 /* Skip UART test during autoconfiguration */
#define ASYNC_AUTO_IRQ  0x0080 /* Do automatic IRQ during autoconfiguration */
#define ASYNC_SESSION_LOCKOUT 0x0100 /* Lock out cua opens based on session */
#define ASYNC_PGRP_LOCKOUT    0x0200 /* Lock out cua opens based on pgrp */
#define ASYNC_CALLOUT_NOHUP   0x0400 /* Don't do hangups for cua device */

#define ASYNC_HARDPPS_CD	0x0800	/* Call hardpps when CD goes high  */

#define ASYNC_SPD_SHI	0x1000	/* Use 230400 instead of 38400 bps */
#define ASYNC_SPD_WARP	0x1010	/* Use 460800 instead of 38400 bps */

#define ASYNC_LOW_LATENCY 0x2000 /* Request low latency behaviour */

#define ASYNC_BUGGY_UART  0x4000 /* This is a buggy UART, skip some safety
				  * checks.  Note: can be dangerous! */

#define ASYNC_AUTOPROBE	 0x8000 /* Port was autoprobed by PCI or PNP code */

#define ASYNC_FLAGS	0x7FFF	/* Possible legal async flags */
#define ASYNC_USR_MASK	0x3430	/* Legal flags that non-privileged
				 * users can set or reset */

/* Internal flags used only by kernel/chr_drv/serial.c */
#define ASYNC_INITIALIZED	0x80000000 /* Serial port was initialized */
#define ASYNC_NORMAL_ACTIVE	0x20000000 /* Normal device is active */
#define ASYNC_BOOT_AUTOCONF	0x10000000 /* Autoconfigure port on bootup */
#define ASYNC_CLOSING		0x08000000 /* Serial port is closing */
#define ASYNC_CTS_FLOW		0x04000000 /* Do CTS flow control */
#define ASYNC_CHECK_CD		0x02000000 /* i.e., CLOCAL */
#define ASYNC_SHARE_IRQ		0x01000000 /* for multifunction cards
					     --- no longer used */
#define ASYNC_CONS_FLOW		0x00800000 /* flow control for console  */

#define ASYNC_BOOT_ONLYMCA	0x00400000 /* Probe only if MCA bus */
#define ASYNC_INTERNAL_FLAGS	0xFFC00000 /* Internal flags */

/*
 * Multiport serial configuration structure --- external structure
 */
struct serial_multiport_struct {
	int		irq;
	int		port1;
	unsigned char	mask1, match1;
	int		port2;
	unsigned char	mask2, match2;
	int		port3;
	unsigned char	mask3, match3;
	int		port4;
	unsigned char	mask4, match4;
	int		port_monitor;
	int	reserved[32];
};

/*
 * Serial input interrupt line counters -- external structure
 * Four lines can interrupt: CTS, DSR, RI, DCD
 */
struct serial_icounter_struct {
	int cts, dsr, rng, dcd;
	int rx, tx;
	int frame, overrun, parity, brk;
	int buf_overrun;
	int reserved[9];
};

/*
 * Serial interface for controlling RS485 settings on chips with suitable
 * support. Set with TIOCSRS485 and get with TIOCGRS485 if supported by your
 * platform. The set function returns the new state, with any unsupported bits
 * reverted appropriately.
 */

struct serial_rs485 {
	__u32	flags;			/* RS485 feature flags */
#define SER_RS485_ENABLED		(1 << 0)
#define SER_RS485_RTS_ON_SEND		(1 << 1)
#define SER_RS485_RTS_AFTER_SEND	(1 << 2)
	__u32	delay_rts_before_send;	/* Milliseconds */
	__u32	padding[6];		/* Memory is cheap, new structs
					   are a royal PITA .. */
};

#ifdef __KERNEL__
#include <linux/compiler.h>

#endif /* __KERNEL__ */
#endif /* _LINUX_SERIAL_H */
