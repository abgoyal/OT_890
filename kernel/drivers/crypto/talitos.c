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
 * talitos - Freescale Integrated Security Engine (SEC) device driver
 *
 * Copyright (c) 2008 Freescale Semiconductor, Inc.
 *
 * Scatterlist Crypto API glue code copied from files with the following:
 * Copyright (c) 2006-2007 Herbert Xu <herbert@gondor.apana.org.au>
 *
 * Crypto algorithm registration code copied from hifn driver:
 * 2007+ Copyright (c) Evgeniy Polyakov <johnpol@2ka.mipt.ru>
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/crypto.h>
#include <linux/hw_random.h>
#include <linux/of_platform.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <linux/spinlock.h>
#include <linux/rtnetlink.h>

#include <crypto/algapi.h>
#include <crypto/aes.h>
#include <crypto/des.h>
#include <crypto/sha.h>
#include <crypto/aead.h>
#include <crypto/authenc.h>

#include "talitos.h"

#define TALITOS_TIMEOUT 100000
#define TALITOS_MAX_DATA_LEN 65535

#define DESC_TYPE(desc_hdr) ((be32_to_cpu(desc_hdr) >> 3) & 0x1f)
#define PRIMARY_EU(desc_hdr) ((be32_to_cpu(desc_hdr) >> 28) & 0xf)
#define SECONDARY_EU(desc_hdr) ((be32_to_cpu(desc_hdr) >> 16) & 0xf)

/* descriptor pointer entry */
struct talitos_ptr {
	__be16 len;	/* length */
	u8 j_extent;	/* jump to sg link table and/or extent */
	u8 eptr;	/* extended address */
	__be32 ptr;	/* address */
};

/* descriptor */
struct talitos_desc {
	__be32 hdr;			/* header high bits */
	__be32 hdr_lo;			/* header low bits */
	struct talitos_ptr ptr[7];	/* ptr/len pair array */
};

/**
 * talitos_request - descriptor submission request
 * @desc: descriptor pointer (kernel virtual)
 * @dma_desc: descriptor's physical bus address
 * @callback: whom to call when descriptor processing is done
 * @context: caller context (optional)
 */
struct talitos_request {
	struct talitos_desc *desc;
	dma_addr_t dma_desc;
	void (*callback) (struct device *dev, struct talitos_desc *desc,
	                  void *context, int error);
	void *context;
};

struct talitos_private {
	struct device *dev;
	struct of_device *ofdev;
	void __iomem *reg;
	int irq;

	/* SEC version geometry (from device tree node) */
	unsigned int num_channels;
	unsigned int chfifo_len;
	unsigned int exec_units;
	unsigned int desc_types;

	/* SEC Compatibility info */
	unsigned long features;

	/* next channel to be assigned next incoming descriptor */
	atomic_t last_chan;

	/* per-channel number of requests pending in channel h/w fifo */
	atomic_t *submit_count;

	/* per-channel request fifo */
	struct talitos_request **fifo;

	/*
	 * length of the request fifo
	 * fifo_len is chfifo_len rounded up to next power of 2
	 * so we can use bitwise ops to wrap
	 */
	unsigned int fifo_len;

	/* per-channel index to next free descriptor request */
	int *head;

	/* per-channel index to next in-progress/done descriptor request */
	int *tail;

	/* per-channel request submission (head) and release (tail) locks */
	spinlock_t *head_lock;
	spinlock_t *tail_lock;

	/* request callback tasklet */
	struct tasklet_struct done_task;

	/* list of registered algorithms */
	struct list_head alg_list;

	/* hwrng device */
	struct hwrng rng;
};

/* .features flag */
#define TALITOS_FTR_SRC_LINK_TBL_LEN_INCLUDES_EXTENT 0x00000001
#define TALITOS_FTR_HW_AUTH_CHECK 0x00000002

/*
 * map virtual single (contiguous) pointer to h/w descriptor pointer
 */
static void map_single_talitos_ptr(struct device *dev,
				   struct talitos_ptr *talitos_ptr,
				   unsigned short len, void *data,
				   unsigned char extent,
				   enum dma_data_direction dir)
{
	talitos_ptr->len = cpu_to_be16(len);
	talitos_ptr->ptr = cpu_to_be32(dma_map_single(dev, data, len, dir));
	talitos_ptr->j_extent = extent;
}

/*
 * unmap bus single (contiguous) h/w descriptor pointer
 */
static void unmap_single_talitos_ptr(struct device *dev,
				     struct talitos_ptr *talitos_ptr,
				     enum dma_data_direction dir)
{
	dma_unmap_single(dev, be32_to_cpu(talitos_ptr->ptr),
			 be16_to_cpu(talitos_ptr->len), dir);
}

static int reset_channel(struct device *dev, int ch)
{
	struct talitos_private *priv = dev_get_drvdata(dev);
	unsigned int timeout = TALITOS_TIMEOUT;

	setbits32(priv->reg + TALITOS_CCCR(ch), TALITOS_CCCR_RESET);

	while ((in_be32(priv->reg + TALITOS_CCCR(ch)) & TALITOS_CCCR_RESET)
	       && --timeout)
		cpu_relax();

	if (timeout == 0) {
		dev_err(dev, "failed to reset channel %d\n", ch);
		return -EIO;
	}

	/* set done writeback and IRQ */
	setbits32(priv->reg + TALITOS_CCCR_LO(ch), TALITOS_CCCR_LO_CDWE |
		  TALITOS_CCCR_LO_CDIE);

	/* and ICCR writeback, if available */
	if (priv->features & TALITOS_FTR_HW_AUTH_CHECK)
		setbits32(priv->reg + TALITOS_CCCR_LO(ch),
		          TALITOS_CCCR_LO_IWSE);

	return 0;
}

static int reset_device(struct device *dev)
{
	struct talitos_private *priv = dev_get_drvdata(dev);
	unsigned int timeout = TALITOS_TIMEOUT;

	setbits32(priv->reg + TALITOS_MCR, TALITOS_MCR_SWR);

	while ((in_be32(priv->reg + TALITOS_MCR) & TALITOS_MCR_SWR)
	       && --timeout)
		cpu_relax();

	if (timeout == 0) {
		dev_err(dev, "failed to reset device\n");
		return -EIO;
	}

	return 0;
}

/*
 * Reset and initialize the device
 */
static int init_device(struct device *dev)
{
	struct talitos_private *priv = dev_get_drvdata(dev);
	int ch, err;

	/*
	 * Master reset
	 * errata documentation: warning: certain SEC interrupts
	 * are not fully cleared by writing the MCR:SWR bit,
	 * set bit twice to completely reset
	 */
	err = reset_device(dev);
	if (err)
		return err;

	err = reset_device(dev);
	if (err)
		return err;

	/* reset channels */
	for (ch = 0; ch < priv->num_channels; ch++) {
		err = reset_channel(dev, ch);
		if (err)
			return err;
	}

	/* enable channel done and error interrupts */
	setbits32(priv->reg + TALITOS_IMR, TALITOS_IMR_INIT);
	setbits32(priv->reg + TALITOS_IMR_LO, TALITOS_IMR_LO_INIT);

	/* disable integrity check error interrupts (use writeback instead) */
	if (priv->features & TALITOS_FTR_HW_AUTH_CHECK)
		setbits32(priv->reg + TALITOS_MDEUICR_LO,
		          TALITOS_MDEUICR_LO_ICE);

	return 0;
}

/**
 * talitos_submit - submits a descriptor to the device for processing
 * @dev:	the SEC device to be used
 * @desc:	the descriptor to be processed by the device
 * @callback:	whom to call when processing is complete
 * @context:	a handle for use by caller (optional)
 *
 * desc must contain valid dma-mapped (bus physical) address pointers.
 * callback must check err and feedback in descriptor header
 * for device processing status.
 */
