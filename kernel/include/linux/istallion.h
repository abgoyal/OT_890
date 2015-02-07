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
 *	istallion.h  -- stallion intelligent multiport serial driver.
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
#ifndef	_ISTALLION_H
#define	_ISTALLION_H
/*****************************************************************************/

/*
 *	Define important driver constants here.
 */
#define	STL_MAXBRDS		4
#define	STL_MAXPANELS		4
#define	STL_MAXPORTS		64
#define	STL_MAXCHANS		(STL_MAXPORTS + 1)
#define	STL_MAXDEVS		(STL_MAXBRDS * STL_MAXPORTS)


/*
 *	Define a set of structures to hold all the board/panel/port info
 *	for our ports. These will be dynamically allocated as required at
 *	driver initialization time.
 */

/*
 *	Port and board structures to hold status info about each object.
 *	The board structure contains pointers to structures for each port
 *	connected to it. Panels are not distinguished here, since
 *	communication with the slave board will always be on a per port
 *	basis.
 */
struct stliport {
	unsigned long		magic;
	struct tty_port		port;
	unsigned int		portnr;
	unsigned int		panelnr;
	unsigned int		brdnr;
	unsigned long		state;
	unsigned int		devnr;
	int			baud_base;
	int			custom_divisor;
	int			closing_wait;
	int			rc;
	int			argsize;
	void			*argp;
	unsigned int		rxmarkmsk;
	wait_queue_head_t	raw_wait;
	struct asysigs		asig;
	unsigned long		addr;
	unsigned long		rxoffset;
	unsigned long		txoffset;
	unsigned long		sigs;
	unsigned long		pflag;
	unsigned int		rxsize;
	unsigned int		txsize;
	unsigned char		reqbit;
	unsigned char		portidx;
	unsigned char		portbit;
};

/*
 *	Use a structure of function pointers to do board level operations.
 *	These include, enable/disable, paging shared memory, interrupting, etc.
 */
struct stlibrd {
	unsigned long	magic;
	unsigned int	brdnr;
	unsigned int	brdtype;
	unsigned int	state;
	unsigned int	nrpanels;
	unsigned int	nrports;
	unsigned int	nrdevs;
	unsigned int	iobase;
	int		iosize;
	unsigned long	memaddr;
	void		__iomem *membase;
	unsigned long	memsize;
	int		pagesize;
	int		hostoffset;
	int		slaveoffset;
	int		bitsize;
	int		enabval;
	unsigned int	panels[STL_MAXPANELS];
	int		panelids[STL_MAXPANELS];
	void		(*init)(struct stlibrd *brdp);
	void		(*enable)(struct stlibrd *brdp);
	void		(*reenable)(struct stlibrd *brdp);
	void		(*disable)(struct stlibrd *brdp);
	void		__iomem *(*getmemptr)(struct stlibrd *brdp, unsigned long offset, int line);
	void		(*intr)(struct stlibrd *brdp);
	void		(*reset)(struct stlibrd *brdp);
	struct stliport	*ports[STL_MAXPORTS];
};


/*
 *	Define MAGIC numbers used for above structures.
 */
#define	STLI_PORTMAGIC	0xe671c7a1
#define	STLI_BOARDMAGIC	0x4bc6c825

/*****************************************************************************/
#endif
