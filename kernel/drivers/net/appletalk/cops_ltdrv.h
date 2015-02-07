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
 *	The firmware this driver downloads into the Localtalk card is a
 *	separate program and is not GPL'd source code, even though the Linux
 *	side driver and the routine that loads this data into the card are.
 *	
 *	It is taken from the COPS SDK and is under the following license
 *
 *	This material is licensed to you strictly for use in conjunction with
 *	the use of COPS LocalTalk adapters.
 *	There is no charge for this SDK. And no waranty express or implied
 *	about its fitness for any purpose. However, we will cheerefully
 *	refund every penny you paid for this SDK...
 *	Regards,
 *
 *	Thomas F. Divine
 *	Chief Scientist
 */


/*      cops_ltdrv.h: LocalTalk driver firmware dump for Linux.
 *
 *      Authors:
 *      - Jay Schulist <jschlst@samba.org>
 */
 

#ifdef CONFIG_COPS_TANGENT

static const unsigned char ltdrv_code[] = {
	58,3,0,50,148,10,33,143,15,62,85,119,
	190,32,9,62,170,119,190,32,3,35,24,241,
	34,146,10,249,17,150,10,33,143,15,183,237,
	82,77,68,11,107,98,19,54,0,237,176,62,
	16,237,57,51,62,0,237,57,50,237,57,54,
	62,12,237,57,49,62,195,33,39,2,50,56,
	0,34,57,0,237,86,205,30,2,251,205,60,
	10,24,169,67,111,112,121,114,105,103,104,116,
	32,40,99,41,32,49,57,56,56,45,49,57,
	57,50,44,32,80,114,105,110,116,105,110,103,
	32,67,111,109,109,117,110,105,99,97,116,105,
	111,110,115,32,65,115,115,111,99,105,97,116,
	101,115,44,32,73,110,99,46,65,108,108,32,
	114,105,103,104,116,115,32,114,101,115,101,114,
	118,101,100,46,32,32,4,4,22,40,255,60,
	4,96,10,224,6,0,7,126,2,64,11,246,
	12,6,13,0,14,193,15,0,5,96,3,192,
	1,0,9,8,62,3,211,82,62,192,211,82,
	201,62,3,211,82,62,213,211,82,201,62,5,
	211,82,62,224,211,82,201,62,5,211,82,62,
	224,211,82,201,62,5,211,82,62,96,211,82,
	201,6,28,33,180,1,14,82,237,163,194,4,
	2,33,39,2,34,64,0,58,3,0,230,1,
	192,62,11,237,121,62,118,237,121,201,33,182,
	10,54,132,205,253,1,201,245,197,213,229,42,
	150,10,14,83,17,98,2,67,20,237,162,58,
	179,1,95,219,82,230,1,32,6,29,32,247,
	195,17,3,62,1,211,82,219,82,95,230,160,
	32,10,237,162,32,225,21,32,222,195,15,3,
	237,162,123,230,96,194,21,3,62,48,211,82,
	62,1,211,82,175,211,82,237,91,150,10,43,
	55,237,82,218,19,3,34,152,10,98,107,58,
	154,10,190,32,81,62,1,50,158,10,35,35,
	62,132,190,32,44,54,133,43,70,58,154,10,
	119,43,112,17,3,0,205,137,3,62,16,211,
	82,62,56,211,82,205,217,1,42,150,10,14,
	83,17,98,2,67,20,58,178,1,95,195,59,
	2,62,129,190,194,227,2,54,130,43,70,58,
	154,10,119,43,112,17,3,0,205,137,3,195,
	254,2,35,35,126,254,132,194,227,2,205,61,
	3,24,20,62,128,166,194,222,2,221,229,221,
	33,175,10,205,93,6,205,144,7,221,225,225,
	209,193,241,251,237,77,221,229,221,33,159,10,
	205,93,6,221,225,205,61,3,195,247,2,24,
	237,24,235,24,233,230,64,40,2,24,227,24,
	225,175,50,179,10,205,208,1,201,197,33,4,
	0,57,126,35,102,111,205,51,3,193,201,62,
	1,50,179,10,34,150,10,54,0,58,179,10,
	183,200,62,14,211,82,62,193,211,82,62,10,
	211,82,62,224,211,82,62,6,211,82,58,154,
	10,211,82,62,16,211,82,62,56,211,82,62,
	48,211,82,219,82,230,1,40,4,219,83,24,
	242,62,14,211,82,62,33,211,82,62,1,211,
	82,62,9,211,82,62,32,211,82,205,217,1,
	201,14,83,205,208,1,24,23,14,83,205,208,
	1,205,226,1,58,174,1,61,32,253,205,244,
	1,58,174,1,61,32,253,205,226,1,58,175,
	1,61,32,253,62,5,211,82,62,233,211,82,
	62,128,211,82,58,176,1,61,32,253,237,163,
	27,62,192,211,82,219,82,230,4,40,250,237,
	163,27,122,179,32,243,219,82,230,4,40,250,
	58,178,1,71,219,82,230,4,40,3,5,32,
	247,219,82,230,4,40,250,205,235,1,58,177,
	1,61,32,253,205,244,1,201,229,213,35,35,
	126,230,128,194,145,4,43,58,154,10,119,43,
	70,33,181,10,119,43,112,17,3,0,243,62,
	10,211,82,219,82,230,128,202,41,4,209,225,
	62,1,55,251,201,205,144,3,58,180,10,254,
	255,202,127,4,205,217,1,58,178,1,71,219,
	82,230,1,32,6,5,32,247,195,173,4,219,
	83,71,58,154,10,184,194,173,4,58,178,1,
	71,219,82,230,1,32,6,5,32,247,195,173,
	4,219,83,58,178,1,71,219,82,230,1,32,
	6,5,32,247,195,173,4,219,83,254,133,194,
	173,4,58,179,1,24,4,58,179,1,135,61,
	32,253,209,225,205,137,3,205,61,3,183,251,
	201,209,225,243,62,10,211,82,219,82,230,128,
	202,164,4,62,1,55,251,201,205,144,3,205,
	61,3,183,251,201,209,225,62,2,55,251,201,
	243,62,14,211,82,62,33,211,82,251,201,33,
	4,0,57,94,35,86,33,2,0,57,126,35,
	102,111,221,229,34,193,10,237,83,195,10,221,
	33,171,10,205,93,6,58,185,10,50,186,10,
	58,184,10,135,50,184,10,205,112,6,254,3,
	56,16,58,185,10,135,60,230,15,50,185,10,
	175,50,184,10,24,23,58,183,10,205,112,6,
	254,3,48,13,58,185,10,203,63,50,185,10,
	62,255,50,183,10,58,185,10,50,186,10,58,
	183,10,135,50,183,10,62,32,50,187,10,50,
	188,10,6,255,219,82,230,16,32,3,5,32,
	247,205,180,4,6,40,219,82,230,16,40,3,
	5,32,247,62,10,211,82,219,82,230,128,194,
	46,5,219,82,230,16,40,214,237,95,71,58,
	186,10,160,230,15,40,32,71,14,10,62,10,
	211,82,219,82,230,128,202,119,5,205,180,4,
	195,156,5,219,82,230,16,202,156,5,13,32,
	229,16,225,42,193,10,237,91,195,10,205,252,
	3,48,7,61,202,156,5,195,197,5,221,225,
	33,0,0,201,221,33,163,10,205,93,6,58,
	188,10,61,50,188,10,40,19,58,186,10,246,
	1,50,186,10,58,183,10,246,1,50,183,10,
	195,46,5,221,225,33,1,0,201,221,33,167,
	10,205,93,6,58,184,10,246,1,50,184,10,
	58,186,10,135,246,1,50,186,10,58,187,10,
	61,50,187,10,194,46,5,221,225,33,2,0,
	201,221,229,33,0,0,57,17,4,0,25,126,
	50,154,10,230,128,50,189,10,58,189,10,183,
	40,6,221,33,88,2,24,4,221,33,150,0,
	58,154,10,183,40,49,60,40,46,61,33,190,
	10,119,35,119,35,54,129,175,50,158,10,221,
	43,221,229,225,124,181,40,42,33,190,10,17,
	3,0,205,206,4,17,232,3,27,123,178,32,
	251,58,158,10,183,40,224,58,154,10,71,62,
	7,128,230,127,71,58,189,10,176,50,154,10,
	24,166,221,225,201,183,221,52,0,192,221,52,
	1,192,221,52,2,192,221,52,3,192,55,201,
	6,8,14,0,31,48,1,12,16,250,121,201,
	33,2,0,57,94,35,86,35,78,35,70,35,
	126,35,102,105,79,120,68,103,237,176,201,33,
	2,0,57,126,35,102,111,62,17,237,57,48,
	125,237,57,40,124,237,57,41,62,0,237,57,
	42,62,64,237,57,43,62,0,237,57,44,33,
	128,2,125,237,57,46,124,237,57,47,62,145,
	237,57,48,211,68,58,149,10,211,66,201,33,
	2,0,57,126,35,102,111,62,33,237,57,48,
	62,64,237,57,32,62,0,237,57,33,237,57,
	34,125,237,57,35,124,237,57,36,62,0,237,
	57,37,33,128,2,125,237,57,38,124,237,57,
	39,62,97,237,57,48,211,67,58,149,10,211,
	66,201,237,56,46,95,237,56,47,87,237,56,
	46,111,237,56,47,103,183,237,82,32,235,33,
	128,2,183,237,82,201,237,56,38,95,237,56,
	39,87,237,56,38,111,237,56,39,103,183,237,
	82,32,235,33,128,2,183,237,82,201,205,106,
	10,221,110,6,221,102,7,126,35,110,103,195,
	118,10,205,106,10,33,0,0,34,205,10,34,
	198,10,34,200,10,33,143,15,34,207,10,237,
	91,207,10,42,146,10,183,237,82,17,0,255,
	25,34,203,10,203,124,40,6,33,0,125,34,
	203,10,42,207,10,229,205,37,3,195,118,10,
	205,106,10,229,42,150,10,35,35,35,229,205,
	70,7,193,124,230,3,103,221,117,254,221,116,
	255,237,91,152,10,35,35,35,183,237,82,32,
	12,17,5,0,42,152,10,205,91,10,242,203,
	7,42,150,10,229,205,37,3,195,118,10,237,
	91,152,10,42,200,10,25,34,200,10,42,205,
	10,25,34,205,10,237,91,203,10,33,158,253,
	25,237,91,205,10,205,91,10,242,245,7,33,
	0,0,34,205,10,62,1,50,197,10,205,5,
	8,33,0,0,57,249,195,118,10,205,106,10,
	58,197,10,183,202,118,10,237,91,198,10,42,
	205,10,205,91,10,242,46,8,237,91,205,10,
	33,98,2,25,237,91,198,10,205,91,10,250,
	78,8,237,91,198,10,42,205,10,183,237,82,
	32,7,42,200,10,125,180,40,13,237,91,205,
	10,42,198,10,205,91,10,242,97,8,237,91,
	207,10,42,205,10,25,229,205,37,3,175,50,
	197,10,195,118,10,205,29,3,33,0,0,57,
	249,195,118,10,205,106,10,58,202,10,183,40,
	22,205,14,7,237,91,209,10,19,19,19,205,
	91,10,242,139,8,33,1,0,195,118,10,33,
	0,0,195,118,10,205,126,10,252,255,205,108,
	8,125,180,194,118,10,237,91,200,10,33,0,
	0,205,91,10,242,118,10,237,91,207,10,42,
	198,10,25,221,117,254,221,116,255,35,35,35,
	229,205,70,7,193,124,230,3,103,35,35,35,
	221,117,252,221,116,253,229,221,110,254,221,102,
	255,229,33,212,10,229,205,124,6,193,193,221,
	110,252,221,102,253,34,209,10,33,211,10,54,
	4,33,209,10,227,205,147,6,193,62,1,50,
	202,10,243,221,94,252,221,86,253,42,200,10,
	183,237,82,34,200,10,203,124,40,17,33,0,
	0,34,200,10,34,205,10,34,198,10,50,197,
	10,24,37,221,94,252,221,86,253,42,198,10,
	25,34,198,10,237,91,203,10,33,158,253,25,
	237,91,198,10,205,91,10,242,68,9,33,0,
	0,34,198,10,205,5,8,33,0,0,57,249,
	251,195,118,10,205,106,10,33,49,13,126,183,
	40,16,205,42,7,237,91,47,13,19,19,19,
	205,91,10,242,117,9,58,142,15,198,1,50,
	142,15,195,118,10,33,49,13,126,254,1,40,
	25,254,3,202,7,10,254,5,202,21,10,33,
	49,13,54,0,33,47,13,229,205,207,6,195,
	118,10,58,141,15,183,32,72,33,51,13,126,
	50,149,10,205,86,7,33,50,13,126,230,127,
	183,32,40,58,142,15,230,127,50,142,15,183,
	32,5,198,1,50,142,15,33,50,13,126,111,
	23,159,103,203,125,58,142,15,40,5,198,128,
	50,142,15,33,50,13,119,33,50,13,126,111,
	23,159,103,229,205,237,5,193,33,211,10,54,
	2,33,2,0,34,209,10,58,154,10,33,212,
	10,119,58,148,10,33,213,10,119,33,209,10,
	229,205,147,6,193,24,128,42,47,13,229,33,
	50,13,229,205,191,4,193,24,239,33,211,10,
	54,6,33,3,0,34,209,10,58,154,10,33,
	212,10,119,58,148,10,33,213,10,119,33,214,
	10,54,5,33,209,10,229,205,147,6,24,200,
	205,106,10,33,49,13,54,0,33,47,13,229,
	205,207,6,33,209,10,227,205,147,6,193,205,
	80,9,205,145,8,24,248,124,170,250,99,10,
	237,82,201,124,230,128,237,82,60,201,225,253,
	229,221,229,221,33,0,0,221,57,233,221,249,
	221,225,253,225,201,233,225,253,229,221,229,221,
	33,0,0,221,57,94,35,86,35,235,57,249,
	235,233,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0
	} ;

#endif
