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
 * SELinux NetLabel Support
 *
 * This file provides the necessary glue to tie NetLabel into the SELinux
 * subsystem.
 *
 * Author: Paul Moore <paul.moore@hp.com>
 *
 */

/*
 * (c) Copyright Hewlett-Packard Development Company, L.P., 2007, 2008
 *
 * This program is free software;  you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY;  without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;  if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <linux/spinlock.h>
#include <linux/rcupdate.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <net/sock.h>
#include <net/netlabel.h>
#include <net/ip.h>
#include <net/ipv6.h>

#include "objsec.h"
#include "security.h"
#include "netlabel.h"

/**
 * selinux_netlbl_sidlookup_cached - Cache a SID lookup
 * @skb: the packet
 * @secattr: the NetLabel security attributes
 * @sid: the SID
 *
 * Description:
 * Query the SELinux security server to lookup the correct SID for the given
 * security attributes.  If the query is successful, cache the result to speed
 * up future lookups.  Returns zero on success, negative values on failure.
 *
 */
static int selinux_netlbl_sidlookup_cached(struct sk_buff *skb,
					   struct netlbl_lsm_secattr *secattr,
					   u32 *sid)
{
	int rc;

	rc = security_netlbl_secattr_to_sid(secattr, sid);
	if (rc == 0 &&
	    (secattr->flags & NETLBL_SECATTR_CACHEABLE) &&
	    (secattr->flags & NETLBL_SECATTR_CACHE))
		netlbl_cache_add(skb, secattr);

	return rc;
}

/**
 * selinux_netlbl_sock_genattr - Generate the NetLabel socket secattr
 * @sk: the socket
 *
 * Description:
 * Generate the NetLabel security attributes for a socket, making full use of
 * the socket's attribute cache.  Returns a pointer to the security attributes
 * on success, NULL on failure.
 *
 */
static struct netlbl_lsm_secattr *selinux_netlbl_sock_genattr(struct sock *sk)
{
	int rc;
	struct sk_security_struct *sksec = sk->sk_security;
	struct netlbl_lsm_secattr *secattr;

	if (sksec->nlbl_secattr != NULL)
		return sksec->nlbl_secattr;

	secattr = netlbl_secattr_alloc(GFP_ATOMIC);
	if (secattr == NULL)
		return NULL;
	rc = security_netlbl_sid_to_secattr(sksec->sid, secattr);
	if (rc != 0) {
		netlbl_secattr_free(secattr);
		return NULL;
	}
	sksec->nlbl_secattr = secattr;

	return secattr;
}

/**
 * selinux_netlbl_sock_setsid - Label a socket using the NetLabel mechanism
 * @sk: the socket to label
 *
 * Description:
 * Attempt to label a socket using the NetLabel mechanism.  Returns zero values
 * on success, negative values on failure.
 *
 */
static int selinux_netlbl_sock_setsid(struct sock *sk)
{
	int rc;
	struct sk_security_struct *sksec = sk->sk_security;
	struct netlbl_lsm_secattr *secattr;

	if (sksec->nlbl_state != NLBL_REQUIRE)
		return 0;

	secattr = selinux_netlbl_sock_genattr(sk);
	if (secattr == NULL)
		return -ENOMEM;
	rc = netlbl_sock_setattr(sk, secattr);
	switch (rc) {
	case 0:
		sksec->nlbl_state = NLBL_LABELED;
		break;
	case -EDESTADDRREQ:
		sksec->nlbl_state = NLBL_REQSKB;
		rc = 0;
		break;
	}

	return rc;
}

/**
 * selinux_netlbl_cache_invalidate - Invalidate the NetLabel cache
 *
 * Description:
 * Invalidate the NetLabel security attribute mapping cache.
 *
 */
void selinux_netlbl_cache_invalidate(void)
{
	netlbl_cache_invalidate();
}

/**
 * selinux_netlbl_err - Handle a NetLabel packet error
 * @skb: the packet
 * @error: the error code
 * @gateway: true if host is acting as a gateway, false otherwise
 *
 * Description:
 * When a packet is dropped due to a call to avc_has_perm() pass the error
 * code to the NetLabel subsystem so any protocol specific processing can be
 * done.  This is safe to call even if you are unsure if NetLabel labeling is
 * present on the packet, NetLabel is smart enough to only act when it should.
 *
 */
