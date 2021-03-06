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
 * Copyright (C) 2005 - 2009 ServerEngines
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.  The full GNU General
 * Public License is included in this distribution in the file called COPYING.
 *
 * Contact Information:
 * linux-drivers@serverengines.com
 *
 * ServerEngines
 * 209 N. Fair Oaks Ave
 * Sunnyvale, CA 94085
 */

#include "be.h"

static int be_mbox_db_ready_wait(void __iomem *db)
{
	int cnt = 0, wait = 5;
	u32 ready;

	do {
		ready = ioread32(db) & MPU_MAILBOX_DB_RDY_MASK;
		if (ready)
			break;

		if (cnt > 200000) {
			printk(KERN_WARNING DRV_NAME
				": mbox_db poll timed out\n");
			return -1;
		}

		if (cnt > 50)
			wait = 200;
		cnt += wait;
		udelay(wait);
	} while (true);

	return 0;
}

/*
 * Insert the mailbox address into the doorbell in two steps
 */
static int be_mbox_db_ring(struct be_ctrl_info *ctrl)
{
	int status;
	u16 compl_status, extd_status;
	u32 val = 0;
	void __iomem *db = ctrl->db + MPU_MAILBOX_DB_OFFSET;
	struct be_dma_mem *mbox_mem = &ctrl->mbox_mem;
	struct be_mcc_mailbox *mbox = mbox_mem->va;
	struct be_mcc_cq_entry *cqe = &mbox->cqe;

	memset(cqe, 0, sizeof(*cqe));

	val &= ~MPU_MAILBOX_DB_RDY_MASK;
	val |= MPU_MAILBOX_DB_HI_MASK;
	/* at bits 2 - 31 place mbox dma addr msb bits 34 - 63 */
	val |= (upper_32_bits(mbox_mem->dma) >> 2) << 2;
	iowrite32(val, db);

	/* wait for ready to be set */
	status = be_mbox_db_ready_wait(db);
	if (status != 0)
		return status;

	val = 0;
	val &= ~MPU_MAILBOX_DB_RDY_MASK;
	val &= ~MPU_MAILBOX_DB_HI_MASK;
	/* at bits 2 - 31 place mbox dma addr lsb bits 4 - 33 */
	val |= (u32)(mbox_mem->dma >> 4) << 2;
	iowrite32(val, db);

	status = be_mbox_db_ready_wait(db);
	if (status != 0)
		return status;

	/* compl entry has been made now */
	be_dws_le_to_cpu(cqe, sizeof(*cqe));
	if (!(cqe->flags & CQE_FLAGS_VALID_MASK)) {
		printk(KERN_WARNING DRV_NAME ": ERROR invalid mbox compl\n");
		return -1;
	}

	compl_status = (cqe->status >> CQE_STATUS_COMPL_SHIFT) &
				CQE_STATUS_COMPL_MASK;
	if (compl_status != MCC_STATUS_SUCCESS) {
		extd_status = (cqe->status >> CQE_STATUS_EXTD_SHIFT) &
				CQE_STATUS_EXTD_MASK;
		printk(KERN_WARNING DRV_NAME
			": ERROR in cmd compl. status(compl/extd)=%d/%d\n",
			compl_status, extd_status);
	}

	return compl_status;
}

static int be_POST_stage_get(struct be_ctrl_info *ctrl, u16 *stage)
{
	u32 sem = ioread32(ctrl->csr + MPU_EP_SEMAPHORE_OFFSET);

	*stage = sem & EP_SEMAPHORE_POST_STAGE_MASK;
	if ((sem >> EP_SEMAPHORE_POST_ERR_SHIFT) & EP_SEMAPHORE_POST_ERR_MASK)
		return -1;
	else
		return 0;
}

static int be_POST_stage_poll(struct be_ctrl_info *ctrl, u16 poll_stage)
{
	u16 stage, cnt, error;
	for (cnt = 0; cnt < 5000; cnt++) {
		error = be_POST_stage_get(ctrl, &stage);
		if (error)
			return -1;

		if (stage == poll_stage)
			break;
		udelay(1000);
	}
	if (stage != poll_stage)
		return -1;
	return 0;
}


