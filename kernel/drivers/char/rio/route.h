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

/****************************************************************************
 *******                                                              *******
 *******                 R O U T E     H E A D E R
 *******                                                              *******
 ****************************************************************************

 Author  : Ian Nandhra / Jeremy Rolls
 Date    :

 *
 *  (C) 1990 - 2000 Specialix International Ltd., Byfleet, Surrey, UK.
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 Version : 0.01


                            Mods
 ----------------------------------------------------------------------------
  Date     By                Description
 ----------------------------------------------------------------------------

 ***************************************************************************/

#ifndef _route_h
#define _route_h

#define MAX_LINKS 4
#define MAX_NODES 17		/* Maximum nodes in a subnet */
#define NODE_BYTES ((MAX_NODES / 8) + 1)	/* Number of bytes needed for
						   1 bit per node */
#define ROUTE_DATA_SIZE  (NODE_BYTES + 2)	/* Number of bytes for complete
						   info about cost etc. */
#define ROUTES_PER_PACKET ((PKT_MAX_DATA_LEN -2)/ ROUTE_DATA_SIZE)
					      /* Number of nodes we can squeeze
					         into one packet */
#define MAX_TOPOLOGY_PACKETS (MAX_NODES / ROUTES_PER_PACKET + 1)
/************************************************
 * Define the types of command for the ROUTE RUP.
 ************************************************/
#define ROUTE_REQUEST    0	/* Request an ID */
#define ROUTE_FOAD       1	/* Kill the RTA */
#define ROUTE_ALREADY    2	/* ID given already */
#define ROUTE_USED       3	/* All ID's used */
#define ROUTE_ALLOCATE   4	/* Here it is */
#define ROUTE_REQ_TOP    5	/* I bet you didn't expect....
				   the Topological Inquisition */
#define ROUTE_TOPOLOGY   6	/* Topology request answered FD */
/*******************************************************************
 * Define the Route Map Structure
 *
 * The route map gives a pointer to a Link Structure to use.
 * This allows Disconnected Links to be checked quickly
 ******************************************************************/
typedef struct COST_ROUTE COST_ROUTE;
struct COST_ROUTE {
	unsigned char cost;	/* Cost down this link */
	unsigned char route[NODE_BYTES];	/* Nodes thorough this route */
};

typedef struct ROUTE_STR ROUTE_STR;
struct ROUTE_STR {
	COST_ROUTE cost_route[MAX_LINKS];
	/* cost / route for this link */
	ushort favoured;	/* favoured link */
};


#define NO_LINK            (short) 5	/* Link unattached */
#define ROUTE_NO_ID        (short) 100	/* No Id */
#define ROUTE_DISCONNECT   (ushort) 0xff	/* Not connected */
#define ROUTE_INTERCONNECT (ushort) 0x40	/* Sub-net interconnect */


#define SYNC_RUP         (ushort) 255
#define COMMAND_RUP      (ushort) 254
#define ERROR_RUP        (ushort) 253
#define POLL_RUP         (ushort) 252
#define BOOT_RUP         (ushort) 251
#define ROUTE_RUP        (ushort) 250
#define STATUS_RUP       (ushort) 249
#define POWER_RUP        (ushort) 248

#define HIGHEST_RUP      (ushort) 255	/* Set to Top one */
#define LOWEST_RUP       (ushort) 248	/* Set to bottom one */

#endif

/*********** end of file ***********/
