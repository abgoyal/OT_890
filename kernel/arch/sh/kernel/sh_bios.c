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
 *  linux/arch/sh/kernel/sh_bios.c
 *  C interface for trapping into the standard LinuxSH BIOS.
 *
 *  Copyright (C) 2000 Greg Banks, Mitch Davis
 *
 */
#include <linux/module.h>
#include <asm/sh_bios.h>

#define BIOS_CALL_CONSOLE_WRITE		0
#define BIOS_CALL_ETH_NODE_ADDR		10
#define BIOS_CALL_SHUTDOWN		11
#define BIOS_CALL_CHAR_OUT		0x1f	/* TODO: hack */
#define BIOS_CALL_GDB_DETACH		0xff

static inline long sh_bios_call(long func, long arg0, long arg1, long arg2,
				    long arg3)
{
	register long r0 __asm__("r0") = func;
	register long r4 __asm__("r4") = arg0;
	register long r5 __asm__("r5") = arg1;
	register long r6 __asm__("r6") = arg2;
	register long r7 __asm__("r7") = arg3;

	__asm__ __volatile__("trapa	#0x3f":"=z"(r0)
			     :"0"(r0), "r"(r4), "r"(r5), "r"(r6), "r"(r7)
			     :"memory");
	return r0;
}

void sh_bios_console_write(const char *buf, unsigned int len)
{
	sh_bios_call(BIOS_CALL_CONSOLE_WRITE, (long)buf, (long)len, 0, 0);
}

void sh_bios_char_out(char ch)
{
	sh_bios_call(BIOS_CALL_CHAR_OUT, ch, 0, 0, 0);
}

void sh_bios_gdb_detach(void)
{
	sh_bios_call(BIOS_CALL_GDB_DETACH, 0, 0, 0, 0);
}
EXPORT_SYMBOL_GPL(sh_bios_gdb_detach);

void sh_bios_get_node_addr(unsigned char *node_addr)
{
	sh_bios_call(BIOS_CALL_ETH_NODE_ADDR, 0, (long)node_addr, 0, 0);
}
EXPORT_SYMBOL_GPL(sh_bios_get_node_addr);

void sh_bios_shutdown(unsigned int how)
{
	sh_bios_call(BIOS_CALL_SHUTDOWN, how, 0, 0, 0);
}