int be_cmd_POST(struct be_ctrl_info *ctrl)
{
	u16 stage, error;

	error = be_POST_stage_get(ctrl, &stage);
	if (error)
		goto err;

	if (stage == POST_STAGE_ARMFW_RDY)
		return 0;

	if (stage != POST_STAGE_AWAITING_HOST_RDY)
		goto err;

	/* On awaiting host rdy, reset and again poll on awaiting host rdy */
	iowrite32(POST_STAGE_BE_RESET, ctrl->csr + MPU_EP_SEMAPHORE_OFFSET);
	error = be_POST_stage_poll(ctrl, POST_STAGE_AWAITING_HOST_RDY);
	if (error)
		goto err;

	/* Now kickoff POST and poll on armfw ready */
	iowrite32(POST_STAGE_HOST_RDY, ctrl->csr + MPU_EP_SEMAPHORE_OFFSET);
	error = be_POST_stage_poll(ctrl, POST_STAGE_ARMFW_RDY);
	if (error)
		goto err;

	return 0;
err:
	printk(KERN_WARNING DRV_NAME ": ERROR, stage=%d\n", stage);
	return -1;
}

static inline void *embedded_payload(struct be_mcc_wrb *wrb)
{
	return wrb->payload.embedded_payload;
}

static inline struct be_sge *nonembedded_sgl(struct be_mcc_wrb *wrb)
{
	return &wrb->payload.sgl[0];
}

/* Don't touch the hdr after it's prepared */
static void be_wrb_hdr_prepare(struct be_mcc_wrb *wrb, int payload_len,
				bool embedded, u8 sge_cnt)
{
	if (embedded)
		wrb->embedded |= MCC_WRB_EMBEDDED_MASK;
	else
		wrb->embedded |= (sge_cnt & MCC_WRB_SGE_CNT_MASK) <<
				MCC_WRB_SGE_CNT_SHIFT;
	wrb->payload_length = payload_len;
	be_dws_cpu_to_le(wrb, 20);
}

/* Don't touch the hdr after it's prepared */
static void be_cmd_hdr_prepare(struct be_cmd_req_hdr *req_hdr,
				u8 subsystem, u8 opcode, int cmd_len)
{
	req_hdr->opcode = opcode;
	req_hdr->subsystem = subsystem;
	req_hdr->request_length = cpu_to_le32(cmd_len - sizeof(*req_hdr));
}

static void be_cmd_page_addrs_prepare(struct phys_addr *pages, u32 max_pages,
			struct be_dma_mem *mem)
{
	int i, buf_pages = min(PAGES_4K_SPANNED(mem->va, mem->size), max_pages);
	u64 dma = (u64)mem->dma;

	for (i = 0; i < buf_pages; i++) {
		pages[i].lo = cpu_to_le32(dma & 0xFFFFFFFF);
		pages[i].hi = cpu_to_le32(upper_32_bits(dma));
		dma += PAGE_SIZE_4K;
	}
}

/* Converts interrupt delay in microseconds to multiplier value */
static u32 eq_delay_to_mult(u32 usec_delay)
{
#define MAX_INTR_RATE			651042
	const u32 round = 10;
	u32 multiplier;

	if (usec_delay == 0)
		multiplier = 0;
	else {
		u32 interrupt_rate = 1000000 / usec_delay;
		/* Max delay, corresponding to the lowest interrupt rate */
		if (interrupt_rate == 0)
			multiplier = 1023;
		else {
			multiplier = (MAX_INTR_RATE - interrupt_rate) * round;
			multiplier /= interrupt_rate;
			/* Round the multiplier to the closest value.*/
			multiplier = (multiplier + round/2) / round;
			multiplier = min(multiplier, (u32)1023);
		}
	}
	return multiplier;
}

static inline struct be_mcc_wrb *wrb_from_mbox(struct be_dma_mem *mbox_mem)
{
	return &((struct be_mcc_mailbox *)(mbox_mem->va))->wrb;
}

