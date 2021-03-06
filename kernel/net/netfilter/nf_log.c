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

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/seq_file.h>
#include <net/protocol.h>
#include <net/netfilter/nf_log.h>

#include "nf_internals.h"

/* Internal logging interface, which relies on the real
   LOG target modules */

#define NF_LOG_PREFIXLEN		128

static const struct nf_logger *nf_loggers[NFPROTO_NUMPROTO] __read_mostly;
static DEFINE_MUTEX(nf_log_mutex);

/* return EBUSY if somebody else is registered, EEXIST if the same logger
 * is registred, 0 on success. */
int nf_log_register(u_int8_t pf, const struct nf_logger *logger)
{
	int ret;

	if (pf >= ARRAY_SIZE(nf_loggers))
		return -EINVAL;

	/* Any setup of logging members must be done before
	 * substituting pointer. */
	ret = mutex_lock_interruptible(&nf_log_mutex);
	if (ret < 0)
		return ret;

	if (!nf_loggers[pf])
		rcu_assign_pointer(nf_loggers[pf], logger);
	else if (nf_loggers[pf] == logger)
		ret = -EEXIST;
	else
		ret = -EBUSY;

	mutex_unlock(&nf_log_mutex);
	return ret;
}
EXPORT_SYMBOL(nf_log_register);

void nf_log_unregister_pf(u_int8_t pf)
{
	if (pf >= ARRAY_SIZE(nf_loggers))
		return;
	mutex_lock(&nf_log_mutex);
	rcu_assign_pointer(nf_loggers[pf], NULL);
	mutex_unlock(&nf_log_mutex);

	/* Give time to concurrent readers. */
	synchronize_rcu();
}
EXPORT_SYMBOL(nf_log_unregister_pf);

void nf_log_unregister(const struct nf_logger *logger)
{
	int i;

	mutex_lock(&nf_log_mutex);
	for (i = 0; i < ARRAY_SIZE(nf_loggers); i++) {
		if (nf_loggers[i] == logger)
			rcu_assign_pointer(nf_loggers[i], NULL);
	}
	mutex_unlock(&nf_log_mutex);

	synchronize_rcu();
}
EXPORT_SYMBOL(nf_log_unregister);

void nf_log_packet(u_int8_t pf,
		   unsigned int hooknum,
		   const struct sk_buff *skb,
		   const struct net_device *in,
		   const struct net_device *out,
		   const struct nf_loginfo *loginfo,
		   const char *fmt, ...)
{
	va_list args;
	char prefix[NF_LOG_PREFIXLEN];
	const struct nf_logger *logger;

	rcu_read_lock();
	logger = rcu_dereference(nf_loggers[pf]);
	if (logger) {
		va_start(args, fmt);
		vsnprintf(prefix, sizeof(prefix), fmt, args);
		va_end(args);
		logger->logfn(pf, hooknum, skb, in, out, loginfo, prefix);
	}
	rcu_read_unlock();
}
EXPORT_SYMBOL(nf_log_packet);

#ifdef CONFIG_PROC_FS
static void *seq_start(struct seq_file *seq, loff_t *pos)
	__acquires(RCU)
{
	rcu_read_lock();

	if (*pos >= ARRAY_SIZE(nf_loggers))
		return NULL;

	return pos;
}

static void *seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	(*pos)++;

	if (*pos >= ARRAY_SIZE(nf_loggers))
		return NULL;

	return pos;
}

static void seq_stop(struct seq_file *s, void *v)
	__releases(RCU)
{
	rcu_read_unlock();
}

static int seq_show(struct seq_file *s, void *v)
{
	loff_t *pos = v;
	const struct nf_logger *logger;

	logger = rcu_dereference(nf_loggers[*pos]);

	if (!logger)
		return seq_printf(s, "%2lld NONE\n", *pos);

	return seq_printf(s, "%2lld %s\n", *pos, logger->name);
}

static const struct seq_operations nflog_seq_ops = {
	.start	= seq_start,
	.next	= seq_next,
	.stop	= seq_stop,
	.show	= seq_show,
};

static int nflog_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &nflog_seq_ops);
}

static const struct file_operations nflog_file_ops = {
	.owner	 = THIS_MODULE,
	.open	 = nflog_open,
	.read	 = seq_read,
	.llseek	 = seq_lseek,
	.release = seq_release,
};

#endif /* PROC_FS */


int __init netfilter_log_init(void)
{
#ifdef CONFIG_PROC_FS
	if (!proc_create("nf_log", S_IRUGO,
			 proc_net_netfilter, &nflog_file_ops))
		return -1;
#endif
	return 0;
}
