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
 * Copyright (c) 2004 Topspin Communications.  All rights reserved.
 * Copyright (c) 2005 Sun Microsystems, Inc. All rights reserved.
 * Copyright (c) 2005 Mellanox Technologies. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <linux/types.h>

#define MTHCA_RD_DOORBELL      0x00
#define MTHCA_SEND_DOORBELL    0x10
#define MTHCA_RECEIVE_DOORBELL 0x18
#define MTHCA_CQ_DOORBELL      0x20
#define MTHCA_EQ_DOORBELL      0x28

#if BITS_PER_LONG == 64
/*
 * Assume that we can just write a 64-bit doorbell atomically.  s390
 * actually doesn't have writeq() but S/390 systems don't even have
 * PCI so we won't worry about it.
 */

#define MTHCA_DECLARE_DOORBELL_LOCK(name)
#define MTHCA_INIT_DOORBELL_LOCK(ptr)    do { } while (0)
#define MTHCA_GET_DOORBELL_LOCK(ptr)      (NULL)

static inline void mthca_write64_raw(__be64 val, void __iomem *dest)
{
	__raw_writeq((__force u64) val, dest);
}

static inline void mthca_write64(u32 hi, u32 lo, void __iomem *dest,
				 spinlock_t *doorbell_lock)
{
	__raw_writeq((__force u64) cpu_to_be64((u64) hi << 32 | lo), dest);
}

static inline void mthca_write_db_rec(__be32 val[2], __be32 *db)
{
	*(u64 *) db = *(u64 *) val;
}

#else

/*
 * Just fall back to a spinlock to protect the doorbell if
 * BITS_PER_LONG is 32 -- there's no portable way to do atomic 64-bit
 * MMIO writes.
 */

#define MTHCA_DECLARE_DOORBELL_LOCK(name) spinlock_t name;
#define MTHCA_INIT_DOORBELL_LOCK(ptr)     spin_lock_init(ptr)
#define MTHCA_GET_DOORBELL_LOCK(ptr)      (ptr)

static inline void mthca_write64_raw(__be64 val, void __iomem *dest)
{
	__raw_writel(((__force u32 *) &val)[0], dest);
	__raw_writel(((__force u32 *) &val)[1], dest + 4);
}

static inline void mthca_write64(u32 hi, u32 lo, void __iomem *dest,
				 spinlock_t *doorbell_lock)
{
	unsigned long flags;

	hi = (__force u32) cpu_to_be32(hi);
	lo = (__force u32) cpu_to_be32(lo);

	spin_lock_irqsave(doorbell_lock, flags);
	__raw_writel(hi, dest);
	__raw_writel(lo, dest + 4);
	spin_unlock_irqrestore(doorbell_lock, flags);
}

static inline void mthca_write_db_rec(__be32 val[2], __be32 *db)
{
	db[0] = val[0];
	wmb();
	db[1] = val[1];
}

#endif