int be_cmd_eq_create(struct be_ctrl_info *ctrl,
		struct be_queue_info *eq, int eq_delay)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_eq_create *req = embedded_payload(wrb);
	struct be_cmd_resp_eq_create *resp = embedded_payload(wrb);
	struct be_dma_mem *q_mem = &eq->dma_mem;
	int status;

	spin_lock(&ctrl->cmd_lock);
	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_COMMON,
		OPCODE_COMMON_EQ_CREATE, sizeof(*req));

	req->num_pages =  cpu_to_le16(PAGES_4K_SPANNED(q_mem->va, q_mem->size));

	AMAP_SET_BITS(struct amap_eq_context, func, req->context,
			ctrl->pci_func);
	AMAP_SET_BITS(struct amap_eq_context, valid, req->context, 1);
	/* 4byte eqe*/
	AMAP_SET_BITS(struct amap_eq_context, size, req->context, 0);
	AMAP_SET_BITS(struct amap_eq_context, count, req->context,
			__ilog2_u32(eq->len/256));
	AMAP_SET_BITS(struct amap_eq_context, delaymult, req->context,
			eq_delay_to_mult(eq_delay));
	be_dws_cpu_to_le(req->context, sizeof(req->context));

	be_cmd_page_addrs_prepare(req->pages, ARRAY_SIZE(req->pages), q_mem);

	status = be_mbox_db_ring(ctrl);
	if (!status) {
		eq->id = le16_to_cpu(resp->eq_id);
		eq->created = true;
	}
	spin_unlock(&ctrl->cmd_lock);
	return status;
}

int be_cmd_mac_addr_query(struct be_ctrl_info *ctrl, u8 *mac_addr,
			u8 type, bool permanent, u32 if_handle)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_mac_query *req = embedded_payload(wrb);
	struct be_cmd_resp_mac_query *resp = embedded_payload(wrb);
	int status;

	spin_lock(&ctrl->cmd_lock);
	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_COMMON,
		OPCODE_COMMON_NTWK_MAC_QUERY, sizeof(*req));

	req->type = type;
	if (permanent) {
		req->permanent = 1;
	} else {
		req->if_id = cpu_to_le16((u16)if_handle);
		req->permanent = 0;
	}

	status = be_mbox_db_ring(ctrl);
	if (!status)
		memcpy(mac_addr, resp->mac.addr, ETH_ALEN);

	spin_unlock(&ctrl->cmd_lock);
	return status;
}

int be_cmd_pmac_add(struct be_ctrl_info *ctrl, u8 *mac_addr,
		u32 if_id, u32 *pmac_id)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_pmac_add *req = embedded_payload(wrb);
	int status;

	spin_lock(&ctrl->cmd_lock);
	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_COMMON,
		OPCODE_COMMON_NTWK_PMAC_ADD, sizeof(*req));

	req->if_id = cpu_to_le32(if_id);
	memcpy(req->mac_address, mac_addr, ETH_ALEN);

	status = be_mbox_db_ring(ctrl);
	if (!status) {
		struct be_cmd_resp_pmac_add *resp = embedded_payload(wrb);
		*pmac_id = le32_to_cpu(resp->pmac_id);
	}

	spin_unlock(&ctrl->cmd_lock);
	return status;
}

int be_cmd_pmac_del(struct be_ctrl_info *ctrl, u32 if_id, u32 pmac_id)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_pmac_del *req = embedded_payload(wrb);
	int status;

	spin_lock(&ctrl->cmd_lock);
	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_COMMON,
		OPCODE_COMMON_NTWK_PMAC_DEL, sizeof(*req));

	req->if_id = cpu_to_le32(if_id);
	req->pmac_id = cpu_to_le32(pmac_id);

	status = be_mbox_db_ring(ctrl);
	spin_unlock(&ctrl->cmd_lock);

	return status;
}