static int talitos_submit(struct device *dev, struct talitos_desc *desc,
			  void (*callback)(struct device *dev,
					   struct talitos_desc *desc,
					   void *context, int error),
			  void *context)
{
	struct talitos_private *priv = dev_get_drvdata(dev);
	struct talitos_request *request;
	unsigned long flags, ch;
	int head;

	/* select done notification */
	desc->hdr |= DESC_HDR_DONE_NOTIFY;

	/* emulate SEC's round-robin channel fifo polling scheme */
	ch = atomic_inc_return(&priv->last_chan) & (priv->num_channels - 1);

	spin_lock_irqsave(&priv->head_lock[ch], flags);

	if (!atomic_inc_not_zero(&priv->submit_count[ch])) {
		/* h/w fifo is full */
		spin_unlock_irqrestore(&priv->head_lock[ch], flags);
		return -EAGAIN;
	}

	head = priv->head[ch];
	request = &priv->fifo[ch][head];

	/* map descriptor and save caller data */
	request->dma_desc = dma_map_single(dev, desc, sizeof(*desc),
					   DMA_BIDIRECTIONAL);
	request->callback = callback;
	request->context = context;

	/* increment fifo head */
	priv->head[ch] = (priv->head[ch] + 1) & (priv->fifo_len - 1);

	smp_wmb();
	request->desc = desc;

	/* GO! */
	wmb();
	out_be32(priv->reg + TALITOS_FF_LO(ch), request->dma_desc);

	spin_unlock_irqrestore(&priv->head_lock[ch], flags);

	return -EINPROGRESS;
}

/*
 * process what was done, notify callback of error if not
 */
static void flush_channel(struct device *dev, int ch, int error, int reset_ch)
{
	struct talitos_private *priv = dev_get_drvdata(dev);
	struct talitos_request *request, saved_req;
	unsigned long flags;
	int tail, status;

	spin_lock_irqsave(&priv->tail_lock[ch], flags);

	tail = priv->tail[ch];
	while (priv->fifo[ch][tail].desc) {
		request = &priv->fifo[ch][tail];

		/* descriptors with their done bits set don't get the error */
		rmb();
		if ((request->desc->hdr & DESC_HDR_DONE) == DESC_HDR_DONE)
			status = 0;
		else
			if (!error)
				break;
			else
				status = error;

		dma_unmap_single(dev, request->dma_desc,
			sizeof(struct talitos_desc), DMA_BIDIRECTIONAL);

		/* copy entries so we can call callback outside lock */
		saved_req.desc = request->desc;
		saved_req.callback = request->callback;
		saved_req.context = request->context;

		/* release request entry in fifo */
		smp_wmb();
		request->desc = NULL;

		/* increment fifo tail */
		priv->tail[ch] = (tail + 1) & (priv->fifo_len - 1);

		spin_unlock_irqrestore(&priv->tail_lock[ch], flags);

		atomic_dec(&priv->submit_count[ch]);

		saved_req.callback(dev, saved_req.desc, saved_req.context,
				   status);
		/* channel may resume processing in single desc error case */
		if (error && !reset_ch && status == error)
			return;
		spin_lock_irqsave(&priv->tail_lock[ch], flags);
		tail = priv->tail[ch];
	}

	spin_unlock_irqrestore(&priv->tail_lock[ch], flags);
}

/*
 * process completed requests for channels that have done status
 */
static void talitos_done(unsigned long data)
{
	struct device *dev = (struct device *)data;
	struct talitos_private *priv = dev_get_drvdata(dev);
	int ch;

	for (ch = 0; ch < priv->num_channels; ch++)
		flush_channel(dev, ch, 0, 0);

	/* At this point, all completed channels have been processed.
	 * Unmask done interrupts for channels completed later on.
	 */
	setbits32(priv->reg + TALITOS_IMR, TALITOS_IMR_INIT);
	setbits32(priv->reg + TALITOS_IMR_LO, TALITOS_IMR_LO_INIT);
}

/*
 * locate current (offending) descriptor
 */
static struct talitos_desc *current_desc(struct device *dev, int ch)
{
	struct talitos_private *priv = dev_get_drvdata(dev);
	int tail = priv->tail[ch];
	dma_addr_t cur_desc;

	cur_desc = in_be32(priv->reg + TALITOS_CDPR_LO(ch));

	while (priv->fifo[ch][tail].dma_desc != cur_desc) {
		tail = (tail + 1) & (priv->fifo_len - 1);
		if (tail == priv->tail[ch]) {
			dev_err(dev, "couldn't locate current descriptor\n");
			return NULL;
		}
	}

	return priv->fifo[ch][tail].desc;
}

/*
 * user diagnostics; report root cause of error based on execution unit status
 */
static void report_eu_error(struct device *dev, int ch, struct talitos_desc *desc)
{
	struct talitos_private *priv = dev_get_drvdata(dev);
	int i;

	switch (desc->hdr & DESC_HDR_SEL0_MASK) {
	case DESC_HDR_SEL0_AFEU:
		dev_err(dev, "AFEUISR 0x%08x_%08x\n",
			in_be32(priv->reg + TALITOS_AFEUISR),
			in_be32(priv->reg + TALITOS_AFEUISR_LO));
		break;
	case DESC_HDR_SEL0_DEU:
		dev_err(dev, "DEUISR 0x%08x_%08x\n",
			in_be32(priv->reg + TALITOS_DEUISR),
			in_be32(priv->reg + TALITOS_DEUISR_LO));
		break;
	case DESC_HDR_SEL0_MDEUA:
	case DESC_HDR_SEL0_MDEUB:
		dev_err(dev, "MDEUISR 0x%08x_%08x\n",
			in_be32(priv->reg + TALITOS_MDEUISR),
			in_be32(priv->reg + TALITOS_MDEUISR_LO));
		break;
	case DESC_HDR_SEL0_RNG:
		dev_err(dev, "RNGUISR 0x%08x_%08x\n",
			in_be32(priv->reg + TALITOS_RNGUISR),
			in_be32(priv->reg + TALITOS_RNGUISR_LO));
		break;
	case DESC_HDR_SEL0_PKEU:
		dev_err(dev, "PKEUISR 0x%08x_%08x\n",
			in_be32(priv->reg + TALITOS_PKEUISR),
			in_be32(priv->reg + TALITOS_PKEUISR_LO));
		break;
	case DESC_HDR_SEL0_AESU:
		dev_err(dev, "AESUISR 0x%08x_%08x\n",
			in_be32(priv->reg + TALITOS_AESUISR),
			in_be32(priv->reg + TALITOS_AESUISR_LO));
		break;
	case DESC_HDR_SEL0_CRCU:
		dev_err(dev, "CRCUISR 0x%08x_%08x\n",
			in_be32(priv->reg + TALITOS_CRCUISR),
			in_be32(priv->reg + TALITOS_CRCUISR_LO));
		break;
	case DESC_HDR_SEL0_KEU:
		dev_err(dev, "KEUISR 0x%08x_%08x\n",
			in_be32(priv->reg + TALITOS_KEUISR),
			in_be32(priv->reg + TALITOS_KEUISR_LO));
		break;
	}

	switch (desc->hdr & DESC_HDR_SEL1_MASK) {
	case DESC_HDR_SEL1_MDEUA:
	case DESC_HDR_SEL1_MDEUB:
		dev_err(dev, "MDEUISR 0x%08x_%08x\n",
			in_be32(priv->reg + TALITOS_MDEUISR),
			in_be32(priv->reg + TALITOS_MDEUISR_LO));
		break;
	case DESC_HDR_SEL1_CRCU:
		dev_err(dev, "CRCUISR 0x%08x_%08x\n",
			in_be32(priv->reg + TALITOS_CRCUISR),
			in_be32(priv->reg + TALITOS_CRCUISR_LO));
		break;
	}

	for (i = 0; i < 8; i++)
		dev_err(dev, "DESCBUF 0x%08x_%08x\n",
			in_be32(priv->reg + TALITOS_DESCBUF(ch) + 8*i),
			in_be32(priv->reg + TALITOS_DESCBUF_LO(ch) + 8*i));
}

/*
 * recover from error interrupts
 */
static void talitos_error(unsigned long data, u32 isr, u32 isr_lo)
{
	struct device *dev = (struct device *)data;
	struct talitos_private *priv = dev_get_drvdata(dev);
	unsigned int timeout = TALITOS_TIMEOUT;
	int ch, error, reset_dev = 0, reset_ch = 0;
	u32 v, v_lo;

	for (ch = 0; ch < priv->num_channels; ch++) {
		/* skip channels without errors */
		if (!(isr & (1 << (ch * 2 + 1))))
			continue;

		error = -EINVAL;

		v = in_be32(priv->reg + TALITOS_CCPSR(ch));
		v_lo = in_be32(priv->reg + TALITOS_CCPSR_LO(ch));

		if (v_lo & TALITOS_CCPSR_LO_DOF) {
			dev_err(dev, "double fetch fifo overflow error\n");
			error = -EAGAIN;
			reset_ch = 1;
		}
		if (v_lo & TALITOS_CCPSR_LO_SOF) {
			/* h/w dropped descriptor */
			dev_err(dev, "single fetch fifo overflow error\n");
			error = -EAGAIN;
		}
		if (v_lo & TALITOS_CCPSR_LO_MDTE)
			dev_err(dev, "master data transfer error\n");
		if (v_lo & TALITOS_CCPSR_LO_SGDLZ)
			dev_err(dev, "s/g data length zero error\n");
		if (v_lo & TALITOS_CCPSR_LO_FPZ)
			dev_err(dev, "fetch pointer zero error\n");
		if (v_lo & TALITOS_CCPSR_LO_IDH)
			dev_err(dev, "illegal descriptor header error\n");
		if (v_lo & TALITOS_CCPSR_LO_IEU)
			dev_err(dev, "invalid execution unit error\n");
		if (v_lo & TALITOS_CCPSR_LO_EU)
			report_eu_error(dev, ch, current_desc(dev, ch));
		if (v_lo & TALITOS_CCPSR_LO_GB)
			dev_err(dev, "gather boundary error\n");
		if (v_lo & TALITOS_CCPSR_LO_GRL)
			dev_err(dev, "gather return/length error\n");
		if (v_lo & TALITOS_CCPSR_LO_SB)
			dev_err(dev, "scatter boundary error\n");
		if (v_lo & TALITOS_CCPSR_LO_SRL)
			dev_err(dev, "scatter return/length error\n");

		flush_channel(dev, ch, error, reset_ch);

		if (reset_ch) {
			reset_channel(dev, ch);
		} else {
			setbits32(priv->reg + TALITOS_CCCR(ch),
				  TALITOS_CCCR_CONT);
			setbits32(priv->reg + TALITOS_CCCR_LO(ch), 0);
			while ((in_be32(priv->reg + TALITOS_CCCR(ch)) &
			       TALITOS_CCCR_CONT) && --timeout)
				cpu_relax();
			if (timeout == 0) {
				dev_err(dev, "failed to restart channel %d\n",
					ch);
				reset_dev = 1;
			}
		}
	}
	if (reset_dev || isr & ~TALITOS_ISR_CHERR || isr_lo) {
		dev_err(dev, "done overflow, internal time out, or rngu error: "
		        "ISR 0x%08x_%08x\n", isr, isr_lo);

		/* purge request queues */
		for (ch = 0; ch < priv->num_channels; ch++)
			flush_channel(dev, ch, -EIO, 1);

		/* reset and reinitialize the device */
		init_device(dev);
	}
}

static irqreturn_t talitos_interrupt(int irq, void *data)
{
	struct device *dev = data;
	struct talitos_private *priv = dev_get_drvdata(dev);
	u32 isr, isr_lo;

	isr = in_be32(priv->reg + TALITOS_ISR);
	isr_lo = in_be32(priv->reg + TALITOS_ISR_LO);
	/* Acknowledge interrupt */
	out_be32(priv->reg + TALITOS_ICR, isr);
	out_be32(priv->reg + TALITOS_ICR_LO, isr_lo);

	if (unlikely((isr & ~TALITOS_ISR_CHDONE) || isr_lo))
		talitos_error((unsigned long)data, isr, isr_lo);
	else
		if (likely(isr & TALITOS_ISR_CHDONE)) {
			/* mask further done interrupts. */
			clrbits32(priv->reg + TALITOS_IMR, TALITOS_IMR_DONE);
			/* done_task will unmask done interrupts at exit */
			tasklet_schedule(&priv->done_task);
		}

	return (isr || isr_lo) ? IRQ_HANDLED : IRQ_NONE;
}

/*
 * hwrng
 */
static int talitos_rng_data_present(struct hwrng *rng, int wait)
{
	struct device *dev = (struct device *)rng->priv;
	struct talitos_private *priv = dev_get_drvdata(dev);
	u32 ofl;
	int i;

	for (i = 0; i < 20; i++) {
		ofl = in_be32(priv->reg + TALITOS_RNGUSR_LO) &
		      TALITOS_RNGUSR_LO_OFL;
		if (ofl || !wait)
			break;
		udelay(10);
	}

	return !!ofl;
}

static int talitos_rng_data_read(struct hwrng *rng, u32 *data)
{
	struct device *dev = (struct device *)rng->priv;
	struct talitos_private *priv = dev_get_drvdata(dev);

	/* rng fifo requires 64-bit accesses */
	*data = in_be32(priv->reg + TALITOS_RNGU_FIFO);
	*data = in_be32(priv->reg + TALITOS_RNGU_FIFO_LO);

	return sizeof(u32);
}

static int talitos_rng_init(struct hwrng *rng)
{
	struct device *dev = (struct device *)rng->priv;
	struct talitos_private *priv = dev_get_drvdata(dev);
	unsigned int timeout = TALITOS_TIMEOUT;

	setbits32(priv->reg + TALITOS_RNGURCR_LO, TALITOS_RNGURCR_LO_SR);
	while (!(in_be32(priv->reg + TALITOS_RNGUSR_LO) & TALITOS_RNGUSR_LO_RD)
	       && --timeout)
		cpu_relax();
	if (timeout == 0) {
		dev_err(dev, "failed to reset rng hw\n");
		return -ENODEV;
	}

	/* start generating */
	setbits32(priv->reg + TALITOS_RNGUDSR_LO, 0);

	return 0;
}

static int talitos_register_rng(struct device *dev)
{
	struct talitos_private *priv = dev_get_drvdata(dev);

	priv->rng.name		= dev_driver_string(dev),
	priv->rng.init		= talitos_rng_init,
	priv->rng.data_present	= talitos_rng_data_present,
	priv->rng.data_read	= talitos_rng_data_read,
	priv->rng.priv		= (unsigned long)dev;

	return hwrng_register(&priv->rng);
}

static void talitos_unregister_rng(struct device *dev)
{
	struct talitos_private *priv = dev_get_drvdata(dev);

	hwrng_unregister(&priv->rng);
}

/*
 * crypto alg
 */
