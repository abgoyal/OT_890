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
 * SPCA505 chip based cameras initialization data
 *
 * V4L2 by Jean-Francis Moine <http://moinejf.free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#define MODULE_NAME "spca505"

#include "gspca.h"

MODULE_AUTHOR("Michel Xhaard <mxhaard@users.sourceforge.net>");
MODULE_DESCRIPTION("GSPCA/SPCA505 USB Camera Driver");
MODULE_LICENSE("GPL");

/* specific webcam descriptor */
struct sd {
	struct gspca_dev gspca_dev;		/* !! must be the first item */

	unsigned char brightness;

	char subtype;
#define IntelPCCameraPro 0
#define Nxultra 1
};

/* V4L2 controls supported by the driver */
static int sd_setbrightness(struct gspca_dev *gspca_dev, __s32 val);
static int sd_getbrightness(struct gspca_dev *gspca_dev, __s32 *val);

static struct ctrl sd_ctrls[] = {
#define SD_BRIGHTNESS 0
	{
	    {
		.id      = V4L2_CID_BRIGHTNESS,
		.type    = V4L2_CTRL_TYPE_INTEGER,
		.name    = "Brightness",
		.minimum = 0,
		.maximum = 255,
		.step    = 1,
		.default_value = 127,
	    },
	    .set = sd_setbrightness,
	    .get = sd_getbrightness,
	},
};

static const struct v4l2_pix_format vga_mode[] = {
	{160, 120, V4L2_PIX_FMT_SPCA505, V4L2_FIELD_NONE,
		.bytesperline = 160,
		.sizeimage = 160 * 120 * 3 / 2,
		.colorspace = V4L2_COLORSPACE_SRGB,
		.priv = 5},
	{176, 144, V4L2_PIX_FMT_SPCA505, V4L2_FIELD_NONE,
		.bytesperline = 176,
		.sizeimage = 176 * 144 * 3 / 2,
		.colorspace = V4L2_COLORSPACE_SRGB,
		.priv = 4},
	{320, 240, V4L2_PIX_FMT_SPCA505, V4L2_FIELD_NONE,
		.bytesperline = 320,
		.sizeimage = 320 * 240 * 3 / 2,
		.colorspace = V4L2_COLORSPACE_SRGB,
		.priv = 2},
	{352, 288, V4L2_PIX_FMT_SPCA505, V4L2_FIELD_NONE,
		.bytesperline = 352,
		.sizeimage = 352 * 288 * 3 / 2,
		.colorspace = V4L2_COLORSPACE_SRGB,
		.priv = 1},
	{640, 480, V4L2_PIX_FMT_SPCA505, V4L2_FIELD_NONE,
		.bytesperline = 640,
		.sizeimage = 640 * 480 * 3 / 2,
		.colorspace = V4L2_COLORSPACE_SRGB,
		.priv = 0},
};

#define SPCA50X_OFFSET_DATA 10

#define SPCA50X_REG_USB 0x02	/* spca505 501 */

#define SPCA50X_USB_CTRL 0x00	/* spca505 */
#define SPCA50X_CUSB_ENABLE 0x01 /* spca505 */
#define SPCA50X_REG_GLOBAL 0x03	/* spca505 */
#define SPCA50X_GMISC0_IDSEL 0x01 /* Global control device ID select spca505 */
#define SPCA50X_GLOBAL_MISC0 0x00 /* Global control miscellaneous 0 spca505 */

#define SPCA50X_GLOBAL_MISC1 0x01 /* 505 */
#define SPCA50X_GLOBAL_MISC3 0x03 /* 505 */
#define SPCA50X_GMISC3_SAA7113RST 0x20	/* Not sure about this one spca505 */

/*
 * Data to initialize a SPCA505. Common to the CCD and external modes
 */