int be_cmd_cq_create(struct be_ctrl_info *ctrl,
		struct be_queue_info *cq, struct be_queue_info *eq,
		bool sol_evts, bool no_delay, int coalesce_wm)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_cq_create *req = embedded_payload(wrb);
	struct be_cmd_resp_cq_create *resp = embedded_payload(wrb);
	struct be_dma_mem *q_mem = &cq->dma_mem;
	void *ctxt = &req->context;
	int status;

	spin_lock(&ctrl->cmd_lock);
	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_COMMON,
		OPCODE_COMMON_CQ_CREATE, sizeof(*req));

	req->num_pages =  cpu_to_le16(PAGES_4K_SPANNED(q_mem->va, q_mem->size));

	AMAP_SET_BITS(struct amap_cq_context, coalescwm, ctxt, coalesce_wm);
	AMAP_SET_BITS(struct amap_cq_context, nodelay, ctxt, no_delay);
	AMAP_SET_BITS(struct amap_cq_context, count, ctxt,
			__ilog2_u32(cq->len/256));
	AMAP_SET_BITS(struct amap_cq_context, valid, ctxt, 1);
	AMAP_SET_BITS(struct amap_cq_context, solevent, ctxt, sol_evts);
	AMAP_SET_BITS(struct amap_cq_context, eventable, ctxt, 1);
	AMAP_SET_BITS(struct amap_cq_context, eqid, ctxt, eq->id);
	AMAP_SET_BITS(struct amap_cq_context, armed, ctxt, 0);
	AMAP_SET_BITS(struct amap_cq_context, func, ctxt, ctrl->pci_func);
	be_dws_cpu_to_le(ctxt, sizeof(req->context));

	be_cmd_page_addrs_prepare(req->pages, ARRAY_SIZE(req->pages), q_mem);

	status = be_mbox_db_ring(ctrl);
	if (!status) {
		cq->id = le16_to_cpu(resp->cq_id);
		cq->created = true;
	}
	spin_unlock(&ctrl->cmd_lock);

	return status;
}

int be_cmd_txq_create(struct be_ctrl_info *ctrl,
			struct be_queue_info *txq,
			struct be_queue_info *cq)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_eth_tx_create *req = embedded_payload(wrb);
	struct be_dma_mem *q_mem = &txq->dma_mem;
	void *ctxt = &req->context;
	int status;
	u32 len_encoded;

	spin_lock(&ctrl->cmd_lock);
	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_ETH, OPCODE_ETH_TX_CREATE,
		sizeof(*req));

	req->num_pages = PAGES_4K_SPANNED(q_mem->va, q_mem->size);
	req->ulp_num = BE_ULP1_NUM;
	req->type = BE_ETH_TX_RING_TYPE_STANDARD;

	len_encoded = fls(txq->len); /* log2(len) + 1 */
	if (len_encoded == 16)
		len_encoded = 0;
	AMAP_SET_BITS(struct amap_tx_context, tx_ring_size, ctxt, len_encoded);
	AMAP_SET_BITS(struct amap_tx_context, pci_func_id, ctxt,
			ctrl->pci_func);
	AMAP_SET_BITS(struct amap_tx_context, ctx_valid, ctxt, 1);
	AMAP_SET_BITS(struct amap_tx_context, cq_id_send, ctxt, cq->id);

	be_dws_cpu_to_le(ctxt, sizeof(req->context));

	be_cmd_page_addrs_prepare(req->pages, ARRAY_SIZE(req->pages), q_mem);

	status = be_mbox_db_ring(ctrl);
	if (!status) {
		struct be_cmd_resp_eth_tx_create *resp = embedded_payload(wrb);
		txq->id = le16_to_cpu(resp->cid);
		txq->created = true;
	}
	spin_unlock(&ctrl->cmd_lock);

	return status;
}