#define TALITOS_CRA_PRIORITY		3000
#define TALITOS_MAX_KEY_SIZE		64
#define TALITOS_MAX_IV_LENGTH		16 /* max of AES_BLOCK_SIZE, DES3_EDE_BLOCK_SIZE */

#define MD5_DIGEST_SIZE   16

struct talitos_ctx {
	struct device *dev;
	__be32 desc_hdr_template;
	u8 key[TALITOS_MAX_KEY_SIZE];
	u8 iv[TALITOS_MAX_IV_LENGTH];
	unsigned int keylen;
	unsigned int enckeylen;
	unsigned int authkeylen;
	unsigned int authsize;
};

static int aead_authenc_setauthsize(struct crypto_aead *authenc,
						 unsigned int authsize)
{
	struct talitos_ctx *ctx = crypto_aead_ctx(authenc);

	ctx->authsize = authsize;

	return 0;
}

static int aead_authenc_setkey(struct crypto_aead *authenc,
					    const u8 *key, unsigned int keylen)
{
	struct talitos_ctx *ctx = crypto_aead_ctx(authenc);
	struct rtattr *rta = (void *)key;
	struct crypto_authenc_key_param *param;
	unsigned int authkeylen;
	unsigned int enckeylen;

	if (!RTA_OK(rta, keylen))
		goto badkey;

	if (rta->rta_type != CRYPTO_AUTHENC_KEYA_PARAM)
		goto badkey;

	if (RTA_PAYLOAD(rta) < sizeof(*param))
		goto badkey;

	param = RTA_DATA(rta);
	enckeylen = be32_to_cpu(param->enckeylen);

	key += RTA_ALIGN(rta->rta_len);
	keylen -= RTA_ALIGN(rta->rta_len);

	if (keylen < enckeylen)
		goto badkey;

	authkeylen = keylen - enckeylen;

	if (keylen > TALITOS_MAX_KEY_SIZE)
		goto badkey;

	memcpy(&ctx->key, key, keylen);

	ctx->keylen = keylen;
	ctx->enckeylen = enckeylen;
	ctx->authkeylen = authkeylen;

	return 0;

badkey:
	crypto_aead_set_flags(authenc, CRYPTO_TFM_RES_BAD_KEY_LEN);
	return -EINVAL;
}

/*
 * ipsec_esp_edesc - s/w-extended ipsec_esp descriptor
 * @src_nents: number of segments in input scatterlist
 * @dst_nents: number of segments in output scatterlist
 * @dma_len: length of dma mapped link_tbl space
 * @dma_link_tbl: bus physical address of link_tbl
 * @desc: h/w descriptor
 * @link_tbl: input and output h/w link tables (if {src,dst}_nents > 1)
 *
 * if decrypting (with authcheck), or either one of src_nents or dst_nents
 * is greater than 1, an integrity check value is concatenated to the end
 * of link_tbl data
 */
struct ipsec_esp_edesc {
	int src_nents;
	int dst_nents;
	int dma_len;
	dma_addr_t dma_link_tbl;
	struct talitos_desc desc;
	struct talitos_ptr link_tbl[0];
};

static void ipsec_esp_unmap(struct device *dev,
			    struct ipsec_esp_edesc *edesc,
			    struct aead_request *areq)
{
	unmap_single_talitos_ptr(dev, &edesc->desc.ptr[6], DMA_FROM_DEVICE);
	unmap_single_talitos_ptr(dev, &edesc->desc.ptr[3], DMA_TO_DEVICE);
	unmap_single_talitos_ptr(dev, &edesc->desc.ptr[2], DMA_TO_DEVICE);
	unmap_single_talitos_ptr(dev, &edesc->desc.ptr[0], DMA_TO_DEVICE);

	dma_unmap_sg(dev, areq->assoc, 1, DMA_TO_DEVICE);

	if (areq->src != areq->dst) {
		dma_unmap_sg(dev, areq->src, edesc->src_nents ? : 1,
			     DMA_TO_DEVICE);
		dma_unmap_sg(dev, areq->dst, edesc->dst_nents ? : 1,
			     DMA_FROM_DEVICE);
	} else {
		dma_unmap_sg(dev, areq->src, edesc->src_nents ? : 1,
			     DMA_BIDIRECTIONAL);
	}

	if (edesc->dma_len)
		dma_unmap_single(dev, edesc->dma_link_tbl, edesc->dma_len,
				 DMA_BIDIRECTIONAL);
}

/*
 * ipsec_esp descriptor callbacks
 */
static void ipsec_esp_encrypt_done(struct device *dev,
				   struct talitos_desc *desc, void *context,
				   int err)
{
	struct aead_request *areq = context;
	struct ipsec_esp_edesc *edesc =
		 container_of(desc, struct ipsec_esp_edesc, desc);
	struct crypto_aead *authenc = crypto_aead_reqtfm(areq);
	struct talitos_ctx *ctx = crypto_aead_ctx(authenc);
	struct scatterlist *sg;
	void *icvdata;

	ipsec_esp_unmap(dev, edesc, areq);

	/* copy the generated ICV to dst */
	if (edesc->dma_len) {
		icvdata = &edesc->link_tbl[edesc->src_nents +
					   edesc->dst_nents + 2];
		sg = sg_last(areq->dst, edesc->dst_nents);
		memcpy((char *)sg_virt(sg) + sg->length - ctx->authsize,
		       icvdata, ctx->authsize);
	}

	kfree(edesc);

	aead_request_complete(areq, err);
}

static void ipsec_esp_decrypt_swauth_done(struct device *dev,
				   struct talitos_desc *desc, void *context,
				   int err)
{
	struct aead_request *req = context;
	struct ipsec_esp_edesc *edesc =
		 container_of(desc, struct ipsec_esp_edesc, desc);
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct talitos_ctx *ctx = crypto_aead_ctx(authenc);
	struct scatterlist *sg;
	void *icvdata;

	ipsec_esp_unmap(dev, edesc, req);

	if (!err) {
		/* auth check */
		if (edesc->dma_len)
			icvdata = &edesc->link_tbl[edesc->src_nents +
						   edesc->dst_nents + 2];
		else
			icvdata = &edesc->link_tbl[0];

		sg = sg_last(req->dst, edesc->dst_nents ? : 1);
		err = memcmp(icvdata, (char *)sg_virt(sg) + sg->length -
			     ctx->authsize, ctx->authsize) ? -EBADMSG : 0;
	}

	kfree(edesc);

	aead_request_complete(req, err);
}

static void ipsec_esp_decrypt_hwauth_done(struct device *dev,
				   struct talitos_desc *desc, void *context,
				   int err)
{
	struct aead_request *req = context;
	struct ipsec_esp_edesc *edesc =
		 container_of(desc, struct ipsec_esp_edesc, desc);

	ipsec_esp_unmap(dev, edesc, req);

	/* check ICV auth status */
	if (!err)
		if ((desc->hdr_lo & DESC_HDR_LO_ICCR1_MASK) !=
		    DESC_HDR_LO_ICCR1_PASS)
			err = -EBADMSG;

	kfree(edesc);

	aead_request_complete(req, err);
}

/*
 * convert scatterlist to SEC h/w link table format
 * stop at cryptlen bytes
 */
