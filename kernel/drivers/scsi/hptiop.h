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
 * HighPoint RR3xxx/4xxx controller driver for Linux
 * Copyright (C) 2006-2007 HighPoint Technologies, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Please report bugs/comments/suggestions to linux@highpoint-tech.com
 *
 * For more information, visit http://www.highpoint-tech.com
 */
#ifndef _HPTIOP_H_
#define _HPTIOP_H_

struct hpt_iopmu_itl {
	__le32 resrved0[4];
	__le32 inbound_msgaddr0;
	__le32 inbound_msgaddr1;
	__le32 outbound_msgaddr0;
	__le32 outbound_msgaddr1;
	__le32 inbound_doorbell;
	__le32 inbound_intstatus;
	__le32 inbound_intmask;
	__le32 outbound_doorbell;
	__le32 outbound_intstatus;
	__le32 outbound_intmask;
	__le32 reserved1[2];
	__le32 inbound_queue;
	__le32 outbound_queue;
};

#define IOPMU_QUEUE_EMPTY            0xffffffff
#define IOPMU_QUEUE_MASK_HOST_BITS   0xf0000000
#define IOPMU_QUEUE_ADDR_HOST_BIT    0x80000000
#define IOPMU_QUEUE_REQUEST_SIZE_BIT    0x40000000
#define IOPMU_QUEUE_REQUEST_RESULT_BIT   0x40000000

#define IOPMU_OUTBOUND_INT_MSG0      1
#define IOPMU_OUTBOUND_INT_MSG1      2
#define IOPMU_OUTBOUND_INT_DOORBELL  4
#define IOPMU_OUTBOUND_INT_POSTQUEUE 8
#define IOPMU_OUTBOUND_INT_PCI       0x10

#define IOPMU_INBOUND_INT_MSG0       1
#define IOPMU_INBOUND_INT_MSG1       2
#define IOPMU_INBOUND_INT_DOORBELL   4
#define IOPMU_INBOUND_INT_ERROR      8
#define IOPMU_INBOUND_INT_POSTQUEUE  0x10

#define MVIOP_QUEUE_LEN  512

struct hpt_iopmu_mv {
	__le32 inbound_head;
	__le32 inbound_tail;
	__le32 outbound_head;
	__le32 outbound_tail;
	__le32 inbound_msg;
	__le32 outbound_msg;
	__le32 reserve[10];
	__le64 inbound_q[MVIOP_QUEUE_LEN];
	__le64 outbound_q[MVIOP_QUEUE_LEN];
};

struct hpt_iopmv_regs {
	__le32 reserved[0x20400 / 4];
	__le32 inbound_doorbell;
	__le32 inbound_intmask;
	__le32 outbound_doorbell;
	__le32 outbound_intmask;
};

#define MVIOP_MU_QUEUE_ADDR_HOST_MASK   (~(0x1full))
#define MVIOP_MU_QUEUE_ADDR_HOST_BIT    4

#define MVIOP_MU_QUEUE_ADDR_IOP_HIGH32  0xffffffff
#define MVIOP_MU_QUEUE_REQUEST_RESULT_BIT   1
#define MVIOP_MU_QUEUE_REQUEST_RETURN_CONTEXT 2

#define MVIOP_MU_INBOUND_INT_MSG        1
#define MVIOP_MU_INBOUND_INT_POSTQUEUE  2
#define MVIOP_MU_OUTBOUND_INT_MSG       1
#define MVIOP_MU_OUTBOUND_INT_POSTQUEUE 2

enum hpt_iopmu_message {
	/* host-to-iop messages */
	IOPMU_INBOUND_MSG0_NOP = 0,
	IOPMU_INBOUND_MSG0_RESET,
	IOPMU_INBOUND_MSG0_FLUSH,
	IOPMU_INBOUND_MSG0_SHUTDOWN,
	IOPMU_INBOUND_MSG0_STOP_BACKGROUND_TASK,
	IOPMU_INBOUND_MSG0_START_BACKGROUND_TASK,
	IOPMU_INBOUND_MSG0_MAX = 0xff,
	/* iop-to-host messages */
	IOPMU_OUTBOUND_MSG0_REGISTER_DEVICE_0 = 0x100,
	IOPMU_OUTBOUND_MSG0_REGISTER_DEVICE_MAX = 0x1ff,
	IOPMU_OUTBOUND_MSG0_UNREGISTER_DEVICE_0 = 0x200,
	IOPMU_OUTBOUND_MSG0_UNREGISTER_DEVICE_MAX = 0x2ff,
	IOPMU_OUTBOUND_MSG0_REVALIDATE_DEVICE_0 = 0x300,
	IOPMU_OUTBOUND_MSG0_REVALIDATE_DEVICE_MAX = 0x3ff,
};

struct hpt_iop_request_header {
	__le32 size;
	__le32 type;
	__le32 flags;
	__le32 result;
	__le32 context; /* host context */
	__le32 context_hi32;
};

#define IOP_REQUEST_FLAG_SYNC_REQUEST 1
#define IOP_REQUEST_FLAG_BIST_REQUEST 2
#define IOP_REQUEST_FLAG_REMAPPED     4
#define IOP_REQUEST_FLAG_OUTPUT_CONTEXT 8

enum hpt_iop_request_type {
	IOP_REQUEST_TYPE_GET_CONFIG = 0,
	IOP_REQUEST_TYPE_SET_CONFIG,
	IOP_REQUEST_TYPE_BLOCK_COMMAND,
	IOP_REQUEST_TYPE_SCSI_COMMAND,
	IOP_REQUEST_TYPE_IOCTL_COMMAND,
	IOP_REQUEST_TYPE_MAX
};

