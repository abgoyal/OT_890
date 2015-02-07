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

/* Debugging stuff for the MN10300-based processors
 *
 * Copyright (C) 2007 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version
 * 2 of the Licence, or (at your option) any later version.
 */
#include <linux/sched.h>
#include <asm/serial-regs.h>

#undef MN10300_CONSOLE_ON_SERIO

/*
 * write a string directly through one of the serial ports on-board the MN10300
 */
#ifdef MN10300_CONSOLE_ON_SERIO
void debug_to_serial_mnser(const char *p, int n)
{
	char ch;

	for (; n > 0; n--) {
		ch = *p++;

#if MN10300_CONSOLE_ON_SERIO == 0
		while (SC0STR & (SC01STR_TBF)) continue;
		SC0TXB = ch;
		while (SC0STR & (SC01STR_TBF)) continue;
		if (ch == 0x0a) {
			SC0TXB = 0x0d;
			while (SC0STR & (SC01STR_TBF)) continue;
		}

#elif MN10300_CONSOLE_ON_SERIO == 1
		while (SC1STR & (SC01STR_TBF)) continue;
		SC1TXB = ch;
		while (SC1STR & (SC01STR_TBF)) continue;
		if (ch == 0x0a) {
			SC1TXB = 0x0d;
			while (SC1STR & (SC01STR_TBF)) continue;
		}

#elif MN10300_CONSOLE_ON_SERIO == 2
		while (SC2STR & (SC2STR_TBF)) continue;
		SC2TXB = ch;
		while (SC2STR & (SC2STR_TBF)) continue;
		if (ch == 0x0a) {
			SC2TXB = 0x0d;
			while (SC2STR & (SC2STR_TBF)) continue;
		}

#endif
	}
}
#endif