static int sg_to_link_tbl(struct scatterlist *sg, int sg_count,
			   int cryptlen, struct talitos_ptr *link_tbl_ptr)
{
	int n_sg = sg_count;

	while (n_sg--) {
		link_tbl_ptr->ptr = cpu_to_be32(sg_dma_address(sg));
		link_tbl_ptr->len = cpu_to_be16(sg_dma_len(sg));
		link_tbl_ptr->j_extent = 0;
		link_tbl_ptr++;
		cryptlen -= sg_dma_len(sg);
		sg = sg_next(sg);
	}

	/* adjust (decrease) last one (or two) entry's len to cryptlen */
	link_tbl_ptr--;
	while (be16_to_cpu(link_tbl_ptr->len) <= (-cryptlen)) {
		/* Empty this entry, and move to previous one */
		cryptlen += be16_to_cpu(link_tbl_ptr->len);
		link_tbl_ptr->len = 0;
		sg_count--;
		link_tbl_ptr--;
	}
	link_tbl_ptr->len = cpu_to_be16(be16_to_cpu(link_tbl_ptr->len)
					+ cryptlen);

	/* tag end of link table */
	link_tbl_ptr->j_extent = DESC_PTR_LNKTBL_RETURN;

	return sg_count;
}

/*
 * fill in and submit ipsec_esp descriptor
 */
static int ipsec_esp(struct ipsec_esp_edesc *edesc, struct aead_request *areq,
		     u8 *giv, u64 seq,
		     void (*callback) (struct device *dev,
				       struct talitos_desc *desc,
				       void *context, int error))
{
	struct crypto_aead *aead = crypto_aead_reqtfm(areq);
	struct talitos_ctx *ctx = crypto_aead_ctx(aead);
	struct device *dev = ctx->dev;
	struct talitos_desc *desc = &edesc->desc;
	unsigned int cryptlen = areq->cryptlen;
	unsigned int authsize = ctx->authsize;
	unsigned int ivsize;
	int sg_count, ret;
	int sg_link_tbl_len;

	/* hmac key */
	map_single_talitos_ptr(dev, &desc->ptr[0], ctx->authkeylen, &ctx->key,
			       0, DMA_TO_DEVICE);
	/* hmac data */
	map_single_talitos_ptr(dev, &desc->ptr[1], sg_virt(areq->src) -
			       sg_virt(areq->assoc), sg_virt(areq->assoc), 0,
			       DMA_TO_DEVICE);
	/* cipher iv */
	ivsize = crypto_aead_ivsize(aead);
	map_single_talitos_ptr(dev, &desc->ptr[2], ivsize, giv ?: areq->iv, 0,
			       DMA_TO_DEVICE);

	/* cipher key */
	map_single_talitos_ptr(dev, &desc->ptr[3], ctx->enckeylen,
			       (char *)&ctx->key + ctx->authkeylen, 0,
			       DMA_TO_DEVICE);

	/*
	 * cipher in
	 * map and adjust cipher len to aead request cryptlen.
	 * extent is bytes of HMAC postpended to ciphertext,
	 * typically 12 for ipsec
	 */
	desc->ptr[4].len = cpu_to_be16(cryptlen);
	desc->ptr[4].j_extent = authsize;

	if (areq->src == areq->dst)
		sg_count = dma_map_sg(dev, areq->src, edesc->src_nents ? : 1,
				      DMA_BIDIRECTIONAL);
	else
		sg_count = dma_map_sg(dev, areq->src, edesc->src_nents ? : 1,
				      DMA_TO_DEVICE);

	if (sg_count == 1) {
		desc->ptr[4].ptr = cpu_to_be32(sg_dma_address(areq->src));
	} else {
		sg_link_tbl_len = cryptlen;

		if ((edesc->desc.hdr & DESC_HDR_MODE1_MDEU_CICV) &&
			(edesc->desc.hdr & DESC_HDR_MODE0_ENCRYPT) == 0) {
			sg_link_tbl_len = cryptlen + authsize;
		}
		sg_count = sg_to_link_tbl(areq->src, sg_count, sg_link_tbl_len,
					  &edesc->link_tbl[0]);
		if (sg_count > 1) {
			desc->ptr[4].j_extent |= DESC_PTR_LNKTBL_JUMP;
			desc->ptr[4].ptr = cpu_to_be32(edesc->dma_link_tbl);
			dma_sync_single_for_device(ctx->dev, edesc->dma_link_tbl,
						   edesc->dma_len, DMA_BIDIRECTIONAL);
		} else {
			/* Only one segment now, so no link tbl needed */
			desc->ptr[4].ptr = cpu_to_be32(sg_dma_address(areq->src));
		}
	}

	/* cipher out */
	desc->ptr[5].len = cpu_to_be16(cryptlen);
	desc->ptr[5].j_extent = authsize;

	if (areq->src != areq->dst) {
		sg_count = dma_map_sg(dev, areq->dst, edesc->dst_nents ? : 1,
				      DMA_FROM_DEVICE);
	}

	if (sg_count == 1) {
		desc->ptr[5].ptr = cpu_to_be32(sg_dma_address(areq->dst));
	} else {
		struct talitos_ptr *link_tbl_ptr =
			&edesc->link_tbl[edesc->src_nents + 1];

		desc->ptr[5].ptr = cpu_to_be32((struct talitos_ptr *)
					       edesc->dma_link_tbl +
					       edesc->src_nents + 1);
		sg_count = sg_to_link_tbl(areq->dst, sg_count, cryptlen,
					  link_tbl_ptr);

		/* Add an entry to the link table for ICV data */
		link_tbl_ptr += sg_count - 1;
		link_tbl_ptr->j_extent = 0;
		sg_count++;
		link_tbl_ptr++;
		link_tbl_ptr->j_extent = DESC_PTR_LNKTBL_RETURN;
		link_tbl_ptr->len = cpu_to_be16(authsize);

		/* icv data follows link tables */
		link_tbl_ptr->ptr = cpu_to_be32((struct talitos_ptr *)
						edesc->dma_link_tbl +
					        edesc->src_nents +
						edesc->dst_nents + 2);

		desc->ptr[5].j_extent |= DESC_PTR_LNKTBL_JUMP;
		dma_sync_single_for_device(ctx->dev, edesc->dma_link_tbl,
					   edesc->dma_len, DMA_BIDIRECTIONAL);
	}

	/* iv out */
	map_single_talitos_ptr(dev, &desc->ptr[6], ivsize, ctx->iv, 0,
			       DMA_FROM_DEVICE);

	ret = talitos_submit(dev, desc, callback, areq);
	if (ret != -EINPROGRESS) {
		ipsec_esp_unmap(dev, edesc, areq);
		kfree(edesc);
	}
	return ret;
}


/*
 * derive number of elements in scatterlist
 */
static int sg_count(struct scatterlist *sg_list, int nbytes)
{
	struct scatterlist *sg = sg_list;
	int sg_nents = 0;

	while (nbytes) {
		sg_nents++;
		nbytes -= sg->length;
		sg = sg_next(sg);
	}

	return sg_nents;
}

/*
 * allocate and map the ipsec_esp extended descriptor
 */
