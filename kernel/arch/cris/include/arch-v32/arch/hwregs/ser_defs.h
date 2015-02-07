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

#ifndef __ser_defs_h
#define __ser_defs_h

/*
 * This file is autogenerated from
 *   file:           ../../inst/ser/rtl/ser_regs.r
 *     id:           ser_regs.r,v 1.23 2005/02/08 13:58:35 perz Exp
 *     last modfied: Mon Apr 11 16:09:21 2005
 *
 *   by /n/asic/design/tools/rdesc/src/rdes2c --outfile ser_defs.h ../../inst/ser/rtl/ser_regs.r
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

/* C-code for register scope ser */

/* Register rw_tr_ctrl, scope ser, type rw */
typedef struct {
  unsigned int base_freq : 3;
  unsigned int en        : 1;
  unsigned int par       : 2;
  unsigned int par_en    : 1;
  unsigned int data_bits : 1;
  unsigned int stop_bits : 1;
  unsigned int stop      : 1;
  unsigned int rts_delay : 3;
  unsigned int rts_setup : 1;
  unsigned int auto_rts  : 1;
  unsigned int txd       : 1;
  unsigned int auto_cts  : 1;
  unsigned int dummy1    : 15;
} reg_ser_rw_tr_ctrl;
#define REG_RD_ADDR_ser_rw_tr_ctrl 0
#define REG_WR_ADDR_ser_rw_tr_ctrl 0

/* Register rw_tr_dma_en, scope ser, type rw */
typedef struct {
  unsigned int en : 1;
  unsigned int dummy1 : 31;
} reg_ser_rw_tr_dma_en;
#define REG_RD_ADDR_ser_rw_tr_dma_en 4
#define REG_WR_ADDR_ser_rw_tr_dma_en 4

/* Register rw_rec_ctrl, scope ser, type rw */
typedef struct {
  unsigned int base_freq   : 3;
  unsigned int en          : 1;
  unsigned int par         : 2;
  unsigned int par_en      : 1;
  unsigned int data_bits   : 1;
  unsigned int dma_mode    : 1;
  unsigned int dma_err     : 1;
  unsigned int sampling    : 1;
  unsigned int timeout     : 3;
  unsigned int auto_eop    : 1;
  unsigned int half_duplex : 1;
  unsigned int rts_n       : 1;
  unsigned int loopback    : 1;
  unsigned int dummy1      : 14;
} reg_ser_rw_rec_ctrl;
#define REG_RD_ADDR_ser_rw_rec_ctrl 8
#define REG_WR_ADDR_ser_rw_rec_ctrl 8

/* Register rw_tr_baud_div, scope ser, type rw */
typedef struct {
  unsigned int div : 16;
  unsigned int dummy1 : 16;
} reg_ser_rw_tr_baud_div;
#define REG_RD_ADDR_ser_rw_tr_baud_div 12
#define REG_WR_ADDR_ser_rw_tr_baud_div 12

/* Register rw_rec_baud_div, scope ser, type rw */
typedef struct {
  unsigned int div : 16;
  unsigned int dummy1 : 16;
} reg_ser_rw_rec_baud_div;
#define REG_RD_ADDR_ser_rw_rec_baud_div 16
#define REG_WR_ADDR_ser_rw_rec_baud_div 16

/* Register rw_xoff, scope ser, type rw */
typedef struct {
  unsigned int chr       : 8;
  unsigned int automatic : 1;
  unsigned int dummy1    : 23;
} reg_ser_rw_xoff;
#define REG_RD_ADDR_ser_rw_xoff 20
#define REG_WR_ADDR_ser_rw_xoff 20

/* Register rw_xoff_clr, scope ser, type rw */
typedef struct {
  unsigned int clr : 1;
  unsigned int dummy1 : 31;
} reg_ser_rw_xoff_clr;
#define REG_RD_ADDR_ser_rw_xoff_clr 24
#define REG_WR_ADDR_ser_rw_xoff_clr 24

/* Register rw_dout, scope ser, type rw */
typedef struct {
  unsigned int data : 8;
  unsigned int dummy1 : 24;
} reg_ser_rw_dout;
#define REG_RD_ADDR_ser_rw_dout 28
#define REG_WR_ADDR_ser_rw_dout 28

/* Register rs_stat_din, scope ser, type rs */
typedef struct {
  unsigned int data        : 8;
  unsigned int dummy1      : 8;
  unsigned int dav         : 1;
  unsigned int framing_err : 1;
  unsigned int par_err     : 1;
  unsigned int orun        : 1;
  unsigned int rec_err     : 1;
  unsigned int rxd         : 1;
  unsigned int tr_idle     : 1;
  unsigned int tr_empty    : 1;
  unsigned int tr_rdy      : 1;
  unsigned int cts_n       : 1;
  unsigned int xoff_detect : 1;
  unsigned int rts_n       : 1;
  unsigned int txd         : 1;
  unsigned int dummy2      : 3;
} reg_ser_rs_stat_din;
#define REG_RD_ADDR_ser_rs_stat_din 32

