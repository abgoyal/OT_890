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

#ifndef _ASM_SOCKET_H
#define _ASM_SOCKET_H

#include <asm/sockios.h>

/* For setsockopt(2) */
/*
 * Note: we only bother about making the SOL_SOCKET options
 * same as OSF/1, as that's all that "normal" programs are
 * likely to set.  We don't necessarily want to be binary
 * compatible with _everything_. 
 */
#define SOL_SOCKET	0xffff

#define SO_DEBUG	0x0001
#define SO_REUSEADDR	0x0004
#define SO_KEEPALIVE	0x0008
#define SO_DONTROUTE	0x0010
#define SO_BROADCAST	0x0020
#define SO_LINGER	0x0080
#define SO_OOBINLINE	0x0100
/* To add :#define SO_REUSEPORT 0x0200 */

#define SO_TYPE		0x1008
#define SO_ERROR	0x1007
#define SO_SNDBUF	0x1001
#define SO_RCVBUF	0x1002
#define SO_SNDBUFFORCE	0x100a
#define SO_RCVBUFFORCE	0x100b
#define	SO_RCVLOWAT	0x1010
#define	SO_SNDLOWAT	0x1011
#define	SO_RCVTIMEO	0x1012
#define	SO_SNDTIMEO	0x1013
#define SO_ACCEPTCONN	0x1014

/* linux-specific, might as well be the same as on i386 */
#define SO_NO_CHECK	11
#define SO_PRIORITY	12
#define SO_BSDCOMPAT	14

#define SO_PASSCRED	17
#define SO_PEERCRED	18
#define SO_BINDTODEVICE 25

/* Socket filtering */
#define SO_ATTACH_FILTER        26
#define SO_DETACH_FILTER        27

#define SO_PEERNAME		28
#define SO_TIMESTAMP		29
#define SCM_TIMESTAMP		SO_TIMESTAMP

#define SO_PEERSEC		30
#define SO_PASSSEC		34
#define SO_TIMESTAMPNS		35
#define SCM_TIMESTAMPNS		SO_TIMESTAMPNS

/* Security levels - as per NRL IPv6 - don't actually do anything */
#define SO_SECURITY_AUTHENTICATION		19
#define SO_SECURITY_ENCRYPTION_TRANSPORT	20
#define SO_SECURITY_ENCRYPTION_NETWORK		21

#define SO_MARK			36

/* O_NONBLOCK clashes with the bits used for socket types.  Therefore we
 * have to define SOCK_NONBLOCK to a different value here.
 */
#define SOCK_NONBLOCK	0x40000000

#endif /* _ASM_SOCKET_H */
