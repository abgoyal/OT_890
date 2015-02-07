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
 * Copyright (c) 1997-2002 Alacritech, Inc. All rights reserved
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY ALACRITECH, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL ALACRITECH, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * official policies, either expressed or implied, of Alacritech, Inc.
 *
 **************************************************************************/
#define GB_RCVUCODE_VERS_STRING	"1.2"
#define GB_RCVUCODE_VERS_DATE  	"2006/03/27 15:12:15"

static u32 GBRcvUCodeLen = 512;

static u8 GBRcvUCode[2560] =
{
0x47, 0x75, 0x01, 0x00, 0x04, 0xa0, 0x13, 0x01, 0x00, 0x1c, 0xb7, 0x5b, 0x09,
0x30, 0x00, 0xb6, 0x5f, 0x01, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x20, 0x18, 0x3b,
0x78, 0x3a, 0x00, 0x1c, 0xa2, 0x77, 0x01, 0x00, 0x1c, 0x07, 0x1d, 0x01, 0x70,
0x18, 0xb3, 0x7b, 0xa9, 0xaa, 0x1e, 0xb4, 0x7b, 0x01, 0x0c, 0x1c, 0xb5, 0x7b,
0x1d, 0x06, 0x1c, 0x00, 0x00, 0x40, 0x64, 0x08, 0x0c, 0x31, 0x56, 0x70, 0x04,
0x0c, 0x31, 0x56, 0x80, 0x04, 0x0c, 0x31, 0x4a, 0x90, 0x04, 0x0c, 0x31, 0x46,
0xa0, 0x00, 0x09, 0x25, 0x51, 0xc0, 0x04, 0x0c, 0x31, 0x4e, 0xb0, 0x00, 0xe9,
0x24, 0x51, 0xc0, 0x04, 0xcc, 0xb3, 0x00, 0x1c, 0x1c, 0xeb, 0x2d, 0x01, 0x00,
0x1c, 0x06, 0x56, 0x42, 0xd4, 0x08, 0x07, 0x9d, 0x00, 0x00, 0x1c, 0x7b, 0xb7,
0x02, 0x00, 0x10, 0xa0, 0x0f, 0x41, 0x54, 0x09, 0x06, 0x56, 0x5a, 0xc0, 0x04,
0xa0, 0x30, 0x6c, 0x03, 0x00, 0xac, 0x30, 0x6d, 0x03, 0x00, 0xcd, 0x03, 0x3a,
0x00, 0x1c, 0x7b, 0xb7, 0x02, 0x00, 0x1c, 0x60, 0x8e, 0x41, 0x54, 0x09, 0x29,
0x25, 0x6d, 0x03, 0x00, 0x80, 0x8e, 0x41, 0x54, 0x09, 0x8c, 0x30, 0x8d, 0x00,
0x04, 0x47, 0x1c, 0x01, 0x00, 0x1c, 0xa0, 0x0f, 0x41, 0x54, 0x09, 0x00, 0x00,
0x60, 0x00, 0x04, 0x47, 0x1c, 0x61, 0xc0, 0x04, 0x47, 0x1c, 0x6d, 0x03, 0x00,
0x6c, 0x30, 0x01, 0x00, 0x1c, 0x4d, 0x34, 0x02, 0x00, 0x1c, 0x7b, 0xb7, 0x02,
0x00, 0x1c, 0xa0, 0x0f, 0x41, 0x54, 0x09, 0xc8, 0x83, 0x37, 0x00, 0x1c, 0x80,
0x01, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x60, 0x00, 0x04, 0xa0, 0x0f, 0x40, 0x54,
0x09, 0x00, 0x00, 0x6c, 0xc3, 0x04, 0x7b, 0xfb, 0xf2, 0x00, 0x1c, 0xcc, 0x33,
0x0d, 0x00, 0x1c, 0xb4, 0x7b, 0xfd, 0x03, 0x1c, 0x80, 0x0e, 0x40, 0x54, 0x09,
0xe0, 0xfb, 0x05, 0x00, 0x1c, 0x00, 0x00, 0xa0, 0x03, 0x00, 0xb3, 0x0f, 0x41,
0x54, 0x09, 0x00, 0x00, 0xe8, 0x70, 0x04, 0x00, 0x00, 0xe8, 0x80, 0x04, 0x00,
0x00, 0xa0, 0x93, 0x00, 0x61, 0x76, 0xa1, 0xc3, 0x04, 0xc0, 0x8d, 0x41, 0x54,
0x09, 0xe0, 0x7b, 0x00, 0xc0, 0x1f, 0xa0, 0xfd, 0xc1, 0x01, 0x00, 0xcc, 0x33,
0x05, 0x00, 0x1c, 0xd4, 0x03, 0x00, 0x3c, 0x1c, 0xd4, 0xd3, 0x1b, 0x00, 0x1c,
0xc0, 0xd3, 0x52, 0x00, 0x1c, 0x00, 0x00, 0x74, 0x13, 0x04, 0x8e, 0x8e, 0x42,
0x54, 0x09, 0x5b, 0x80, 0x76, 0x13, 0x04, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00,
0x00, 0x90, 0x01, 0x00, 0xa0, 0x0f, 0x41, 0x54, 0x09, 0xa0, 0x0f, 0x41, 0x54,
0x09, 0xc0, 0x03, 0xfc, 0x7f, 0x1c, 0xa0, 0x01, 0x9c, 0x01, 0x00, 0x00, 0x00,
0xa0, 0x01, 0x00, 0xa0, 0x0f, 0x41, 0x54, 0x09, 0xc0, 0x03, 0xfc, 0x03, 0x1c,
0xf5, 0x77, 0x01, 0x00, 0x1c, 0x26, 0x7a, 0xf6, 0x05, 0x1c, 0xa0, 0x0f, 0x41,
0x54, 0x09, 0xb3, 0x0f, 0x41, 0x54, 0x09, 0xb5, 0x02, 0x02, 0x00, 0x1c, 0xa0,
0x0f, 0x41, 0x54, 0x09, 0x7a, 0x06, 0x02, 0x00, 0x1c, 0xb5, 0x02, 0x02, 0x00,
0x1c, 0x53, 0x0f, 0x42, 0x54, 0x09, 0xaf, 0x03, 0x01, 0x00, 0x1c, 0x7a, 0x0e,
0x42, 0x54, 0x09, 0xb5, 0x02, 0x02, 0x00, 0x1c, 0x00, 0x00, 0x02, 0x00, 0x1c,
0xa0, 0x3d, 0xa6, 0x11, 0x04, 0x00, 0x00, 0xa8, 0x11, 0x04, 0xd4, 0xd3, 0x52,
0x00, 0x1c, 0xb5, 0x3e, 0xae, 0x01, 0x00, 0x20, 0xfb, 0xfd, 0xff, 0x1f, 0x80,
0x2c, 0x84, 0x03, 0x00, 0xb9, 0x3a, 0x9a, 0x01, 0x00, 0x75, 0x3b, 0x02, 0x00,
0x1c, 0xa7, 0x1c, 0x01, 0x00, 0x10, 0xdb, 0x83, 0x16, 0x00, 0x1c, 0xc7, 0x1d,
0x1d, 0xc1, 0x04, 0xb9, 0x3b, 0x89, 0xc1, 0x04, 0x8b, 0x2c, 0x01, 0x00, 0x1c,
0x6b, 0x2c, 0x31, 0xc1, 0x04, 0x00, 0x00, 0x74, 0x11, 0x00, 0xcb, 0x2c, 0x75,
0xc1, 0x04, 0xa0, 0x0f, 0x41, 0x54, 0x09, 0xa0, 0x0f, 0x41, 0x54, 0x09, 0x54,
0xd0, 0x02, 0x00, 0x1c, 0x49, 0x25, 0xad, 0x01, 0x00, 0xab, 0x2c, 0x7d, 0xc1,
0x04, 0xa7, 0x1d, 0x6d, 0x03, 0x00, 0xcc, 0x33, 0x09, 0x00, 0x1c, 0xeb, 0x2d,
0x01, 0x00, 0x1c, 0xea, 0x29, 0x01, 0x00, 0x1c, 0xa0, 0x0f, 0x41, 0x54, 0x09,
0xae, 0x0f, 0x41, 0x54, 0x09, 0xa0, 0x0f, 0x41, 0x54, 0x09, 0xd4, 0x07, 0xfc,
0x03, 0x1c, 0x99, 0x3a, 0x02, 0x00, 0x1c, 0xbb, 0x38, 0x02, 0x00, 0x1c, 0x00,
0x38, 0x00, 0x00, 0x1c, 0x00, 0x00, 0xf8, 0x01, 0x04, 0xdb, 0x3b, 0x7e, 0x00,
0x1c, 0xc7, 0x1d, 0x01, 0x00, 0x1c, 0x26, 0x7a, 0x0a, 0x06, 0x1c, 0x27, 0x1d,
0x01, 0x00, 0x1c, 0xb3, 0x0f, 0x41, 0x54, 0x09, 0x7a, 0x0e, 0x42, 0x54, 0x09,
0x53, 0x0f, 0x42, 0x54, 0x09, 0x7a, 0x0e, 0x42, 0x54, 0x09, 0x53, 0x0f, 0x42,
0x54, 0x09, 0x7a, 0x0e, 0x42, 0x54, 0x09, 0x53, 0x0f, 0x42, 0x54, 0x09, 0xa0,
0x0f, 0x41, 0x54, 0x09, 0x7a, 0x06, 0x02, 0x00, 0x1c, 0x53, 0x0f, 0x42, 0x54,
0x09, 0xaf, 0x03, 0x01, 0x00, 0x1c, 0x7a, 0x0e, 0x42, 0x54, 0x09, 0x53, 0x0f,
0x42, 0x54, 0x09, 0x7a, 0x0e, 0x42, 0x54, 0x09, 0x53, 0x0f, 0x42, 0x54, 0x09,
0x7a, 0x0e, 0x42, 0x54, 0x09, 0x53, 0x0f, 0x42, 0x54, 0x09, 0x7a, 0x0e, 0x42,
0x54, 0x09, 0x00, 0x3d, 0x02, 0x00, 0x1c, 0x00, 0x00, 0x54, 0x12, 0x00, 0xcb,
0x2c, 0x01, 0x00, 0x1c, 0x75, 0x3b, 0x02, 0x00, 0x1c, 0xa7, 0x1c, 0x01, 0x00,
0x10, 0xa6, 0x7b, 0xf1, 0x05, 0x1c, 0x00, 0x00, 0x88, 0xc2, 0x04, 0xa6, 0x7b,
0xf1, 0x05, 0x1c, 0x00, 0x00, 0xa0, 0xc2, 0x04, 0xcb, 0x2f, 0x05, 0x00, 0x1c,
0x60, 0x2c, 0x00, 0x00, 0x1c, 0xc7, 0x1c, 0xe1, 0x02, 0x00, 0x53, 0x0f, 0x42,
0x54, 0x09, 0xc0, 0x83, 0xf1, 0x32, 0x1c, 0x00, 0x00, 0x5c, 0x02, 0x04, 0x46,
0x7a, 0xda, 0x05, 0x1c, 0x7a, 0x0e, 0x42, 0x54, 0x09, 0xc0, 0x83, 0xf1, 0x32,
0x1c, 0x00, 0x00, 0x64, 0x02, 0x04, 0x40, 0xfa, 0x15, 0x00, 0x1c, 0x00, 0x00,
0xa0, 0x02, 0x04, 0x46, 0x7a, 0xda, 0x05, 0x1c, 0xa0, 0x0f, 0x41, 0x54, 0x09,
0xa0, 0x0f, 0x41, 0x54, 0x09, 0xa0, 0x0f, 0x41, 0x54, 0x09, 0xa0, 0x0f, 0x41,
0x54, 0x09, 0xb3, 0x7b, 0x01, 0xc0, 0x1f, 0x74, 0x0e, 0x40, 0x54, 0x09, 0xc0,
0x03, 0x9c, 0x00, 0x1c, 0x80, 0x00, 0xf0, 0x02, 0x00, 0x00, 0x00, 0xf0, 0x02,
0x04, 0x00, 0x00, 0xc4, 0x12, 0x05, 0x07, 0x1d, 0x01, 0x00, 0x1c, 0xd4, 0xd3,
0x2b, 0x00, 0x1c, 0xd4, 0xd3, 0x52, 0x00, 0x1c, 0x80, 0x76, 0x95, 0x13, 0x04,
0x00, 0x00, 0xf8, 0x02, 0x00, 0xa6, 0x7b, 0xa9, 0x03, 0x10, 0xc7, 0x9c, 0x00,
0x00, 0x1c, 0x80, 0x2c, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x78, 0x02, 0x04, 0x00,
0x00, 0x6c, 0xc3, 0x04, 0xab, 0x2d, 0xf1, 0x12, 0x05, 0x07, 0x1d, 0xcd, 0xc2,
0x04, 0x8b, 0x2d, 0x01, 0x00, 0x1c, 0x69, 0x25, 0x01, 0x00, 0x1c, 0xa6, 0x7b,
0xa9, 0x03, 0x10, 0xcb, 0x2f, 0x09, 0x00, 0x1c, 0x60, 0x2c, 0x00, 0x00, 0x1c,
0x00, 0x00, 0x60, 0x03, 0x00, 0x53, 0x0f, 0x42, 0x54, 0x09, 0x46, 0x7a, 0xda,
0x05, 0x1c, 0x7a, 0x0e, 0x42, 0x54, 0x09, 0x40, 0xfa, 0x15, 0x00, 0x1c, 0x00,
0x00, 0x28, 0x03, 0x04, 0x46, 0x7a, 0xda, 0x05, 0x1c, 0xb5, 0x0f, 0x41, 0x54,
0x09, 0xa0, 0x0f, 0x41, 0x54, 0x09, 0x73, 0xec, 0x42, 0x03, 0x04, 0x60, 0x2c,
0x00, 0x00, 0x1c, 0x00, 0x00, 0x40, 0x03, 0x00, 0xc7, 0x1c, 0x01, 0x00, 0x1c,
0x00, 0x00, 0x40, 0x13, 0x05, 0x07, 0x1d, 0x01, 0x00, 0x1c, 0xc0, 0xd7, 0x22,
0x00, 0x1c, 0x75, 0x56, 0x96, 0x13, 0x04, 0x60, 0x2c, 0x00, 0x00, 0x1c, 0xe7,
0x1c, 0x5d, 0x03, 0x04, 0xe7, 0x9c, 0x00, 0x00, 0x1c, 0xa6, 0x7b, 0xa9, 0x03,
0x10, 0x80, 0x2c, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x10, 0x03, 0x04, 0x00, 0x00,
0x6c, 0xc3, 0x04, 0xb9, 0x7b, 0x01, 0x00, 0x1c, 0x00, 0x00, 0xa0, 0xc3, 0x04,
0xcb, 0xaf, 0xfc, 0x07, 0x1c, 0xcb, 0x2f, 0x01, 0x04, 0x1c, 0xc7, 0x9f, 0x80,
0x03, 0x1c, 0x00, 0x00, 0xa0, 0xc3, 0x04, 0xcb, 0xaf, 0xfc, 0x07, 0x1c, 0xcb,
0x2f, 0x0d, 0x04, 0x1c, 0xc7, 0x9f, 0x80, 0x03, 0x1c, 0x00, 0x00, 0xa0, 0xc3,
0x04, 0xcb, 0xaf, 0x00, 0xf8, 0x1d, 0xcb, 0x2f, 0x01, 0x00, 0x1d, 0x00, 0x00,
0xa0, 0xc3, 0x04, 0x00, 0x00, 0xa0, 0x13, 0x05, 0x07, 0x1d, 0x01, 0x00, 0x1c,
0xc0, 0x1d, 0xf0, 0xd3, 0x08, 0x27, 0x9d, 0xf8, 0x03, 0x00, 0xa0, 0xee, 0x56,
0xd4, 0x00, 0xfb, 0x75, 0x19, 0x14, 0x04, 0x20, 0x7b, 0x06, 0x00, 0x1c, 0xc0,
0x1c, 0x2c, 0x04, 0x00, 0x00, 0x00, 0xc4, 0xd3, 0x08, 0x00, 0x00, 0x10, 0xf4,
0x00, 0xc0, 0xef, 0xf2, 0x00, 0x1c, 0x20, 0x25, 0x6c, 0x14, 0x04, 0x60, 0xb7,
0xe6, 0x03, 0x00, 0x00, 0x00, 0x1c, 0x15, 0x00, 0xcc, 0xb3, 0xfc, 0x03, 0x1c,
0xcc, 0x33, 0x05, 0x02, 0x1c, 0x00, 0x00, 0x1c, 0xc5, 0x04, 0x60, 0xb7, 0x1e,
0x05, 0x04, 0x00, 0x00, 0x1c, 0x15, 0x04, 0x00, 0x00, 0x6c, 0xc4, 0x04, 0xc0,
0x1d, 0xac, 0xf3, 0x04, 0x00, 0x00, 0x78, 0xc4, 0x04, 0x07, 0x9d, 0x00, 0x00,
0x1c, 0x1b, 0x74, 0x0d, 0xf4, 0x04, 0xa0, 0x0f, 0x41, 0x54, 0x09, 0xe0, 0x7b,
0x00, 0xfc, 0x1f, 0x39, 0x7f, 0x02, 0x00, 0x1c, 0x07, 0x1d, 0xb1, 0xc3, 0x04,
0xa6, 0x7b, 0xc1, 0x03, 0x1c, 0x00, 0x00, 0x78, 0xc4, 0x04, 0xe0, 0x1c, 0x00,
0x00, 0x1c, 0x00, 0x00, 0xb8, 0x03, 0x04, 0xcb, 0xaf, 0x00, 0xf8, 0x1d, 0xcb,
0x2f, 0x01, 0x10, 0x1d, 0x00, 0x00, 0xc0, 0xc3, 0x04, 0x00, 0x00, 0xc0, 0x03,
0x04, 0xcb, 0xaf, 0x00, 0xf8, 0x1d, 0xcb, 0x2f, 0x01, 0x18, 0x1d, 0xc7, 0x9f,
0x00, 0x0b, 0x1c, 0x00, 0x00, 0xc0, 0xc3, 0x04, 0xfb, 0x75, 0x01, 0x00, 0x1c,
0x07, 0x1d, 0x01, 0x00, 0x1c, 0xcc, 0xb3, 0xfc, 0x03, 0x1c, 0xcc, 0x33, 0x01,
0x02, 0x1c, 0x00, 0x00, 0xc0, 0xc3, 0x04, 0xa0, 0x1c, 0x00, 0x00, 0x1c, 0xa0,
0xee, 0xb6, 0x03, 0x04, 0xcb, 0xaf, 0xfc, 0x07, 0x1c, 0xcb, 0x2f, 0x09, 0x04,
0x1c, 0xfb, 0x75, 0x01, 0x00, 0x1c, 0x00, 0x00, 0xc0, 0xc3, 0x04, 0xcc, 0xb3,
0xfc, 0x03, 0x1c, 0xcc, 0x33, 0x01, 0x02, 0x1c, 0x00, 0x00, 0x1c, 0xc5, 0x04,
0x00, 0x00, 0x88, 0x34, 0x05, 0xcc, 0xb3, 0xfc, 0x03, 0x1c, 0xcc, 0x33, 0x15,
0x02, 0x1c, 0x47, 0x9d, 0x64, 0xc4, 0x04, 0x00, 0x00, 0x88, 0x44, 0x00, 0x80,
0x1d, 0x8c, 0x54, 0x04, 0x87, 0x1d, 0x9d, 0x04, 0x00, 0xce, 0x76, 0x01, 0x00,
0x1c, 0xef, 0x76, 0xad, 0xc4, 0x04, 0xa4, 0x77, 0x9d, 0x24, 0x09, 0xe4, 0x76,
0x01, 0x00, 0x1c, 0xc4, 0x76, 0x01, 0x00, 0x1c, 0x00, 0x00, 0xa8, 0x54, 0x04,
0xd7, 0x76, 0x01, 0x50, 0x18, 0xf6, 0x76, 0x01, 0x00, 0x1c, 0x00, 0x00, 0x00,
0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x10, 0xcc, 0x30, 0x51, 0xc5, 0x04, 0xeb,
0x2d, 0x01, 0x00, 0x1c, 0xea, 0x29, 0x01, 0x00, 0x1c, 0xc0, 0x59, 0x01, 0x00,
0x1c, 0xf5, 0x77, 0x39, 0xc5, 0x04, 0xe0, 0x30, 0xec, 0x04, 0x00, 0x00, 0x4c,
0xc0, 0x04, 0x00, 0x20, 0x4c, 0x04, 0x05, 0x00, 0x00, 0x00, 0xf8, 0x04, 0x00,
0xcc, 0xb3, 0xfc, 0x03, 0x1c, 0xcc, 0x33, 0x09, 0x02, 0x1c, 0xeb, 0x2d, 0xc5,
0xc4, 0x04, 0xcc, 0xb3, 0xfc, 0x03, 0x1c, 0xcc, 0x33, 0x19, 0x02, 0x1c, 0xeb,
0x2d, 0xc5, 0xc4, 0x04, 0xcc, 0xb3, 0xfc, 0x03, 0x1c, 0xcc, 0x33, 0x0d, 0x02,
0x1c, 0xeb, 0x2d, 0xc5, 0xc4, 0x04, 0xcc, 0xb3, 0xfc, 0x03, 0x1c, 0xcc, 0x33,
0x11, 0x02, 0x1c, 0xeb, 0x2d, 0xc5, 0xc4, 0x04, 0x00, 0x7b, 0x00, 0x80, 0x1c,
0xae, 0x77, 0x51, 0x05, 0x00, 0x00, 0x00, 0x04, 0xc0, 0x04, 0xd3, 0x8b, 0x00,
0xfc, 0x1f, 0x60, 0x7a, 0x3c, 0x00, 0x1c, 0x60, 0x4c, 0xd0, 0x04, 0x00, 0xc0,
0x2f, 0x20, 0x05, 0x1f, 0xe0, 0x30, 0xc0, 0x04, 0x00, 0x80, 0x25, 0xc0, 0x04,
0x00, 0xb5, 0x5b, 0xc1, 0x04, 0x04, 0x69, 0x26, 0x01, 0x00, 0x1c, 0x6a, 0x2b,
0x01, 0x00, 0x1c, 0x80, 0x1d, 0x00, 0x00, 0x1c, 0xa9, 0x25, 0x51, 0x05, 0x00,
0xee, 0x30, 0x00, 0x00, 0x1c, 0xaf, 0x77, 0x11, 0x05, 0x00, 0xb4, 0x5f, 0x01,
0x40, 0x18, 0x07, 0x9d, 0x54, 0x55, 0x04, 0xb7, 0x76, 0x01, 0x00, 0x1c, 0x96,
0x76, 0x01, 0x00, 0x1c, 0x47, 0x1d, 0x01, 0x00, 0x1c, 0xa4, 0x33, 0x01, 0x60,
0x18, 0xa4, 0x2f, 0x01, 0x60, 0x18, 0x64, 0x77, 0x01, 0x60, 0x18, 0x24, 0x77,
0x01, 0x60, 0x18, 0x44, 0x77, 0x01, 0x00, 0x1c, 0x64, 0x88, 0x03, 0x00, 0x1c,
0xa4, 0x3f, 0x01, 0x00, 0x1c, 0xa4, 0x3b, 0x01, 0x00, 0x1c, 0x53, 0x77, 0x01,
0x00, 0x1c, 0xd3, 0xcf, 0x3b, 0x00, 0x1c, 0x53, 0x4f, 0x02, 0x00, 0x1c, 0xd3,
0xcf, 0x00, 0x00, 0x1f, 0xda, 0xcf, 0x0b, 0x00, 0x1c, 0xd5, 0x57, 0x0f, 0x00,
0x1c, 0xd3, 0xd3, 0x37, 0x00, 0x1c, 0xd4, 0x53, 0x0f, 0x00, 0x1c, 0xe0, 0x29,
0x00, 0x00, 0x1c, 0xf5, 0xd5, 0xc0, 0x05, 0x00, 0x00, 0x00, 0xac, 0x55, 0x04,
0x77, 0x56, 0x01, 0x00, 0x1c, 0x56, 0x53, 0x01, 0x00, 0x1c, 0x00, 0x00, 0x00,
0x10, 0x18, 0x00, 0x00, 0x04, 0xc0, 0x04, 0xf5, 0x55, 0x01, 0x00, 0x1c, 0x00,
0x00, 0xc4, 0x55, 0x04, 0x77, 0x56, 0x01, 0x00, 0x1c, 0x56, 0x53, 0x01, 0x00,
0x1c, 0x00, 0x00, 0x00, 0x10, 0x18, 0x00, 0x00, 0x04, 0xc0, 0x04, 0xcb, 0x2f,
0x01, 0x18, 0x10, 0xcb, 0x2f, 0x01, 0x10, 0x10, 0xcb, 0x2f, 0x01, 0x08, 0x10,
0xcb, 0x2f, 0x01, 0x08, 0x10, 0xcb, 0x2f, 0x01, 0x20, 0x10, 0xcb, 0x2f, 0x01,
0x00, 0x10, 0xcb, 0x2f, 0x01, 0x28, 0x10, 0x89, 0x25, 0x6d, 0xc2, 0x04, 0x00,
0x00, 0x04, 0xc3, 0x04, 0x00, 0x00, 0x6c, 0xc3, 0x04, 0x00, 0x00, 0x6c, 0xc3,
0x04, 0x00, 0x00, 0x6c, 0xc3, 0x04, 0x00, 0x00, 0x6c, 0xc2, 0x04, 0x00, 0x00,
0x04, 0xc3, 0x04, 0x00, 0x00, 0x6c, 0xc3, 0x04, 0x00, 0x00, 0x6c, 0xc3, 0x04,
0x00, 0x00, 0x6c, 0xc3, 0x04, 0x40, 0x1c, 0x68, 0xc0, 0x04, 0x40, 0x1c, 0x98,
0xc0, 0x04, 0xa7, 0x77, 0x6d, 0xc3, 0x04, 0x00, 0x00, 0xc0, 0xc0, 0x04, 0x27,
0x1d, 0xed, 0xc0, 0x04, 0x00, 0x00, 0x6c, 0xc3, 0x04, 0x00, 0x00, 0x6c, 0xc3,
0x04, 0x00, 0x00, 0x6c, 0xc3, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00,
0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04,
0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c,
0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00,
0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6,
0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00,
0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04,
0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c,
0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00,
0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6,
0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00,
0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04,
0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c,
0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00,
0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6,
0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00,
0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04,
0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c,
0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00,
0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6,
0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00,
0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04,
0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c,
0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00,
0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6,
0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00,
0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04,
0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c,
0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00,
0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6,
0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00,
0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04,
0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c,
0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00,
0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6,
0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00,
0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04,
0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c,
0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00,
0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6,
0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00,
0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04,
0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c,
0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04, 0x00, 0x00, 0x3c, 0xc6, 0x04,
};
