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

#ifndef _NFNETLINK_LOG_H
#define _NFNETLINK_LOG_H

/* This file describes the netlink messages (i.e. 'protocol packets'),
 * and not any kind of function definitions.  It is shared between kernel and
 * userspace.  Don't put kernel specific stuff in here */

#include <linux/types.h>
#include <linux/netfilter/nfnetlink.h>

enum nfulnl_msg_types {
	NFULNL_MSG_PACKET,		/* packet from kernel to userspace */
	NFULNL_MSG_CONFIG,		/* connect to a particular queue */

	NFULNL_MSG_MAX
};

struct nfulnl_msg_packet_hdr {
	__be16		hw_protocol;	/* hw protocol (network order) */
	u_int8_t	hook;		/* netfilter hook */
	u_int8_t	_pad;
};

struct nfulnl_msg_packet_hw {
	__be16		hw_addrlen;
	u_int16_t	_pad;
	u_int8_t	hw_addr[8];
};

struct nfulnl_msg_packet_timestamp {
	aligned_be64	sec;
	aligned_be64	usec;
};

enum nfulnl_attr_type {
	NFULA_UNSPEC,
	NFULA_PACKET_HDR,
	NFULA_MARK,			/* u_int32_t nfmark */
	NFULA_TIMESTAMP,		/* nfulnl_msg_packet_timestamp */
	NFULA_IFINDEX_INDEV,		/* u_int32_t ifindex */
	NFULA_IFINDEX_OUTDEV,		/* u_int32_t ifindex */
	NFULA_IFINDEX_PHYSINDEV,	/* u_int32_t ifindex */
	NFULA_IFINDEX_PHYSOUTDEV,	/* u_int32_t ifindex */
	NFULA_HWADDR,			/* nfulnl_msg_packet_hw */
	NFULA_PAYLOAD,			/* opaque data payload */
	NFULA_PREFIX,			/* string prefix */
	NFULA_UID,			/* user id of socket */
	NFULA_SEQ,			/* instance-local sequence number */
	NFULA_SEQ_GLOBAL,		/* global sequence number */
	NFULA_GID,			/* group id of socket */
	NFULA_HWTYPE,			/* hardware type */
	NFULA_HWHEADER,			/* hardware header */
	NFULA_HWLEN,			/* hardware header length */

	__NFULA_MAX
};
#define NFULA_MAX (__NFULA_MAX - 1)

enum nfulnl_msg_config_cmds {
	NFULNL_CFG_CMD_NONE,
	NFULNL_CFG_CMD_BIND,
	NFULNL_CFG_CMD_UNBIND,
	NFULNL_CFG_CMD_PF_BIND,
	NFULNL_CFG_CMD_PF_UNBIND,
};

struct nfulnl_msg_config_cmd {
	u_int8_t	command;	/* nfulnl_msg_config_cmds */
} __attribute__ ((packed));

struct nfulnl_msg_config_mode {
	__be32		copy_range;
	u_int8_t	copy_mode;
	u_int8_t	_pad;
} __attribute__ ((packed));

enum nfulnl_attr_config {
	NFULA_CFG_UNSPEC,
	NFULA_CFG_CMD,			/* nfulnl_msg_config_cmd */
	NFULA_CFG_MODE,			/* nfulnl_msg_config_mode */
	NFULA_CFG_NLBUFSIZ,		/* u_int32_t buffer size */
	NFULA_CFG_TIMEOUT,		/* u_int32_t in 1/100 s */
	NFULA_CFG_QTHRESH,		/* u_int32_t */
	NFULA_CFG_FLAGS,		/* u_int16_t */
	__NFULA_CFG_MAX
};
#define NFULA_CFG_MAX (__NFULA_CFG_MAX -1)

#define NFULNL_COPY_NONE	0x00
#define NFULNL_COPY_META	0x01
#define NFULNL_COPY_PACKET	0x02

#define NFULNL_CFG_F_SEQ	0x0001
#define NFULNL_CFG_F_SEQ_GLOBAL	0x0002

#endif /* _NFNETLINK_LOG_H */