void selinux_netlbl_err(struct sk_buff *skb, int error, int gateway)
{
	netlbl_skbuff_err(skb, error, gateway);
}

/**
 * selinux_netlbl_sk_security_free - Free the NetLabel fields
 * @sssec: the sk_security_struct
 *
 * Description:
 * Free all of the memory in the NetLabel fields of a sk_security_struct.
 *
 */
void selinux_netlbl_sk_security_free(struct sk_security_struct *ssec)
{
	if (ssec->nlbl_secattr != NULL)
		netlbl_secattr_free(ssec->nlbl_secattr);
}

/**
 * selinux_netlbl_sk_security_reset - Reset the NetLabel fields
 * @ssec: the sk_security_struct
 * @family: the socket family
 *
 * Description:
 * Called when the NetLabel state of a sk_security_struct needs to be reset.
 * The caller is responsibile for all the NetLabel sk_security_struct locking.
 *
 */
void selinux_netlbl_sk_security_reset(struct sk_security_struct *ssec,
				      int family)
{
	if (family == PF_INET)
		ssec->nlbl_state = NLBL_REQUIRE;
	else
		ssec->nlbl_state = NLBL_UNSET;
}

/**
 * selinux_netlbl_skbuff_getsid - Get the sid of a packet using NetLabel
 * @skb: the packet
 * @family: protocol family
 * @type: NetLabel labeling protocol type
 * @sid: the SID
 *
 * Description:
 * Call the NetLabel mechanism to get the security attributes of the given
 * packet and use those attributes to determine the correct context/SID to
 * assign to the packet.  Returns zero on success, negative values on failure.
 *
 */
int selinux_netlbl_skbuff_getsid(struct sk_buff *skb,
				 u16 family,
				 u32 *type,
				 u32 *sid)
{
	int rc;
	struct netlbl_lsm_secattr secattr;

	if (!netlbl_enabled()) {
		*sid = SECSID_NULL;
		return 0;
	}

	netlbl_secattr_init(&secattr);
	rc = netlbl_skbuff_getattr(skb, family, &secattr);
	if (rc == 0 && secattr.flags != NETLBL_SECATTR_NONE)
		rc = selinux_netlbl_sidlookup_cached(skb, &secattr, sid);
	else
		*sid = SECSID_NULL;
	*type = secattr.type;
	netlbl_secattr_destroy(&secattr);

	return rc;
}

/**
 * selinux_netlbl_skbuff_setsid - Set the NetLabel on a packet given a sid
 * @skb: the packet
 * @family: protocol family
 * @sid: the SID
 *
 * Description
 * Call the NetLabel mechanism to set the label of a packet using @sid.
 * Returns zero on auccess, negative values on failure.
 *
 */
int selinux_netlbl_skbuff_setsid(struct sk_buff *skb,
				 u16 family,
				 u32 sid)
{
	int rc;
	struct netlbl_lsm_secattr secattr_storage;
	struct netlbl_lsm_secattr *secattr = NULL;
	struct sock *sk;

	/* if this is a locally generated packet check to see if it is already
	 * being labeled by it's parent socket, if it is just exit */
	sk = skb->sk;
	if (sk != NULL) {
		struct sk_security_struct *sksec = sk->sk_security;
		if (sksec->nlbl_state != NLBL_REQSKB)
			return 0;
		secattr = sksec->nlbl_secattr;
	}
	if (secattr == NULL) {
		secattr = &secattr_storage;
		netlbl_secattr_init(secattr);
		rc = security_netlbl_sid_to_secattr(sid, secattr);
		if (rc != 0)
			goto skbuff_setsid_return;
	}

	rc = netlbl_skbuff_setattr(skb, family, secattr);

skbuff_setsid_return:
	if (secattr == &secattr_storage)
		netlbl_secattr_destroy(secattr);
	return rc;
}

/**
 * selinux_netlbl_inet_conn_established - Netlabel the newly accepted connection
 * @sk: the new connection
 *
 * Description:
 * A new connection has been established on @sk so make sure it is labeled
 * correctly with the NetLabel susbsystem.
 *
 */
