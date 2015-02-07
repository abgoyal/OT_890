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

#ifndef __intr_vect_defs_h
#define __intr_vect_defs_h

/*
 * This file is autogenerated from
 *   file:           ../../inst/intr_vect/rtl/guinness/ivmask.config.r
 *     id:           ivmask.config.r,v 1.4 2005/02/15 16:05:38 stefans Exp 
 *     last modfied: Mon Apr 11 16:08:03 2005
 * 
 *   by /n/asic/design/tools/rdesc/src/rdes2c --outfile intr_vect_defs.h ../../inst/intr_vect/rtl/guinness/ivmask.config.r
 *      id: $Id$
 * Any changes here will be lost.
 *
 * -*- buffer-read-only: t -*-
 */
/* Main access macros */
#ifndef REG_RD
#define REG_RD( scope, inst, reg ) \
  REG_READ( reg_##scope##_##reg, \
            (inst) + REG_RD_ADDR_##scope##_##reg )
#endif

#ifndef REG_WR
#define REG_WR( scope, inst, reg, val ) \
  REG_WRITE( reg_##scope##_##reg, \
             (inst) + REG_WR_ADDR_##scope##_##reg, (val) )
#endif

#ifndef REG_RD_VECT
#define REG_RD_VECT( scope, inst, reg, index ) \
  REG_READ( reg_##scope##_##reg, \
            (inst) + REG_RD_ADDR_##scope##_##reg + \
	    (index) * STRIDE_##scope##_##reg )
#endif

#ifndef REG_WR_VECT
#define REG_WR_VECT( scope, inst, reg, index, val ) \
  REG_WRITE( reg_##scope##_##reg, \
             (inst) + REG_WR_ADDR_##scope##_##reg + \
	     (index) * STRIDE_##scope##_##reg, (val) )
#endif

#ifndef REG_RD_INT
#define REG_RD_INT( scope, inst, reg ) \
  REG_READ( int, (inst) + REG_RD_ADDR_##scope##_##reg )
#endif

#ifndef REG_WR_INT
#define REG_WR_INT( scope, inst, reg, val ) \
  REG_WRITE( int, (inst) + REG_WR_ADDR_##scope##_##reg, (val) )
#endif

#ifndef REG_RD_INT_VECT
#define REG_RD_INT_VECT( scope, inst, reg, index ) \
  REG_READ( int, (inst) + REG_RD_ADDR_##scope##_##reg + \
	    (index) * STRIDE_##scope##_##reg )
#endif

#ifndef REG_WR_INT_VECT
#define REG_WR_INT_VECT( scope, inst, reg, index, val ) \
  REG_WRITE( int, (inst) + REG_WR_ADDR_##scope##_##reg + \
	     (index) * STRIDE_##scope##_##reg, (val) )
#endif

#ifndef REG_TYPE_CONV
#define REG_TYPE_CONV( type, orgtype, val ) \
  ( { union { orgtype o; type n; } r; r.o = val; r.n; } )
#endif

#ifndef reg_page_size
#define reg_page_size 8192
#endif

#ifndef REG_ADDR
#define REG_ADDR( scope, inst, reg ) \
  ( (inst) + REG_RD_ADDR_##scope##_##reg )
#endif

#ifndef REG_ADDR_VECT
#define REG_ADDR_VECT( scope, inst, reg, index ) \
  ( (inst) + REG_RD_ADDR_##scope##_##reg + \
    (index) * STRIDE_##scope##_##reg )
#endif

/* C-code for register scope intr_vect */

#define STRIDE_intr_vect_rw_mask 0
/* Register rw_mask, scope intr_vect, type rw */
typedef struct {
  unsigned int memarb  : 1;
  unsigned int gen_io  : 1;
  unsigned int iop0    : 1;
  unsigned int iop1    : 1;
  unsigned int iop2    : 1;
  unsigned int iop3    : 1;
  unsigned int dma0    : 1;
  unsigned int dma1    : 1;
  unsigned int dma2    : 1;
  unsigned int dma3    : 1;
  unsigned int dma4    : 1;
  unsigned int dma5    : 1;
  unsigned int dma6    : 1;
  unsigned int dma7    : 1;
  unsigned int dma8    : 1;
  unsigned int dma9    : 1;
  unsigned int ata     : 1;
  unsigned int sser0   : 1;
  unsigned int sser1   : 1;
  unsigned int ser0    : 1;
  unsigned int ser1    : 1;
  unsigned int ser2    : 1;
  unsigned int ser3    : 1;
  unsigned int p21     : 1;
  unsigned int eth0    : 1;
  unsigned int eth1    : 1;
  unsigned int timer0  : 1;
  unsigned int bif_arb : 1;
  unsigned int bif_dma : 1;
  unsigned int ext     : 1;
  unsigned int dummy1  : 2;
} reg_intr_vect_rw_mask;
#define REG_RD_ADDR_intr_vect_rw_mask 0
#define REG_WR_ADDR_intr_vect_rw_mask 0

#define STRIDE_intr_vect_r_vect 0
/* Register r_vect, scope intr_vect, type r */
typedef struct {
  unsigned int memarb  : 1;
  unsigned int gen_io  : 1;
  unsigned int iop0    : 1;
  unsigned int iop1    : 1;
  unsigned int iop2    : 1;
  unsigned int iop3    : 1;
  unsigned int dma0    : 1;
  unsigned int dma1    : 1;
  unsigned int dma2    : 1;
  unsigned int dma3    : 1;
  unsigned int dma4    : 1;
  unsigned int dma5    : 1;
  unsigned int dma6    : 1;
  unsigned int dma7    : 1;
  unsigned int dma8    : 1;
  unsigned int dma9    : 1;
  unsigned int ata     : 1;
  unsigned int sser0   : 1;
  unsigned int sser1   : 1;
  unsigned int ser0    : 1;
  unsigned int ser1    : 1;
  unsigned int ser2    : 1;
  unsigned int ser3    : 1;
  unsigned int p21     : 1;
  unsigned int eth0    : 1;
  unsigned int eth1    : 1;
  unsigned int timer   : 1;
  unsigned int bif_arb : 1;
  unsigned int bif_dma : 1;
  unsigned int ext     : 1;
  unsigned int dummy1  : 2;
} reg_intr_vect_r_vect;
#define REG_RD_ADDR_intr_vect_r_vect 4

#define STRIDE_intr_vect_r_masked_vect 0
/* Register r_masked_vect, scope intr_vect, type r */
typedef struct {
  unsigned int memarb  : 1;
  unsigned int gen_io  : 1;
  unsigned int iop0    : 1;
  unsigned int iop1    : 1;
  unsigned int iop2    : 1;
  unsigned int iop3    : 1;
  unsigned int dma0    : 1;
  unsigned int dma1    : 1;
  unsigned int dma2    : 1;
  unsigned int dma3    : 1;
  unsigned int dma4    : 1;
  unsigned int dma5    : 1;
  unsigned int dma6    : 1;
  unsigned int dma7    : 1;
  unsigned int dma8    : 1;
  unsigned int dma9    : 1;
  unsigned int ata     : 1;
  unsigned int sser0   : 1;
  unsigned int sser1   : 1;
  unsigned int ser0    : 1;
  unsigned int ser1    : 1;
  unsigned int ser2    : 1;
  unsigned int ser3    : 1;
  unsigned int p21     : 1;
  unsigned int eth0    : 1;
  unsigned int eth1    : 1;
  unsigned int timer   : 1;
  unsigned int bif_arb : 1;
  unsigned int bif_dma : 1;
  unsigned int ext     : 1;
  unsigned int dummy1  : 2;
} reg_intr_vect_r_masked_vect;
#define REG_RD_ADDR_intr_vect_r_masked_vect 8

/* Register r_nmi, scope intr_vect, type r */
typedef struct {
  unsigned int ext      : 1;
  unsigned int watchdog : 1;
  unsigned int dummy1   : 30;
} reg_intr_vect_r_nmi;
#define REG_RD_ADDR_intr_vect_r_nmi 12

/* Register r_guru, scope intr_vect, type r */
typedef struct {
  unsigned int jtag : 1;
  unsigned int dummy1 : 31;
} reg_intr_vect_r_guru;
#define REG_RD_ADDR_intr_vect_r_guru 16

/* Register rw_ipi, scope intr_vect, type rw */
typedef struct 
{
  unsigned int vector;
} reg_intr_vect_rw_ipi;
#define REG_RD_ADDR_intr_vect_rw_ipi 20
#define REG_WR_ADDR_intr_vect_rw_ipi 20

/* Constants */
enum {
  regk_intr_vect_off                       = 0x00000000,
  regk_intr_vect_on                        = 0x00000001,
  regk_intr_vect_rw_mask_default           = 0x00000000
};
#endif /* __intr_vect_defs_h */
