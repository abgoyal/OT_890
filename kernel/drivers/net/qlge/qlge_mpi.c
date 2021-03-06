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

#include "qlge.h"

static int ql_read_mbox_reg(struct ql_adapter *qdev, u32 reg, u32 *data)
{
	int status;
	/* wait for reg to come ready */
	status = ql_wait_reg_rdy(qdev, PROC_ADDR, PROC_ADDR_RDY, PROC_ADDR_ERR);
	if (status)
		goto exit;
	/* set up for reg read */
	ql_write32(qdev, PROC_ADDR, reg | PROC_ADDR_R);
	/* wait for reg to come ready */
	status = ql_wait_reg_rdy(qdev, PROC_ADDR, PROC_ADDR_RDY, PROC_ADDR_ERR);
	if (status)
		goto exit;
	/* get the data */
	*data = ql_read32(qdev, PROC_DATA);
exit:
	return status;
}

static int ql_get_mb_sts(struct ql_adapter *qdev, struct mbox_params *mbcp)
{
	int i, status;

	status = ql_sem_spinlock(qdev, SEM_PROC_REG_MASK);
	if (status)
		return -EBUSY;
	for (i = 0; i < mbcp->out_count; i++) {
		status =
		    ql_read_mbox_reg(qdev, qdev->mailbox_out + i,
				     &mbcp->mbox_out[i]);
		if (status) {
			QPRINTK(qdev, DRV, ERR, "Failed mailbox read.\n");
			break;
		}
	}
	ql_sem_unlock(qdev, SEM_PROC_REG_MASK);	/* does flush too */
	return status;
}

static void ql_link_up(struct ql_adapter *qdev, struct mbox_params *mbcp)
{
	mbcp->out_count = 2;

	if (ql_get_mb_sts(qdev, mbcp))
		goto exit;

	qdev->link_status = mbcp->mbox_out[1];
	QPRINTK(qdev, DRV, ERR, "Link Up.\n");
	QPRINTK(qdev, DRV, INFO, "Link Status = 0x%.08x.\n", mbcp->mbox_out[1]);
	if (!netif_carrier_ok(qdev->ndev)) {
		QPRINTK(qdev, LINK, INFO, "Link is Up.\n");
		netif_carrier_on(qdev->ndev);
		netif_wake_queue(qdev->ndev);
	}
exit:
	/* Clear the MPI firmware status. */
	ql_write32(qdev, CSR, CSR_CMD_CLR_R2PCI_INT);
}

static void ql_link_down(struct ql_adapter *qdev, struct mbox_params *mbcp)
{
	mbcp->out_count = 3;

	if (ql_get_mb_sts(qdev, mbcp)) {
		QPRINTK(qdev, DRV, ERR, "Firmware did not initialize!\n");
		goto exit;
	}

	if (netif_carrier_ok(qdev->ndev)) {
		QPRINTK(qdev, LINK, INFO, "Link is Down.\n");
		netif_carrier_off(qdev->ndev);
		netif_stop_queue(qdev->ndev);
	}
	QPRINTK(qdev, DRV, ERR, "Link Down.\n");
	QPRINTK(qdev, DRV, ERR, "Link Status = 0x%.08x.\n", mbcp->mbox_out[1]);
exit:
	/* Clear the MPI firmware status. */
	ql_write32(qdev, CSR, CSR_CMD_CLR_R2PCI_INT);
}

static void ql_init_fw_done(struct ql_adapter *qdev, struct mbox_params *mbcp)
{
	mbcp->out_count = 2;

	if (ql_get_mb_sts(qdev, mbcp)) {
		QPRINTK(qdev, DRV, ERR, "Firmware did not initialize!\n");
		goto exit;
	}
	QPRINTK(qdev, DRV, ERR, "Firmware initialized!\n");
	QPRINTK(qdev, DRV, ERR, "Firmware status = 0x%.08x.\n",
		mbcp->mbox_out[0]);
	QPRINTK(qdev, DRV, ERR, "Firmware Revision  = 0x%.08x.\n",
		mbcp->mbox_out[1]);
exit:
	/* Clear the MPI firmware status. */
	ql_write32(qdev, CSR, CSR_CMD_CLR_R2PCI_INT);
}

void ql_mpi_work(struct work_struct *work)
{
	struct ql_adapter *qdev =
	    container_of(work, struct ql_adapter, mpi_work.work);
	struct mbox_params mbc;
	struct mbox_params *mbcp = &mbc;
	mbcp->out_count = 1;

	while (ql_read32(qdev, STS) & STS_PI) {
		if (ql_get_mb_sts(qdev, mbcp)) {
			QPRINTK(qdev, DRV, ERR,
				"Could not read MPI, resetting ASIC!\n");
			ql_queue_asic_error(qdev);
		}

		switch (mbcp->mbox_out[0]) {
		case AEN_LINK_UP:
			ql_link_up(qdev, mbcp);
			break;
		case AEN_LINK_DOWN:
			ql_link_down(qdev, mbcp);
			break;
		case AEN_FW_INIT_DONE:
			ql_init_fw_done(qdev, mbcp);
			break;
		case MB_CMD_STS_GOOD:
			break;
		case AEN_FW_INIT_FAIL:
		case AEN_SYS_ERR:
		case MB_CMD_STS_ERR:
			ql_queue_fw_error(qdev);
		default:
			/* Clear the MPI firmware status. */
			ql_write32(qdev, CSR, CSR_CMD_CLR_R2PCI_INT);
			break;
		}
	}
	ql_enable_completion_interrupt(qdev, 0);
}

void ql_mpi_reset_work(struct work_struct *work)
{
	struct ql_adapter *qdev =
	    container_of(work, struct ql_adapter, mpi_reset_work.work);
	QPRINTK(qdev, DRV, ERR,
		"Enter, qdev = %p..\n", qdev);
	ql_write32(qdev, CSR, CSR_CMD_SET_RST);
	msleep(50);
	ql_write32(qdev, CSR, CSR_CMD_CLR_RST);
}