static struct ipsec_esp_edesc *ipsec_esp_edesc_alloc(struct aead_request *areq,
						     int icv_stashing)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(areq);
	struct talitos_ctx *ctx = crypto_aead_ctx(authenc);
	struct ipsec_esp_edesc *edesc;
	int src_nents, dst_nents, alloc_len, dma_len;
	gfp_t flags = areq->base.flags & CRYPTO_TFM_REQ_MAY_SLEEP ? GFP_KERNEL :
		      GFP_ATOMIC;

	if (areq->cryptlen + ctx->authsize > TALITOS_MAX_DATA_LEN) {
		dev_err(ctx->dev, "cryptlen exceeds h/w max limit\n");
		return ERR_PTR(-EINVAL);
	}

	src_nents = sg_count(areq->src, areq->cryptlen + ctx->authsize);
	src_nents = (src_nents == 1) ? 0 : src_nents;

	if (areq->dst == areq->src) {
		dst_nents = src_nents;
	} else {
		dst_nents = sg_count(areq->dst, areq->cryptlen + ctx->authsize);
		dst_nents = (dst_nents == 1) ? 0 : dst_nents;
	}

	/*
	 * allocate space for base edesc plus the link tables,
	 * allowing for two separate entries for ICV and generated ICV (+ 2),
	 * and the ICV data itself
	 */
	alloc_len = sizeof(struct ipsec_esp_edesc);
	if (src_nents || dst_nents) {
		dma_len = (src_nents + dst_nents + 2) *
				 sizeof(struct talitos_ptr) + ctx->authsize;
		alloc_len += dma_len;
	} else {
		dma_len = 0;
		alloc_len += icv_stashing ? ctx->authsize : 0;
	}

	edesc = kmalloc(alloc_len, GFP_DMA | flags);
	if (!edesc) {
		dev_err(ctx->dev, "could not allocate edescriptor\n");
		return ERR_PTR(-ENOMEM);
	}

	edesc->src_nents = src_nents;
	edesc->dst_nents = dst_nents;
	edesc->dma_len = dma_len;
	edesc->dma_link_tbl = dma_map_single(ctx->dev, &edesc->link_tbl[0],
					     edesc->dma_len, DMA_BIDIRECTIONAL);

	return edesc;
}

static int aead_authenc_encrypt(struct aead_request *req)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct talitos_ctx *ctx = crypto_aead_ctx(authenc);
	struct ipsec_esp_edesc *edesc;

	/* allocate extended descriptor */
	edesc = ipsec_esp_edesc_alloc(req, 0);
	if (IS_ERR(edesc))
		return PTR_ERR(edesc);

	/* set encrypt */
	edesc->desc.hdr = ctx->desc_hdr_template | DESC_HDR_MODE0_ENCRYPT;

	return ipsec_esp(edesc, req, NULL, 0, ipsec_esp_encrypt_done);
}



static int aead_authenc_decrypt(struct aead_request *req)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct talitos_ctx *ctx = crypto_aead_ctx(authenc);
	unsigned int authsize = ctx->authsize;
	struct talitos_private *priv = dev_get_drvdata(ctx->dev);
	struct ipsec_esp_edesc *edesc;
	struct scatterlist *sg;
	void *icvdata;

	req->cryptlen -= authsize;

	/* allocate extended descriptor */
	edesc = ipsec_esp_edesc_alloc(req, 1);
	if (IS_ERR(edesc))
		return PTR_ERR(edesc);

	if ((priv->features & TALITOS_FTR_HW_AUTH_CHECK) &&
	    (((!edesc->src_nents && !edesc->dst_nents) ||
		priv->features & TALITOS_FTR_SRC_LINK_TBL_LEN_INCLUDES_EXTENT))) {

		/* decrypt and check the ICV */
		edesc->desc.hdr = ctx->desc_hdr_template | DESC_HDR_DIR_INBOUND |
				  DESC_HDR_MODE1_MDEU_CICV;

		/* reset integrity check result bits */
		edesc->desc.hdr_lo = 0;

		return ipsec_esp(edesc, req, NULL, 0, ipsec_esp_decrypt_hwauth_done);

	} else {

		/* Have to check the ICV with software */

		edesc->desc.hdr = ctx->desc_hdr_template | DESC_HDR_DIR_INBOUND;

		/* stash incoming ICV for later cmp with ICV generated by the h/w */
		if (edesc->dma_len)
			icvdata = &edesc->link_tbl[edesc->src_nents +
						   edesc->dst_nents + 2];
		else
			icvdata = &edesc->link_tbl[0];

		sg = sg_last(req->src, edesc->src_nents ? : 1);

		memcpy(icvdata, (char *)sg_virt(sg) + sg->length - ctx->authsize,
		       ctx->authsize);

		return ipsec_esp(edesc, req, NULL, 0, ipsec_esp_decrypt_swauth_done);
	}
}

static int aead_authenc_givencrypt(
	struct aead_givcrypt_request *req)
{
	struct aead_request *areq = &req->areq;
	struct crypto_aead *authenc = crypto_aead_reqtfm(areq);
	struct talitos_ctx *ctx = crypto_aead_ctx(authenc);
	struct ipsec_esp_edesc *edesc;

	/* allocate extended descriptor */
	edesc = ipsec_esp_edesc_alloc(areq, 0);
	if (IS_ERR(edesc))
		return PTR_ERR(edesc);

	/* set encrypt */
	edesc->desc.hdr = ctx->desc_hdr_template | DESC_HDR_MODE0_ENCRYPT;

	memcpy(req->giv, ctx->iv, crypto_aead_ivsize(authenc));
	/* avoid consecutive packets going out with same IV */
	*(__be64 *)req->giv ^= cpu_to_be64(req->seq);

	return ipsec_esp(edesc, areq, req->giv, req->seq,
			 ipsec_esp_encrypt_done);
}

struct talitos_alg_template {
	char name[CRYPTO_MAX_ALG_NAME];
	char driver_name[CRYPTO_MAX_ALG_NAME];
	unsigned int blocksize;
	struct aead_alg aead;
	struct device *dev;
	__be32 desc_hdr_template;
};

