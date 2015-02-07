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
 *
 *  Copyright (C) 2005 Mike Isely <isely@pobox.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __PVRUSB2_CONTEXT_H
#define __PVRUSB2_CONTEXT_H

#include <linux/mutex.h>
#include <linux/usb.h>
#include <linux/workqueue.h>

struct pvr2_hdw;     /* hardware interface - defined elsewhere */
struct pvr2_stream;  /* stream interface - defined elsewhere */

struct pvr2_context;        /* All central state */
struct pvr2_channel;        /* One I/O pathway to a user */
struct pvr2_context_stream; /* Wrapper for a stream */
struct pvr2_ioread;         /* Low level stream structure */

struct pvr2_context_stream {
	struct pvr2_channel *user;
	struct pvr2_stream *stream;
};

struct pvr2_context {
	struct pvr2_channel *mc_first;
	struct pvr2_channel *mc_last;
	struct pvr2_context *exist_next;
	struct pvr2_context *exist_prev;
	struct pvr2_context *notify_next;
	struct pvr2_context *notify_prev;
	struct pvr2_hdw *hdw;
	struct pvr2_context_stream video_stream;
	struct mutex mutex;
	int notify_flag;
	int initialized_flag;
	int disconnect_flag;

	/* Called after pvr2_context initialization is complete */
	void (*setup_func)(struct pvr2_context *);

};

struct pvr2_channel {
	struct pvr2_context *mc_head;
	struct pvr2_channel *mc_next;
	struct pvr2_channel *mc_prev;
	struct pvr2_context_stream *stream;
	struct pvr2_hdw *hdw;
	unsigned int input_mask;
	void (*check_func)(struct pvr2_channel *);
};

struct pvr2_context *pvr2_context_create(struct usb_interface *intf,
					 const struct usb_device_id *devid,
					 void (*setup_func)(struct pvr2_context *));
void pvr2_context_disconnect(struct pvr2_context *);

void pvr2_channel_init(struct pvr2_channel *,struct pvr2_context *);
void pvr2_channel_done(struct pvr2_channel *);
int pvr2_channel_limit_inputs(struct pvr2_channel *,unsigned int);
unsigned int pvr2_channel_get_limited_inputs(struct pvr2_channel *);
int pvr2_channel_claim_stream(struct pvr2_channel *,
			      struct pvr2_context_stream *);
struct pvr2_ioread *pvr2_channel_create_mpeg_stream(
	struct pvr2_context_stream *);

int pvr2_context_global_init(void);
void pvr2_context_global_done(void);

#endif /* __PVRUSB2_CONTEXT_H */
/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 75 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */