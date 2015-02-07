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

/******************************************************************************
 *
 *	(C)Copyright 1998,1999 SysKonnect,
 *	a business unit of Schneider & Koch & Co. Datensysteme GmbH.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	The information in this file is provided "AS IS" without warranty.
 *
 ******************************************************************************/

/*
 * Synchronous Bandwidth Allocation (SBA) structs
 */
 
#ifndef _SBA_
#define _SBA_

#include "h/mbuf.h"
#include "h/sba_def.h"

#ifdef	SBA

/* Timer Cell Template */
struct timer_cell {
	struct timer_cell	*next_ptr ;
	struct timer_cell	*prev_ptr ;
	u_long			start_time ;
	struct s_sba_node_vars	*node_var ;
} ;

/*
 * Node variables
 */
struct s_sba_node_vars {
	u_char			change_resp_flag ;
	u_char			report_resp_flag ;
	u_char			change_req_flag ;
	u_char			report_req_flag ;
	long			change_amount ;
	long			node_overhead ;
	long			node_payload ;
	u_long			node_status ;
	u_char			deallocate_status ;
	u_char			timer_state ;
	u_short			report_cnt ;
	long			lastrep_req_tranid ;
	struct fddi_addr	mac_address ;
	struct s_sba_sessions 	*node_sessions ;
	struct timer_cell	timer ;
} ;

/*
 * Session variables
 */
struct s_sba_sessions {
	u_long			deallocate_status ;
	long			session_overhead ;
	u_long			min_segment_size ;
	long			session_payload ;
	u_long			session_status ;
	u_long			sba_category ;
	long			lastchg_req_tranid ;
	u_short			session_id ;
	u_char			class ;
	u_char			fddi2 ;
	u_long			max_t_neg ;
	struct s_sba_sessions	*next_session ;
} ;

struct s_sba {

	struct s_sba_node_vars	node[MAX_NODES] ;
	struct s_sba_sessions	session[MAX_SESSIONS] ;

	struct s_sba_sessions	*free_session ;	/* points to the first */
						/* free session */

	struct timer_cell	*tail_timer ;	/* points to the last timer cell */

	/*
	 * variables for allocation actions
	 */
	long	total_payload ;		/* Total Payload */
	long	total_overhead ;	/* Total Overhead */
	long	sba_allocatable ;	/* allocatable sync bandwidth */

	/*
	 * RAF message receive parameters
	 */
	long		msg_path_index ;	/* Path Type */
	long		msg_sba_pl_req ;	/* Payload Request */
	long		msg_sba_ov_req ;	/* Overhead Request */
	long		msg_mib_pl ;		/* Current Payload for this Path */
	long		msg_mib_ov ;		/* Current Overhead for this Path*/
	long		msg_category ;		/* Category of the Allocation */
	u_long		msg_max_t_neg ;		/* longest T_Neg acceptable */
	u_long		msg_min_seg_siz ;	/* minimum segement size */
	struct smt_header	*sm ;		/* points to the rec message */
	struct fddi_addr	*msg_alloc_addr ;	/* Allocation Address */

	/*
	 * SBA variables
	 */
	u_long	sba_t_neg ;		/* holds the last T_NEG */
	long	sba_max_alloc ;		/* the parsed value of SBAAvailable */	

	/*
	 * SBA state machine variables
	 */
	short	sba_next_state ;	/* the next state of the SBA */
	char	sba_command ;		/* holds the execuded SBA cmd */
	u_char	sba_available ;		/* parsed value after possible check */
} ;

#endif	/* SBA */

	/*
	 * variables for the End Station Support
	 */
struct s_ess {

	/*
	 * flags and counters
	 */
	u_char	sync_bw_available ;	/* is set if sync bw is allocated */
	u_char	local_sba_active ;	/* set when a local sba is available */
	char	raf_act_timer_poll ;	/* activate the timer to send allc req */
	char	timer_count ;		/* counts every timer function call */

	SMbuf	*sba_reply_pend ;	/* local reply for the sba is pending */
	
	/*
	 * variables for the ess bandwidth control
	 */
	long	sync_bw ;		/* holds the allocaed sync bw */
	u_long	alloc_trans_id ;	/* trans id of the last alloc req */
} ;
#endif
