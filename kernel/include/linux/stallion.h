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

/*****************************************************************************/

/*
 *	stallion.h  -- stallion multiport serial driver.
 *
 *	Copyright (C) 1996-1998  Stallion Technologies
 *	Copyright (C) 1994-1996  Greg Ungerer.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*****************************************************************************/
#ifndef	_STALLION_H
#define	_STALLION_H
/*****************************************************************************/

/*
 *	Define important driver constants here.
 */
#define	STL_MAXBRDS		4
#define	STL_MAXPANELS		4
#define	STL_MAXBANKS		8
#define	STL_PORTSPERPANEL	16
#define	STL_MAXPORTS		64
#define	STL_MAXDEVS		(STL_MAXBRDS * STL_MAXPORTS)


/*
 *	Define a set of structures to hold all the board/panel/port info
 *	for our ports. These will be dynamically allocated as required.
 */

/*
 *	Define a ring queue structure for each port. This will hold the
 *	TX data waiting to be output. Characters are fed into this buffer
 *	from the line discipline (or even direct from user space!) and
 *	then fed into the UARTs during interrupts. Will use a classic ring
 *	queue here for this. The good thing about this type of ring queue
 *	is that the head and tail pointers can be updated without interrupt
 *	protection - since "write" code only needs to change the head, and
 *	interrupt code only needs to change the tail.
 */
struct stlrq {
	char	*buf;
	char	*head;
	char	*tail;
};

/*
 *	Port, panel and board structures to hold status info about each.
 *	The board structure contains pointers to structures for each panel
 *	connected to it, and in turn each panel structure contains pointers
 *	for each port structure for each port on that panel. Note that
 *	the port structure also contains the board and panel number that it
 *	is associated with, this makes it (fairly) easy to get back to the
 *	board/panel info for a port.
 */
struct stlport {
	unsigned long		magic;
	struct tty_port		port;
	unsigned int		portnr;
	unsigned int		panelnr;
	unsigned int		brdnr;
	int			ioaddr;
	int			uartaddr;
	unsigned int		pagenr;
	unsigned long		istate;
	int			baud_base;
	int			custom_divisor;
	int			close_delay;
	int			closing_wait;
	int			openwaitcnt;
	int			brklen;
	unsigned int		sigs;
	unsigned int		rxignoremsk;
	unsigned int		rxmarkmsk;
	unsigned int		imr;
	unsigned int		crenable;
	unsigned long		clk;
	unsigned long		hwid;
	void			*uartp;
	comstats_t		stats;
	struct stlrq		tx;
};

struct stlpanel {
	unsigned long	magic;
	unsigned int	panelnr;
	unsigned int	brdnr;
	unsigned int	pagenr;
	unsigned int	nrports;
	int		iobase;
	void		*uartp;
	void		(*isr)(struct stlpanel *panelp, unsigned int iobase);
	unsigned int	hwid;
	unsigned int	ackmask;
	struct stlport	*ports[STL_PORTSPERPANEL];
};

struct stlbrd {
	unsigned long	magic;
	unsigned int	brdnr;
	unsigned int	brdtype;
	unsigned int	state;
	unsigned int	nrpanels;
	unsigned int	nrports;
	unsigned int	nrbnks;
	int		irq;
	int		irqtype;
	int		(*isr)(struct stlbrd *brdp);
	unsigned int	ioaddr1;
	unsigned int	ioaddr2;
	unsigned int	iosize1;
	unsigned int	iosize2;
	unsigned int	iostatus;
	unsigned int	ioctrl;
	unsigned int	ioctrlval;
	unsigned int	hwid;
	unsigned long	clk;
	unsigned int	bnkpageaddr[STL_MAXBANKS];
	unsigned int	bnkstataddr[STL_MAXBANKS];
	struct stlpanel	*bnk2panel[STL_MAXBANKS];
	struct stlpanel	*panels[STL_MAXPANELS];
};


/*
 *	Define MAGIC numbers used for above structures.
 */
#define	STL_PORTMAGIC	0x5a7182c9
#define	STL_PANELMAGIC	0x7ef621a1
#define	STL_BOARDMAGIC	0xa2267f52

/*****************************************************************************/
#endif