void selinux_netlbl_inet_conn_established(struct sock *sk, u16 family)
{
	int rc;
	struct sk_security_struct *sksec = sk->sk_security;
	struct netlbl_lsm_secattr *secattr;
	struct inet_sock *sk_inet = inet_sk(sk);
	struct sockaddr_in addr;

	if (sksec->nlbl_state != NLBL_REQUIRE)
		return;

	secattr = selinux_netlbl_sock_genattr(sk);
	if (secattr == NULL)
		return;

	rc = netlbl_sock_setattr(sk, secattr);
	switch (rc) {
	case 0:
		sksec->nlbl_state = NLBL_LABELED;
		break;
	case -EDESTADDRREQ:
		/* no PF_INET6 support yet because we don't support any IPv6
		 * labeling protocols */
		if (family != PF_INET) {
			sksec->nlbl_state = NLBL_UNSET;
			return;
		}

		addr.sin_family = family;
		addr.sin_addr.s_addr = sk_inet->daddr;
		if (netlbl_conn_setattr(sk, (struct sockaddr *)&addr,
					secattr) != 0) {
			/* we failed to label the connected socket (could be
			 * for a variety of reasons, the actual "why" isn't
			 * important here) so we have to go to our backup plan,
			 * labeling the packets individually in the netfilter
			 * local output hook.  this is okay but we need to
			 * adjust the MSS of the connection to take into
			 * account any labeling overhead, since we don't know
			 * the exact overhead at this point we'll use the worst
			 * case value which is 40 bytes for IPv4 */
			struct inet_connection_sock *sk_conn = inet_csk(sk);
			sk_conn->icsk_ext_hdr_len += 40 -
				      (sk_inet->opt ? sk_inet->opt->optlen : 0);
			sk_conn->icsk_sync_mss(sk, sk_conn->icsk_pmtu_cookie);

			sksec->nlbl_state = NLBL_REQSKB;
		} else
			sksec->nlbl_state = NLBL_CONNLABELED;
		break;
	default:
		/* note that we are failing to label the socket which could be
		 * a bad thing since it means traffic could leave the system
		 * without the desired labeling, however, all is not lost as
		 * we have a check in selinux_netlbl_inode_permission() to
		 * pick up the pieces that we might drop here because we can't
		 * return an error code */
		break;
	}
}

/**
 * selinux_netlbl_socket_post_create - Label a socket using NetLabel
 * @sock: the socket to label
 *
 * Description:
 * Attempt to label a socket using the NetLabel mechanism using the given
 * SID.  Returns zero values on success, negative values on failure.
 *
 */
int selinux_netlbl_socket_post_create(struct socket *sock)
{
	return selinux_netlbl_sock_setsid(sock->sk);
}

/**
 * selinux_netlbl_inode_permission - Verify the socket is NetLabel labeled
 * @inode: the file descriptor's inode
 * @mask: the permission mask
 *
 * Description:
 * Looks at a file's inode and if it is marked as a socket protected by
 * NetLabel then verify that the socket has been labeled, if not try to label
 * the socket now with the inode's SID.  Returns zero on success, negative
 * values on failure.
 *
 */
int selinux_netlbl_inode_permission(struct inode *inode, int mask)
{
	int rc;
	struct sock *sk;
	struct socket *sock;
	struct sk_security_struct *sksec;

	if (!S_ISSOCK(inode->i_mode) ||
	    ((mask & (MAY_WRITE | MAY_APPEND)) == 0))
		return 0;
	sock = SOCKET_I(inode);
	sk = sock->sk;
	if (sk == NULL)
		return 0;
	sksec = sk->sk_security;
	if (sksec == NULL || sksec->nlbl_state != NLBL_REQUIRE)
		return 0;

	local_bh_disable();
	bh_lock_sock_nested(sk);
	if (likely(sksec->nlbl_state == NLBL_REQUIRE))
		rc = selinux_netlbl_sock_setsid(sk);
	else
		rc = 0;
	bh_unlock_sock(sk);
	local_bh_enable();

	return rc;
}

/**
 * selinux_netlbl_sock_rcv_skb - Do an inbound access check using NetLabel
 * @sksec: the sock's sk_security_struct
 * @skb: the packet
 * @family: protocol family
 * @ad: the audit data
 *
 * Description:
 * Fetch the NetLabel security attributes from @skb and perform an access check
 * against the receiving socket.  Returns zero on success, negative values on
 * error.
 *
 */
