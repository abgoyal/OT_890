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
 * linux/fs/nfsd/nfscache.c
 *
 * Request reply cache. This is currently a global cache, but this may
 * change in the future and be a per-client cache.
 *
 * This code is heavily inspired by the 44BSD implementation, although
 * it does things a bit differently.
 *
 * Copyright (C) 1995, 1996 Olaf Kirch <okir@monad.swb.de>
 */

#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/spinlock.h>
#include <linux/list.h>

#include <linux/sunrpc/svc.h>
#include <linux/nfsd/nfsd.h>
#include <linux/nfsd/cache.h>

/* Size of reply cache. Common values are:
 * 4.3BSD:	128
 * 4.4BSD:	256
 * Solaris2:	1024
 * DEC Unix:	512-4096
 */
#define CACHESIZE		1024
#define HASHSIZE		64
#define REQHASH(xid)		(((((__force __u32)xid) >> 24) ^ ((__force __u32)xid)) & (HASHSIZE-1))

static struct hlist_head *	hash_list;
static struct list_head 	lru_head;
static int			cache_disabled = 1;

static int	nfsd_cache_append(struct svc_rqst *rqstp, struct kvec *vec);

/* 
 * locking for the reply cache:
 * A cache entry is "single use" if c_state == RC_INPROG
 * Otherwise, it when accessing _prev or _next, the lock must be held.
 */
static DEFINE_SPINLOCK(cache_lock);

int nfsd_reply_cache_init(void)
{
	struct svc_cacherep	*rp;
	int			i;

	INIT_LIST_HEAD(&lru_head);
	i = CACHESIZE;
	while (i) {
		rp = kmalloc(sizeof(*rp), GFP_KERNEL);
		if (!rp)
			goto out_nomem;
		list_add(&rp->c_lru, &lru_head);
		rp->c_state = RC_UNUSED;
		rp->c_type = RC_NOCACHE;
		INIT_HLIST_NODE(&rp->c_hash);
		i--;
	}

	hash_list = kcalloc (HASHSIZE, sizeof(struct hlist_head), GFP_KERNEL);
	if (!hash_list)
		goto out_nomem;

	cache_disabled = 0;
	return 0;
out_nomem:
	printk(KERN_ERR "nfsd: failed to allocate reply cache\n");
	nfsd_reply_cache_shutdown();
	return -ENOMEM;
}

void nfsd_reply_cache_shutdown(void)
{
	struct svc_cacherep	*rp;

	while (!list_empty(&lru_head)) {
		rp = list_entry(lru_head.next, struct svc_cacherep, c_lru);
		if (rp->c_state == RC_DONE && rp->c_type == RC_REPLBUFF)
			kfree(rp->c_replvec.iov_base);
		list_del(&rp->c_lru);
		kfree(rp);
	}

	cache_disabled = 1;

	kfree (hash_list);
	hash_list = NULL;
}

/*
 * Move cache entry to end of LRU list
 */
static void
lru_put_end(struct svc_cacherep *rp)
{
	list_move_tail(&rp->c_lru, &lru_head);
}

/*
 * Move a cache entry from one hash list to another
 */
static void
hash_refile(struct svc_cacherep *rp)
{
	hlist_del_init(&rp->c_hash);
	hlist_add_head(&rp->c_hash, hash_list + REQHASH(rp->c_xid));
}

/*
 * Try to find an entry matching the current call in the cache. When none
 * is found, we grab the oldest unlocked entry off the LRU list.
 * Note that no operation within the loop may sleep.
 */
