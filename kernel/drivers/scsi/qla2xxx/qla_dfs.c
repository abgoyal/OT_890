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
 * QLogic Fibre Channel HBA Driver
 * Copyright (c)  2003-2008 QLogic Corporation
 *
 * See LICENSE.qla2xxx for copyright and licensing details.
 */
#include "qla_def.h"

#include <linux/debugfs.h>
#include <linux/seq_file.h>

static struct dentry *qla2x00_dfs_root;
static atomic_t qla2x00_dfs_root_count;

static int
qla2x00_dfs_fce_show(struct seq_file *s, void *unused)
{
	scsi_qla_host_t *vha = s->private;
	uint32_t cnt;
	uint32_t *fce;
	uint64_t fce_start;
	struct qla_hw_data *ha = vha->hw;

	mutex_lock(&ha->fce_mutex);

	seq_printf(s, "FCE Trace Buffer\n");
	seq_printf(s, "In Pointer = %llx\n\n", (unsigned long long)ha->fce_wr);
	seq_printf(s, "Base = %llx\n\n", (unsigned long long) ha->fce_dma);
	seq_printf(s, "FCE Enable Registers\n");
	seq_printf(s, "%08x %08x %08x %08x %08x %08x\n",
	    ha->fce_mb[0], ha->fce_mb[2], ha->fce_mb[3], ha->fce_mb[4],
	    ha->fce_mb[5], ha->fce_mb[6]);

	fce = (uint32_t *) ha->fce;
	fce_start = (unsigned long long) ha->fce_dma;
	for (cnt = 0; cnt < fce_calc_size(ha->fce_bufs) / 4; cnt++) {
		if (cnt % 8 == 0)
			seq_printf(s, "\n%llx: ",
			    (unsigned long long)((cnt * 4) + fce_start));
		else
			seq_printf(s, " ");
		seq_printf(s, "%08x", *fce++);
	}

	seq_printf(s, "\nEnd\n");

	mutex_unlock(&ha->fce_mutex);

	return 0;
}

static int
qla2x00_dfs_fce_open(struct inode *inode, struct file *file)
{
	scsi_qla_host_t *vha = inode->i_private;
	struct qla_hw_data *ha = vha->hw;
	int rval;

	if (!ha->flags.fce_enabled)
		goto out;

	mutex_lock(&ha->fce_mutex);

	/* Pause tracing to flush FCE buffers. */
	rval = qla2x00_disable_fce_trace(vha, &ha->fce_wr, &ha->fce_rd);
	if (rval)
		qla_printk(KERN_WARNING, ha,
		    "DebugFS: Unable to disable FCE (%d).\n", rval);

	ha->flags.fce_enabled = 0;

	mutex_unlock(&ha->fce_mutex);
out:
	return single_open(file, qla2x00_dfs_fce_show, ha);
}

static int
qla2x00_dfs_fce_release(struct inode *inode, struct file *file)
{
	scsi_qla_host_t *vha = inode->i_private;
	struct qla_hw_data *ha = vha->hw;
	int rval;

	if (ha->flags.fce_enabled)
		goto out;

	mutex_lock(&ha->fce_mutex);

	/* Re-enable FCE tracing. */
	ha->flags.fce_enabled = 1;
	memset(ha->fce, 0, fce_calc_size(ha->fce_bufs));
	rval = qla2x00_enable_fce_trace(vha, ha->fce_dma, ha->fce_bufs,
	    ha->fce_mb, &ha->fce_bufs);
	if (rval) {
		qla_printk(KERN_WARNING, ha,
		    "DebugFS: Unable to reinitialize FCE (%d).\n", rval);
		ha->flags.fce_enabled = 0;
	}

	mutex_unlock(&ha->fce_mutex);
out:
	return single_release(inode, file);
}

static const struct file_operations dfs_fce_ops = {
	.open		= qla2x00_dfs_fce_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= qla2x00_dfs_fce_release,
};

int
qla2x00_dfs_setup(scsi_qla_host_t *vha)
{
	struct qla_hw_data *ha = vha->hw;

	if (!IS_QLA25XX(ha) && !IS_QLA81XX(ha))
		goto out;
	if (!ha->fce)
		goto out;

	if (qla2x00_dfs_root)
		goto create_dir;

	atomic_set(&qla2x00_dfs_root_count, 0);
	qla2x00_dfs_root = debugfs_create_dir(QLA2XXX_DRIVER_NAME, NULL);
	if (!qla2x00_dfs_root) {
		qla_printk(KERN_NOTICE, ha,
		    "DebugFS: Unable to create root directory.\n");
		goto out;
	}

create_dir:
	if (ha->dfs_dir)
		goto create_nodes;

	mutex_init(&ha->fce_mutex);
	ha->dfs_dir = debugfs_create_dir(vha->host_str, qla2x00_dfs_root);
	if (!ha->dfs_dir) {
		qla_printk(KERN_NOTICE, ha,
		    "DebugFS: Unable to create ha directory.\n");
		goto out;
	}

	atomic_inc(&qla2x00_dfs_root_count);

create_nodes:
	ha->dfs_fce = debugfs_create_file("fce", S_IRUSR, ha->dfs_dir, ha,
	    &dfs_fce_ops);
	if (!ha->dfs_fce) {
		qla_printk(KERN_NOTICE, ha,
		    "DebugFS: Unable to fce node.\n");
		goto out;
	}
out:
	return 0;
}

int
qla2x00_dfs_remove(scsi_qla_host_t *vha)
{
	struct qla_hw_data *ha = vha->hw;
	if (ha->dfs_fce) {
		debugfs_remove(ha->dfs_fce);
		ha->dfs_fce = NULL;
	}

	if (ha->dfs_dir) {
		debugfs_remove(ha->dfs_dir);
		ha->dfs_dir = NULL;
		atomic_dec(&qla2x00_dfs_root_count);
	}

	if (atomic_read(&qla2x00_dfs_root_count) == 0 &&
	    qla2x00_dfs_root) {
		debugfs_remove(qla2x00_dfs_root);
		qla2x00_dfs_root = NULL;
	}

	return 0;
}