static const __u16 spca505_init_data[][3] = {
	/* line	   bmRequest,value,index */
	/* 1819 */
	{SPCA50X_REG_GLOBAL, SPCA50X_GMISC3_SAA7113RST, SPCA50X_GLOBAL_MISC3},
	/* Sensor reset */
	/* 1822 */ {SPCA50X_REG_GLOBAL, 0x00, SPCA50X_GLOBAL_MISC3},
	/* 1825 */ {SPCA50X_REG_GLOBAL, 0x00, SPCA50X_GLOBAL_MISC1},
	/* Block USB reset */
	/* 1828 */ {SPCA50X_REG_GLOBAL, SPCA50X_GMISC0_IDSEL,
		SPCA50X_GLOBAL_MISC0},

	/* 1831 */ {0x5, 0x01, 0x10},
					/* Maybe power down some stuff */
	/* 1834 */ {0x5, 0x0f, 0x11},

	/* Setup internal CCD  ? */
	/* 1837 */ {0x6, 0x10, 0x08},
	/* 1840 */ {0x6, 0x00, 0x09},
	/* 1843 */ {0x6, 0x00, 0x0a},
	/* 1846 */ {0x6, 0x00, 0x0b},
	/* 1849 */ {0x6, 0x10, 0x0c},
	/* 1852 */ {0x6, 0x00, 0x0d},
	/* 1855 */ {0x6, 0x00, 0x0e},
	/* 1858 */ {0x6, 0x00, 0x0f},
	/* 1861 */ {0x6, 0x10, 0x10},
	/* 1864 */ {0x6, 0x02, 0x11},
	/* 1867 */ {0x6, 0x00, 0x12},
	/* 1870 */ {0x6, 0x04, 0x13},
	/* 1873 */ {0x6, 0x02, 0x14},
	/* 1876 */ {0x6, 0x8a, 0x51},
	/* 1879 */ {0x6, 0x40, 0x52},
	/* 1882 */ {0x6, 0xb6, 0x53},
	/* 1885 */ {0x6, 0x3d, 0x54},
	{}
};

/*
 * Data to initialize the camera using the internal CCD
 */
