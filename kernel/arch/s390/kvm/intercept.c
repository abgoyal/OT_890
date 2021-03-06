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
 * intercept.c - in-kernel handling for sie intercepts
 *
 * Copyright IBM Corp. 2008
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License (version 2 only)
 * as published by the Free Software Foundation.
 *
 *    Author(s): Carsten Otte <cotte@de.ibm.com>
 *               Christian Borntraeger <borntraeger@de.ibm.com>
 */

#include <linux/kvm_host.h>
#include <linux/errno.h>
#include <linux/pagemap.h>

#include <asm/kvm_host.h>

#include "kvm-s390.h"
#include "gaccess.h"

static int handle_lctlg(struct kvm_vcpu *vcpu)
{
	int reg1 = (vcpu->arch.sie_block->ipa & 0x00f0) >> 4;
	int reg3 = vcpu->arch.sie_block->ipa & 0x000f;
	int base2 = vcpu->arch.sie_block->ipb >> 28;
	int disp2 = ((vcpu->arch.sie_block->ipb & 0x0fff0000) >> 16) +
			((vcpu->arch.sie_block->ipb & 0xff00) << 4);
	u64 useraddr;
	int reg, rc;

	vcpu->stat.instruction_lctlg++;
	if ((vcpu->arch.sie_block->ipb & 0xff) != 0x2f)
		return -ENOTSUPP;

	useraddr = disp2;
	if (base2)
		useraddr += vcpu->arch.guest_gprs[base2];

	if (useraddr & 7)
		return kvm_s390_inject_program_int(vcpu, PGM_SPECIFICATION);

	reg = reg1;

	VCPU_EVENT(vcpu, 5, "lctlg r1:%x, r3:%x,b2:%x,d2:%x", reg1, reg3, base2,
		   disp2);

	do {
		rc = get_guest_u64(vcpu, useraddr,
				   &vcpu->arch.sie_block->gcr[reg]);
		if (rc == -EFAULT) {
			kvm_s390_inject_program_int(vcpu, PGM_ADDRESSING);
			break;
		}
		useraddr += 8;
		if (reg == reg3)
			break;
		reg = (reg + 1) % 16;
	} while (1);
	return 0;
}

static int handle_lctl(struct kvm_vcpu *vcpu)
{
	int reg1 = (vcpu->arch.sie_block->ipa & 0x00f0) >> 4;
	int reg3 = vcpu->arch.sie_block->ipa & 0x000f;
	int base2 = vcpu->arch.sie_block->ipb >> 28;
	int disp2 = ((vcpu->arch.sie_block->ipb & 0x0fff0000) >> 16);
	u64 useraddr;
	u32 val = 0;
	int reg, rc;

	vcpu->stat.instruction_lctl++;

	useraddr = disp2;
	if (base2)
		useraddr += vcpu->arch.guest_gprs[base2];

	if (useraddr & 3)
		return kvm_s390_inject_program_int(vcpu, PGM_SPECIFICATION);

	VCPU_EVENT(vcpu, 5, "lctl r1:%x, r3:%x,b2:%x,d2:%x", reg1, reg3, base2,
		   disp2);

	reg = reg1;
	do {
		rc = get_guest_u32(vcpu, useraddr, &val);
		if (rc == -EFAULT) {
			kvm_s390_inject_program_int(vcpu, PGM_ADDRESSING);
			break;
		}
		vcpu->arch.sie_block->gcr[reg] &= 0xffffffff00000000ul;
		vcpu->arch.sie_block->gcr[reg] |= val;
		useraddr += 4;
		if (reg == reg3)
			break;
		reg = (reg + 1) % 16;
	} while (1);
	return 0;
}

static intercept_handler_t instruction_handlers[256] = {
	[0x83] = kvm_s390_handle_diag,
	[0xae] = kvm_s390_handle_sigp,
	[0xb2] = kvm_s390_handle_priv,
	[0xb7] = handle_lctl,
	[0xeb] = handle_lctlg,
};