enum hpt_iop_result_type {
	IOP_RESULT_PENDING = 0,
	IOP_RESULT_SUCCESS,
	IOP_RESULT_FAIL,
	IOP_RESULT_BUSY,
	IOP_RESULT_RESET,
	IOP_RESULT_INVALID_REQUEST,
	IOP_RESULT_BAD_TARGET,
	IOP_RESULT_CHECK_CONDITION,
};

struct hpt_iop_request_get_config {
	struct hpt_iop_request_header header;
	__le32 interface_version;
	__le32 firmware_version;
	__le32 max_requests;
	__le32 request_size;
	__le32 max_sg_count;
	__le32 data_transfer_length;
	__le32 alignment_mask;
	__le32 max_devices;
	__le32 sdram_size;
};

struct hpt_iop_request_set_config {
	struct hpt_iop_request_header header;
	__le32 iop_id;
	__le16 vbus_id;
	__le16 max_host_request_size;
	__le32 reserve[6];
};

struct hpt_iopsg {
	__le32 size;
	__le32 eot; /* non-zero: end of table */
	__le64 pci_address;
};

struct hpt_iop_request_block_command {
	struct hpt_iop_request_header header;
	u8     channel;
	u8     target;
	u8     lun;
	u8     pad1;
	__le16 command; /* IOP_BLOCK_COMMAND_{READ,WRITE} */
	__le16 sectors;
	__le64 lba;
	struct hpt_iopsg sg_list[1];
};

#define IOP_BLOCK_COMMAND_READ     1
#define IOP_BLOCK_COMMAND_WRITE    2
#define IOP_BLOCK_COMMAND_VERIFY   3
#define IOP_BLOCK_COMMAND_FLUSH    4
#define IOP_BLOCK_COMMAND_SHUTDOWN 5

struct hpt_iop_request_scsi_command {
	struct hpt_iop_request_header header;
	u8     channel;
	u8     target;
	u8     lun;
	u8     pad1;
	u8     cdb[16];
	__le32 dataxfer_length;
	struct hpt_iopsg sg_list[1];
};

struct hpt_iop_request_ioctl_command {
	struct hpt_iop_request_header header;
	__le32 ioctl_code;
	__le32 inbuf_size;
	__le32 outbuf_size;
	__le32 bytes_returned;
	u8     buf[1];
	/* out data should be put at buf[(inbuf_size+3)&~3] */
};

#define HPTIOP_MAX_REQUESTS  256u

struct hptiop_request {
	struct hptiop_request *next;
	void                  *req_virt;
	u32                   req_shifted_phy;
	struct scsi_cmnd      *scp;
	int                   index;
};

struct hpt_scsi_pointer {
	int mapped;
	int sgcnt;
	dma_addr_t dma_handle;
};

#define HPT_SCP(scp) ((struct hpt_scsi_pointer *)&(scp)->SCp)

struct hptiop_hba {
	struct hptiop_adapter_ops *ops;
	union {
		struct {
			struct hpt_iopmu_itl __iomem *iop;
		} itl;
		struct {
			struct hpt_iopmv_regs *regs;
			struct hpt_iopmu_mv __iomem *mu;
			void *internal_req;
			dma_addr_t internal_req_phy;
		} mv;
	} u;

	struct Scsi_Host *host;
	struct pci_dev *pcidev;

	/* IOP config info */
	u32     interface_version;
	u32     firmware_version;
	u32     sdram_size;
	u32     max_devices;
	u32     max_requests;
	u32     max_request_size;
	u32     max_sg_descriptors;

	u32     req_size; /* host-allocated request buffer size */

	u32     iopintf_v2: 1;
	u32     initialized: 1;
	u32     msg_done: 1;

	struct hptiop_request * req_list;
	struct hptiop_request reqs[HPTIOP_MAX_REQUESTS];

	/* used to free allocated dma area */
	void        *dma_coherent;
	dma_addr_t  dma_coherent_handle;

	atomic_t    reset_count;
	atomic_t    resetting;

	wait_queue_head_t reset_wq;
	wait_queue_head_t ioctl_wq;
};

struct hpt_ioctl_k {
	struct hptiop_hba * hba;
	u32    ioctl_code;
	u32    inbuf_size;
	u32    outbuf_size;
	void   *inbuf;
	void   *outbuf;
	u32    *bytes_returned;
	void (*done)(struct hpt_ioctl_k *);
	int    result; /* HPT_IOCTL_RESULT_ */
};

struct hptiop_adapter_ops {
	int  (*iop_wait_ready)(struct hptiop_hba *hba, u32 millisec);
	int  (*internal_memalloc)(struct hptiop_hba *hba);
	int  (*internal_memfree)(struct hptiop_hba *hba);
	int  (*map_pci_bar)(struct hptiop_hba *hba);
	void (*unmap_pci_bar)(struct hptiop_hba *hba);
	void (*enable_intr)(struct hptiop_hba *hba);
	void (*disable_intr)(struct hptiop_hba *hba);
	int  (*get_config)(struct hptiop_hba *hba,
				struct hpt_iop_request_get_config *config);
	int  (*set_config)(struct hptiop_hba *hba,
				struct hpt_iop_request_set_config *config);
	int  (*iop_intr)(struct hptiop_hba *hba);
	void (*post_msg)(struct hptiop_hba *hba, u32 msg);
	void (*post_req)(struct hptiop_hba *hba, struct hptiop_request *_req);
};

#define HPT_IOCTL_RESULT_OK         0
#define HPT_IOCTL_RESULT_FAILED     (-1)

#if 0
#define dprintk(fmt, args...) do { printk(fmt, ##args); } while(0)
#else
#define dprintk(fmt, args...)
#endif

#endif