/* Register r_stat_din, scope ser, type r */
typedef struct {
  unsigned int data        : 8;
  unsigned int dummy1      : 8;
  unsigned int dav         : 1;
  unsigned int framing_err : 1;
  unsigned int par_err     : 1;
  unsigned int orun        : 1;
  unsigned int rec_err     : 1;
  unsigned int rxd         : 1;
  unsigned int tr_idle     : 1;
  unsigned int tr_empty    : 1;
  unsigned int tr_rdy      : 1;
  unsigned int cts_n       : 1;
  unsigned int xoff_detect : 1;
  unsigned int rts_n       : 1;
  unsigned int txd         : 1;
  unsigned int dummy2      : 3;
} reg_ser_r_stat_din;
#define REG_RD_ADDR_ser_r_stat_din 36

/* Register rw_rec_eop, scope ser, type rw */
typedef struct {
  unsigned int set : 1;
  unsigned int dummy1 : 31;
} reg_ser_rw_rec_eop;
#define REG_RD_ADDR_ser_rw_rec_eop 40
#define REG_WR_ADDR_ser_rw_rec_eop 40

/* Register rw_intr_mask, scope ser, type rw */
typedef struct {
  unsigned int tr_rdy   : 1;
  unsigned int tr_empty : 1;
  unsigned int tr_idle  : 1;
  unsigned int dav      : 1;
  unsigned int dummy1   : 28;
} reg_ser_rw_intr_mask;
#define REG_RD_ADDR_ser_rw_intr_mask 44
#define REG_WR_ADDR_ser_rw_intr_mask 44

/* Register rw_ack_intr, scope ser, type rw */
typedef struct {
  unsigned int tr_rdy   : 1;
  unsigned int tr_empty : 1;
  unsigned int tr_idle  : 1;
  unsigned int dav      : 1;
  unsigned int dummy1   : 28;
} reg_ser_rw_ack_intr;
#define REG_RD_ADDR_ser_rw_ack_intr 48
#define REG_WR_ADDR_ser_rw_ack_intr 48

/* Register r_intr, scope ser, type r */
typedef struct {
  unsigned int tr_rdy   : 1;
  unsigned int tr_empty : 1;
  unsigned int tr_idle  : 1;
  unsigned int dav      : 1;
  unsigned int dummy1   : 28;
} reg_ser_r_intr;
#define REG_RD_ADDR_ser_r_intr 52

/* Register r_masked_intr, scope ser, type r */
typedef struct {
  unsigned int tr_rdy   : 1;
  unsigned int tr_empty : 1;
  unsigned int tr_idle  : 1;
  unsigned int dav      : 1;
  unsigned int dummy1   : 28;
} reg_ser_r_masked_intr;
#define REG_RD_ADDR_ser_r_masked_intr 56


/* Constants */
enum {
  regk_ser_active                          = 0x00000000,
  regk_ser_bits1                           = 0x00000000,
  regk_ser_bits2                           = 0x00000001,
  regk_ser_bits7                           = 0x00000001,
  regk_ser_bits8                           = 0x00000000,
  regk_ser_del0_5                          = 0x00000000,
  regk_ser_del1                            = 0x00000001,
  regk_ser_del1_5                          = 0x00000002,
  regk_ser_del2                            = 0x00000003,
  regk_ser_del2_5                          = 0x00000004,
  regk_ser_del3                            = 0x00000005,
  regk_ser_del3_5                          = 0x00000006,
  regk_ser_del4                            = 0x00000007,
  regk_ser_even                            = 0x00000000,
  regk_ser_ext                             = 0x00000001,
  regk_ser_f100                            = 0x00000007,
  regk_ser_f29_493                         = 0x00000004,
  regk_ser_f32                             = 0x00000005,
  regk_ser_f32_768                         = 0x00000006,
  regk_ser_ignore                          = 0x00000001,
  regk_ser_inactive                        = 0x00000001,
  regk_ser_majority                        = 0x00000001,
  regk_ser_mark                            = 0x00000002,
  regk_ser_middle                          = 0x00000000,
  regk_ser_no                              = 0x00000000,
  regk_ser_odd                             = 0x00000001,
  regk_ser_off                             = 0x00000000,
  regk_ser_rw_intr_mask_default            = 0x00000000,
  regk_ser_rw_rec_baud_div_default         = 0x00000000,
  regk_ser_rw_rec_ctrl_default             = 0x00010000,
  regk_ser_rw_tr_baud_div_default          = 0x00000000,
  regk_ser_rw_tr_ctrl_default              = 0x00008000,
  regk_ser_rw_tr_dma_en_default            = 0x00000000,
  regk_ser_rw_xoff_default                 = 0x00000000,
  regk_ser_space                           = 0x00000003,
  regk_ser_stop                            = 0x00000000,
  regk_ser_yes                             = 0x00000001
};
#endif /* __ser_defs_h */