int selinux_netlbl_sock_rcv_skb(struct sk_security_struct *sksec,
				struct sk_buff *skb,
				u16 family,
				struct avc_audit_data *ad)
{
	int rc;
	u32 nlbl_sid;
	u32 perm;
	struct netlbl_lsm_secattr secattr;

	if (!netlbl_enabled())
		return 0;

	netlbl_secattr_init(&secattr);
	rc = netlbl_skbuff_getattr(skb, family, &secattr);
	if (rc == 0 && secattr.flags != NETLBL_SECATTR_NONE)
		rc = selinux_netlbl_sidlookup_cached(skb, &secattr, &nlbl_sid);
	else
		nlbl_sid = SECINITSID_UNLABELED;
	netlbl_secattr_destroy(&secattr);
	if (rc != 0)
		return rc;

	switch (sksec->sclass) {
	case SECCLASS_UDP_SOCKET:
		perm = UDP_SOCKET__RECVFROM;
		break;
	case SECCLASS_TCP_SOCKET:
		perm = TCP_SOCKET__RECVFROM;
		break;
	default:
		perm = RAWIP_SOCKET__RECVFROM;
	}

	rc = avc_has_perm(sksec->sid, nlbl_sid, sksec->sclass, perm, ad);
	if (rc == 0)
		return 0;

	if (nlbl_sid != SECINITSID_UNLABELED)
		netlbl_skbuff_err(skb, rc, 0);
	return rc;
}

/**
 * selinux_netlbl_socket_setsockopt - Do not allow users to remove a NetLabel
 * @sock: the socket
 * @level: the socket level or protocol
 * @optname: the socket option name
 *
 * Description:
 * Check the setsockopt() call and if the user is trying to replace the IP
 * options on a socket and a NetLabel is in place for the socket deny the
 * access; otherwise allow the access.  Returns zero when the access is
 * allowed, -EACCES when denied, and other negative values on error.
 *
 */
int selinux_netlbl_socket_setsockopt(struct socket *sock,
				     int level,
				     int optname)
{
	int rc = 0;
	struct sock *sk = sock->sk;
	struct sk_security_struct *sksec = sk->sk_security;
	struct netlbl_lsm_secattr secattr;

	if (level == IPPROTO_IP && optname == IP_OPTIONS &&
	    (sksec->nlbl_state == NLBL_LABELED ||
	     sksec->nlbl_state == NLBL_CONNLABELED)) {
		netlbl_secattr_init(&secattr);
		lock_sock(sk);
		rc = netlbl_sock_getattr(sk, &secattr);
		release_sock(sk);
		if (rc == 0)
			rc = -EACCES;
		else if (rc == -ENOMSG)
			rc = 0;
		netlbl_secattr_destroy(&secattr);
	}

	return rc;
}

/**
 * selinux_netlbl_socket_connect - Label a client-side socket on connect
 * @sk: the socket to label
 * @addr: the destination address
 *
 * Description:
 * Attempt to label a connected socket with NetLabel using the given address.
 * Returns zero values on success, negative values on failure.
 *
 */
int selinux_netlbl_socket_connect(struct sock *sk, struct sockaddr *addr)
{
	int rc;
	struct sk_security_struct *sksec = sk->sk_security;
	struct netlbl_lsm_secattr *secattr;

	if (sksec->nlbl_state != NLBL_REQSKB &&
	    sksec->nlbl_state != NLBL_CONNLABELED)
		return 0;

	local_bh_disable();
	bh_lock_sock_nested(sk);

	/* connected sockets are allowed to disconnect when the address family
	 * is set to AF_UNSPEC, if that is what is happening we want to reset
	 * the socket */
	if (addr->sa_family == AF_UNSPEC) {
		netlbl_sock_delattr(sk);
		sksec->nlbl_state = NLBL_REQSKB;
		rc = 0;
		goto socket_connect_return;
	}
	secattr = selinux_netlbl_sock_genattr(sk);
	if (secattr == NULL) {
		rc = -ENOMEM;
		goto socket_connect_return;
	}
	rc = netlbl_conn_setattr(sk, addr, secattr);
	if (rc == 0)
		sksec->nlbl_state = NLBL_CONNLABELED;

socket_connect_return:
	bh_unlock_sock(sk);
	local_bh_enable();
	return rc;
}
