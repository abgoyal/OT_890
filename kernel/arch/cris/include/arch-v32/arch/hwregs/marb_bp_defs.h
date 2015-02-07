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

#ifndef __marb_bp_defs_h
#define __marb_bp_defs_h

/*
 * This file is autogenerated from
 *   file:           ../../inst/memarb/rtl/guinness/marb_top.r
 *     id:           <not found>
 *     last modfied: Fri Nov  7 15:36:04 2003
 *
 *   by /n/asic/projects/guinness/design/top/inst/rdesc/rdes2c ../../rtl/global.rmap ../../mod/modreg.rmap -base 0xb0000000 ../../inst/memarb/rtl/guinness/marb_top.r
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

/* C-code for register scope marb_bp */

/* Register rw_first_addr, scope marb_bp, type rw */
typedef unsigned int reg_marb_bp_rw_first_addr;
#define REG_RD_ADDR_marb_bp_rw_first_addr 0
#define REG_WR_ADDR_marb_bp_rw_first_addr 0

/* Register rw_last_addr, scope marb_bp, type rw */
typedef unsigned int reg_marb_bp_rw_last_addr;
#define REG_RD_ADDR_marb_bp_rw_last_addr 4
#define REG_WR_ADDR_marb_bp_rw_last_addr 4

/* Register rw_op, scope marb_bp, type rw */
typedef struct {
  unsigned int read         : 1;
  unsigned int write        : 1;
  unsigned int read_excl    : 1;
  unsigned int pri_write    : 1;
  unsigned int us_read      : 1;
  unsigned int us_write     : 1;
  unsigned int us_read_excl : 1;
  unsigned int us_pri_write : 1;
  unsigned int dummy1       : 24;
} reg_marb_bp_rw_op;
#define REG_RD_ADDR_marb_bp_rw_op 8
#define REG_WR_ADDR_marb_bp_rw_op 8

/* Register rw_clients, scope marb_bp, type rw */
typedef struct {
  unsigned int dma0  : 1;
  unsigned int dma1  : 1;
  unsigned int dma2  : 1;
  unsigned int dma3  : 1;
  unsigned int dma4  : 1;
  unsigned int dma5  : 1;
  unsigned int dma6  : 1;
  unsigned int dma7  : 1;
  unsigned int dma8  : 1;
  unsigned int dma9  : 1;
  unsigned int cpui  : 1;
  unsigned int cpud  : 1;
  unsigned int iop   : 1;
  unsigned int slave : 1;
  unsigned int dummy1 : 18;
} reg_marb_bp_rw_clients;
#define REG_RD_ADDR_marb_bp_rw_clients 12
#define REG_WR_ADDR_marb_bp_rw_clients 12

/* Register rw_options, scope marb_bp, type rw */
typedef struct {
  unsigned int wrap : 1;
  unsigned int dummy1 : 31;
} reg_marb_bp_rw_options;
#define REG_RD_ADDR_marb_bp_rw_options 16
#define REG_WR_ADDR_marb_bp_rw_options 16

/* Register r_break_addr, scope marb_bp, type r */
typedef unsigned int reg_marb_bp_r_break_addr;
#define REG_RD_ADDR_marb_bp_r_break_addr 20

/* Register r_break_op, scope marb_bp, type r */
typedef struct {
  unsigned int read         : 1;
  unsigned int write        : 1;
  unsigned int read_excl    : 1;
  unsigned int pri_write    : 1;
  unsigned int us_read      : 1;
  unsigned int us_write     : 1;
  unsigned int us_read_excl : 1;
  unsigned int us_pri_write : 1;
  unsigned int dummy1       : 24;
} reg_marb_bp_r_break_op;
#define REG_RD_ADDR_marb_bp_r_break_op 24

/* Register r_break_clients, scope marb_bp, type r */
typedef struct {
  unsigned int dma0  : 1;
  unsigned int dma1  : 1;
  unsigned int dma2  : 1;
  unsigned int dma3  : 1;
  unsigned int dma4  : 1;
  unsigned int dma5  : 1;
  unsigned int dma6  : 1;
  unsigned int dma7  : 1;
  unsigned int dma8  : 1;
  unsigned int dma9  : 1;
  unsigned int cpui  : 1;
  unsigned int cpud  : 1;
  unsigned int iop   : 1;
  unsigned int slave : 1;
  unsigned int dummy1 : 18;
} reg_marb_bp_r_break_clients;
#define REG_RD_ADDR_marb_bp_r_break_clients 28

/* Register r_break_first_client, scope marb_bp, type r */
typedef struct {
  unsigned int dma0  : 1;
  unsigned int dma1  : 1;
  unsigned int dma2  : 1;
  unsigned int dma3  : 1;
  unsigned int dma4  : 1;
  unsigned int dma5  : 1;
  unsigned int dma6  : 1;
  unsigned int dma7  : 1;
  unsigned int dma8  : 1;
  unsigned int dma9  : 1;
  unsigned int cpui  : 1;
  unsigned int cpud  : 1;
  unsigned int iop   : 1;
  unsigned int slave : 1;
  unsigned int dummy1 : 18;
} reg_marb_bp_r_break_first_client;
#define REG_RD_ADDR_marb_bp_r_break_first_client 32

/* Register r_break_size, scope marb_bp, type r */
typedef unsigned int reg_marb_bp_r_break_size;
#define REG_RD_ADDR_marb_bp_r_break_size 36

/* Register rw_ack, scope marb_bp, type rw */
typedef unsigned int reg_marb_bp_rw_ack;
#define REG_RD_ADDR_marb_bp_rw_ack 40
#define REG_WR_ADDR_marb_bp_rw_ack 40


/* Constants */
enum {
  regk_marb_bp_no                          = 0x00000000,
  regk_marb_bp_rw_op_default               = 0x00000000,
  regk_marb_bp_rw_options_default          = 0x00000000,
  regk_marb_bp_yes                         = 0x00000001
};
#endif /* __marb_bp_defs_h */