int
nfsd_cache_lookup(struct svc_rqst *rqstp, int type)
{
	struct hlist_node	*hn;
	struct hlist_head 	*rh;
	struct svc_cacherep	*rp;
	__be32			xid = rqstp->rq_xid;
	u32			proto =  rqstp->rq_prot,
				vers = rqstp->rq_vers,
				proc = rqstp->rq_proc;
	unsigned long		age;
	int rtn;

	rqstp->rq_cacherep = NULL;
	if (cache_disabled || type == RC_NOCACHE) {
		nfsdstats.rcnocache++;
		return RC_DOIT;
	}

	spin_lock(&cache_lock);
	rtn = RC_DOIT;

	rh = &hash_list[REQHASH(xid)];
	hlist_for_each_entry(rp, hn, rh, c_hash) {
		if (rp->c_state != RC_UNUSED &&
		    xid == rp->c_xid && proc == rp->c_proc &&
		    proto == rp->c_prot && vers == rp->c_vers &&
		    time_before(jiffies, rp->c_timestamp + 120*HZ) &&
		    memcmp((char*)&rqstp->rq_addr, (char*)&rp->c_addr, sizeof(rp->c_addr))==0) {
			nfsdstats.rchits++;
			goto found_entry;
		}
	}
	nfsdstats.rcmisses++;

	/* This loop shouldn't take more than a few iterations normally */
	{
	int	safe = 0;
	list_for_each_entry(rp, &lru_head, c_lru) {
		if (rp->c_state != RC_INPROG)
			break;
		if (safe++ > CACHESIZE) {
			printk("nfsd: loop in repcache LRU list\n");
			cache_disabled = 1;
			goto out;
		}
	}
	}

	/* This should not happen */
	if (rp == NULL) {
		static int	complaints;

		printk(KERN_WARNING "nfsd: all repcache entries locked!\n");
		if (++complaints > 5) {
			printk(KERN_WARNING "nfsd: disabling repcache.\n");
			cache_disabled = 1;
		}
		goto out;
	}

	rqstp->rq_cacherep = rp;
	rp->c_state = RC_INPROG;
	rp->c_xid = xid;
	rp->c_proc = proc;
	memcpy(&rp->c_addr, svc_addr_in(rqstp), sizeof(rp->c_addr));
	rp->c_prot = proto;
	rp->c_vers = vers;
	rp->c_timestamp = jiffies;

	hash_refile(rp);

	/* release any buffer */
	if (rp->c_type == RC_REPLBUFF) {
		kfree(rp->c_replvec.iov_base);
		rp->c_replvec.iov_base = NULL;
	}
	rp->c_type = RC_NOCACHE;
 out:
	spin_unlock(&cache_lock);
	return rtn;

found_entry:
	/* We found a matching entry which is either in progress or done. */
	age = jiffies - rp->c_timestamp;
	rp->c_timestamp = jiffies;
	lru_put_end(rp);

	rtn = RC_DROPIT;
	/* Request being processed or excessive rexmits */
	if (rp->c_state == RC_INPROG || age < RC_DELAY)
		goto out;

	/* From the hall of fame of impractical attacks:
	 * Is this a user who tries to snoop on the cache? */
	rtn = RC_DOIT;
	if (!rqstp->rq_secure && rp->c_secure)
		goto out;

	/* Compose RPC reply header */
	switch (rp->c_type) {
	case RC_NOCACHE:
		break;
	case RC_REPLSTAT:
		svc_putu32(&rqstp->rq_res.head[0], rp->c_replstat);
		rtn = RC_REPLY;
		break;
	case RC_REPLBUFF:
		if (!nfsd_cache_append(rqstp, &rp->c_replvec))
			goto out;	/* should not happen */
		rtn = RC_REPLY;
		break;
	default:
		printk(KERN_WARNING "nfsd: bad repcache type %d\n", rp->c_type);
		rp->c_state = RC_UNUSED;
	}

	goto out;
}

/*
 * Update a cache entry. This is called from nfsd_dispatch when
 * the procedure has been executed and the complete reply is in
 * rqstp->rq_res.
 *
 * We're copying around data here rather than swapping buffers because
 * the toplevel loop requires max-sized buffers, which would be a waste
 * of memory for a cache with a max reply size of 100 bytes (diropokres).
 *
 * If we should start to use different types of cache entries tailored
 * specifically for attrstat and fh's, we may save even more space.
 *
 * Also note that a cachetype of RC_NOCACHE can legally be passed when
 * nfsd failed to encode a reply that otherwise would have been cached.
 * In this case, nfsd_cache_update is called with statp == NULL.
 */
void
nfsd_cache_update(struct svc_rqst *rqstp, int cachetype, __be32 *statp)
{
	struct svc_cacherep *rp;
	struct kvec	*resv = &rqstp->rq_res.head[0], *cachv;
	int		len;

	if (!(rp = rqstp->rq_cacherep) || cache_disabled)
		return;

	len = resv->iov_len - ((char*)statp - (char*)resv->iov_base);
	len >>= 2;
	
	/* Don't cache excessive amounts of data and XDR failures */
	if (!statp || len > (256 >> 2)) {
		rp->c_state = RC_UNUSED;
		return;
	}

	switch (cachetype) {
	case RC_REPLSTAT:
		if (len != 1)
			printk("nfsd: RC_REPLSTAT/reply len %d!\n",len);
		rp->c_replstat = *statp;
		break;
	case RC_REPLBUFF:
		cachv = &rp->c_replvec;
		cachv->iov_base = kmalloc(len << 2, GFP_KERNEL);
		if (!cachv->iov_base) {
			spin_lock(&cache_lock);
			rp->c_state = RC_UNUSED;
			spin_unlock(&cache_lock);
			return;
		}
		cachv->iov_len = len << 2;
		memcpy(cachv->iov_base, statp, len << 2);
		break;
	}
	spin_lock(&cache_lock);
	lru_put_end(rp);
	rp->c_secure = rqstp->rq_secure;
	rp->c_type = cachetype;
	rp->c_state = RC_DONE;
	rp->c_timestamp = jiffies;
	spin_unlock(&cache_lock);
	return;
}

/*
 * Copy cached reply to current reply buffer. Should always fit.
 * FIXME as reply is in a page, we should just attach the page, and
 * keep a refcount....
 */
static int
nfsd_cache_append(struct svc_rqst *rqstp, struct kvec *data)
{
	struct kvec	*vec = &rqstp->rq_res.head[0];

	if (vec->iov_len + data->iov_len > PAGE_SIZE) {
		printk(KERN_WARNING "nfsd: cached reply too large (%Zd).\n",
				data->iov_len);
		return 0;
	}
	memcpy((char*)vec->iov_base + vec->iov_len, data->iov_base, data->iov_len);
	vec->iov_len += data->iov_len;
	return 1;
}
