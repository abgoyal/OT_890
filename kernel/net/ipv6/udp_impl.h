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

#ifndef _UDP6_IMPL_H
#define _UDP6_IMPL_H
#include <net/udp.h>
#include <net/udplite.h>
#include <net/protocol.h>
#include <net/addrconf.h>
#include <net/inet_common.h>
#include <net/transp_v6.h>

extern int  	__udp6_lib_rcv(struct sk_buff *, struct udp_table *, int );
extern void 	__udp6_lib_err(struct sk_buff *, struct inet6_skb_parm *,
			       int , int , int , __be32 , struct udp_table *);

extern int	udp_v6_get_port(struct sock *sk, unsigned short snum);

extern int	udpv6_getsockopt(struct sock *sk, int level, int optname,
				 char __user *optval, int __user *optlen);
extern int	udpv6_setsockopt(struct sock *sk, int level, int optname,
				 char __user *optval, int optlen);
#ifdef CONFIG_COMPAT
extern int	compat_udpv6_setsockopt(struct sock *sk, int level, int optname,
					char __user *optval, int optlen);
extern int	compat_udpv6_getsockopt(struct sock *sk, int level, int optname,
				       char __user *optval, int __user *optlen);
#endif
extern int	udpv6_sendmsg(struct kiocb *iocb, struct sock *sk,
			      struct msghdr *msg, size_t len);
extern int	udpv6_recvmsg(struct kiocb *iocb, struct sock *sk,
			      struct msghdr *msg, size_t len,
			      int noblock, int flags, int *addr_len);
extern int	udpv6_queue_rcv_skb(struct sock * sk, struct sk_buff *skb);
extern void	udpv6_destroy_sock(struct sock *sk);

#ifdef CONFIG_PROC_FS
extern int	udp6_seq_show(struct seq_file *seq, void *v);
#endif
#endif	/* _UDP6_IMPL_H */