static struct talitos_alg_template driver_algs[] = {
	/* single-pass ipsec_esp descriptor */
	{
		.name = "authenc(hmac(sha1),cbc(aes))",
		.driver_name = "authenc-hmac-sha1-cbc-aes-talitos",
		.blocksize = AES_BLOCK_SIZE,
		.aead = {
			.setkey = aead_authenc_setkey,
			.setauthsize = aead_authenc_setauthsize,
			.encrypt = aead_authenc_encrypt,
			.decrypt = aead_authenc_decrypt,
			.givencrypt = aead_authenc_givencrypt,
			.geniv = "<built-in>",
			.ivsize = AES_BLOCK_SIZE,
			.maxauthsize = SHA1_DIGEST_SIZE,
			},
		.desc_hdr_template = DESC_HDR_TYPE_IPSEC_ESP |
			             DESC_HDR_SEL0_AESU |
		                     DESC_HDR_MODE0_AESU_CBC |
		                     DESC_HDR_SEL1_MDEUA |
		                     DESC_HDR_MODE1_MDEU_INIT |
		                     DESC_HDR_MODE1_MDEU_PAD |
		                     DESC_HDR_MODE1_MDEU_SHA1_HMAC,
	},
	{
		.name = "authenc(hmac(sha1),cbc(des3_ede))",
		.driver_name = "authenc-hmac-sha1-cbc-3des-talitos",
		.blocksize = DES3_EDE_BLOCK_SIZE,
		.aead = {
			.setkey = aead_authenc_setkey,
			.setauthsize = aead_authenc_setauthsize,
			.encrypt = aead_authenc_encrypt,
			.decrypt = aead_authenc_decrypt,
			.givencrypt = aead_authenc_givencrypt,
			.geniv = "<built-in>",
			.ivsize = DES3_EDE_BLOCK_SIZE,
			.maxauthsize = SHA1_DIGEST_SIZE,
			},
		.desc_hdr_template = DESC_HDR_TYPE_IPSEC_ESP |
			             DESC_HDR_SEL0_DEU |
		                     DESC_HDR_MODE0_DEU_CBC |
		                     DESC_HDR_MODE0_DEU_3DES |
		                     DESC_HDR_SEL1_MDEUA |
		                     DESC_HDR_MODE1_MDEU_INIT |
		                     DESC_HDR_MODE1_MDEU_PAD |
		                     DESC_HDR_MODE1_MDEU_SHA1_HMAC,
	},
	{
		.name = "authenc(hmac(sha256),cbc(aes))",
		.driver_name = "authenc-hmac-sha256-cbc-aes-talitos",
		.blocksize = AES_BLOCK_SIZE,
		.aead = {
			.setkey = aead_authenc_setkey,
			.setauthsize = aead_authenc_setauthsize,
			.encrypt = aead_authenc_encrypt,
			.decrypt = aead_authenc_decrypt,
			.givencrypt = aead_authenc_givencrypt,
			.geniv = "<built-in>",
			.ivsize = AES_BLOCK_SIZE,
			.maxauthsize = SHA256_DIGEST_SIZE,
			},
		.desc_hdr_template = DESC_HDR_TYPE_IPSEC_ESP |
			             DESC_HDR_SEL0_AESU |
		                     DESC_HDR_MODE0_AESU_CBC |
		                     DESC_HDR_SEL1_MDEUA |
		                     DESC_HDR_MODE1_MDEU_INIT |
		                     DESC_HDR_MODE1_MDEU_PAD |
		                     DESC_HDR_MODE1_MDEU_SHA256_HMAC,
	},
	{
		.name = "authenc(hmac(sha256),cbc(des3_ede))",
		.driver_name = "authenc-hmac-sha256-cbc-3des-talitos",
		.blocksize = DES3_EDE_BLOCK_SIZE,
		.aead = {
			.setkey = aead_authenc_setkey,
			.setauthsize = aead_authenc_setauthsize,
			.encrypt = aead_authenc_encrypt,
			.decrypt = aead_authenc_decrypt,
			.givencrypt = aead_authenc_givencrypt,
			.geniv = "<built-in>",
			.ivsize = DES3_EDE_BLOCK_SIZE,
			.maxauthsize = SHA256_DIGEST_SIZE,
			},
		.desc_hdr_template = DESC_HDR_TYPE_IPSEC_ESP |
			             DESC_HDR_SEL0_DEU |
		                     DESC_HDR_MODE0_DEU_CBC |
		                     DESC_HDR_MODE0_DEU_3DES |
		                     DESC_HDR_SEL1_MDEUA |
		                     DESC_HDR_MODE1_MDEU_INIT |
		                     DESC_HDR_MODE1_MDEU_PAD |
		                     DESC_HDR_MODE1_MDEU_SHA256_HMAC,
	},
	{
		.name = "authenc(hmac(md5),cbc(aes))",
		.driver_name = "authenc-hmac-md5-cbc-aes-talitos",
		.blocksize = AES_BLOCK_SIZE,
		.aead = {
			.setkey = aead_authenc_setkey,
			.setauthsize = aead_authenc_setauthsize,
			.encrypt = aead_authenc_encrypt,
			.decrypt = aead_authenc_decrypt,
			.givencrypt = aead_authenc_givencrypt,
			.geniv = "<built-in>",
			.ivsize = AES_BLOCK_SIZE,
			.maxauthsize = MD5_DIGEST_SIZE,
			},
		.desc_hdr_template = DESC_HDR_TYPE_IPSEC_ESP |
			             DESC_HDR_SEL0_AESU |
		                     DESC_HDR_MODE0_AESU_CBC |
		                     DESC_HDR_SEL1_MDEUA |
		                     DESC_HDR_MODE1_MDEU_INIT |
		                     DESC_HDR_MODE1_MDEU_PAD |
		                     DESC_HDR_MODE1_MDEU_MD5_HMAC,
	},
	{
		.name = "authenc(hmac(md5),cbc(des3_ede))",
		.driver_name = "authenc-hmac-md5-cbc-3des-talitos",
		.blocksize = DES3_EDE_BLOCK_SIZE,
		.aead = {
			.setkey = aead_authenc_setkey,
			.setauthsize = aead_authenc_setauthsize,
			.encrypt = aead_authenc_encrypt,
			.decrypt = aead_authenc_decrypt,
			.givencrypt = aead_authenc_givencrypt,
			.geniv = "<built-in>",
			.ivsize = DES3_EDE_BLOCK_SIZE,
			.maxauthsize = MD5_DIGEST_SIZE,
			},
		.desc_hdr_template = DESC_HDR_TYPE_IPSEC_ESP |
			             DESC_HDR_SEL0_DEU |
		                     DESC_HDR_MODE0_DEU_CBC |
		                     DESC_HDR_MODE0_DEU_3DES |
		                     DESC_HDR_SEL1_MDEUA |
		                     DESC_HDR_MODE1_MDEU_INIT |
		                     DESC_HDR_MODE1_MDEU_PAD |
		                     DESC_HDR_MODE1_MDEU_MD5_HMAC,
	}
};

struct talitos_crypto_alg {
	struct list_head entry;
	struct device *dev;
	__be32 desc_hdr_template;
	struct crypto_alg crypto_alg;
};

static int talitos_cra_init(struct crypto_tfm *tfm)
{
	struct crypto_alg *alg = tfm->__crt_alg;
	struct talitos_crypto_alg *talitos_alg =
		 container_of(alg, struct talitos_crypto_alg, crypto_alg);
	struct talitos_ctx *ctx = crypto_tfm_ctx(tfm);

	/* update context with ptr to dev */
	ctx->dev = talitos_alg->dev;
	/* copy descriptor header template value */
	ctx->desc_hdr_template = talitos_alg->desc_hdr_template;

	/* random first IV */
	get_random_bytes(ctx->iv, TALITOS_MAX_IV_LENGTH);

	return 0;
}

/*
 * given the alg's descriptor header template, determine whether descriptor
 * type and primary/secondary execution units required match the hw
 * capabilities description provided in the device tree node.
 */
static int hw_supports(struct device *dev, __be32 desc_hdr_template)
{
	struct talitos_private *priv = dev_get_drvdata(dev);
	int ret;

	ret = (1 << DESC_TYPE(desc_hdr_template) & priv->desc_types) &&
	      (1 << PRIMARY_EU(desc_hdr_template) & priv->exec_units);

	if (SECONDARY_EU(desc_hdr_template))
		ret = ret && (1 << SECONDARY_EU(desc_hdr_template)
		              & priv->exec_units);

	return ret;
}

static int talitos_remove(struct of_device *ofdev)
{
	struct device *dev = &ofdev->dev;
	struct talitos_private *priv = dev_get_drvdata(dev);
	struct talitos_crypto_alg *t_alg, *n;
	int i;

	list_for_each_entry_safe(t_alg, n, &priv->alg_list, entry) {
		crypto_unregister_alg(&t_alg->crypto_alg);
		list_del(&t_alg->entry);
		kfree(t_alg);
	}

	if (hw_supports(dev, DESC_HDR_SEL0_RNG))
		talitos_unregister_rng(dev);

	kfree(priv->submit_count);
	kfree(priv->tail);
	kfree(priv->head);

	if (priv->fifo)
		for (i = 0; i < priv->num_channels; i++)
			kfree(priv->fifo[i]);

	kfree(priv->fifo);
	kfree(priv->head_lock);
	kfree(priv->tail_lock);

	if (priv->irq != NO_IRQ) {
		free_irq(priv->irq, dev);
		irq_dispose_mapping(priv->irq);
	}

	tasklet_kill(&priv->done_task);

	iounmap(priv->reg);

	dev_set_drvdata(dev, NULL);

	kfree(priv);

	return 0;
}

