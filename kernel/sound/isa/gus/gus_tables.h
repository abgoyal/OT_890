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
 *  Copyright (c) by Jaroslav Kysela <perex@perex.cz>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#define SNDRV_GF1_SCALE_TABLE_SIZE	128
#define SNDRV_GF1_ATTEN_TABLE_SIZE	128

#ifdef __GUS_TABLES_ALLOC__

#if 0

unsigned int snd_gf1_scale_table[SNDRV_GF1_SCALE_TABLE_SIZE] =
{
      8372,      8870,      9397,      9956,     10548,     11175,
     11840,     12544,     13290,     14080,     14917,     15804,
     16744,     17740,     18795,     19912,     21096,     22351,
     23680,     25088,     26580,     28160,     29834,     31609,
     33488,     35479,     37589,     39824,     42192,     44701,
     47359,     50175,     53159,     56320,     59669,     63217,
     66976,     70959,     75178,     79649,     84385,     89402,
     94719,    100351,    106318,    112640,    119338,    126434,
    133952,    141918,    150356,    159297,    168769,    178805,
    189437,    200702,    212636,    225280,    238676,    252868,
    267905,    283835,    300713,    318594,    337539,    357610,
    378874,    401403,    425272,    450560,    477352,    505737,
    535809,    567670,    601425,    637188,    675077,    715219,
    757749,    802807,    850544,    901120,    954703,   1011473,
   1071618,   1135340,   1202851,   1274376,   1350154,   1430439,
   1515497,   1605613,   1701088,   1802240,   1909407,   2022946,
   2143237,   2270680,   2405702,   2548752,   2700309,   2860878,
   3030994,   3211227,   3402176,   3604480,   3818814,   4045892,
   4286473,   4541360,   4811404,   5097505,   5400618,   5721755,
   6061989,   6422453,   6804352,   7208960,   7637627,   8091784,
   8572947,   9082720,   9622807,  10195009,  10801236,  11443511,
  12123977,  12844906
};

#endif  /*  0  */

unsigned short snd_gf1_atten_table[SNDRV_GF1_ATTEN_TABLE_SIZE] = {
  4095 /* 0   */,1789 /* 1   */,1533 /* 2   */,1383 /* 3   */,1277 /* 4   */,
  1195 /* 5   */,1127 /* 6   */,1070 /* 7   */,1021 /* 8   */,978  /* 9   */,
  939  /* 10  */,903  /* 11  */,871  /* 12  */,842  /* 13  */,814  /* 14  */,
  789  /* 15  */,765  /* 16  */,743  /* 17  */,722  /* 18  */,702  /* 19  */,
  683  /* 20  */,665  /* 21  */,647  /* 22  */,631  /* 23  */,615  /* 24  */,
  600  /* 25  */,586  /* 26  */,572  /* 27  */,558  /* 28  */,545  /* 29  */,
  533  /* 30  */,521  /* 31  */,509  /* 32  */,498  /* 33  */,487  /* 34  */,
  476  /* 35  */,466  /* 36  */,455  /* 37  */,446  /* 38  */,436  /* 39  */,
  427  /* 40  */,418  /* 41  */,409  /* 42  */,400  /* 43  */,391  /* 44  */,
  383  /* 45  */,375  /* 46  */,367  /* 47  */,359  /* 48  */,352  /* 49  */,
  344  /* 50  */,337  /* 51  */,330  /* 52  */,323  /* 53  */,316  /* 54  */,
  309  /* 55  */,302  /* 56  */,296  /* 57  */,289  /* 58  */,283  /* 59  */,
  277  /* 60  */,271  /* 61  */,265  /* 62  */,259  /* 63  */,253  /* 64  */,
  247  /* 65  */,242  /* 66  */,236  /* 67  */,231  /* 68  */,225  /* 69  */,
  220  /* 70  */,215  /* 71  */,210  /* 72  */,205  /* 73  */,199  /* 74  */,
  195  /* 75  */,190  /* 76  */,185  /* 77  */,180  /* 78  */,175  /* 79  */,
  171  /* 80  */,166  /* 81  */,162  /* 82  */,157  /* 83  */,153  /* 84  */,
  148  /* 85  */,144  /* 86  */,140  /* 87  */,135  /* 88  */,131  /* 89  */,
  127  /* 90  */,123  /* 91  */,119  /* 92  */,115  /* 93  */,111  /* 94  */,
  107  /* 95  */,103  /* 96  */,100  /* 97  */,96   /* 98  */,92   /* 99  */,
  88   /* 100 */,85   /* 101 */,81   /* 102 */,77   /* 103 */,74   /* 104 */,
  70   /* 105 */,67   /* 106 */,63   /* 107 */,60   /* 108 */,56   /* 109 */,
  53   /* 110 */,50   /* 111 */,46   /* 112 */,43   /* 113 */,40   /* 114 */,
  37   /* 115 */,33   /* 116 */,30   /* 117 */,27   /* 118 */,24   /* 119 */,
  21   /* 120 */,18   /* 121 */,15   /* 122 */,12   /* 123 */,9    /* 124 */,
  6    /* 125 */,3    /* 126 */,0    /* 127 */,
};

#else

extern unsigned int snd_gf1_scale_table[SNDRV_GF1_SCALE_TABLE_SIZE];
extern unsigned short snd_gf1_atten_table[SNDRV_GF1_ATTEN_TABLE_SIZE];

#endif
