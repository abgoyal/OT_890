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
 * IrDA netlink layer, for stack configuration.
 *
 * Copyright (c) 2007 Samuel Ortiz <samuel@sortiz.org>
 *
 * Partly based on the 802.11 nelink implementation
 * (see net/wireless/nl80211.c) which is:
 * Copyright 2006 Johannes Berg <johannes@sipsolutions.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/socket.h>
#include <linux/irda.h>
#include <net/net_namespace.h>
#include <net/sock.h>
#include <net/irda/irda.h>
#include <net/irda/irlap.h>
#include <net/genetlink.h>



static struct genl_family irda_nl_family = {
	.id = GENL_ID_GENERATE,
	.name = IRDA_NL_NAME,
	.hdrsize = 0,
	.version = IRDA_NL_VERSION,
	.maxattr = IRDA_NL_CMD_MAX,
};

static struct net_device * ifname_to_netdev(struct net *net, struct genl_info *info)
{
	char * ifname;

	if (!info->attrs[IRDA_NL_ATTR_IFNAME])
		return NULL;

	ifname = nla_data(info->attrs[IRDA_NL_ATTR_IFNAME]);

	IRDA_DEBUG(5, "%s(): Looking for %s\n", __func__, ifname);

	return dev_get_by_name(net, ifname);
}

static int irda_nl_set_mode(struct sk_buff *skb, struct genl_info *info)
{
	struct net_device * dev;
	struct irlap_cb * irlap;
	u32 mode;

	if (!info->attrs[IRDA_NL_ATTR_MODE])
		return -EINVAL;

	mode = nla_get_u32(info->attrs[IRDA_NL_ATTR_MODE]);

	IRDA_DEBUG(5, "%s(): Switching to mode: %d\n", __func__, mode);

	dev = ifname_to_netdev(&init_net, info);
	if (!dev)
		return -ENODEV;

	irlap = (struct irlap_cb *)dev->atalk_ptr;
	if (!irlap) {
		dev_put(dev);
		return -ENODEV;
	}

	irlap->mode = mode;

	dev_put(dev);

	return 0;
}

static int irda_nl_get_mode(struct sk_buff *skb, struct genl_info *info)
{
	struct net_device * dev;
	struct irlap_cb * irlap;
	struct sk_buff *msg;
	void *hdr;
	int ret = -ENOBUFS;

	dev = ifname_to_netdev(&init_net, info);
	if (!dev)
		return -ENODEV;

	msg = nlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!msg) {
		dev_put(dev);
		return -ENOMEM;
	}

	irlap = (struct irlap_cb *)dev->atalk_ptr;
	if (!irlap) {
		ret = -ENODEV;
		goto err_out;
	}

	hdr = genlmsg_put(msg, info->snd_pid, info->snd_seq,
			  &irda_nl_family, 0,  IRDA_NL_CMD_GET_MODE);
	if (hdr == NULL) {
		ret = -EMSGSIZE;
		goto err_out;
	}

	if(nla_put_string(msg, IRDA_NL_ATTR_IFNAME,
			  dev->name))
		goto err_out;

	if(nla_put_u32(msg, IRDA_NL_ATTR_MODE, irlap->mode))
		goto err_out;

	genlmsg_end(msg, hdr);

	return genlmsg_unicast(msg, info->snd_pid);

 err_out:
	nlmsg_free(msg);
	dev_put(dev);

	return ret;
}

static struct nla_policy irda_nl_policy[IRDA_NL_ATTR_MAX + 1] = {
	[IRDA_NL_ATTR_IFNAME] = { .type = NLA_NUL_STRING,
				  .len = IFNAMSIZ-1 },
	[IRDA_NL_ATTR_MODE] = { .type = NLA_U32 },
};

static struct genl_ops irda_nl_ops[] = {
	{
		.cmd = IRDA_NL_CMD_SET_MODE,
		.doit = irda_nl_set_mode,
		.policy = irda_nl_policy,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = IRDA_NL_CMD_GET_MODE,
		.doit = irda_nl_get_mode,
		.policy = irda_nl_policy,
		/* can be retrieved by unprivileged users */
	},

};

int irda_nl_register(void)
{
	int err, i;

	err = genl_register_family(&irda_nl_family);
	if (err)
		return err;

	for (i = 0; i < ARRAY_SIZE(irda_nl_ops); i++) {
		err = genl_register_ops(&irda_nl_family, &irda_nl_ops[i]);
		if (err)
			goto err_out;
	}
	return 0;
 err_out:
	genl_unregister_family(&irda_nl_family);
	return err;
}

void irda_nl_unregister(void)
{
	genl_unregister_family(&irda_nl_family);
}