static const __u16 spca505_open_data_ccd[][3] = {
	/* line	   bmRequest,value,index */
	/* Internal CCD data set */
	/* 1891 */ {0x3, 0x04, 0x01},
	/* This could be a reset */
	/* 1894 */ {0x3, 0x00, 0x01},

	/* Setup compression and image registers. 0x6 and 0x7 seem to be
	   related to H&V hold, and are resolution mode specific */
		/* 1897 */ {0x4, 0x10, 0x01},
		/* DIFF(0x50), was (0x10) */
	/* 1900 */ {0x4, 0x00, 0x04},
	/* 1903 */ {0x4, 0x00, 0x05},
	/* 1906 */ {0x4, 0x20, 0x06},
	/* 1909 */ {0x4, 0x20, 0x07},

	/* 1912 */ {0x8, 0x0a, 0x00},
	/* DIFF (0x4a), was (0xa) */

	/* 1915 */ {0x5, 0x00, 0x10},
	/* 1918 */ {0x5, 0x00, 0x11},
	/* 1921 */ {0x5, 0x00, 0x00},
	/* DIFF not written */
	/* 1924 */ {0x5, 0x00, 0x01},
	/* DIFF not written */
	/* 1927 */ {0x5, 0x00, 0x02},
	/* DIFF not written */
	/* 1930 */ {0x5, 0x00, 0x03},
	/* DIFF not written */
	/* 1933 */ {0x5, 0x00, 0x04},
	/* DIFF not written */
		/* 1936 */ {0x5, 0x80, 0x05},
		/* DIFF not written */
		/* 1939 */ {0x5, 0xe0, 0x06},
		/* DIFF not written */
		/* 1942 */ {0x5, 0x20, 0x07},
		/* DIFF not written */
		/* 1945 */ {0x5, 0xa0, 0x08},
		/* DIFF not written */
		/* 1948 */ {0x5, 0x0, 0x12},
		/* DIFF not written */
	/* 1951 */ {0x5, 0x02, 0x0f},
	/* DIFF not written */
		/* 1954 */ {0x5, 0x10, 0x46},
		/* DIFF not written */
		/* 1957 */ {0x5, 0x8, 0x4a},
		/* DIFF not written */

	/* 1960 */ {0x3, 0x08, 0x03},
	/* DIFF (0x3,0x28,0x3) */
	/* 1963 */ {0x3, 0x08, 0x01},
	/* 1966 */ {0x3, 0x0c, 0x03},
	/* DIFF not written */
		/* 1969 */ {0x3, 0x21, 0x00},
		/* DIFF (0x39) */

/* Extra block copied from init to hopefully ensure CCD is in a sane state */
	/* 1837 */ {0x6, 0x10, 0x08},
	/* 1840 */ {0x6, 0x00, 0x09},
	/* 1843 */ {0x6, 0x00, 0x0a},
	/* 1846 */ {0x6, 0x00, 0x0b},
	/* 1849 */ {0x6, 0x10, 0x0c},
	/* 1852 */ {0x6, 0x00, 0x0d},
	/* 1855 */ {0x6, 0x00, 0x0e},
	/* 1858 */ {0x6, 0x00, 0x0f},
	/* 1861 */ {0x6, 0x10, 0x10},
	/* 1864 */ {0x6, 0x02, 0x11},
	/* 1867 */ {0x6, 0x00, 0x12},
	/* 1870 */ {0x6, 0x04, 0x13},
	/* 1873 */ {0x6, 0x02, 0x14},
	/* 1876 */ {0x6, 0x8a, 0x51},
	/* 1879 */ {0x6, 0x40, 0x52},
	/* 1882 */ {0x6, 0xb6, 0x53},
	/* 1885 */ {0x6, 0x3d, 0x54},
	/* End of extra block */

		/* 1972 */ {0x6, 0x3f, 0x1},
		/* Block skipped */
	/* 1975 */ {0x6, 0x10, 0x02},
	/* 1978 */ {0x6, 0x64, 0x07},
	/* 1981 */ {0x6, 0x10, 0x08},
	/* 1984 */ {0x6, 0x00, 0x09},
	/* 1987 */ {0x6, 0x00, 0x0a},
	/* 1990 */ {0x6, 0x00, 0x0b},
	/* 1993 */ {0x6, 0x10, 0x0c},
	/* 1996 */ {0x6, 0x00, 0x0d},
	/* 1999 */ {0x6, 0x00, 0x0e},
	/* 2002 */ {0x6, 0x00, 0x0f},
	/* 2005 */ {0x6, 0x10, 0x10},
	/* 2008 */ {0x6, 0x02, 0x11},
	/* 2011 */ {0x6, 0x00, 0x12},
	/* 2014 */ {0x6, 0x04, 0x13},
	/* 2017 */ {0x6, 0x02, 0x14},
	/* 2020 */ {0x6, 0x8a, 0x51},
	/* 2023 */ {0x6, 0x40, 0x52},
	/* 2026 */ {0x6, 0xb6, 0x53},
	/* 2029 */ {0x6, 0x3d, 0x54},
	/* 2032 */ {0x6, 0x60, 0x57},
	/* 2035 */ {0x6, 0x20, 0x58},
	/* 2038 */ {0x6, 0x15, 0x59},
	/* 2041 */ {0x6, 0x05, 0x5a},

	/* 2044 */ {0x5, 0x01, 0xc0},
	/* 2047 */ {0x5, 0x10, 0xcb},
		/* 2050 */ {0x5, 0x80, 0xc1},
		/* */
		/* 2053 */ {0x5, 0x0, 0xc2},
		/* 4 was 0 */
	/* 2056 */ {0x5, 0x00, 0xca},
		/* 2059 */ {0x5, 0x80, 0xc1},
		/*  */
	/* 2062 */ {0x5, 0x04, 0xc2},
	/* 2065 */ {0x5, 0x00, 0xca},
		/* 2068 */ {0x5, 0x0, 0xc1},
		/*  */
	/* 2071 */ {0x5, 0x00, 0xc2},
	/* 2074 */ {0x5, 0x00, 0xca},
		/* 2077 */ {0x5, 0x40, 0xc1},
		/* */
	/* 2080 */ {0x5, 0x17, 0xc2},
	/* 2083 */ {0x5, 0x00, 0xca},
		/* 2086 */ {0x5, 0x80, 0xc1},
		/* */
	/* 2089 */ {0x5, 0x06, 0xc2},
	/* 2092 */ {0x5, 0x00, 0xca},
		/* 2095 */ {0x5, 0x80, 0xc1},
		/* */
	/* 2098 */ {0x5, 0x04, 0xc2},
	/* 2101 */ {0x5, 0x00, 0xca},

	/* 2104 */ {0x3, 0x4c, 0x3},
	/* 2107 */ {0x3, 0x18, 0x1},

	/* 2110 */ {0x6, 0x70, 0x51},
	/* 2113 */ {0x6, 0xbe, 0x53},
	/* 2116 */ {0x6, 0x71, 0x57},
	/* 2119 */ {0x6, 0x20, 0x58},
	/* 2122 */ {0x6, 0x05, 0x59},
	/* 2125 */ {0x6, 0x15, 0x5a},

	/* 2128 */ {0x4, 0x00, 0x08},
	/* Compress = OFF (0x1 to turn on) */
	/* 2131 */ {0x4, 0x12, 0x09},
	/* 2134 */ {0x4, 0x21, 0x0a},
	/* 2137 */ {0x4, 0x10, 0x0b},
	/* 2140 */ {0x4, 0x21, 0x0c},
	/* 2143 */ {0x4, 0x05, 0x00},
	/* was 5 (Image Type ? ) */
	/* 2146 */ {0x4, 0x00, 0x01},

	/* 2149 */ {0x6, 0x3f, 0x01},

	/* 2152 */ {0x4, 0x00, 0x04},
	/* 2155 */ {0x4, 0x00, 0x05},
	/* 2158 */ {0x4, 0x40, 0x06},
	/* 2161 */ {0x4, 0x40, 0x07},

	/* 2164 */ {0x6, 0x1c, 0x17},
	/* 2167 */ {0x6, 0xe2, 0x19},
	/* 2170 */ {0x6, 0x1c, 0x1b},
	/* 2173 */ {0x6, 0xe2, 0x1d},
	/* 2176 */ {0x6, 0xaa, 0x1f},
	/* 2179 */ {0x6, 0x70, 0x20},

	/* 2182 */ {0x5, 0x01, 0x10},
	/* 2185 */ {0x5, 0x00, 0x11},
	/* 2188 */ {0x5, 0x01, 0x00},
	/* 2191 */ {0x5, 0x05, 0x01},
		/* 2194 */ {0x5, 0x00, 0xc1},
		/* */
	/* 2197 */ {0x5, 0x00, 0xc2},
	/* 2200 */ {0x5, 0x00, 0xca},

	/* 2203 */ {0x6, 0x70, 0x51},
	/* 2206 */ {0x6, 0xbe, 0x53},
	{}
};

