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
 * File yam111.mcs converted to h format by mcs2h
 *
 * (C) F6FBB 1998
 *
 * Tue Aug 25 20:23:03 1998
 *
 */

static unsigned char bits_9600[]= {
0xff,0xf2,0x00,0xa5,0xad,0xff,0xfe,0x9f,0xff,0xef,0xfb,0xcb,0xff,0xdb,0xfe,0xf2,
0xff,0xf6,0xff,0x9c,0xbf,0xfd,0xbf,0xef,0x2e,0x3f,0x6f,0xf1,0xfd,0xb4,0xfd,0xbf,
0xff,0x6f,0xff,0x6f,0xff,0x0b,0xff,0xdb,0xff,0xf2,0xff,0xf6,0xff,0xff,0xff,0xff,
0xf0,0x6f,0xff,0xff,0xff,0xfe,0xff,0xfd,0xdf,0xff,0xff,0xff,0xf7,0xff,0xff,0xff,
0xfb,0xff,0xff,0xf7,0xff,0xff,0xff,0xfe,0xff,0x7f,0xf1,0xff,0xfe,0xff,0xbf,0xbf,
0xff,0xff,0xff,0xff,0xff,0xf7,0xff,0xff,0xff,0xfe,0xff,0xfe,0xff,0xff,0xff,0xf0,
0xef,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xbf,0xff,0xff,0xff,0xf7,
0xff,0xff,0xf7,0xef,0xff,0xff,0xff,0xff,0xff,0xf1,0xff,0xff,0xff,0x7e,0xff,0xff,
0xff,0xff,0xff,0xff,0xdf,0xff,0xff,0xff,0xff,0xff,0xfd,0xff,0xff,0xff,0xf0,0xdf,
0xff,0xff,0xff,0xfe,0xff,0xff,0xdf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xef,0xff,0xf3,0xfb,0xfe,0xff,0xf1,0xff,0xfd,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xdf,0xff,0xf0,0x7f,0xff,
0xff,0xff,0xfe,0xff,0xff,0xef,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xdf,0xff,0xff,0xff,0xf7,0xf1,0xff,0xff,0xff,0xdf,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xf0,0x0f,0xff,0xff,
0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xf1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf5,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0x2f,0xff,0xff,0xff,
0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xfb,0xff,0xff,0xff,0xef,0xff,0x7f,0xff,0xef,
0xff,0xef,0xff,0x7f,0xef,0xf1,0xff,0xef,0xff,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xf0,0x9f,0xff,0xff,0xff,0xfe,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xf1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0xbf,0xff,0xff,0xff,0xfe,0xff,
0xff,0xff,0xff,0xbd,0xff,0xef,0x7f,0xef,0x7f,0xfb,0xdf,0xd3,0x5a,0xfe,0xd7,0xd6,
0xf7,0x7f,0xbd,0xf1,0xbb,0x5d,0xd6,0xf7,0xfe,0x96,0xff,0xbd,0xaf,0xad,0xbf,0xef,
0x7f,0x6b,0x7f,0xfb,0xd6,0xfe,0xf7,0xff,0x10,0xef,0xff,0xff,0xff,0xfe,0xbe,0xef,
0xff,0xff,0xdb,0xff,0xf6,0xff,0xf6,0xff,0xfd,0xbf,0xfd,0xbf,0xff,0x7f,0xff,0x7f,
0xdf,0xdb,0xf1,0xfd,0x35,0xff,0x6f,0xff,0x6f,0xff,0xdb,0xff,0xcb,0xff,0xf6,0xff,
0xf2,0xfd,0xfd,0xbf,0xff,0xff,0xff,0xd0,0xef,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x55,0xff,0xcc,0xc0,0x3f,0xff,
0xff,0xf1,0x24,0xf0,0xff,0xff,0xcf,0xef,0x3f,0xff,0xf0,0xff,0xff,0xff,0xfc,0x3f,
0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0xcf,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x55,0xff,0xcc,0xc0,0x3f,0xff,0xff,
0xf1,0x00,0xf0,0xff,0xff,0xcf,0xdf,0xff,0xff,0xf0,0xff,0xff,0xff,0xfc,0x3f,0xff,
0xff,0xff,0x7d,0xff,0xff,0xf0,0xff,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xdf,0xfe,0x7f,0xdf,0xff,0xff,0xff,0xf1,
0xff,0xcf,0xff,0xf3,0xff,0x97,0xff,0xff,0x8f,0xe7,0xff,0xff,0xfc,0x71,0xff,0xff,
0xff,0xff,0xff,0xff,0xf0,0xef,0xff,0xff,0xff,0xfe,0xf5,0xff,0xbf,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xe3,0xf7,0xef,0xff,0xff,0xfc,0x7b,0xff,0xf1,0x3f,
0xff,0xef,0xff,0xcf,0xe3,0xe3,0xff,0xff,0xff,0xff,0x3f,0xff,0xff,0xff,0xbf,0xff,
0xbf,0xff,0xda,0xf0,0x7f,0xff,0xff,0xff,0xfe,0xf2,0xc0,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,
0x01,0x3c,0xf0,0xaf,0xff,0xff,0xff,0xfe,0xfd,0xbf,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xdb,0xff,0xff,0xff,0xff,0xff,0xff,0xf1,0xff,0x9f,0xff,
0xff,0xff,0xf7,0xff,0xef,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xdb,0xf0,0x7f,0xff,0xff,0xff,0xfe,0xf0,0xbb,0xdf,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xef,0xfb,0xdf,0xbf,0xf1,0xfe,0xfd,0xf7,0xff,
0xff,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x77,0xfd,0xf2,
0xf0,0x1f,0xff,0xff,0xff,0xfe,0xf8,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,
0x00,0x00,0x00,0x02,0x00,0x90,0x00,0x00,0x00,0x0c,0x01,0x00,0x00,0x04,0x24,0x00,
0x40,0x01,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x01,0xc0,0xf0,
0x4f,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xbf,0xff,0xff,0x6f,0xff,0xdf,0xff,0xd1,0xff,0xfe,0xff,0xff,0xff,0xff,
0xff,0xff,0xdf,0xff,0xfb,0xff,0xfb,0xef,0xff,0xff,0xee,0xff,0xff,0x7f,0xf0,0xdf,
0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0x8f,0xff,
0xff,0xff,0xfe,0xff,0xff,0xff,0xf5,0xad,0xff,0x69,0x2a,0xed,0x6b,0xfb,0xdf,0x3a,
0xdc,0xf4,0x96,0xee,0xb3,0x3d,0x35,0xc1,0xbb,0xdd,0xfe,0xf6,0xfe,0xd6,0xb5,0xad,
0xbf,0xa5,0xad,0x49,0x2f,0x4f,0x2b,0xda,0x5f,0xff,0xff,0xff,0xf0,0x2f,0xff,0xff,
0xff,0xfe,0xbf,0xff,0xff,0xfb,0x5b,0xf7,0xf6,0xff,0xf6,0xff,0xfd,0xbf,0xfd,0xa5,
0xf3,0x6f,0xf3,0x6e,0xfa,0x7b,0xd1,0xfd,0xb5,0x77,0x6f,0xe9,0x6f,0xff,0xdb,0xfb,
0xdb,0xdf,0xf6,0xff,0xf6,0xff,0xfd,0x3f,0xfe,0xf7,0xff,0xd0,0x4f,0xff,0xff,0xff,
0xfe,0xff,0x9f,0xff,0xff,0x0f,0xff,0xc0,0x3f,0x9c,0x03,0xff,0xff,0x8b,0xa5,0xfe,
0x80,0x3e,0xc2,0xbf,0xac,0xb1,0x24,0xff,0xff,0xff,0xff,0xff,0xff,0x0f,0xff,0xa3,
0xff,0xfd,0x6b,0xff,0xff,0xf0,0xa5,0xff,0xff,0xff,0xf0,0xaf,0xff,0xff,0xff,0xfe,
0xff,0xff,0xff,0xff,0x0f,0xff,0xc0,0x3f,0xd4,0x6b,0xff,0xff,0xdb,0xff,0xfe,0x86,
0xbf,0xc2,0xbf,0x30,0xa1,0x24,0xff,0xff,0xff,0xff,0xcc,0xff,0x0f,0xff,0xa3,0xff,
0x05,0x6b,0xff,0xff,0xf0,0xa5,0xff,0xff,0xff,0xf0,0x7f,0xff,0xff,0xff,0xfe,0xff,
0xff,0xff,0xfb,0xc7,0xff,0xc4,0xff,0xff,0x7f,0xff,0xec,0xfe,0x7f,0xdf,0xd8,0xb9,
0x47,0xfc,0x36,0xc1,0xdf,0xff,0xff,0xf9,0xff,0xf3,0xff,0xf7,0xff,0xfc,0xff,0xfd,
0x3f,0xff,0xff,0xff,0x3f,0xff,0xff,0xff,0xf0,0x7f,0xff,0xff,0xff,0xfe,0xf5,0xff,
0xff,0xff,0xff,0xfe,0xff,0xff,0x7e,0xbd,0x3f,0xff,0x2b,0xfe,0x2f,0xf5,0xa3,0xfc,
0x5b,0xfe,0x61,0x9f,0x7f,0xef,0xff,0xff,0xa7,0xfb,0xff,0xff,0xfa,0xfe,0xff,0x33,
0xf1,0xff,0xbf,0xff,0xff,0xff,0xfa,0xf0,0x7f,0xff,0xff,0xff,0xfe,0xf1,0xc0,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x30,0x24,0x04,
0x00,0x01,0x00,0x80,0x40,0x00,0x08,0x00,0x00,0x00,0x02,0x01,0x01,0x00,0x02,0x00,
0x00,0x00,0x00,0x00,0x01,0x3d,0xf0,0x2f,0xff,0xff,0xff,0xfe,0xfd,0xbd,0xff,0xfd,
0xff,0xff,0xff,0xff,0xff,0xfb,0xff,0xff,0x7f,0xf6,0xef,0xbf,0xf7,0xff,0x73,0xeb,
0xf1,0xff,0xff,0xff,0xdf,0xff,0xff,0xff,0xff,0xff,0xf9,0xff,0xfd,0xfe,0xff,0xff,
0xff,0xff,0xff,0xff,0xd9,0xf0,0xdf,0xff,0xff,0xff,0xfe,0xf0,0xbf,0x7f,0xff,0xff,
0xff,0x7f,0xff,0xff,0xde,0xff,0xff,0xef,0xdd,0xde,0x77,0xf2,0xfb,0xed,0xe7,0xf1,
0x73,0xfd,0xfd,0xdf,0xff,0x7d,0xbe,0xdf,0xff,0xfb,0xff,0xef,0xff,0xef,0xff,0xff,
0xff,0xff,0xff,0xd0,0xf0,0xbf,0xff,0xff,0xff,0xfe,0xf8,0x30,0x20,0x02,0x00,0x22,
0x40,0xc0,0x00,0x00,0x00,0x08,0x00,0x02,0x41,0x02,0x12,0x00,0x21,0x87,0x81,0x00,
0x00,0x80,0x04,0x0b,0x28,0x01,0xb0,0x00,0x82,0x00,0x40,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0xc1,0xf0,0xdf,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xfd,0xff,
0xf7,0xff,0xfe,0x7f,0xed,0x79,0xff,0xde,0xeb,0x7f,0x74,0xf7,0xf7,0xe1,0xf9,0xff,
0xf6,0x5f,0x7f,0xff,0xff,0xff,0xd7,0xdb,0xef,0xff,0xbb,0xff,0xff,0xff,0xcc,0xff,
0xff,0xff,0xf0,0xcf,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf1,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xf0,0x0f,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xfd,0x3d,0xcd,0x49,0x7f,0x6f,
0x2b,0xba,0x5c,0xd2,0xda,0xf6,0xf3,0x3e,0xf7,0xff,0xbd,0xf1,0xfa,0xdf,0xfe,0xf7,
0xcc,0xf6,0xbb,0xa5,0xb3,0xad,0xbf,0x6f,0x7d,0x6f,0x6b,0xdb,0xdf,0xbd,0xff,0xfe,
0xb0,0x5f,0xff,0xff,0xff,0xfe,0xbf,0xff,0xff,0xfb,0xdb,0x57,0xf6,0xfe,0x9f,0xd5,
0xb7,0xff,0xaf,0xe5,0x3f,0xff,0xff,0x6f,0xff,0xdb,0xf1,0xfd,0xbf,0xff,0x6f,0x69,
0x6c,0xdf,0xda,0xdf,0xcb,0xff,0xf6,0xff,0x76,0xfd,0xfd,0xbf,0xff,0xff,0xff,0xd0,
0x3f,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xfd,0xbd,0x08,0x03,0x89,0x4f,0x5a,
0x0f,0xf0,0xff,0xf8,0xbf,0xff,0xff,0xff,0xff,0xf1,0x5a,0xff,0xff,0xff,0xff,0xf3,
0xfa,0xa0,0xf0,0xf2,0xbf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0xff,
0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xfc,0xfd,0x00,0x6b,0xff,0xff,0x5a,0x0f,
0xf0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf1,0x5a,0xff,0xff,0xff,0xff,0xb3,0xf5,
0x50,0xf0,0xf0,0xff,0xff,0xff,0xd7,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0x7f,0xff,
0xff,0xff,0xfe,0xff,0xff,0xff,0xfd,0xbc,0xff,0xe4,0xe7,0x71,0xff,0xf9,0xc4,0xf4,
0x7f,0x7f,0xcf,0xff,0xff,0xff,0xff,0xf1,0xff,0xff,0xff,0xfb,0xf7,0x73,0xbf,0x14,
0xff,0xe6,0xff,0xff,0xe1,0x7d,0xff,0xff,0xe7,0xff,0xff,0xff,0xf0,0x3f,0xff,0xff,
0xff,0xfe,0xf5,0xff,0xff,0xfe,0xd2,0xfa,0xff,0xc4,0xf4,0x5c,0xbf,0xfa,0xff,0xff,
0xec,0x7e,0xbf,0xff,0xff,0xff,0xf1,0xff,0xff,0xef,0xff,0xff,0x6b,0xdb,0xff,0xdf,
0xf9,0xfb,0xbf,0xff,0xf1,0xff,0xbf,0xff,0xff,0xff,0xfb,0xf0,0xbf,0xff,0xff,0xff,
0xfe,0xf3,0xc0,0x00,0x02,0x00,0x00,0x00,0x00,0x82,0x00,0x00,0x00,0x00,0x80,0x00,
0x00,0x00,0x00,0x40,0x00,0x01,0x00,0x00,0x00,0x01,0x08,0x20,0x00,0x00,0x00,0x00,
0x01,0x00,0x01,0x00,0x00,0x80,0x02,0x00,0x01,0x3c,0xf0,0x5f,0xff,0xff,0xff,0xfe,
0xfd,0xbf,0xff,0xff,0xff,0xdf,0xff,0xff,0xff,0xff,0x7f,0xff,0xdf,0xff,0xef,0xff,
0xff,0xff,0xff,0xff,0xf1,0xff,0xff,0xff,0xff,0xff,0xf7,0xff,0xfb,0xff,0xfd,0xff,
0xff,0xff,0xff,0xff,0xff,0xfd,0xff,0xff,0xc3,0xf0,0xaf,0xff,0xff,0xff,0xfe,0xf0,
0xff,0xdf,0xff,0xff,0xf7,0x23,0xff,0xff,0xfd,0xff,0xef,0xff,0xfe,0x7f,0x7d,0xf7,
0xfe,0xff,0x7f,0x71,0xff,0xfb,0x7f,0xff,0xff,0xff,0x6e,0xfd,0xf7,0xfd,0xff,0xbf,
0xff,0xbf,0xf9,0xfd,0xff,0xdf,0xef,0xf0,0xf0,0xaf,0xff,0xff,0xff,0xfe,0xf8,0x30,
0x40,0x01,0x00,0x83,0x00,0x00,0x00,0x0c,0x06,0x08,0x04,0x26,0x26,0x00,0x00,0x06,
0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x04,0x00,0x70,0x08,0x80,0x00,0x20,0x01,0x20,
0x00,0x02,0x00,0x30,0x00,0x00,0xc1,0xf0,0x5f,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,
0xff,0xff,0x7b,0x3f,0xf7,0xff,0xd7,0xfe,0xfe,0xfb,0xfe,0x3b,0xfe,0xbd,0xff,0x2f,
0xff,0x71,0xff,0xfb,0x7f,0xe7,0xff,0xf9,0xef,0xff,0xd7,0xfa,0xff,0xb7,0xbb,0xfe,
0xff,0xff,0x74,0xff,0xf7,0xff,0xf0,0xcf,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xf1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xf0,0x8f,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xb5,
0xbd,0x6f,0x7c,0xeb,0x7f,0xfb,0xdb,0xd3,0x4b,0xee,0xd6,0xf6,0xb7,0xfd,0xac,0xa1,
0xfb,0xdf,0xfe,0xf7,0xf4,0x96,0xbd,0xb4,0xc5,0xa5,0xaf,0x6f,0x69,0x4f,0x7f,0xba,
0xdb,0xff,0xff,0xff,0xf0,0x3f,0xff,0xff,0xff,0xfe,0xbf,0xff,0xff,0xff,0xdb,0xff,
0xf6,0xff,0xf6,0xff,0xbd,0xbf,0xa5,0xbf,0xff,0x7d,0x7f,0xef,0xff,0xfb,0xf1,0xfd,
0xbf,0xff,0x6f,0xff,0x6b,0x7a,0xdb,0xff,0xdb,0xdf,0xf6,0xfe,0xb6,0xfd,0xfd,0xbf,
0xfe,0xf7,0xff,0xd0,0xef,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xf4,0x2f,0xff,
0xfc,0x43,0x6b,0xff,0xff,0xff,0x0d,0xff,0xfc,0x33,0x3f,0xf0,0x5f,0xf1,0xff,0xff,
0xff,0xff,0xf9,0xde,0xf0,0x4c,0xfe,0x77,0xaf,0xff,0xff,0xef,0xff,0xf0,0xff,0xdb,
0xff,0x5f,0xf0,0xef,0xff,0xff,0xff,0xfe,0xff,0xfe,0xf7,0xff,0xf0,0x2f,0xff,0xfd,
0x43,0x7f,0xff,0xff,0xf1,0x0f,0xff,0xfc,0x33,0x3f,0xff,0xaf,0xf1,0xff,0xff,0xff,
0xff,0xf6,0xd7,0xff,0xbc,0xfd,0xbd,0xff,0xff,0xff,0xff,0xff,0xf0,0xff,0xff,0xff,
0xff,0xf0,0xef,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xfc,0xff,0xff,0xfb,0xf1,
0xbf,0xff,0xf9,0xfd,0xcf,0xf2,0x70,0xff,0x1f,0x9f,0xf3,0xf1,0xff,0xff,0xff,0xff,
0xfc,0xf7,0xff,0x13,0x9f,0xfc,0xff,0xff,0x84,0xf7,0xff,0xff,0x47,0xff,0xff,0xff,
0xf0,0xbf,0xff,0xff,0xff,0xfe,0xf5,0xff,0xff,0xff,0xf1,0xfc,0xff,0xfe,0xfe,0x79,
0x3f,0xff,0x1d,0x46,0xcf,0xff,0xcf,0xfc,0x7b,0xff,0xf1,0xff,0xff,0xff,0xff,0xed,
0xf3,0xab,0xff,0xcb,0xff,0xf8,0xff,0xfc,0xf5,0xff,0xbf,0xff,0xff,0xff,0xfa,0xf0,
0x8f,0xff,0xff,0xff,0xfe,0xf3,0xc2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,
0x00,0x00,0x20,0x00,0x20,0x00,0x00,0x04,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x20,
0x0c,0x00,0x00,0x04,0x01,0x00,0x01,0x00,0x00,0x80,0x00,0x00,0x01,0x3c,0xf0,0x7f,
0xff,0xff,0xff,0xfe,0xfd,0xbf,0xff,0xff,0xfd,0xfe,0xff,0xff,0xff,0xff,0xfe,0xff,
0xdf,0xff,0xff,0xf7,0xff,0xff,0xff,0xef,0xf1,0xff,0xff,0xff,0xff,0xff,0xff,0xeb,
0xff,0xdf,0xff,0xff,0xfb,0xf7,0x7f,0xff,0xfe,0xff,0xff,0xbf,0xdb,0xf0,0xff,0xff,
0xff,0xff,0xfe,0xf0,0xff,0xff,0xff,0xff,0xff,0xdf,0xff,0xff,0xff,0x7f,0xf7,0xff,
0xbf,0xbf,0xcf,0xff,0xff,0xff,0x3e,0xf1,0x7f,0xff,0xff,0xef,0xff,0xff,0xff,0xfe,
0xff,0xfd,0xff,0xbf,0xbd,0xfe,0xff,0xfb,0xf7,0xdf,0xfb,0xd0,0xf0,0x9f,0xff,0xff,
0xff,0xfe,0xf8,0x30,0x20,0x00,0x40,0x01,0x80,0xc0,0x30,0x00,0x00,0x20,0x00,0x10,
0x50,0x88,0x20,0x00,0x00,0x13,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,
0x00,0x00,0x01,0x80,0x08,0x00,0x00,0xa0,0x00,0x10,0xc1,0xf0,0xef,0xff,0xff,0xff,
0xfe,0xfd,0xef,0x7f,0xff,0xff,0xbf,0xff,0xf7,0xff,0xef,0xfb,0xfd,0x77,0xef,0xbf,
0xf7,0x7f,0xff,0xff,0xbf,0xd1,0x7f,0xff,0xff,0xf7,0xff,0xff,0xff,0xff,0xaf,0xff,
0xdf,0xf7,0xfb,0xff,0xfd,0xff,0xfc,0xff,0xfd,0xff,0xf0,0xff,0xff,0xff,0xff,0xfe,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xf1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0x5f,0xff,0xff,0xff,0xfe,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xdf,0xff,0xff,0xff,
0xff,0xff,0xff,0xf1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0x3f,0xff,0xff,0xff,0xfe,0xdd,0xff,
0xff,0xff,0xa5,0xfd,0x6f,0x7d,0x6d,0x7f,0x52,0xdf,0x5a,0x4b,0xee,0xb6,0xee,0xf2,
0xbb,0xac,0xa1,0x5b,0x4d,0xd6,0xf7,0xfe,0xb2,0xbd,0x35,0xb5,0xb5,0xdd,0x6f,0x7f,
0xe9,0x5f,0x52,0xdf,0xbd,0xff,0xff,0xf0,0xdf,0xff,0xff,0xff,0xfe,0xbf,0xff,0xff,
0xff,0xdb,0xfe,0xf6,0xff,0xf6,0xff,0xfd,0xbf,0xfd,0xb5,0xbf,0xf9,0x7f,0x6f,0xff,
0xdb,0xf1,0xfd,0xbf,0xff,0x6f,0xff,0x69,0x7f,0xdb,0xff,0xd3,0xff,0xf6,0xfe,0xf2,
0xff,0xad,0xbf,0xff,0xff,0xff,0xd0,0xdf,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xf5,
0x30,0x0f,0xff,0xff,0xfd,0x6b,0xca,0xff,0xf0,0x0f,0xd6,0xbf,0xcf,0x3f,0xff,0xff,
0xf1,0xff,0xff,0xff,0xca,0xfe,0xbf,0xff,0xf0,0x05,0xaf,0x0f,0xff,0xfc,0xf0,0xcf,
0xf0,0xff,0xff,0xff,0xff,0xf0,0xef,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xf5,0x30,
0x0f,0xff,0xff,0xfc,0x3f,0xca,0xff,0x0f,0x0f,0xd6,0xbf,0xff,0xff,0xf5,0x5f,0xf1,
0xff,0x8b,0xff,0xc3,0xff,0xff,0xff,0xff,0xff,0xff,0x0f,0xff,0xfc,0xf0,0xcf,0xf0,
0xff,0xff,0xff,0xff,0xf0,0x3f,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xcf,0xff,
0xff,0xbf,0x9f,0x3f,0xfe,0xfc,0xff,0x4f,0xff,0xff,0xff,0xff,0xff,0xf7,0xf1,0xff,
0xdf,0xfe,0x7e,0x3f,0x9f,0xf4,0xfc,0x7f,0xfc,0xff,0xff,0x3f,0xff,0x3f,0xfe,0x3f,
0xff,0xff,0xff,0xf0,0x4f,0xff,0xff,0xff,0xfe,0xf5,0xff,0xff,0xfb,0xff,0xfe,0xff,
0xff,0xff,0xff,0xbf,0xfb,0xff,0xf8,0xed,0xff,0x8f,0xff,0xbb,0xff,0xb1,0xf3,0xef,
0x8f,0xf7,0xff,0xff,0xdb,0xff,0xff,0xff,0xef,0xbf,0xfd,0x79,0xbf,0xbf,0xff,0xff,
0xff,0xfb,0xf0,0xdf,0xff,0xff,0xff,0xfe,0xf3,0xc0,0x00,0x00,0x00,0x04,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x04,0x08,0x08,0x01,0x01,0x00,0x90,
0x00,0x00,0x00,0x04,0x00,0x08,0x00,0x00,0x00,0x00,0x08,0x00,0x04,0x00,0x00,0x01,
0x3c,0xf0,0xdf,0xff,0xff,0xff,0xfe,0xfd,0xbf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0x9f,0xff,0xaf,0xdf,0xff,0xff,0xff,0xf1,0xff,0xff,0xff,0xff,
0xbf,0xef,0xff,0xff,0xff,0xed,0xff,0xff,0xff,0xef,0xff,0xbf,0xff,0xff,0xff,0xc3,
0xf0,0x3f,0xff,0xff,0xff,0xfe,0xf0,0xff,0xfd,0xff,0xff,0xff,0xfb,0xff,0xbb,0xff,
0xff,0xff,0x7f,0xf6,0xff,0x7f,0xfb,0xfd,0xed,0xff,0xf1,0xff,0xfe,0x7f,0xff,0xff,
0xff,0x5f,0xff,0xf7,0xff,0x7e,0xff,0xfd,0xff,0xef,0xff,0xff,0xff,0xef,0xf0,0xf0,
0x8f,0xff,0xff,0xff,0xfe,0xf8,0x30,0x80,0x00,0x04,0x00,0x00,0x40,0x02,0x00,0x03,
0x00,0x05,0x04,0x20,0x00,0x00,0x01,0xd0,0x00,0x81,0x00,0x20,0x04,0x04,0x00,0x00,
0x81,0x04,0x08,0x80,0x10,0x00,0xc0,0x00,0x00,0x00,0x20,0x00,0x08,0xc1,0xf0,0x6f,
0xff,0xff,0xff,0xfe,0xff,0xff,0x7f,0xff,0xff,0xff,0xff,0xf3,0xfd,0xff,0xed,0xfc,
0xff,0xff,0x9f,0xfb,0xfd,0xff,0xff,0xff,0xf1,0xff,0xff,0x7f,0xfb,0x3e,0xff,0x9f,
0xff,0xff,0xff,0xff,0xfd,0xf9,0xff,0xff,0xff,0xfd,0xff,0xff,0xff,0xf0,0x6f,0xff,
0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0xcf,0xff,0xff,
0xff,0xfe,0xff,0xff,0xff,0xfd,0xbd,0xff,0xef,0x7c,0xeb,0x7f,0xfb,0xdb,0xfa,0xdc,
0xee,0xf7,0xf6,0xd7,0xf5,0x2d,0xa1,0xbb,0xdd,0xee,0xf7,0x54,0xf7,0xfb,0x2c,0xb5,
0xb4,0xbd,0x6b,0x6f,0xef,0x6f,0xbb,0xdf,0xff,0xff,0xff,0xf0,0x1f,0xff,0xff,0xff,
0xfe,0xbf,0xff,0xff,0xff,0xfb,0xff,0xf6,0xff,0xf6,0xff,0xfd,0xbf,0xff,0xbf,0xef,
0x6f,0xff,0x6f,0xfa,0xdb,0xf1,0xc5,0xbd,0xf5,0x6f,0xff,0x6f,0xca,0xdb,0xff,0xdb,
0xfb,0xf6,0x97,0xf6,0xff,0xfd,0xbf,0xfe,0xf7,0xff,0xd0,0x9f,0xff,0xff,0xff,0xfe,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x8b,0x7f,0xff,0xff,0xe7,0x63,0xff,0xff,
0xff,0xfc,0x77,0xdf,0xf1,0xdb,0xff,0xd6,0xa8,0x3f,0xff,0xff,0x08,0x2f,0xf0,0xff,
0xc3,0xff,0xeb,0xff,0xff,0xff,0xff,0xff,0x5f,0xf0,0xef,0xff,0xff,0xff,0xfe,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x8b,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xfc,0xff,0xcf,0xf1,0xdb,0xff,0xd6,0xa8,0x3f,0xff,0xff,0x08,0x2f,0xf0,0xff,0xc3,
0xff,0xeb,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0x5f,0xff,0xff,0xff,0xfe,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xf5,0xbf,0xff,0xca,0xff,0x9f,0xff,0xfa,0xb9,0xe7,
0x9f,0xf3,0x81,0xff,0xff,0xfc,0x73,0xd7,0xff,0xff,0x77,0xff,0xfd,0xff,0xfc,0xff,
0xff,0xff,0xff,0xcf,0xff,0xff,0xff,0xf0,0x1f,0xff,0xff,0xff,0xfe,0xf5,0xff,0xff,
0xff,0xf7,0xde,0xff,0xfe,0x7e,0xff,0xbf,0xff,0xbf,0xf1,0xb3,0xff,0xff,0xe3,0xfb,
0xff,0xe1,0x1f,0x7f,0xff,0xf8,0x78,0xff,0xfb,0x1e,0xff,0xf7,0xfe,0xe7,0xff,0xff,
0xff,0xbf,0xff,0xff,0xff,0xfa,0xf0,0x4f,0xff,0xff,0xff,0xfe,0xf3,0xc0,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0x00,0x00,0x00,0x04,0x00,
0x01,0x80,0x40,0x40,0x20,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
0x80,0x00,0x00,0x01,0x3c,0xf0,0xaf,0xff,0xff,0xff,0xfe,0xfd,0xbf,0xff,0xfb,0xff,
0xff,0xff,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xef,0xf7,0xf1,
0xfd,0xff,0xff,0xff,0xdf,0xff,0xef,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0xff,
0xff,0xff,0xff,0xdb,0xf0,0x8f,0xff,0xff,0xff,0xfe,0xf0,0xff,0xdf,0xff,0xff,0x7f,
0xff,0xff,0xff,0xbe,0xd7,0xff,0xed,0xbd,0x7e,0xbf,0xfe,0xf6,0x7f,0xbf,0x71,0xff,
0xff,0xda,0xff,0xf9,0xff,0xbf,0x7f,0xfe,0xff,0x6f,0x7f,0xff,0xff,0xff,0xff,0xff,
0x7f,0xff,0xd0,0xf0,0xcf,0xff,0xff,0xff,0xfe,0xf8,0x30,0x42,0x00,0x00,0x00,0x00,
0x80,0xc1,0x00,0x00,0x90,0x00,0xc4,0x00,0x00,0x12,0x20,0x43,0x22,0x81,0x84,0x00,
0x00,0x14,0x00,0x01,0x00,0x08,0x80,0x00,0x02,0x00,0x02,0x00,0x04,0x02,0x00,0x00,
0x10,0xc1,0xf0,0x1f,0xff,0xff,0xff,0xfe,0xff,0xff,0xfd,0xff,0xff,0xdd,0xfe,0xff,
0xb6,0x76,0xe5,0xbc,0xf9,0xf7,0xaf,0x5f,0xbf,0xfc,0xdf,0xcf,0xf1,0xff,0xef,0x79,
0xff,0xbd,0xff,0xef,0xff,0xff,0xf7,0x6f,0x5f,0xff,0xff,0xfd,0xef,0xef,0xbf,0xff,
0xff,0xf0,0x9f,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf1,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xf0,0xff,0xff,0xff,0xff,0xfe,0xdb,0xff,0xff,0xfd,0x2d,0xff,0x69,0x2a,0xef,0x77,
0xbb,0xdd,0x5a,0xdf,0xf6,0xf6,0xd6,0xf7,0x7d,0xbd,0xd1,0xb2,0x4a,0xd6,0xb2,0xbe,
0x97,0xf5,0xbd,0xb3,0xad,0xff,0xef,0x7f,0x69,0x6b,0xfb,0xdf,0xff,0xff,0xff,0xf0,
0x2f,0xff,0xff,0xff,0xfe,0xbf,0xff,0xff,0xff,0xdb,0xff,0xf6,0xfe,0x9f,0xd4,0xbf,
0xed,0xaf,0xff,0x6b,0x6f,0xf7,0xff,0xdd,0xdb,0x31,0xfd,0xbf,0xff,0x6f,0x7f,0xff,
0xff,0xdb,0xff,0xcb,0xdf,0xf6,0xff,0xf6,0xff,0xfd,0xbf,0xfe,0xf7,0xff,0xd0,0x8f,
0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xfd,0x1f,0xff,0x46,0x2f,0x9f,0xff,0xff,0xff,
0xa5,0xff,0xff,0xff,0xdf,0xb7,0xff,0xff,0xf1,0xff,0xff,0xff,0xf7,0xe9,0x6a,0xbf,
0xff,0xff,0xfd,0xff,0xff,0xfd,0x55,0x57,0xff,0xff,0xff,0xff,0xaf,0xf0,0x4f,0xff,
0xff,0xff,0xfe,0xfe,0xdf,0xff,0xfd,0x1f,0xff,0x46,0x2f,0x9f,0xff,0xff,0xff,0xa5,
0xff,0xff,0xff,0xc0,0x37,0xff,0xff,0xf1,0x99,0x8e,0xdc,0x7f,0xe9,0x6a,0xbf,0xff,
0xf0,0x0f,0xff,0xff,0xfd,0x55,0x57,0xff,0xff,0xff,0xff,0xff,0xf0,0x0f,0xff,0xff,
0xff,0xfe,0xff,0xff,0xff,0xff,0x07,0xff,0xc0,0xbe,0xff,0xff,0xcf,0xef,0x9f,0xff,
0xff,0xfb,0xff,0xe7,0xff,0xff,0xa1,0xe3,0xce,0x3c,0x58,0x3f,0xf3,0xff,0xfd,0xef,
0xf9,0xff,0xff,0xf7,0xf1,0x7f,0xff,0xcb,0xff,0xff,0xff,0xf0,0x2f,0xff,0xff,0xff,
0xfe,0xf5,0x7f,0xff,0xf0,0xff,0xfe,0xff,0xc4,0x75,0xe7,0xb9,0xff,0xff,0xff,0xef,
0xff,0xc7,0x37,0x3b,0xff,0xf0,0x13,0x9e,0x0f,0xf4,0xff,0xfe,0xfb,0xff,0xff,0xf9,
0xfc,0xff,0xff,0xff,0xff,0xbf,0xff,0xff,0xff,0xfa,0xf0,0xef,0xff,0xff,0xff,0xfe,
0xf3,0xc0,0x01,0x00,0x00,0x02,0x00,0x02,0x22,0x00,0x00,0xc0,0x40,0x00,0x40,0x00,
0x04,0x08,0x04,0x0a,0x01,0x01,0x10,0x20,0x20,0x00,0x00,0x04,0x08,0x08,0x04,0x00,
0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x01,0x3c,0xf0,0xcf,0xff,0xff,0xff,0xfe,0xfd,
0x3f,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0xff,0x7f,0xff,0x7f,0xff,0xcf,0x9d,0xff,
0xff,0xf7,0xfd,0xf1,0xff,0xff,0xff,0xee,0xbf,0xff,0xff,0xff,0xff,0xfe,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xdb,0xf0,0x6f,0xff,0xff,0xff,0xfe,0xf0,0xff,
0xff,0xff,0xf7,0xf7,0xff,0xff,0xfe,0xbf,0xf7,0xff,0xff,0x5b,0xff,0xbf,0xf7,0xff,
0xfd,0x7f,0x71,0xfd,0xff,0xed,0xf7,0xfe,0xef,0xff,0xff,0x7f,0xff,0xff,0xff,0xff,
0xff,0xff,0xef,0xff,0x7f,0xff,0xd0,0xf0,0xff,0xff,0xff,0xff,0xfe,0xf8,0x30,0x11,
0x00,0x48,0x60,0x40,0x82,0x60,0x24,0x60,0x00,0xcc,0x00,0x80,0x04,0x01,0x00,0x00,
0x14,0x01,0x0c,0x04,0x00,0x30,0x00,0x00,0x00,0x08,0x08,0x00,0x01,0x00,0xc2,0x00,
0x00,0x02,0x00,0x80,0x00,0xc1,0xf0,0x5f,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,
0xf7,0x7b,0xff,0xf3,0xeb,0xbf,0xff,0xf7,0xff,0xff,0xff,0xe7,0x5d,0x3f,0xff,0xf6,
0xd1,0xfd,0xff,0xeb,0xf7,0x3d,0xff,0xff,0xff,0x5f,0xff,0x7f,0x7f,0xf3,0xff,0xff,
0xef,0xfd,0xbf,0xff,0xff,0xf0,0x5f,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf1,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xf0,0xdf,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xf5,0xb5,0xdf,
0x6f,0x7d,0x69,0x7f,0xfb,0xdf,0x52,0x5f,0xf6,0xf7,0xfe,0xf6,0xf3,0xbd,0xb1,0xda,
0xcd,0xfe,0xf6,0xee,0xd2,0xbd,0xa5,0xaf,0xbd,0xff,0x6f,0x7c,0xeb,0x2b,0xfa,0xda,
0xff,0xfe,0xdf,0xf0,0x4f,0xff,0xff,0xff,0xfe,0xbf,0xff,0xff,0xff,0xdb,0xff,0xf6,
0xff,0xf6,0xff,0xbd,0xbf,0xcd,0xbf,0xeb,0x6f,0xf7,0x6f,0xdf,0xdb,0x51,0xfd,0xbd,
0xff,0x6f,0xff,0x6f,0xfb,0x5b,0xff,0xdb,0xff,0xf6,0xfe,0xf6,0xfd,0xfd,0xbf,0xfe,
0xf7,0xff,0xd0,0xff,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xfa,0x50,0xff,0xff,0xff,
0xf0,0x6f,0xff,0xff,0xf0,0x96,0xff,0xff,0xc6,0x2b,0xff,0xff,0xf1,0xfc,0xff,0xff,
0xf7,0xdb,0xc3,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0xc1,0x4f,0xc3,0xff,0xff,0xff,
0xaf,0xf0,0x9f,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xf5,0xa0,0xff,0xff,0xff,0xf0,
0x6f,0xff,0xff,0xf0,0x96,0xff,0xff,0xc6,0x2b,0xff,0xff,0xf1,0x5a,0xff,0xff,0xff,
0xf3,0xc3,0xff,0x00,0xff,0xff,0xff,0xff,0xff,0xc1,0x4f,0xc3,0xff,0xff,0xff,0xff,
0xf0,0xcf,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xfc,0xff,0xff,0x9f,0xf0,0x7f,
0xff,0xf9,0xfc,0x4f,0xf3,0xff,0x27,0xeb,0xff,0xfc,0x81,0xfc,0x7f,0xfe,0x7b,0xff,
0xf7,0xff,0x12,0x7f,0xff,0xff,0xff,0xff,0x18,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,
0x7f,0xff,0xff,0xff,0xfe,0xf5,0xff,0xff,0xff,0xdf,0xfe,0xff,0xfc,0x7e,0x7f,0xbf,
0xff,0xff,0xaf,0xef,0xff,0xdf,0xdf,0xfb,0xff,0xf1,0xc3,0xfe,0x6f,0xf1,0xcf,0x3f,
0xfb,0xff,0xff,0xcf,0xfe,0xff,0xff,0xfe,0x7f,0xbf,0xff,0xff,0xbf,0xfa,0xf0,0xdf,
0xff,0xff,0xff,0xfe,0xf3,0xc0,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x01,0x00,0x00,
0x20,0x00,0x01,0x00,0x10,0x00,0x00,0x00,0x01,0x00,0x02,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x02,0x00,0x00,0x80,0x00,0x02,0x80,0x00,0x02,0x3c,0xf0,0x2f,0xff,
0xff,0xff,0xfe,0xfd,0xbf,0xff,0xfb,0xfd,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xf5,0xf1,0xff,0x7f,0xff,0xff,0xff,0xff,0xef,0xff,
0xff,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xdb,0xf0,0x2f,0xff,0xff,
0xff,0xfe,0xf0,0xff,0xff,0xff,0xfb,0xff,0xbf,0xff,0xff,0xff,0xff,0xf7,0xbf,0xfb,
0xff,0xff,0xff,0xdf,0xf7,0xff,0xf1,0xf7,0xbf,0xfb,0xff,0xff,0xff,0x7f,0xde,0xff,
0xff,0xff,0xff,0xff,0xff,0xed,0xf7,0xff,0xff,0x7f,0xd0,0xf0,0x3f,0xff,0xff,0xff,
0xfe,0xf8,0x30,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0xe0,0x00,0x00,0x80,
0x20,0x01,0x01,0x92,0x00,0x01,0x01,0x00,0xe0,0x1c,0x60,0x20,0x30,0x08,0x08,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0xc1,0xf0,0x6f,0xff,0xff,0xff,0xfe,
0xff,0xff,0xff,0xff,0xff,0xdb,0xfe,0xff,0xff,0xdf,0xff,0xfc,0x7f,0xfb,0xbf,0xff,
0xff,0xff,0xff,0xff,0xf1,0xf6,0xff,0xf7,0x7e,0x3f,0xff,0x7f,0xff,0xff,0xff,0xf7,
0xff,0xff,0xff,0xed,0xff,0xdf,0xff,0xb7,0xff,0xf0,0x3f,0xff,0xff,0xff,0xfe,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xf1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0xff,0xff,0xff,0xff,0xfe,0xff,0xff,
0xff,0xff,0xff,0xff,0xdf,0xff,0xff,0xff,0xdf,0xff,0xff,0xff,0xff,0xbf,0xff,0xdf,
0x57,0xef,0xf1,0xfd,0xfe,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xdf,0xfb,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0x7f,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,
0xff,0xff,0xff,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfb,0xff,0xdf,0xff,
0xff,0xf1,0xfd,0xff,0x7f,0xbf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xfe,0xff,0xff,0xff,0xff,0xf0,0x9f,0xff,0xff,0xff,0xfe,0xf7,0xfd,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xbf,0xff,0xff,0xff,0xff,0xff,
0xf1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xf0,0x6f,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf1,
0xff,0xff,0xfd,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xf0,0xcf,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xfb,0xff,0xff,0xff,0xfe,0xff,0xff,0xfb,0x6f,0xff,0xfe,0xbf,0xff,0xf1,0xff,
0xf7,0xff,0xff,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfd,
0xff,0xff,0xff,0xf0,0xef,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xfb,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0x57,0xff,0xfd,0xbf,0xff,0xf1,0xff,0xef,
0xfe,0xff,0xbf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xff,
0xde,0xff,0xf0,0xcf,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xf7,0xdb,0xff,0xdb,0xfd,
0xf6,0xff,0xf6,0xff,0x3c,0xbc,0xbc,0xbf,0xdf,0x6f,0xe7,0x2f,0xf1,0x3c,0xbf,0xfd,
0xbf,0xdf,0x6f,0xff,0x6f,0xf7,0xdb,0xff,0xdb,0xfd,0xf6,0xff,0xf6,0xff,0xff,0xff,
0x02,0x01,0xdf,0xff,0xff,0xff,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff };
