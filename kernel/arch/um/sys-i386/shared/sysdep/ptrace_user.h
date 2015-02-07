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
 * Copyright (C) 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#ifndef __SYSDEP_I386_PTRACE_USER_H__
#define __SYSDEP_I386_PTRACE_USER_H__

#include <sys/ptrace.h>
#include <linux/ptrace.h>
#include <asm/ptrace.h>
#include "user_constants.h"

#define PT_OFFSET(r) ((r) * sizeof(long))

#define PT_SYSCALL_NR(regs) ((regs)[ORIG_EAX])
#define PT_SYSCALL_NR_OFFSET PT_OFFSET(ORIG_EAX)

#define PT_SYSCALL_ARG1_OFFSET PT_OFFSET(EBX)
#define PT_SYSCALL_ARG2_OFFSET PT_OFFSET(ECX)
#define PT_SYSCALL_ARG3_OFFSET PT_OFFSET(EDX)
#define PT_SYSCALL_ARG4_OFFSET PT_OFFSET(ESI)
#define PT_SYSCALL_ARG5_OFFSET PT_OFFSET(EDI)
#define PT_SYSCALL_ARG6_OFFSET PT_OFFSET(EBP)

#define PT_SYSCALL_RET_OFFSET PT_OFFSET(EAX)

#define REGS_SYSCALL_NR EAX /* This is used before a system call */
#define REGS_SYSCALL_ARG1 EBX
#define REGS_SYSCALL_ARG2 ECX
#define REGS_SYSCALL_ARG3 EDX
#define REGS_SYSCALL_ARG4 ESI
#define REGS_SYSCALL_ARG5 EDI
#define REGS_SYSCALL_ARG6 EBP

#define REGS_IP_INDEX EIP
#define REGS_SP_INDEX UESP

#define PT_IP_OFFSET PT_OFFSET(EIP)
#define PT_IP(regs) ((regs)[EIP])
#define PT_SP_OFFSET PT_OFFSET(UESP)
#define PT_SP(regs) ((regs)[UESP])

#define FP_SIZE ((HOST_FPX_SIZE > HOST_FP_SIZE) ? HOST_FPX_SIZE : HOST_FP_SIZE)

#ifndef FRAME_SIZE
#define FRAME_SIZE (17)
#endif

#endif