/*
   Made by Tomasz Zablocki (skalamandra@poczta.onet.pl)
 * SPCA505b chip based cameras initialization data
 *
 */
/* jfm */
#define initial_brightness 0x7f	/* 0x0(white)-0xff(black) */
/* #define initial_brightness 0x0	//0x0(white)-0xff(black) */
/*
 * Data to initialize a SPCA505. Common to the CCD and external modes
 */
static const __u16 spca505b_init_data[][3] = {
/* start */
	{0x02, 0x00, 0x00},		/* init */
	{0x02, 0x00, 0x01},
	{0x02, 0x00, 0x02},
	{0x02, 0x00, 0x03},
	{0x02, 0x00, 0x04},
	{0x02, 0x00, 0x05},
	{0x02, 0x00, 0x06},
	{0x02, 0x00, 0x07},
	{0x02, 0x00, 0x08},
	{0x02, 0x00, 0x09},
	{0x03, 0x00, 0x00},
	{0x03, 0x00, 0x01},
	{0x03, 0x00, 0x02},
	{0x03, 0x00, 0x03},
	{0x03, 0x00, 0x04},
	{0x03, 0x00, 0x05},
	{0x03, 0x00, 0x06},
	{0x04, 0x00, 0x00},
	{0x04, 0x00, 0x02},
	{0x04, 0x00, 0x04},
	{0x04, 0x00, 0x05},
	{0x04, 0x00, 0x06},
	{0x04, 0x00, 0x07},
	{0x04, 0x00, 0x08},
	{0x04, 0x00, 0x09},
	{0x04, 0x00, 0x0a},
	{0x04, 0x00, 0x0b},
	{0x04, 0x00, 0x0c},
	{0x07, 0x00, 0x00},
	{0x07, 0x00, 0x03},
	{0x08, 0x00, 0x00},
	{0x08, 0x00, 0x01},
	{0x08, 0x00, 0x02},
	{0x00, 0x01, 0x00},
	{0x00, 0x01, 0x01},
	{0x00, 0x01, 0x34},
	{0x00, 0x01, 0x35},
	{0x06, 0x18, 0x08},
	{0x06, 0xfc, 0x09},
	{0x06, 0xfc, 0x0a},
	{0x06, 0xfc, 0x0b},
	{0x06, 0x18, 0x0c},
	{0x06, 0xfc, 0x0d},
	{0x06, 0xfc, 0x0e},
	{0x06, 0xfc, 0x0f},
	{0x06, 0x18, 0x10},
	{0x06, 0xfe, 0x12},
	{0x06, 0x00, 0x11},
	{0x06, 0x00, 0x14},
	{0x06, 0x00, 0x13},
	{0x06, 0x28, 0x51},
	{0x06, 0xff, 0x53},
	{0x02, 0x00, 0x08},

	{0x03, 0x00, 0x03},
	{0x03, 0x10, 0x03},
	{}
};