int be_cmd_rxq_create(struct be_ctrl_info *ctrl,
		struct be_queue_info *rxq, u16 cq_id, u16 frag_size,
		u16 max_frame_size, u32 if_id, u32 rss)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_eth_rx_create *req = embedded_payload(wrb);
	struct be_dma_mem *q_mem = &rxq->dma_mem;
	int status;

	spin_lock(&ctrl->cmd_lock);
	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_ETH, OPCODE_ETH_RX_CREATE,
		sizeof(*req));

	req->cq_id = cpu_to_le16(cq_id);
	req->frag_size = fls(frag_size) - 1;
	req->num_pages = 2;
	be_cmd_page_addrs_prepare(req->pages, ARRAY_SIZE(req->pages), q_mem);
	req->interface_id = cpu_to_le32(if_id);
	req->max_frame_size = cpu_to_le16(max_frame_size);
	req->rss_queue = cpu_to_le32(rss);

	status = be_mbox_db_ring(ctrl);
	if (!status) {
		struct be_cmd_resp_eth_rx_create *resp = embedded_payload(wrb);
		rxq->id = le16_to_cpu(resp->id);
		rxq->created = true;
	}
	spin_unlock(&ctrl->cmd_lock);

	return status;
}

/* Generic destroyer function for all types of queues */
int be_cmd_q_destroy(struct be_ctrl_info *ctrl, struct be_queue_info *q,
		int queue_type)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_q_destroy *req = embedded_payload(wrb);
	u8 subsys = 0, opcode = 0;
	int status;

	spin_lock(&ctrl->cmd_lock);

	memset(wrb, 0, sizeof(*wrb));
	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	switch (queue_type) {
	case QTYPE_EQ:
		subsys = CMD_SUBSYSTEM_COMMON;
		opcode = OPCODE_COMMON_EQ_DESTROY;
		break;
	case QTYPE_CQ:
		subsys = CMD_SUBSYSTEM_COMMON;
		opcode = OPCODE_COMMON_CQ_DESTROY;
		break;
	case QTYPE_TXQ:
		subsys = CMD_SUBSYSTEM_ETH;
		opcode = OPCODE_ETH_TX_DESTROY;
		break;
	case QTYPE_RXQ:
		subsys = CMD_SUBSYSTEM_ETH;
		opcode = OPCODE_ETH_RX_DESTROY;
		break;
	default:
		printk(KERN_WARNING DRV_NAME ":bad Q type in Q destroy cmd\n");
		status = -1;
		goto err;
	}
	be_cmd_hdr_prepare(&req->hdr, subsys, opcode, sizeof(*req));
	req->id = cpu_to_le16(q->id);

	status = be_mbox_db_ring(ctrl);
err:
	spin_unlock(&ctrl->cmd_lock);

	return status;
}

/* Create an rx filtering policy configuration on an i/f */
int be_cmd_if_create(struct be_ctrl_info *ctrl, u32 flags, u8 *mac,
		bool pmac_invalid, u32 *if_handle, u32 *pmac_id)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_if_create *req = embedded_payload(wrb);
	int status;

	spin_lock(&ctrl->cmd_lock);
	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_COMMON,
		OPCODE_COMMON_NTWK_INTERFACE_CREATE, sizeof(*req));

	req->capability_flags = cpu_to_le32(flags);
	req->enable_flags = cpu_to_le32(flags);
	if (!pmac_invalid)
		memcpy(req->mac_addr, mac, ETH_ALEN);

	status = be_mbox_db_ring(ctrl);
	if (!status) {
		struct be_cmd_resp_if_create *resp = embedded_payload(wrb);
		*if_handle = le32_to_cpu(resp->interface_id);
		if (!pmac_invalid)
			*pmac_id = le32_to_cpu(resp->pmac_id);
	}

	spin_unlock(&ctrl->cmd_lock);
	return status;
}

int be_cmd_if_destroy(struct be_ctrl_info *ctrl, u32 interface_id)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_if_destroy *req = embedded_payload(wrb);
	int status;

	spin_lock(&ctrl->cmd_lock);
	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_COMMON,
		OPCODE_COMMON_NTWK_INTERFACE_DESTROY, sizeof(*req));

	req->interface_id = cpu_to_le32(interface_id);
	status = be_mbox_db_ring(ctrl);

	spin_unlock(&ctrl->cmd_lock);

	return status;
}

