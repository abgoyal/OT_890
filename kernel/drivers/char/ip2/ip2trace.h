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


//
union ip2breadcrumb 
{
	struct { 
		unsigned char port, cat, codes, label;
	} __attribute__ ((packed)) hdr;
	unsigned long value;
};

#define ITRC_NO_PORT 	0xFF
#define CHANN	(pCh->port_index)

#define	ITRC_ERROR	'!'
#define	ITRC_INIT 	'A'
#define	ITRC_OPEN	'B'
#define	ITRC_CLOSE	'C'
#define	ITRC_DRAIN	'D'
#define	ITRC_IOCTL	'E'
#define	ITRC_FLUSH	'F'
#define	ITRC_STATUS	'G'
#define	ITRC_HANGUP	'H'
#define	ITRC_INTR 	'I'
#define	ITRC_SFLOW	'J'
#define	ITRC_SBCMD	'K'
#define	ITRC_SICMD	'L'
#define	ITRC_MODEM	'M'
#define	ITRC_INPUT	'N'
#define	ITRC_OUTPUT	'O'
#define	ITRC_PUTC	'P'
#define	ITRC_QUEUE	'Q'
#define	ITRC_STFLW	'R'
#define	ITRC_SFIFO	'S'
#define	ITRC_VERIFY	'V'
#define	ITRC_WRITE	'W'

#define	ITRC_ENTER	0x00
#define	ITRC_RETURN	0xFF

#define	ITRC_QUEUE_ROOM	2
#define	ITRC_QUEUE_CMD	6