/*
 * Data to initialize the camera using the internal CCD
 */
static const __u16 spca505b_open_data_ccd[][3] = {

/* {0x02,0x00,0x00}, */
	{0x03, 0x04, 0x01},		/* rst */
	{0x03, 0x00, 0x01},
	{0x03, 0x00, 0x00},
	{0x03, 0x21, 0x00},
	{0x03, 0x00, 0x04},
	{0x03, 0x00, 0x03},
	{0x03, 0x18, 0x03},
	{0x03, 0x08, 0x01},
	{0x03, 0x1c, 0x03},
	{0x03, 0x5c, 0x03},
	{0x03, 0x5c, 0x03},
	{0x03, 0x18, 0x01},

/* same as 505 */
	{0x04, 0x10, 0x01},
	{0x04, 0x00, 0x04},
	{0x04, 0x00, 0x05},
	{0x04, 0x20, 0x06},
	{0x04, 0x20, 0x07},

	{0x08, 0x0a, 0x00},

	{0x05, 0x00, 0x10},
	{0x05, 0x00, 0x11},
	{0x05, 0x00, 0x12},
	{0x05, 0x6f, 0x00},
	{0x05, initial_brightness >> 6, 0x00},
	{0x05, initial_brightness << 2, 0x01},
	{0x05, 0x00, 0x02},
	{0x05, 0x01, 0x03},
	{0x05, 0x00, 0x04},
	{0x05, 0x03, 0x05},
	{0x05, 0xe0, 0x06},
	{0x05, 0x20, 0x07},
	{0x05, 0xa0, 0x08},
	{0x05, 0x00, 0x12},
	{0x05, 0x02, 0x0f},
	{0x05, 128, 0x14},		/* max exposure off (0=on) */
	{0x05, 0x01, 0xb0},
	{0x05, 0x01, 0xbf},
	{0x03, 0x02, 0x06},
	{0x05, 0x10, 0x46},
	{0x05, 0x08, 0x4a},

	{0x06, 0x00, 0x01},
	{0x06, 0x10, 0x02},
	{0x06, 0x64, 0x07},
	{0x06, 0x18, 0x08},
	{0x06, 0xfc, 0x09},
	{0x06, 0xfc, 0x0a},
	{0x06, 0xfc, 0x0b},
	{0x04, 0x00, 0x01},
	{0x06, 0x18, 0x0c},
	{0x06, 0xfc, 0x0d},
	{0x06, 0xfc, 0x0e},
	{0x06, 0xfc, 0x0f},
	{0x06, 0x11, 0x10},		/* contrast */
	{0x06, 0x00, 0x11},
	{0x06, 0xfe, 0x12},
	{0x06, 0x00, 0x13},
	{0x06, 0x00, 0x14},
	{0x06, 0x9d, 0x51},
	{0x06, 0x40, 0x52},
	{0x06, 0x7c, 0x53},
	{0x06, 0x40, 0x54},
	{0x06, 0x02, 0x57},
	{0x06, 0x03, 0x58},
	{0x06, 0x15, 0x59},
	{0x06, 0x05, 0x5a},
	{0x06, 0x03, 0x56},
	{0x06, 0x02, 0x3f},
	{0x06, 0x00, 0x40},
	{0x06, 0x39, 0x41},
	{0x06, 0x69, 0x42},
	{0x06, 0x87, 0x43},
	{0x06, 0x9e, 0x44},
	{0x06, 0xb1, 0x45},
	{0x06, 0xbf, 0x46},
	{0x06, 0xcc, 0x47},
	{0x06, 0xd5, 0x48},
	{0x06, 0xdd, 0x49},
	{0x06, 0xe3, 0x4a},
	{0x06, 0xe8, 0x4b},
	{0x06, 0xed, 0x4c},
	{0x06, 0xf2, 0x4d},
	{0x06, 0xf7, 0x4e},
	{0x06, 0xfc, 0x4f},
	{0x06, 0xff, 0x50},

	{0x05, 0x01, 0xc0},
	{0x05, 0x10, 0xcb},
	{0x05, 0x40, 0xc1},
	{0x05, 0x04, 0xc2},
	{0x05, 0x00, 0xca},
	{0x05, 0x40, 0xc1},
	{0x05, 0x09, 0xc2},
	{0x05, 0x00, 0xca},
	{0x05, 0xc0, 0xc1},
	{0x05, 0x09, 0xc2},
	{0x05, 0x00, 0xca},
	{0x05, 0x40, 0xc1},
	{0x05, 0x59, 0xc2},
	{0x05, 0x00, 0xca},
	{0x04, 0x00, 0x01},
	{0x05, 0x80, 0xc1},
	{0x05, 0xec, 0xc2},
	{0x05, 0x0, 0xca},

	{0x06, 0x02, 0x57},
	{0x06, 0x01, 0x58},
	{0x06, 0x15, 0x59},
	{0x06, 0x0a, 0x5a},
	{0x06, 0x01, 0x57},
	{0x06, 0x8a, 0x03},
	{0x06, 0x0a, 0x6c},
	{0x06, 0x30, 0x01},
	{0x06, 0x20, 0x02},
	{0x06, 0x00, 0x03},

	{0x05, 0x8c, 0x25},

	{0x06, 0x4d, 0x51},		/* maybe saturation (4d) */
	{0x06, 0x84, 0x53},		/* making green (84) */
	{0x06, 0x00, 0x57},		/* sharpness (1) */
	{0x06, 0x18, 0x08},
	{0x06, 0xfc, 0x09},
	{0x06, 0xfc, 0x0a},
	{0x06, 0xfc, 0x0b},
	{0x06, 0x18, 0x0c},		/* maybe hue (18) */
	{0x06, 0xfc, 0x0d},
	{0x06, 0xfc, 0x0e},
	{0x06, 0xfc, 0x0f},
	{0x06, 0x18, 0x10},		/* maybe contrast (18) */

	{0x05, 0x01, 0x02},

	{0x04, 0x00, 0x08},		/* compression */
	{0x04, 0x12, 0x09},
	{0x04, 0x21, 0x0a},
	{0x04, 0x10, 0x0b},
	{0x04, 0x21, 0x0c},
	{0x04, 0x1d, 0x00},		/* imagetype (1d) */
	{0x04, 0x41, 0x01},		/* hardware snapcontrol */

	{0x04, 0x00, 0x04},
	{0x04, 0x00, 0x05},
	{0x04, 0x10, 0x06},
	{0x04, 0x10, 0x07},
	{0x04, 0x40, 0x06},
	{0x04, 0x40, 0x07},
	{0x04, 0x00, 0x04},
	{0x04, 0x00, 0x05},

	{0x06, 0x1c, 0x17},
	{0x06, 0xe2, 0x19},
	{0x06, 0x1c, 0x1b},
	{0x06, 0xe2, 0x1d},
	{0x06, 0x5f, 0x1f},
	{0x06, 0x32, 0x20},

	{0x05, initial_brightness >> 6, 0x00},
	{0x05, initial_brightness << 2, 0x01},
	{0x05, 0x06, 0xc1},
	{0x05, 0x58, 0xc2},
	{0x05, 0x0, 0xca},
	{0x05, 0x0, 0x11},
	{}
};

static int reg_write(struct usb_device *dev,
		     __u16 reg, __u16 index, __u16 value)
{
	int ret;

	ret = usb_control_msg(dev,
			usb_sndctrlpipe(dev, 0),
			reg,
			USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			value, index, NULL, 0, 500);
	PDEBUG(D_PACK, "reg write: 0x%02x,0x%02x:0x%02x, 0x%x",
		reg, index, value, ret);
	if (ret < 0)
		PDEBUG(D_ERR, "reg write: error %d", ret);
	return ret;
}

/* returns: negative is error, pos or zero is data */
static int reg_read(struct gspca_dev *gspca_dev,
			__u16 reg,	/* bRequest */
			__u16 index,	/* wIndex */
			__u16 length)	/* wLength (1 or 2 only) */
{
	int ret;

	gspca_dev->usb_buf[1] = 0;
	ret = usb_control_msg(gspca_dev->dev,
			usb_rcvctrlpipe(gspca_dev->dev, 0),
			reg,
			USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			(__u16) 0,		/* value */
			(__u16) index,
			gspca_dev->usb_buf, length,
			500);			/* timeout */
	if (ret < 0) {
		PDEBUG(D_ERR, "reg_read err %d", ret);
		return -1;
	}
	return (gspca_dev->usb_buf[1] << 8) + gspca_dev->usb_buf[0];
}