/* Get stats is a non embedded command: the request is not embedded inside
 * WRB but is a separate dma memory block
 */
int be_cmd_get_stats(struct be_ctrl_info *ctrl, struct be_dma_mem *nonemb_cmd)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_get_stats *req = nonemb_cmd->va;
	struct be_sge *sge = nonembedded_sgl(wrb);
	int status;

	spin_lock(&ctrl->cmd_lock);
	memset(wrb, 0, sizeof(*wrb));

	memset(req, 0, sizeof(*req));

	be_wrb_hdr_prepare(wrb, sizeof(*req), false, 1);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_ETH,
		OPCODE_ETH_GET_STATISTICS, sizeof(*req));
	sge->pa_hi = cpu_to_le32(upper_32_bits(nonemb_cmd->dma));
	sge->pa_lo = cpu_to_le32(nonemb_cmd->dma & 0xFFFFFFFF);
	sge->len = cpu_to_le32(nonemb_cmd->size);

	status = be_mbox_db_ring(ctrl);
	if (!status) {
		struct be_cmd_resp_get_stats *resp = nonemb_cmd->va;
		be_dws_le_to_cpu(&resp->hw_stats, sizeof(resp->hw_stats));
	}

	spin_unlock(&ctrl->cmd_lock);
	return status;
}

int be_cmd_link_status_query(struct be_ctrl_info *ctrl,
			struct be_link_info *link)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_link_status *req = embedded_payload(wrb);
	int status;

	spin_lock(&ctrl->cmd_lock);
	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_COMMON,
		OPCODE_COMMON_NTWK_LINK_STATUS_QUERY, sizeof(*req));

	status = be_mbox_db_ring(ctrl);
	if (!status) {
		struct be_cmd_resp_link_status *resp = embedded_payload(wrb);
		link->speed = resp->mac_speed;
		link->duplex = resp->mac_duplex;
		link->fault = resp->mac_fault;
	} else {
		link->speed = PHY_LINK_SPEED_ZERO;
	}

	spin_unlock(&ctrl->cmd_lock);
	return status;
}

int be_cmd_get_fw_ver(struct be_ctrl_info *ctrl, char *fw_ver)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_get_fw_version *req = embedded_payload(wrb);
	int status;

	spin_lock(&ctrl->cmd_lock);
	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_COMMON,
		OPCODE_COMMON_GET_FW_VERSION, sizeof(*req));

	status = be_mbox_db_ring(ctrl);
	if (!status) {
		struct be_cmd_resp_get_fw_version *resp = embedded_payload(wrb);
		strncpy(fw_ver, resp->firmware_version_string, FW_VER_LEN);
	}

	spin_unlock(&ctrl->cmd_lock);
	return status;
}

/* set the EQ delay interval of an EQ to specified value */
int be_cmd_modify_eqd(struct be_ctrl_info *ctrl, u32 eq_id, u32 eqd)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_modify_eq_delay *req = embedded_payload(wrb);
	int status;

	spin_lock(&ctrl->cmd_lock);
	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_COMMON,
		OPCODE_COMMON_MODIFY_EQ_DELAY, sizeof(*req));

	req->num_eq = cpu_to_le32(1);
	req->delay[0].eq_id = cpu_to_le32(eq_id);
	req->delay[0].phase = 0;
	req->delay[0].delay_multiplier = cpu_to_le32(eqd);

	status = be_mbox_db_ring(ctrl);

	spin_unlock(&ctrl->cmd_lock);
	return status;
}

int be_cmd_vlan_config(struct be_ctrl_info *ctrl, u32 if_id, u16 *vtag_array,
			u32 num, bool untagged, bool promiscuous)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_vlan_config *req = embedded_payload(wrb);
	int status;

	spin_lock(&ctrl->cmd_lock);
	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_COMMON,
		OPCODE_COMMON_NTWK_VLAN_CONFIG, sizeof(*req));

	req->interface_id = if_id;
	req->promiscuous = promiscuous;
	req->untagged = untagged;
	req->num_vlan = num;
	if (!promiscuous) {
		memcpy(req->normal_vlan, vtag_array,
			req->num_vlan * sizeof(vtag_array[0]));
	}

	status = be_mbox_db_ring(ctrl);

	spin_unlock(&ctrl->cmd_lock);
	return status;
}