static int handle_noop(struct kvm_vcpu *vcpu)
{
	switch (vcpu->arch.sie_block->icptcode) {
	case 0x0:
		vcpu->stat.exit_null++;
		break;
	case 0x10:
		vcpu->stat.exit_external_request++;
		break;
	case 0x14:
		vcpu->stat.exit_external_interrupt++;
		break;
	default:
		break; /* nothing */
	}
	return 0;
}

static int handle_stop(struct kvm_vcpu *vcpu)
{
	int rc;

	vcpu->stat.exit_stop_request++;
	atomic_clear_mask(CPUSTAT_RUNNING, &vcpu->arch.sie_block->cpuflags);
	spin_lock_bh(&vcpu->arch.local_int.lock);
	if (vcpu->arch.local_int.action_bits & ACTION_STORE_ON_STOP) {
		vcpu->arch.local_int.action_bits &= ~ACTION_STORE_ON_STOP;
		rc = __kvm_s390_vcpu_store_status(vcpu,
						  KVM_S390_STORE_STATUS_NOADDR);
		if (rc >= 0)
			rc = -ENOTSUPP;
	}

	if (vcpu->arch.local_int.action_bits & ACTION_STOP_ON_STOP) {
		vcpu->arch.local_int.action_bits &= ~ACTION_STOP_ON_STOP;
		VCPU_EVENT(vcpu, 3, "%s", "cpu stopped");
		rc = -ENOTSUPP;
	} else
		rc = 0;
	spin_unlock_bh(&vcpu->arch.local_int.lock);
	return rc;
}

static int handle_validity(struct kvm_vcpu *vcpu)
{
	int viwhy = vcpu->arch.sie_block->ipb >> 16;
	vcpu->stat.exit_validity++;
	if (viwhy == 0x37) {
		fault_in_pages_writeable((char __user *)
					 vcpu->kvm->arch.guest_origin +
					 vcpu->arch.sie_block->prefix,
					 PAGE_SIZE);
		return 0;
	}
	VCPU_EVENT(vcpu, 2, "unhandled validity intercept code %d",
		   viwhy);
	return -ENOTSUPP;
}

static int handle_instruction(struct kvm_vcpu *vcpu)
{
	intercept_handler_t handler;

	vcpu->stat.exit_instruction++;
	handler = instruction_handlers[vcpu->arch.sie_block->ipa >> 8];
	if (handler)
		return handler(vcpu);
	return -ENOTSUPP;
}

static int handle_prog(struct kvm_vcpu *vcpu)
{
	vcpu->stat.exit_program_interruption++;
	return kvm_s390_inject_program_int(vcpu, vcpu->arch.sie_block->iprcc);
}

static int handle_instruction_and_prog(struct kvm_vcpu *vcpu)
{
	int rc, rc2;

	vcpu->stat.exit_instr_and_program++;
	rc = handle_instruction(vcpu);
	rc2 = handle_prog(vcpu);

	if (rc == -ENOTSUPP)
		vcpu->arch.sie_block->icptcode = 0x04;
	if (rc)
		return rc;
	return rc2;
}

static const intercept_handler_t intercept_funcs[0x48 >> 2] = {
	[0x00 >> 2] = handle_noop,
	[0x04 >> 2] = handle_instruction,
	[0x08 >> 2] = handle_prog,
	[0x0C >> 2] = handle_instruction_and_prog,
	[0x10 >> 2] = handle_noop,
	[0x14 >> 2] = handle_noop,
	[0x1C >> 2] = kvm_s390_handle_wait,
	[0x20 >> 2] = handle_validity,
	[0x28 >> 2] = handle_stop,
};

int kvm_handle_sie_intercept(struct kvm_vcpu *vcpu)
{
	intercept_handler_t func;
	u8 code = vcpu->arch.sie_block->icptcode;

	if (code & 3 || code > 0x48)
		return -ENOTSUPP;
	func = intercept_funcs[code >> 2];
	if (func)
		return func(vcpu);
	return -ENOTSUPP;
}