static int write_vector(struct gspca_dev *gspca_dev,
			const __u16 data[][3])
{
	struct usb_device *dev = gspca_dev->dev;
	int ret, i = 0;

	while (data[i][0] != 0 || data[i][1] != 0 || data[i][2] != 0) {
		ret = reg_write(dev, data[i][0], data[i][2], data[i][1]);
		if (ret < 0) {
			PDEBUG(D_ERR,
				"Register write failed for 0x%x,0x%x,0x%x",
				data[i][0], data[i][1], data[i][2]);
			return ret;
		}
		i++;
	}
	return 0;
}

/* this function is called at probe time */
static int sd_config(struct gspca_dev *gspca_dev,
			const struct usb_device_id *id)
{
	struct sd *sd = (struct sd *) gspca_dev;
	struct cam *cam;

	cam = &gspca_dev->cam;
	cam->epaddr = 0x01;
	cam->cam_mode = vga_mode;
	sd->subtype = id->driver_info;
	if (sd->subtype != IntelPCCameraPro)
		cam->nmodes = sizeof vga_mode / sizeof vga_mode[0];
	else			/* no 640x480 for IntelPCCameraPro */
		cam->nmodes = sizeof vga_mode / sizeof vga_mode[0] - 1;
	sd->brightness = sd_ctrls[SD_BRIGHTNESS].qctrl.default_value;

	if (sd->subtype == Nxultra) {
		if (write_vector(gspca_dev, spca505b_init_data))
			return -EIO;
	} else {
		if (write_vector(gspca_dev, spca505_init_data))
			return -EIO;
	}
	return 0;
}

/* this function is called at probe and resume time */
static int sd_init(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;
	int ret;

	PDEBUG(D_STREAM, "Initializing SPCA505");
	if (sd->subtype == Nxultra)
		write_vector(gspca_dev, spca505b_open_data_ccd);
	else
		write_vector(gspca_dev, spca505_open_data_ccd);
	ret = reg_read(gspca_dev, 6, 0x16, 2);

	if (ret < 0) {
		PDEBUG(D_ERR|D_STREAM,
		       "register read failed for after vector read err = %d",
		       ret);
		return -EIO;
	}
	PDEBUG(D_STREAM,
		"After vector read returns : 0x%x should be 0x0101",
		ret & 0xffff);

	ret = reg_write(gspca_dev->dev, 6, 0x16, 0x0a);
	if (ret < 0) {
		PDEBUG(D_ERR, "register write failed for (6,0xa,0x16) err=%d",
		       ret);
		return -EIO;
	}
	reg_write(gspca_dev->dev, 5, 0xc2, 18);
	return 0;
}

static int sd_start(struct gspca_dev *gspca_dev)
{
	struct usb_device *dev = gspca_dev->dev;
	int ret;

	/* necessary because without it we can see stream
	 * only once after loading module */
	/* stopping usb registers Tomasz change */
	reg_write(dev, 0x02, 0x0, 0x0);
	switch (gspca_dev->cam.cam_mode[(int) gspca_dev->curr_mode].priv) {
	case 0:
		reg_write(dev, 0x04, 0x00, 0x00);
		reg_write(dev, 0x04, 0x06, 0x10);
		reg_write(dev, 0x04, 0x07, 0x10);
		break;
	case 1:
		reg_write(dev, 0x04, 0x00, 0x01);
		reg_write(dev, 0x04, 0x06, 0x1a);
		reg_write(dev, 0x04, 0x07, 0x1a);
		break;
	case 2:
		reg_write(dev, 0x04, 0x00, 0x02);
		reg_write(dev, 0x04, 0x06, 0x1c);
		reg_write(dev, 0x04, 0x07, 0x1d);
		break;
	case 4:
		reg_write(dev, 0x04, 0x00, 0x04);
		reg_write(dev, 0x04, 0x06, 0x34);
		reg_write(dev, 0x04, 0x07, 0x34);
		break;
	default:
/*	case 5: */
		reg_write(dev, 0x04, 0x00, 0x05);
		reg_write(dev, 0x04, 0x06, 0x40);
		reg_write(dev, 0x04, 0x07, 0x40);
		break;
	}
/* Enable ISO packet machine - should we do this here or in ISOC init ? */
	ret = reg_write(dev, SPCA50X_REG_USB,
			 SPCA50X_USB_CTRL,
			 SPCA50X_CUSB_ENABLE);

/*	reg_write(dev, 0x5, 0x0, 0x0); */
/*	reg_write(dev, 0x5, 0x0, 0x1); */
/*	reg_write(dev, 0x5, 0x11, 0x2); */
	return ret;
}