static struct talitos_crypto_alg *talitos_alg_alloc(struct device *dev,
						    struct talitos_alg_template
						           *template)
{
	struct talitos_crypto_alg *t_alg;
	struct crypto_alg *alg;

	t_alg = kzalloc(sizeof(struct talitos_crypto_alg), GFP_KERNEL);
	if (!t_alg)
		return ERR_PTR(-ENOMEM);

	alg = &t_alg->crypto_alg;

	snprintf(alg->cra_name, CRYPTO_MAX_ALG_NAME, "%s", template->name);
	snprintf(alg->cra_driver_name, CRYPTO_MAX_ALG_NAME, "%s",
		 template->driver_name);
	alg->cra_module = THIS_MODULE;
	alg->cra_init = talitos_cra_init;
	alg->cra_priority = TALITOS_CRA_PRIORITY;
	alg->cra_flags = CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_ASYNC;
	alg->cra_blocksize = template->blocksize;
	alg->cra_alignmask = 0;
	alg->cra_type = &crypto_aead_type;
	alg->cra_ctxsize = sizeof(struct talitos_ctx);
	alg->cra_u.aead = template->aead;

	t_alg->desc_hdr_template = template->desc_hdr_template;
	t_alg->dev = dev;

	return t_alg;
}

static int talitos_probe(struct of_device *ofdev,
			 const struct of_device_id *match)
{
	struct device *dev = &ofdev->dev;
	struct device_node *np = ofdev->node;
	struct talitos_private *priv;
	const unsigned int *prop;
	int i, err;

	priv = kzalloc(sizeof(struct talitos_private), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	dev_set_drvdata(dev, priv);

	priv->ofdev = ofdev;

	tasklet_init(&priv->done_task, talitos_done, (unsigned long)dev);

	INIT_LIST_HEAD(&priv->alg_list);

	priv->irq = irq_of_parse_and_map(np, 0);

	if (priv->irq == NO_IRQ) {
		dev_err(dev, "failed to map irq\n");
		err = -EINVAL;
		goto err_out;
	}

	/* get the irq line */
	err = request_irq(priv->irq, talitos_interrupt, 0,
			  dev_driver_string(dev), dev);
	if (err) {
		dev_err(dev, "failed to request irq %d\n", priv->irq);
		irq_dispose_mapping(priv->irq);
		priv->irq = NO_IRQ;
		goto err_out;
	}

	priv->reg = of_iomap(np, 0);
	if (!priv->reg) {
		dev_err(dev, "failed to of_iomap\n");
		err = -ENOMEM;
		goto err_out;
	}

	/* get SEC version capabilities from device tree */
	prop = of_get_property(np, "fsl,num-channels", NULL);
	if (prop)
		priv->num_channels = *prop;

	prop = of_get_property(np, "fsl,channel-fifo-len", NULL);
	if (prop)
		priv->chfifo_len = *prop;

	prop = of_get_property(np, "fsl,exec-units-mask", NULL);
	if (prop)
		priv->exec_units = *prop;

	prop = of_get_property(np, "fsl,descriptor-types-mask", NULL);
	if (prop)
		priv->desc_types = *prop;

	if (!is_power_of_2(priv->num_channels) || !priv->chfifo_len ||
	    !priv->exec_units || !priv->desc_types) {
		dev_err(dev, "invalid property data in device tree node\n");
		err = -EINVAL;
		goto err_out;
	}

	if (of_device_is_compatible(np, "fsl,sec3.0"))
		priv->features |= TALITOS_FTR_SRC_LINK_TBL_LEN_INCLUDES_EXTENT;

	if (of_device_is_compatible(np, "fsl,sec2.1"))
		priv->features |= TALITOS_FTR_HW_AUTH_CHECK;

	priv->head_lock = kmalloc(sizeof(spinlock_t) * priv->num_channels,
				  GFP_KERNEL);
	priv->tail_lock = kmalloc(sizeof(spinlock_t) * priv->num_channels,
				  GFP_KERNEL);
	if (!priv->head_lock || !priv->tail_lock) {
		dev_err(dev, "failed to allocate fifo locks\n");
		err = -ENOMEM;
		goto err_out;
	}

	for (i = 0; i < priv->num_channels; i++) {
		spin_lock_init(&priv->head_lock[i]);
		spin_lock_init(&priv->tail_lock[i]);
	}

	priv->fifo = kmalloc(sizeof(struct talitos_request *) *
			     priv->num_channels, GFP_KERNEL);
	if (!priv->fifo) {
		dev_err(dev, "failed to allocate request fifo\n");
		err = -ENOMEM;
		goto err_out;
	}

	priv->fifo_len = roundup_pow_of_two(priv->chfifo_len);

	for (i = 0; i < priv->num_channels; i++) {
		priv->fifo[i] = kzalloc(sizeof(struct talitos_request) *
					priv->fifo_len, GFP_KERNEL);
		if (!priv->fifo[i]) {
			dev_err(dev, "failed to allocate request fifo %d\n", i);
			err = -ENOMEM;
			goto err_out;
		}
	}

	priv->submit_count = kmalloc(sizeof(atomic_t) * priv->num_channels,
				     GFP_KERNEL);
	if (!priv->submit_count) {
		dev_err(dev, "failed to allocate fifo submit count space\n");
		err = -ENOMEM;
		goto err_out;
	}
	for (i = 0; i < priv->num_channels; i++)
		atomic_set(&priv->submit_count[i], -(priv->chfifo_len - 1));

	priv->head = kzalloc(sizeof(int) * priv->num_channels, GFP_KERNEL);
	priv->tail = kzalloc(sizeof(int) * priv->num_channels, GFP_KERNEL);
	if (!priv->head || !priv->tail) {
		dev_err(dev, "failed to allocate request index space\n");
		err = -ENOMEM;
		goto err_out;
	}

	/* reset and initialize the h/w */
	err = init_device(dev);
	if (err) {
		dev_err(dev, "failed to initialize device\n");
		goto err_out;
	}

	/* register the RNG, if available */
	if (hw_supports(dev, DESC_HDR_SEL0_RNG)) {
		err = talitos_register_rng(dev);
		if (err) {
			dev_err(dev, "failed to register hwrng: %d\n", err);
			goto err_out;
		} else
			dev_info(dev, "hwrng\n");
	}

	/* register crypto algorithms the device supports */
	for (i = 0; i < ARRAY_SIZE(driver_algs); i++) {
		if (hw_supports(dev, driver_algs[i].desc_hdr_template)) {
			struct talitos_crypto_alg *t_alg;

			t_alg = talitos_alg_alloc(dev, &driver_algs[i]);
			if (IS_ERR(t_alg)) {
				err = PTR_ERR(t_alg);
				goto err_out;
			}

			err = crypto_register_alg(&t_alg->crypto_alg);
			if (err) {
				dev_err(dev, "%s alg registration failed\n",
					t_alg->crypto_alg.cra_driver_name);
				kfree(t_alg);
			} else {
				list_add_tail(&t_alg->entry, &priv->alg_list);
				dev_info(dev, "%s\n",
					 t_alg->crypto_alg.cra_driver_name);
			}
		}
	}

	return 0;

err_out:
	talitos_remove(ofdev);

	return err;
}

static struct of_device_id talitos_match[] = {
	{
		.compatible = "fsl,sec2.0",
	},
	{},
};
MODULE_DEVICE_TABLE(of, talitos_match);

static struct of_platform_driver talitos_driver = {
	.name = "talitos",
	.match_table = talitos_match,
	.probe = talitos_probe,
	.remove = talitos_remove,
};

static int __init talitos_init(void)
{
	return of_register_platform_driver(&talitos_driver);
}
module_init(talitos_init);

static void __exit talitos_exit(void)
{
	of_unregister_platform_driver(&talitos_driver);
}
module_exit(talitos_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kim Phillips <kim.phillips@freescale.com>");
MODULE_DESCRIPTION("Freescale integrated security engine (SEC) driver");