int be_cmd_promiscuous_config(struct be_ctrl_info *ctrl, u8 port_num, bool en)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_promiscuous_config *req = embedded_payload(wrb);
	int status;

	spin_lock(&ctrl->cmd_lock);
	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_ETH,
		OPCODE_ETH_PROMISCUOUS, sizeof(*req));

	if (port_num)
		req->port1_promiscuous = en;
	else
		req->port0_promiscuous = en;

	status = be_mbox_db_ring(ctrl);

	spin_unlock(&ctrl->cmd_lock);
	return status;
}

int be_cmd_mcast_mac_set(struct be_ctrl_info *ctrl, u32 if_id, u8 *mac_table,
			u32 num, bool promiscuous)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_mcast_mac_config *req = embedded_payload(wrb);
	int status;

	spin_lock(&ctrl->cmd_lock);
	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_COMMON,
		OPCODE_COMMON_NTWK_MULTICAST_SET, sizeof(*req));

	req->interface_id = if_id;
	req->promiscuous = promiscuous;
	if (!promiscuous) {
		req->num_mac = cpu_to_le16(num);
		if (num)
			memcpy(req->mac, mac_table, ETH_ALEN * num);
	}

	status = be_mbox_db_ring(ctrl);

	spin_unlock(&ctrl->cmd_lock);
	return status;
}

int be_cmd_set_flow_control(struct be_ctrl_info *ctrl, u32 tx_fc, u32 rx_fc)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_set_flow_control *req = embedded_payload(wrb);
	int status;

	spin_lock(&ctrl->cmd_lock);

	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_COMMON,
		OPCODE_COMMON_SET_FLOW_CONTROL, sizeof(*req));

	req->tx_flow_control = cpu_to_le16((u16)tx_fc);
	req->rx_flow_control = cpu_to_le16((u16)rx_fc);

	status = be_mbox_db_ring(ctrl);

	spin_unlock(&ctrl->cmd_lock);
	return status;
}

int be_cmd_get_flow_control(struct be_ctrl_info *ctrl, u32 *tx_fc, u32 *rx_fc)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_get_flow_control *req = embedded_payload(wrb);
	int status;

	spin_lock(&ctrl->cmd_lock);

	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_COMMON,
		OPCODE_COMMON_GET_FLOW_CONTROL, sizeof(*req));

	status = be_mbox_db_ring(ctrl);
	if (!status) {
		struct be_cmd_resp_get_flow_control *resp =
						embedded_payload(wrb);
		*tx_fc = le16_to_cpu(resp->tx_flow_control);
		*rx_fc = le16_to_cpu(resp->rx_flow_control);
	}

	spin_unlock(&ctrl->cmd_lock);
	return status;
}

int be_cmd_query_fw_cfg(struct be_ctrl_info *ctrl, u32 *port_num)
{
	struct be_mcc_wrb *wrb = wrb_from_mbox(&ctrl->mbox_mem);
	struct be_cmd_req_query_fw_cfg *req = embedded_payload(wrb);
	int status;

	spin_lock(&ctrl->cmd_lock);

	memset(wrb, 0, sizeof(*wrb));

	be_wrb_hdr_prepare(wrb, sizeof(*req), true, 0);

	be_cmd_hdr_prepare(&req->hdr, CMD_SUBSYSTEM_COMMON,
		OPCODE_COMMON_QUERY_FIRMWARE_CONFIG, sizeof(*req));

	status = be_mbox_db_ring(ctrl);
	if (!status) {
		struct be_cmd_resp_query_fw_cfg *resp = embedded_payload(wrb);
		*port_num = le32_to_cpu(resp->phys_port);
	}

	spin_unlock(&ctrl->cmd_lock);
	return status;
}