static void sd_stopN(struct gspca_dev *gspca_dev)
{
	/* Disable ISO packet machine */
	reg_write(gspca_dev->dev, 0x02, 0x00, 0x00);
}

/* called on streamoff with alt 0 and on disconnect */
static void sd_stop0(struct gspca_dev *gspca_dev)
{
	if (!gspca_dev->present)
		return;

	/* This maybe reset or power control */
	reg_write(gspca_dev->dev, 0x03, 0x03, 0x20);
	reg_write(gspca_dev->dev, 0x03, 0x01, 0x0);
	reg_write(gspca_dev->dev, 0x03, 0x00, 0x1);
	reg_write(gspca_dev->dev, 0x05, 0x10, 0x1);
	reg_write(gspca_dev->dev, 0x05, 0x11, 0xf);
}

static void sd_pkt_scan(struct gspca_dev *gspca_dev,
			struct gspca_frame *frame,	/* target */
			__u8 *data,			/* isoc packet */
			int len)			/* iso packet length */
{
	switch (data[0]) {
	case 0:				/* start of frame */
		frame = gspca_frame_add(gspca_dev, LAST_PACKET, frame,
					data, 0);
		data += SPCA50X_OFFSET_DATA;
		len -= SPCA50X_OFFSET_DATA;
		gspca_frame_add(gspca_dev, FIRST_PACKET, frame,
				data, len);
		break;
	case 0xff:			/* drop */
/*		gspca_dev->last_packet_type = DISCARD_PACKET; */
		break;
	default:
		data += 1;
		len -= 1;
		gspca_frame_add(gspca_dev, INTER_PACKET, frame,
				data, len);
		break;
	}
}

static void setbrightness(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;

	__u8 brightness = sd->brightness;
	reg_write(gspca_dev->dev, 5, 0x00, (255 - brightness) >> 6);
	reg_write(gspca_dev->dev, 5, 0x01, (255 - brightness) << 2);

}
static void getbrightness(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;

	sd->brightness = 255
		- ((reg_read(gspca_dev, 5, 0x01, 1) >> 2)
			+ (reg_read(gspca_dev, 5, 0x0, 1) << 6));
}

static int sd_setbrightness(struct gspca_dev *gspca_dev, __s32 val)
{
	struct sd *sd = (struct sd *) gspca_dev;

	sd->brightness = val;
	if (gspca_dev->streaming)
		setbrightness(gspca_dev);
	return 0;
}

static int sd_getbrightness(struct gspca_dev *gspca_dev, __s32 *val)
{
	struct sd *sd = (struct sd *) gspca_dev;

	getbrightness(gspca_dev);
	*val = sd->brightness;
	return 0;
}

/* sub-driver description */
static const struct sd_desc sd_desc = {
	.name = MODULE_NAME,
	.ctrls = sd_ctrls,
	.nctrls = ARRAY_SIZE(sd_ctrls),
	.config = sd_config,
	.init = sd_init,
	.start = sd_start,
	.stopN = sd_stopN,
	.stop0 = sd_stop0,
	.pkt_scan = sd_pkt_scan,
};

/* -- module initialisation -- */
static const __devinitdata struct usb_device_id device_table[] = {
	{USB_DEVICE(0x041e, 0x401d), .driver_info = Nxultra},
	{USB_DEVICE(0x0733, 0x0430), .driver_info = IntelPCCameraPro},
/*fixme: may be UsbGrabberPV321 BRIDGE_SPCA506 SENSOR_SAA7113 */
	{}
};
MODULE_DEVICE_TABLE(usb, device_table);

/* -- device connect -- */
static int sd_probe(struct usb_interface *intf,
			const struct usb_device_id *id)
{
	return gspca_dev_probe(intf, id, &sd_desc, sizeof(struct sd),
				THIS_MODULE);
}

static struct usb_driver sd_driver = {
	.name = MODULE_NAME,
	.id_table = device_table,
	.probe = sd_probe,
	.disconnect = gspca_disconnect,
#ifdef CONFIG_PM
	.suspend = gspca_suspend,
	.resume = gspca_resume,
#endif
};

/* -- module insert / remove -- */
static int __init sd_mod_init(void)
{
	if (usb_register(&sd_driver) < 0)
		return -1;
	PDEBUG(D_PROBE, "registered");
	return 0;
}
static void __exit sd_mod_exit(void)
{
	usb_deregister(&sd_driver);
	PDEBUG(D_PROBE, "deregistered");
}

module_init(sd_mod_init);
module_exit(sd_mod_exit);
