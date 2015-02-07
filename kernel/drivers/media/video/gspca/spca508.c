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
 * SPCA508 chip based cameras subdriver
 *
 * V4L2 by Jean-Francois Moine <http://moinejf.free.fr>
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
 */

#define MODULE_NAME "spca508"

#include "gspca.h"

MODULE_AUTHOR("Michel Xhaard <mxhaard@users.sourceforge.net>");
MODULE_DESCRIPTION("GSPCA/SPCA508 USB Camera Driver");
MODULE_LICENSE("GPL");

/* specific webcam descriptor */
struct sd {
	struct gspca_dev gspca_dev;		/* !! must be the first item */

	unsigned char brightness;

	char subtype;
#define CreativeVista 0
#define HamaUSBSightcam 1
#define HamaUSBSightcam2 2
#define IntelEasyPCCamera 3
#define MicroInnovationIC200 4
#define ViewQuestVQ110 5
};

/* V4L2 controls supported by the driver */
static int sd_setbrightness(struct gspca_dev *gspca_dev, __s32 val);
static int sd_getbrightness(struct gspca_dev *gspca_dev, __s32 *val);

static struct ctrl sd_ctrls[] = {
	{
	    {
		.id      = V4L2_CID_BRIGHTNESS,
		.type    = V4L2_CTRL_TYPE_INTEGER,
		.name    = "Brightness",
		.minimum = 0,
		.maximum = 255,
		.step    = 1,
#define BRIGHTNESS_DEF 128
		.default_value = BRIGHTNESS_DEF,
	    },
	    .set = sd_setbrightness,
	    .get = sd_getbrightness,
	},
};

static const struct v4l2_pix_format sif_mode[] = {
	{160, 120, V4L2_PIX_FMT_SPCA508, V4L2_FIELD_NONE,
		.bytesperline = 160,
		.sizeimage = 160 * 120 * 3 / 2,
		.colorspace = V4L2_COLORSPACE_SRGB,
		.priv = 3},
	{176, 144, V4L2_PIX_FMT_SPCA508, V4L2_FIELD_NONE,
		.bytesperline = 176,
		.sizeimage = 176 * 144 * 3 / 2,
		.colorspace = V4L2_COLORSPACE_SRGB,
		.priv = 2},
	{320, 240, V4L2_PIX_FMT_SPCA508, V4L2_FIELD_NONE,
		.bytesperline = 320,
		.sizeimage = 320 * 240 * 3 / 2,
		.colorspace = V4L2_COLORSPACE_SRGB,
		.priv = 1},
	{352, 288, V4L2_PIX_FMT_SPCA508, V4L2_FIELD_NONE,
		.bytesperline = 352,
		.sizeimage = 352 * 288 * 3 / 2,
		.colorspace = V4L2_COLORSPACE_SRGB,
		.priv = 0},
};

/* Frame packet header offsets for the spca508 */
#define SPCA508_OFFSET_TYPE 1
#define SPCA508_OFFSET_COMPRESS 2
#define SPCA508_OFFSET_FRAMSEQ 8
#define SPCA508_OFFSET_WIN1LUM 11
#define SPCA508_OFFSET_DATA 37

#define SPCA508_SNAPBIT 0x20
#define SPCA508_SNAPCTRL 0x40
/*************** I2c ****************/
#define SPCA508_INDEX_I2C_BASE 0x8800

/*
 * Initialization data: this is the first set-up data written to the
 * device (before the open data).
 */
static const __u16 spca508_init_data[][3] =
#define IGN(x)			/* nothing */
{
	/*  line   URB      value, index */
	/* 44274  1804 */ {0x0000, 0x870b},

	/* 44299  1805 */ {0x0020, 0x8112},
	/* Video drop enable, ISO streaming disable */
	/* 44324  1806 */ {0x0003, 0x8111},
	/* Reset compression & memory */
	/* 44349  1807 */ {0x0000, 0x8110},
	/* Disable all outputs */
	/* 44372  1808 */ /* READ {0x0000, 0x8114} -> 0000: 00  */
	/* 44398  1809 */ {0x0000, 0x8114},
	/* SW GPIO data */
	/* 44423  1810 */ {0x0008, 0x8110},
	/* Enable charge pump output */
	/* 44527  1811 */ {0x0002, 0x8116},
	/* 200 kHz pump clock */
	/* 44555  1812 */
		/* UNKNOWN DIRECTION (URB_FUNCTION_SELECT_INTERFACE:) */
	/* 44590  1813 */ {0x0003, 0x8111},
	/* Reset compression & memory */
	/* 44615  1814 */ {0x0000, 0x8111},
	/* Normal mode (not reset) */
	/* 44640  1815 */ {0x0098, 0x8110},
	/* Enable charge pump output, sync.serial,external 2x clock */
	/* 44665  1816 */ {0x000d, 0x8114},
	/* SW GPIO data */
	/* 44690  1817 */ {0x0002, 0x8116},
	/* 200 kHz pump clock */
	/* 44715  1818 */ {0x0020, 0x8112},
	/* Video drop enable, ISO streaming disable */
/* --------------------------------------- */
	/* 44740  1819 */ {0x000f, 0x8402},
	/* memory bank */
	/* 44765  1820 */ {0x0000, 0x8403},
	/* ... address */
/* --------------------------------------- */
/* 0x88__ is Synchronous Serial Interface. */
/* TBD: This table could be expressed more compactly */
/* using spca508_write_i2c_vector(). */
/* TBD: Should see if the values in spca50x_i2c_data */
/* would work with the VQ110 instead of the values */
/* below. */
	/* 44790  1821 */ {0x00c0, 0x8804},
	/* SSI slave addr */
	/* 44815  1822 */ {0x0008, 0x8802},
	/* 375 Khz SSI clock */
	/* 44838  1823 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 44862  1824 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 44888  1825 */ {0x0008, 0x8802},
	/* 375 Khz SSI clock */
	/* 44913  1826 */ {0x0012, 0x8801},
	/* SSI reg addr */
	/* 44938  1827 */ {0x0080, 0x8800},
	/* SSI data to write */
	/* 44961  1828 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 44985  1829 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 45009  1830 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 45035  1831 */ {0x0008, 0x8802},
	/* 375 Khz SSI clock */
	/* 45060  1832 */ {0x0012, 0x8801},
	/* SSI reg addr */
	/* 45085  1833 */ {0x0000, 0x8800},
	/* SSI data to write */
	/* 45108  1834 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 45132  1835 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 45156  1836 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 45182  1837 */ {0x0008, 0x8802},
	/* 375 Khz SSI clock */
	/* 45207  1838 */ {0x0011, 0x8801},
	/* SSI reg addr */
	/* 45232  1839 */ {0x0040, 0x8800},
	/* SSI data to write */
	/* 45255  1840 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 45279  1841 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 45303  1842 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 45329  1843 */ {0x0008, 0x8802},
	/* 45354  1844 */ {0x0013, 0x8801},
	/* 45379  1845 */ {0x0000, 0x8800},
	/* 45402  1846 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 45426  1847 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 45450  1848 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 45476  1849 */ {0x0008, 0x8802},
	/* 45501  1850 */ {0x0014, 0x8801},
	/* 45526  1851 */ {0x0000, 0x8800},
	/* 45549  1852 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 45573  1853 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 45597  1854 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 45623  1855 */ {0x0008, 0x8802},
	/* 45648  1856 */ {0x0015, 0x8801},
	/* 45673  1857 */ {0x0001, 0x8800},
	/* 45696  1858 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 45720  1859 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 45744  1860 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 45770  1861 */ {0x0008, 0x8802},
	/* 45795  1862 */ {0x0016, 0x8801},
	/* 45820  1863 */ {0x0003, 0x8800},
	/* 45843  1864 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 45867  1865 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 45891  1866 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 45917  1867 */ {0x0008, 0x8802},
	/* 45942  1868 */ {0x0017, 0x8801},
	/* 45967  1869 */ {0x0036, 0x8800},
	/* 45990  1870 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 46014  1871 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 46038  1872 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 46064  1873 */ {0x0008, 0x8802},
	/* 46089  1874 */ {0x0018, 0x8801},
	/* 46114  1875 */ {0x00ec, 0x8800},
	/* 46137  1876 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 46161  1877 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 46185  1878 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 46211  1879 */ {0x0008, 0x8802},
	/* 46236  1880 */ {0x001a, 0x8801},
	/* 46261  1881 */ {0x0094, 0x8800},
	/* 46284  1882 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 46308  1883 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 46332  1884 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 46358  1885 */ {0x0008, 0x8802},
	/* 46383  1886 */ {0x001b, 0x8801},
	/* 46408  1887 */ {0x0000, 0x8800},
	/* 46431  1888 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 46455  1889 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 46479  1890 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 46505  1891 */ {0x0008, 0x8802},
	/* 46530  1892 */ {0x0027, 0x8801},
	/* 46555  1893 */ {0x00a2, 0x8800},
	/* 46578  1894 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 46602  1895 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 46626  1896 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 46652  1897 */ {0x0008, 0x8802},
	/* 46677  1898 */ {0x0028, 0x8801},
	/* 46702  1899 */ {0x0040, 0x8800},
	/* 46725  1900 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 46749  1901 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 46773  1902 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 46799  1903 */ {0x0008, 0x8802},
	/* 46824  1904 */ {0x002a, 0x8801},
	/* 46849  1905 */ {0x0084, 0x8800},
	/* 46872  1906 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 46896  1907 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00 */
	/* 46920  1908 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 46946  1909 */ {0x0008, 0x8802},
	/* 46971  1910 */ {0x002b, 0x8801},
	/* 46996  1911 */ {0x00a8, 0x8800},
	/* 47019  1912 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 47043  1913 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 47067  1914 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 47093  1915 */ {0x0008, 0x8802},
	/* 47118  1916 */ {0x002c, 0x8801},
	/* 47143  1917 */ {0x00fe, 0x8800},
	/* 47166  1918 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 47190  1919 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 47214  1920 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 47240  1921 */ {0x0008, 0x8802},
	/* 47265  1922 */ {0x002d, 0x8801},
	/* 47290  1923 */ {0x0003, 0x8800},
	/* 47313  1924 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 47337  1925 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 47361  1926 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 47387  1927 */ {0x0008, 0x8802},
	/* 47412  1928 */ {0x0038, 0x8801},
	/* 47437  1929 */ {0x0083, 0x8800},
	/* 47460  1930 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 47484  1931 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 47508  1932 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 47534  1933 */ {0x0008, 0x8802},
	/* 47559  1934 */ {0x0033, 0x8801},
	/* 47584  1935 */ {0x0081, 0x8800},
	/* 47607  1936 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 47631  1937 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 47655  1938 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 47681  1939 */ {0x0008, 0x8802},
	/* 47706  1940 */ {0x0034, 0x8801},
	/* 47731  1941 */ {0x004a, 0x8800},
	/* 47754  1942 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 47778  1943 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 47802  1944 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 47828  1945 */ {0x0008, 0x8802},
	/* 47853  1946 */ {0x0039, 0x8801},
	/* 47878  1947 */ {0x0000, 0x8800},
	/* 47901  1948 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 47925  1949 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 47949  1950 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 47975  1951 */ {0x0008, 0x8802},
	/* 48000  1952 */ {0x0010, 0x8801},
	/* 48025  1953 */ {0x00a8, 0x8800},
	/* 48048  1954 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 48072  1955 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 48096  1956 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 48122  1957 */ {0x0008, 0x8802},
	/* 48147  1958 */ {0x0006, 0x8801},
	/* 48172  1959 */ {0x0058, 0x8800},
	/* 48195  1960 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 48219  1961 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00 */
	/* 48243  1962 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 48269  1963 */ {0x0008, 0x8802},
	/* 48294  1964 */ {0x0000, 0x8801},
	/* 48319  1965 */ {0x0004, 0x8800},
	/* 48342  1966 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 48366  1967 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 48390  1968 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 48416  1969 */ {0x0008, 0x8802},
	/* 48441  1970 */ {0x0040, 0x8801},
	/* 48466  1971 */ {0x0080, 0x8800},
	/* 48489  1972 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 48513  1973 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 48537  1974 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 48563  1975 */ {0x0008, 0x8802},
	/* 48588  1976 */ {0x0041, 0x8801},
	/* 48613  1977 */ {0x000c, 0x8800},
	/* 48636  1978 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 48660  1979 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 48684  1980 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 48710  1981 */ {0x0008, 0x8802},
	/* 48735  1982 */ {0x0042, 0x8801},
	/* 48760  1983 */ {0x000c, 0x8800},
	/* 48783  1984 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 48807  1985 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 48831  1986 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 48857  1987 */ {0x0008, 0x8802},
	/* 48882  1988 */ {0x0043, 0x8801},
	/* 48907  1989 */ {0x0028, 0x8800},
	/* 48930  1990 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 48954  1991 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 48978  1992 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 49004  1993 */ {0x0008, 0x8802},
	/* 49029  1994 */ {0x0044, 0x8801},
	/* 49054  1995 */ {0x0080, 0x8800},
	/* 49077  1996 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 49101  1997 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 49125  1998 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 49151  1999 */ {0x0008, 0x8802},
	/* 49176  2000 */ {0x0045, 0x8801},
	/* 49201  2001 */ {0x0020, 0x8800},
	/* 49224  2002 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 49248  2003 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 49272  2004 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 49298  2005 */ {0x0008, 0x8802},
	/* 49323  2006 */ {0x0046, 0x8801},
	/* 49348  2007 */ {0x0020, 0x8800},
	/* 49371  2008 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 49395  2009 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 49419  2010 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 49445  2011 */ {0x0008, 0x8802},
	/* 49470  2012 */ {0x0047, 0x8801},
	/* 49495  2013 */ {0x0080, 0x8800},
	/* 49518  2014 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 49542  2015 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 49566  2016 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 49592  2017 */ {0x0008, 0x8802},
	/* 49617  2018 */ {0x0048, 0x8801},
	/* 49642  2019 */ {0x004c, 0x8800},
	/* 49665  2020 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 49689  2021 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 49713  2022 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 49739  2023 */ {0x0008, 0x8802},
	/* 49764  2024 */ {0x0049, 0x8801},
	/* 49789  2025 */ {0x0084, 0x8800},
	/* 49812  2026 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 49836  2027 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 49860  2028 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 49886  2029 */ {0x0008, 0x8802},
	/* 49911  2030 */ {0x004a, 0x8801},
	/* 49936  2031 */ {0x0084, 0x8800},
	/* 49959  2032 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 49983  2033 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 50007  2034 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 50033  2035 */ {0x0008, 0x8802},
	/* 50058  2036 */ {0x004b, 0x8801},
	/* 50083  2037 */ {0x0084, 0x8800},
	/* 50106  2038 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* --------------------------------------- */
	/* 50132  2039 */ {0x0012, 0x8700},
	/* Clock speed 48Mhz/(2+2)/2= 6 Mhz */
	/* 50157  2040 */ {0x0000, 0x8701},
	/* CKx1 clock delay adj */
	/* 50182  2041 */ {0x0000, 0x8701},
	/* CKx1 clock delay adj */
	/* 50207  2042 */ {0x0001, 0x870c},
	/* CKOx2 output */
	/* --------------------------------------- */
	/* 50232  2043 */ {0x0080, 0x8600},
	/* Line memory read counter (L) */
	/* 50257  2044 */ {0x0001, 0x8606},
	/* reserved */
	/* 50282  2045 */ {0x0064, 0x8607},
	/* Line memory read counter (H) 0x6480=25,728 */
	/* 50307  2046 */ {0x002a, 0x8601},
	/* CDSP sharp interpolation mode,
	 *			line sel for color sep, edge enhance enab */
	/* 50332  2047 */ {0x0000, 0x8602},
	/* optical black level for user settng = 0 */
	/* 50357  2048 */ {0x0080, 0x8600},
	/* Line memory read counter (L) */
	/* 50382  2049 */ {0x000a, 0x8603},
	/* optical black level calc mode: auto; optical black offset = 10 */
	/* 50407  2050 */ {0x00df, 0x865b},
	/* Horiz offset for valid pixels (L)=0xdf */
	/* 50432  2051 */ {0x0012, 0x865c},
	/* Vert offset for valid lines (L)=0x12 */

/* The following two lines seem to be the "wrong" resolution. */
/* But perhaps these indicate the actual size of the sensor */
/* rather than the size of the current video mode. */
	/* 50457  2052 */ {0x0058, 0x865d},
	/* Horiz valid pixels (*4) (L) = 352 */
	/* 50482  2053 */ {0x0048, 0x865e},
	/* Vert valid lines (*4) (L) = 288 */

	/* 50507  2054 */ {0x0015, 0x8608},
	/* A11 Coef ... */
	/* 50532  2055 */ {0x0030, 0x8609},
	/* 50557  2056 */ {0x00fb, 0x860a},
	/* 50582  2057 */ {0x003e, 0x860b},
	/* 50607  2058 */ {0x00ce, 0x860c},
	/* 50632  2059 */ {0x00f4, 0x860d},
	/* 50657  2060 */ {0x00eb, 0x860e},
	/* 50682  2061 */ {0x00dc, 0x860f},
	/* 50707  2062 */ {0x0039, 0x8610},
	/* 50732  2063 */ {0x0001, 0x8611},
	/* R offset for white balance ... */
	/* 50757  2064 */ {0x0000, 0x8612},
	/* 50782  2065 */ {0x0001, 0x8613},
	/* 50807  2066 */ {0x0000, 0x8614},
	/* 50832  2067 */ {0x005b, 0x8651},
	/* R gain for white balance ... */
	/* 50857  2068 */ {0x0040, 0x8652},
	/* 50882  2069 */ {0x0060, 0x8653},
	/* 50907  2070 */ {0x0040, 0x8654},
	/* 50932  2071 */ {0x0000, 0x8655},
	/* 50957  2072 */ {0x0001, 0x863f},
	/* Fixed gamma correction enable, USB control,
	 *			 lum filter disable, lum noise clip disable */
	/* 50982  2073 */ {0x00a1, 0x8656},
	/* Window1 size 256x256, Windows2 size 64x64,
	 *		 gamma look-up disable, new edge enhancement enable */
	/* 51007  2074 */ {0x0018, 0x8657},
	/* Edge gain high thresh */
	/* 51032  2075 */ {0x0020, 0x8658},
	/* Edge gain low thresh */
	/* 51057  2076 */ {0x000a, 0x8659},
	/* Edge bandwidth high threshold */
	/* 51082  2077 */ {0x0005, 0x865a},
	/* Edge bandwidth low threshold */
	/* -------------------------------- */
	/* 51107  2078 */ {0x0030, 0x8112},
	/* Video drop enable, ISO streaming enable */
	/* 51130  2079 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 51154  2080 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 51180  2081 */ {0xa908, 0x8802},
	/* 51205  2082 */ {0x0034, 0x8801},
	/* SSI reg addr */
	/* 51230  2083 */ {0x00ca, 0x8800},
	/* SSI data to write */
	/* 51253  2084 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 51277  2085 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 51301  2086 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 51327  2087 */ {0x1f08, 0x8802},
	/* 51352  2088 */ {0x0006, 0x8801},
	/* 51377  2089 */ {0x0080, 0x8800},
	/* 51400  2090 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */

/* ----- Read back coefs we wrote earlier. */
	/* 51424  2091 */ /* READ { 0, 0x0000, 0x8608 } -> 0000: 15  */
	/* 51448  2092 */ /* READ { 0, 0x0000, 0x8609 } -> 0000: 30  */
	/* 51472  2093 */ /* READ { 0, 0x0000, 0x860a } -> 0000: fb  */
	/* 51496  2094 */ /* READ { 0, 0x0000, 0x860b } -> 0000: 3e  */
	/* 51520  2095 */ /* READ { 0, 0x0000, 0x860c } -> 0000: ce  */
	/* 51544  2096 */ /* READ { 0, 0x0000, 0x860d } -> 0000: f4  */
	/* 51568  2097 */ /* READ { 0, 0x0000, 0x860e } -> 0000: eb  */
	/* 51592  2098 */ /* READ { 0, 0x0000, 0x860f } -> 0000: dc  */
	/* 51616  2099 */ /* READ { 0, 0x0000, 0x8610 } -> 0000: 39  */
	/* 51640  2100 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 51664  2101 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 08  */
	/* 51690  2102 */ {0xb008, 0x8802},
	/* 51715  2103 */ {0x0006, 0x8801},
	/* 51740  2104 */ {0x007d, 0x8800},
	/* 51763  2105 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */


	/* This chunk is seemingly redundant with */
	/* earlier commands (A11 Coef...), but if I disable it, */
	/* the image appears too dark.  Maybe there was some kind of */
	/* reset since the earlier commands, so this is necessary again. */
	/* 51789  2106 */ {0x0015, 0x8608},
	/* 51814  2107 */ {0x0030, 0x8609},
	/* 51839  2108 */ {0xfffb, 0x860a},
	/* 51864  2109 */ {0x003e, 0x860b},
	/* 51889  2110 */ {0xffce, 0x860c},
	/* 51914  2111 */ {0xfff4, 0x860d},
	/* 51939  2112 */ {0xffeb, 0x860e},
	/* 51964  2113 */ {0xffdc, 0x860f},
	/* 51989  2114 */ {0x0039, 0x8610},
	/* 52014  2115 */ {0x0018, 0x8657},

	/* 52039  2116 */ {0x0000, 0x8508},
	/* Disable compression. */
	/* Previous line was:
	 * 52039  2116 *  { 0, 0x0021, 0x8508 },  * Enable compression. */
	/* 52064  2117 */ {0x0032, 0x850b},
	/* compression stuff */
	/* 52089  2118 */ {0x0003, 0x8509},
	/* compression stuff */
	/* 52114  2119 */ {0x0011, 0x850a},
	/* compression stuff */
	/* 52139  2120 */ {0x0021, 0x850d},
	/* compression stuff */
	/* 52164  2121 */ {0x0010, 0x850c},
	/* compression stuff */
	/* 52189  2122 */ {0x0003, 0x8500},
	/* *** Video mode: 160x120 */
	/* 52214  2123 */ {0x0001, 0x8501},
	/* Hardware-dominated snap control */
	/* 52239  2124 */ {0x0061, 0x8656},
	/* Window1 size 128x128, Windows2 size 128x128,
	 *		gamma look-up disable, new edge enhancement enable */
	/* 52264  2125 */ {0x0018, 0x8617},
	/* Window1 start X (*2) */
	/* 52289  2126 */ {0x0008, 0x8618},
	/* Window1 start Y (*2) */
	/* 52314  2127 */ {0x0061, 0x8656},
	/* Window1 size 128x128, Windows2 size 128x128,
	 *		gamma look-up disable, new edge enhancement enable */
	/* 52339  2128 */ {0x0058, 0x8619},
	/* Window2 start X (*2) */
	/* 52364  2129 */ {0x0008, 0x861a},
	/* Window2 start Y (*2) */
	/* 52389  2130 */ {0x00ff, 0x8615},
	/* High lum thresh for white balance */
	/* 52414  2131 */ {0x0000, 0x8616},
	/* Low lum thresh for white balance */
	/* 52439  2132 */ {0x0012, 0x8700},
	/* Clock speed 48Mhz/(2+2)/2= 6 Mhz */
	/* 52464  2133 */ {0x0012, 0x8700},
	/* Clock speed 48Mhz/(2+2)/2= 6 Mhz */
	/* 52487  2134 */ /* READ { 0, 0x0000, 0x8656 } -> 0000: 61  */
	/* 52513  2135 */ {0x0028, 0x8802},
	/* 375 Khz SSI clock, SSI r/w sync with VSYNC */
	/* 52536  2136 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 52560  2137 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 28  */
	/* 52586  2138 */ {0x1f28, 0x8802},
	/* 375 Khz SSI clock, SSI r/w sync with VSYNC */
	/* 52611  2139 */ {0x0010, 0x8801},
	/* SSI reg addr */
	/* 52636  2140 */ {0x003e, 0x8800},
	/* SSI data to write */
	/* 52659  2141 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 52685  2142 */ {0x0028, 0x8802},
	/* 52708  2143 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 52732  2144 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 28  */
	/* 52758  2145 */ {0x1f28, 0x8802},
	/* 52783  2146 */ {0x0000, 0x8801},
	/* 52808  2147 */ {0x001f, 0x8800},
	/* 52831  2148 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 52857  2149 */ {0x0001, 0x8602},
	/* optical black level for user settning = 1 */

	/* Original: */
	/* 52882  2150 */ {0x0023, 0x8700},
	/* Clock speed 48Mhz/(3+2)/4= 2.4 Mhz */
	/* 52907  2151 */ {0x000f, 0x8602},
	/* optical black level for user settning = 15 */

	/* 52932  2152 */ {0x0028, 0x8802},
	/* 52955  2153 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 52979  2154 */ /* READ { 0, 0x0001, 0x8802 } -> 0000: 28  */
	/* 53005  2155 */ {0x1f28, 0x8802},
	/* 53030  2156 */ {0x0010, 0x8801},
	/* 53055  2157 */ {0x007b, 0x8800},
	/* 53078  2158 */ /* READ { 0, 0x0001, 0x8803 } -> 0000: 00  */
	/* 53104  2159 */ {0x002f, 0x8651},
	/* R gain for white balance ... */
	/* 53129  2160 */ {0x0080, 0x8653},
	/* 53152  2161 */ /* READ { 0, 0x0000, 0x8655 } -> 0000: 00  */
	/* 53178  2162 */ {0x0000, 0x8655},

	/* 53203  2163 */ {0x0030, 0x8112},
	/* Video drop enable, ISO streaming enable */
	/* 53228  2164 */ {0x0020, 0x8112},
	/* Video drop enable, ISO streaming disable */
	/* 53252  2165 */
	     /* UNKNOWN DIRECTION (URB_FUNCTION_SELECT_INTERFACE: (ALT=0) ) */
	{}
};


/*
 * Initialization data for Intel EasyPC Camera CS110
 */
static const __u16 spca508cs110_init_data[][3] = {
	{0x0000, 0x870b}, /* Reset CTL3 */
	{0x0003, 0x8111}, /* Soft Reset compression, memory, TG & CDSP */
	{0x0000, 0x8111}, /* Normal operation on reset */
	{0x0090, 0x8110},
		 /* External Clock 2x & Synchronous Serial Interface Output */
	{0x0020, 0x8112}, /* Video Drop packet enable */
	{0x0000, 0x8114}, /* Software GPIO output data */
	{0x0001, 0x8114},
	{0x0001, 0x8114},
	{0x0001, 0x8114},
	{0x0003, 0x8114},

	/* Initial sequence Synchronous Serial Interface */
	{0x000f, 0x8402}, /* Memory bank Address */
	{0x0000, 0x8403}, /* Memory bank Address */
	{0x00ba, 0x8804}, /* SSI Slave address */
	{0x0010, 0x8802}, /* 93.75kHz SSI Clock Two DataByte */
	{0x0010, 0x8802}, /* 93.75kHz SSI Clock two DataByte */

	{0x0001, 0x8801},
	{0x000a, 0x8805},/* a - NWG: Dunno what this is about */
	{0x0000, 0x8800},
	{0x0010, 0x8802},

	{0x0002, 0x8801},
	{0x0000, 0x8805},
	{0x0000, 0x8800},
	{0x0010, 0x8802},

	{0x0003, 0x8801},
	{0x0027, 0x8805},
	{0x0001, 0x8800},
	{0x0010, 0x8802},

	{0x0004, 0x8801},
	{0x0065, 0x8805},
	{0x0001, 0x8800},
	{0x0010, 0x8802},

	{0x0005, 0x8801},
	{0x0003, 0x8805},
	{0x0000, 0x8800},
	{0x0010, 0x8802},

	{0x0006, 0x8801},
	{0x001c, 0x8805},
	{0x0000, 0x8800},
	{0x0010, 0x8802},

	{0x0007, 0x8801},
	{0x002a, 0x8805},
	{0x0000, 0x8800},
	{0x0010, 0x8802},

	{0x0002, 0x8704}, /* External input CKIx1 */
	{0x0001, 0x8606}, /* 1 Line memory Read Counter (H) Result: (d)410 */
	{0x009a, 0x8600}, /* Line memory Read Counter (L) */
	{0x0001, 0x865b}, /* 1 Horizontal Offset for Valid Pixel(L) */
	{0x0003, 0x865c}, /* 3 Vertical Offset for Valid Lines(L) */
	{0x0058, 0x865d}, /* 58 Horizontal Valid Pixel Window(L) */

	{0x0006, 0x8660}, /* Nibble data + input order */

	{0x000a, 0x8602}, /* Optical black level set to 0x0a */
/* 1945 */ {0x0000, 0x8603}, /* Optical black level Offset */

/* 1962 *  {0, 0x0000, 0x8611},  * 0 R  Offset for white Balance */
/* 1963 *  {0, 0x0000, 0x8612},  * 1 Gr Offset for white Balance */
/* 1964 *  {0, 0x0000, 0x8613},  * 1f B  Offset for white Balance */
/* 1965 *  {0, 0x0000, 0x8614},  * f0 Gb Offset for white Balance */

	{0x0040, 0x8651}, /* 2b BLUE gain for white balance  good at all 60 */
	{0x0030, 0x8652}, /* 41 Gr Gain for white Balance (L) */
	{0x0035, 0x8653}, /* 26 RED gain for white balance */
	{0x0035, 0x8654}, /* 40Gb Gain for white Balance (L) */
	{0x0041, 0x863f},
	      /* Fixed Gamma correction enabled (makes colours look better) */

/* 2422 */ {0x0000, 0x8655},
	/* High bits for white balance*****brightness control*** */
	{}
};

static const __u16 spca508_sightcam_init_data[][3] = {
/* This line seems to setup the frame/canvas */
	/*368  */ {0x000f, 0x8402},

/* Theese 6 lines are needed to startup the webcam */
	/*398  */ {0x0090, 0x8110},
	/*399  */ {0x0001, 0x8114},
	/*400  */ {0x0001, 0x8114},
	/*401  */ {0x0001, 0x8114},
	/*402  */ {0x0003, 0x8114},
	/*403  */ {0x0080, 0x8804},

/* This part seems to make the pictures darker? (autobrightness?) */
	/*436  */ {0x0001, 0x8801},
	/*437  */ {0x0004, 0x8800},
	/*439  */ {0x0003, 0x8801},
	/*440  */ {0x00e0, 0x8800},
	/*442  */ {0x0004, 0x8801},
	/*443  */ {0x00b4, 0x8800},
	/*445  */ {0x0005, 0x8801},
	/*446  */ {0x0000, 0x8800},

	/*448  */ {0x0006, 0x8801},
	/*449  */ {0x00e0, 0x8800},
	/*451  */ {0x0007, 0x8801},
	/*452  */ {0x000c, 0x8800},

/* This section is just needed, it probably
 * does something like the previous section,
 * but the cam won't start if it's not included.
 */
	/*484  */ {0x0014, 0x8801},
	/*485  */ {0x0008, 0x8800},
	/*487  */ {0x0015, 0x8801},
	/*488  */ {0x0067, 0x8800},
	/*490  */ {0x0016, 0x8801},
	/*491  */ {0x0000, 0x8800},
	/*493  */ {0x0017, 0x8801},
	/*494  */ {0x0020, 0x8800},
	/*496  */ {0x0018, 0x8801},
	/*497  */ {0x0044, 0x8800},

/* Makes the picture darker - and the
 * cam won't start if not included
 */
	/*505  */ {0x001e, 0x8801},
	/*506  */ {0x00ea, 0x8800},
	/*508  */ {0x001f, 0x8801},
	/*509  */ {0x0001, 0x8800},
	/*511  */ {0x0003, 0x8801},
	/*512  */ {0x00e0, 0x8800},

/* seems to place the colors ontop of each other #1 */
	/*517  */ {0x0006, 0x8704},
	/*518  */ {0x0001, 0x870c},
	/*519  */ {0x0016, 0x8600},
	/*520  */ {0x0002, 0x8606},

/* if not included the pictures becomes _very_ dark */
	/*521  */ {0x0064, 0x8607},
	/*522  */ {0x003a, 0x8601},
	/*523  */ {0x0000, 0x8602},

/* seems to place the colors ontop of each other #2 */
	/*524  */ {0x0016, 0x8600},
	/*525  */ {0x0018, 0x8617},
	/*526  */ {0x0008, 0x8618},
	/*527  */ {0x00a1, 0x8656},

/* webcam won't start if not included */
	/*528  */ {0x0007, 0x865b},
	/*529  */ {0x0001, 0x865c},
	/*530  */ {0x0058, 0x865d},
	/*531  */ {0x0048, 0x865e},

/* adjusts the colors */
	/*541  */ {0x0049, 0x8651},
	/*542  */ {0x0040, 0x8652},
	/*543  */ {0x004c, 0x8653},
	/*544  */ {0x0040, 0x8654},
	{}
};

static const __u16 spca508_sightcam2_init_data[][3] = {
/* 35 */ {0x0020, 0x8112},

/* 36 */ {0x000f, 0x8402},
/* 37 */ {0x0000, 0x8403},

/* 38 */ {0x0008, 0x8201},
/* 39 */ {0x0008, 0x8200},
/* 40 */ {0x0001, 0x8200},
/* 43 */ {0x0009, 0x8201},
/* 44 */ {0x0008, 0x8200},
/* 45 */ {0x0001, 0x8200},
/* 48 */ {0x000a, 0x8201},
/* 49 */ {0x0008, 0x8200},
/* 50 */ {0x0001, 0x8200},
/* 53 */ {0x000b, 0x8201},
/* 54 */ {0x0008, 0x8200},
/* 55 */ {0x0001, 0x8200},
/* 58 */ {0x000c, 0x8201},
/* 59 */ {0x0008, 0x8200},
/* 60 */ {0x0001, 0x8200},
/* 63 */ {0x000d, 0x8201},
/* 64 */ {0x0008, 0x8200},
/* 65 */ {0x0001, 0x8200},
/* 68 */ {0x000e, 0x8201},
/* 69 */ {0x0008, 0x8200},
/* 70 */ {0x0001, 0x8200},
/* 73 */ {0x0007, 0x8201},
/* 74 */ {0x0008, 0x8200},
/* 75 */ {0x0001, 0x8200},
/* 78 */ {0x000f, 0x8201},
/* 79 */ {0x0008, 0x8200},
/* 80 */ {0x0001, 0x8200},

/* 84 */ {0x0018, 0x8660},
/* 85 */ {0x0010, 0x8201},

/* 86 */ {0x0008, 0x8200},
/* 87 */ {0x0001, 0x8200},
/* 90 */ {0x0011, 0x8201},
/* 91 */ {0x0008, 0x8200},
/* 92 */ {0x0001, 0x8200},

/* 95 */ {0x0000, 0x86b0},
/* 96 */ {0x0034, 0x86b1},
/* 97 */ {0x0000, 0x86b2},
/* 98 */ {0x0049, 0x86b3},
/* 99 */ {0x0000, 0x86b4},
/* 100 */ {0x0000, 0x86b4},

/* 101 */ {0x0012, 0x8201},
/* 102 */ {0x0008, 0x8200},
/* 103 */ {0x0001, 0x8200},
/* 106 */ {0x0013, 0x8201},
/* 107 */ {0x0008, 0x8200},
/* 108 */ {0x0001, 0x8200},

/* 111 */ {0x0001, 0x86b0},
/* 112 */ {0x00aa, 0x86b1},
/* 113 */ {0x0000, 0x86b2},
/* 114 */ {0x00e4, 0x86b3},
/* 115 */ {0x0000, 0x86b4},
/* 116 */ {0x0000, 0x86b4},

/* 118 */ {0x0018, 0x8660},

/* 119 */ {0x0090, 0x8110},
/* 120 */ {0x0001, 0x8114},
/* 121 */ {0x0001, 0x8114},
/* 122 */ {0x0001, 0x8114},
/* 123 */ {0x0003, 0x8114},

/* 124 */ {0x0080, 0x8804},
/* 157 */ {0x0003, 0x8801},
/* 158 */ {0x0012, 0x8800},
/* 160 */ {0x0004, 0x8801},
/* 161 */ {0x0005, 0x8800},
/* 163 */ {0x0005, 0x8801},
/* 164 */ {0x0000, 0x8800},
/* 166 */ {0x0006, 0x8801},
/* 167 */ {0x0000, 0x8800},
/* 169 */ {0x0007, 0x8801},
/* 170 */ {0x0000, 0x8800},
/* 172 */ {0x0008, 0x8801},
/* 173 */ {0x0005, 0x8800},
/* 175 */ {0x000a, 0x8700},
/* 176 */ {0x000e, 0x8801},
/* 177 */ {0x0004, 0x8800},
/* 179 */ {0x0005, 0x8801},
/* 180 */ {0x0047, 0x8800},
/* 182 */ {0x0006, 0x8801},
/* 183 */ {0x0000, 0x8800},
/* 185 */ {0x0007, 0x8801},
/* 186 */ {0x00c0, 0x8800},
/* 188 */ {0x0008, 0x8801},
/* 189 */ {0x0003, 0x8800},
/* 191 */ {0x0013, 0x8801},
/* 192 */ {0x0001, 0x8800},
/* 194 */ {0x0009, 0x8801},
/* 195 */ {0x0000, 0x8800},
/* 197 */ {0x000a, 0x8801},
/* 198 */ {0x0000, 0x8800},
/* 200 */ {0x000b, 0x8801},
/* 201 */ {0x0000, 0x8800},
/* 203 */ {0x000c, 0x8801},
/* 204 */ {0x0000, 0x8800},
/* 206 */ {0x000e, 0x8801},
/* 207 */ {0x0004, 0x8800},
/* 209 */ {0x000f, 0x8801},
/* 210 */ {0x0000, 0x8800},
/* 212 */ {0x0010, 0x8801},
/* 213 */ {0x0006, 0x8800},
/* 215 */ {0x0011, 0x8801},
/* 216 */ {0x0006, 0x8800},
/* 218 */ {0x0012, 0x8801},
/* 219 */ {0x0000, 0x8800},
/* 221 */ {0x0013, 0x8801},
/* 222 */ {0x0001, 0x8800},

/* 224 */ {0x000a, 0x8700},
/* 225 */ {0x0000, 0x8702},
/* 226 */ {0x0000, 0x8703},
/* 227 */ {0x00c2, 0x8704},
/* 228 */ {0x0001, 0x870c},

/* 229 */ {0x0044, 0x8600},
/* 230 */ {0x0002, 0x8606},
/* 231 */ {0x0064, 0x8607},
/* 232 */ {0x003a, 0x8601},
/* 233 */ {0x0008, 0x8602},
/* 234 */ {0x0044, 0x8600},
/* 235 */ {0x0018, 0x8617},
/* 236 */ {0x0008, 0x8618},
/* 237 */ {0x00a1, 0x8656},
/* 238 */ {0x0004, 0x865b},
/* 239 */ {0x0002, 0x865c},
/* 240 */ {0x0058, 0x865d},
/* 241 */ {0x0048, 0x865e},
/* 242 */ {0x0012, 0x8608},
/* 243 */ {0x002c, 0x8609},
/* 244 */ {0x0002, 0x860a},
/* 245 */ {0x002c, 0x860b},
/* 246 */ {0x00db, 0x860c},
/* 247 */ {0x00f9, 0x860d},
/* 248 */ {0x00f1, 0x860e},
/* 249 */ {0x00e3, 0x860f},
/* 250 */ {0x002c, 0x8610},
/* 251 */ {0x006c, 0x8651},
/* 252 */ {0x0041, 0x8652},
/* 253 */ {0x0059, 0x8653},
/* 254 */ {0x0040, 0x8654},
/* 255 */ {0x00fa, 0x8611},
/* 256 */ {0x00ff, 0x8612},
/* 257 */ {0x00f8, 0x8613},
/* 258 */ {0x0000, 0x8614},
/* 259 */ {0x0001, 0x863f},
/* 260 */ {0x0000, 0x8640},
/* 261 */ {0x0026, 0x8641},
/* 262 */ {0x0045, 0x8642},
/* 263 */ {0x0060, 0x8643},
/* 264 */ {0x0075, 0x8644},
/* 265 */ {0x0088, 0x8645},
/* 266 */ {0x009b, 0x8646},
/* 267 */ {0x00b0, 0x8647},
/* 268 */ {0x00c5, 0x8648},
/* 269 */ {0x00d2, 0x8649},
/* 270 */ {0x00dc, 0x864a},
/* 271 */ {0x00e5, 0x864b},
/* 272 */ {0x00eb, 0x864c},
/* 273 */ {0x00f0, 0x864d},
/* 274 */ {0x00f6, 0x864e},
/* 275 */ {0x00fa, 0x864f},
/* 276 */ {0x00ff, 0x8650},
/* 277 */ {0x0060, 0x8657},
/* 278 */ {0x0010, 0x8658},
/* 279 */ {0x0018, 0x8659},
/* 280 */ {0x0005, 0x865a},
/* 281 */ {0x0018, 0x8660},
/* 282 */ {0x0003, 0x8509},
/* 283 */ {0x0011, 0x850a},
/* 284 */ {0x0032, 0x850b},
/* 285 */ {0x0010, 0x850c},
/* 286 */ {0x0021, 0x850d},
/* 287 */ {0x0001, 0x8500},
/* 288 */ {0x0000, 0x8508},
/* 289 */ {0x0012, 0x8608},
/* 290 */ {0x002c, 0x8609},
/* 291 */ {0x0002, 0x860a},
/* 292 */ {0x0039, 0x860b},
/* 293 */ {0x00d0, 0x860c},
/* 294 */ {0x00f7, 0x860d},
/* 295 */ {0x00ed, 0x860e},
/* 296 */ {0x00db, 0x860f},
/* 297 */ {0x0039, 0x8610},
/* 298 */ {0x0012, 0x8657},
/* 299 */ {0x000c, 0x8619},
/* 300 */ {0x0004, 0x861a},
/* 301 */ {0x00a1, 0x8656},
/* 302 */ {0x00c8, 0x8615},
/* 303 */ {0x0032, 0x8616},

/* 306 */ {0x0030, 0x8112},
/* 313 */ {0x0020, 0x8112},
/* 314 */ {0x0020, 0x8112},
/* 315 */ {0x000f, 0x8402},
/* 316 */ {0x0000, 0x8403},

/* 317 */ {0x0090, 0x8110},
/* 318 */ {0x0001, 0x8114},
/* 319 */ {0x0001, 0x8114},
/* 320 */ {0x0001, 0x8114},
/* 321 */ {0x0003, 0x8114},
/* 322 */ {0x0080, 0x8804},

/* 355 */ {0x0003, 0x8801},
/* 356 */ {0x0012, 0x8800},
/* 358 */ {0x0004, 0x8801},
/* 359 */ {0x0005, 0x8800},
/* 361 */ {0x0005, 0x8801},
/* 362 */ {0x0047, 0x8800},
/* 364 */ {0x0006, 0x8801},
/* 365 */ {0x0000, 0x8800},
/* 367 */ {0x0007, 0x8801},
/* 368 */ {0x00c0, 0x8800},
/* 370 */ {0x0008, 0x8801},
/* 371 */ {0x0003, 0x8800},
/* 373 */ {0x000a, 0x8700},
/* 374 */ {0x000e, 0x8801},
/* 375 */ {0x0004, 0x8800},
/* 377 */ {0x0005, 0x8801},
/* 378 */ {0x0047, 0x8800},
/* 380 */ {0x0006, 0x8801},
/* 381 */ {0x0000, 0x8800},
/* 383 */ {0x0007, 0x8801},
/* 384 */ {0x00c0, 0x8800},
/* 386 */ {0x0008, 0x8801},
/* 387 */ {0x0003, 0x8800},
/* 389 */ {0x0013, 0x8801},
/* 390 */ {0x0001, 0x8800},
/* 392 */ {0x0009, 0x8801},
/* 393 */ {0x0000, 0x8800},
/* 395 */ {0x000a, 0x8801},
/* 396 */ {0x0000, 0x8800},
/* 398 */ {0x000b, 0x8801},
/* 399 */ {0x0000, 0x8800},
/* 401 */ {0x000c, 0x8801},
/* 402 */ {0x0000, 0x8800},
/* 404 */ {0x000e, 0x8801},
/* 405 */ {0x0004, 0x8800},
/* 407 */ {0x000f, 0x8801},
/* 408 */ {0x0000, 0x8800},
/* 410 */ {0x0010, 0x8801},
/* 411 */ {0x0006, 0x8800},
/* 413 */ {0x0011, 0x8801},
/* 414 */ {0x0006, 0x8800},
/* 416 */ {0x0012, 0x8801},
/* 417 */ {0x0000, 0x8800},
/* 419 */ {0x0013, 0x8801},
/* 420 */ {0x0001, 0x8800},
/* 422 */ {0x000a, 0x8700},
/* 423 */ {0x0000, 0x8702},
/* 424 */ {0x0000, 0x8703},
/* 425 */ {0x00c2, 0x8704},
/* 426 */ {0x0001, 0x870c},
/* 427 */ {0x0044, 0x8600},
/* 428 */ {0x0002, 0x8606},
/* 429 */ {0x0064, 0x8607},
/* 430 */ {0x003a, 0x8601},
/* 431 */ {0x0008, 0x8602},
/* 432 */ {0x0044, 0x8600},
/* 433 */ {0x0018, 0x8617},
/* 434 */ {0x0008, 0x8618},
/* 435 */ {0x00a1, 0x8656},
/* 436 */ {0x0004, 0x865b},
/* 437 */ {0x0002, 0x865c},
/* 438 */ {0x0058, 0x865d},
/* 439 */ {0x0048, 0x865e},
/* 440 */ {0x0012, 0x8608},
/* 441 */ {0x002c, 0x8609},
/* 442 */ {0x0002, 0x860a},
/* 443 */ {0x002c, 0x860b},
/* 444 */ {0x00db, 0x860c},
/* 445 */ {0x00f9, 0x860d},
/* 446 */ {0x00f1, 0x860e},
/* 447 */ {0x00e3, 0x860f},
/* 448 */ {0x002c, 0x8610},
/* 449 */ {0x006c, 0x8651},
/* 450 */ {0x0041, 0x8652},
/* 451 */ {0x0059, 0x8653},
/* 452 */ {0x0040, 0x8654},
/* 453 */ {0x00fa, 0x8611},
/* 454 */ {0x00ff, 0x8612},
/* 455 */ {0x00f8, 0x8613},
/* 456 */ {0x0000, 0x8614},
/* 457 */ {0x0001, 0x863f},
/* 458 */ {0x0000, 0x8640},
/* 459 */ {0x0026, 0x8641},
/* 460 */ {0x0045, 0x8642},
/* 461 */ {0x0060, 0x8643},
/* 462 */ {0x0075, 0x8644},
/* 463 */ {0x0088, 0x8645},
/* 464 */ {0x009b, 0x8646},
/* 465 */ {0x00b0, 0x8647},
/* 466 */ {0x00c5, 0x8648},
/* 467 */ {0x00d2, 0x8649},
/* 468 */ {0x00dc, 0x864a},
/* 469 */ {0x00e5, 0x864b},
/* 470 */ {0x00eb, 0x864c},
/* 471 */ {0x00f0, 0x864d},
/* 472 */ {0x00f6, 0x864e},
/* 473 */ {0x00fa, 0x864f},
/* 474 */ {0x00ff, 0x8650},
/* 475 */ {0x0060, 0x8657},
/* 476 */ {0x0010, 0x8658},
/* 477 */ {0x0018, 0x8659},
/* 478 */ {0x0005, 0x865a},
/* 479 */ {0x0018, 0x8660},
/* 480 */ {0x0003, 0x8509},
/* 481 */ {0x0011, 0x850a},
/* 482 */ {0x0032, 0x850b},
/* 483 */ {0x0010, 0x850c},
/* 484 */ {0x0021, 0x850d},
/* 485 */ {0x0001, 0x8500},
/* 486 */ {0x0000, 0x8508},

/* 487 */ {0x0012, 0x8608},
/* 488 */ {0x002c, 0x8609},
/* 489 */ {0x0002, 0x860a},
/* 490 */ {0x0039, 0x860b},
/* 491 */ {0x00d0, 0x860c},
/* 492 */ {0x00f7, 0x860d},
/* 493 */ {0x00ed, 0x860e},
/* 494 */ {0x00db, 0x860f},
/* 495 */ {0x0039, 0x8610},
/* 496 */ {0x0012, 0x8657},
/* 497 */ {0x0064, 0x8619},

/* This line starts it all, it is not needed here */
/* since it has been build into the driver */
/* jfm: don't start now */
/* 590  *  {0x0030, 0x8112}, */
	{}
};

/*
 * Initialization data for Creative Webcam Vista
 */
static const __u16 spca508_vista_init_data[][3] = {
	{0x0008, 0x8200},	/* Clear register */
	{0x0000, 0x870b},	/* Reset CTL3 */
	{0x0020, 0x8112},	/* Video Drop packet enable */
	{0x0003, 0x8111},  /* Soft Reset compression, memory, TG & CDSP */
	{0x0000, 0x8110},	/* Disable everything */
	{0x0000, 0x8114},	/* Software GPIO output data */
	{0x0000, 0x8114},

	{0x0003, 0x8111},
	{0x0000, 0x8111},
	{0x0090, 0x8110},  /* Enable: SSI output, External 2X clock output */
	{0x0020, 0x8112},
	{0x0000, 0x8114},
	{0x0001, 0x8114},
	{0x0001, 0x8114},
	{0x0001, 0x8114},
	{0x0003, 0x8114},

	{0x000f, 0x8402},	/* Memory bank Address */
	{0x0000, 0x8403},	/* Memory bank Address */
	{0x00ba, 0x8804},	/* SSI Slave address */
	{0x0010, 0x8802},	/* 93.75kHz SSI Clock Two DataByte */

	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */
	/* READ { 0, 0x0001, 0x8802 } ->
		0000: 10  */
	{0x0010, 0x8802},	/* Will write 2 bytes (DATA1+DATA2) */
	{0x0020, 0x8801},	/* Register address for SSI read/write */
	{0x0044, 0x8805},	/* DATA2 */
	{0x0004, 0x8800},	/* DATA1 -> write triggered */
	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */

	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */
	/* READ { 0, 0x0001, 0x8802 } ->
		0000: 10  */
	{0x0010, 0x8802},
	{0x0009, 0x8801},
	{0x0042, 0x8805},
	{0x0001, 0x8800},
	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */

	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */
	/* READ { 0, 0x0001, 0x8802 } ->
		0000: 10  */
	{0x0010, 0x8802},
	{0x003c, 0x8801},
	{0x0001, 0x8805},
	{0x0000, 0x8800},
	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */

	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */
	/* READ { 0, 0x0001, 0x8802 } ->
		0000: 10  */
	{0x0010, 0x8802},
	{0x0001, 0x8801},
	{0x000a, 0x8805},
	{0x0000, 0x8800},
	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */

	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */
	/* READ { 0, 0x0001, 0x8802 } ->
		0000: 10  */
	{0x0010, 0x8802},
	{0x0002, 0x8801},
	{0x0000, 0x8805},
	{0x0000, 0x8800},
	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */

	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */
	/* READ { 0, 0x0001, 0x8802 } ->
		0000: 10  */
	{0x0010, 0x8802},
	{0x0003, 0x8801},
	{0x0027, 0x8805},
	{0x0001, 0x8800},
	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */

	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */
	/* READ { 0, 0x0001, 0x8802 } ->
		0000: 10  */
	{0x0010, 0x8802},
	{0x0004, 0x8801},
	{0x0065, 0x8805},
	{0x0001, 0x8800},
	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */

	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */
	/* READ { 0, 0x0001, 0x8802 } ->
		0000: 10  */
	{0x0010, 0x8802},
	{0x0005, 0x8801},
	{0x0003, 0x8805},
	{0x0000, 0x8800},
	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */

	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */
	/* READ { 0, 0x0001, 0x8802 } ->
		0000: 10  */
	{0x0010, 0x8802},
	{0x0006, 0x8801},
	{0x001c, 0x8805},
	{0x0000, 0x8800},
	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */

	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */
	/* READ { 0, 0x0001, 0x8802 } ->
		0000: 10  */
	{0x0010, 0x8802},
	{0x0007, 0x8801},
	{0x002a, 0x8805},
	{0x0000, 0x8800},
	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */

	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */
	/* READ { 0, 0x0001, 0x8802 } ->
		0000: 10  */
	{0x0010, 0x8802},
	{0x000e, 0x8801},
	{0x0000, 0x8805},
	{0x0000, 0x8800},
	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */

	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */
	/* READ { 0, 0x0001, 0x8802 } ->
		0000: 10  */
	{0x0010, 0x8802},
	{0x0028, 0x8801},
	{0x002e, 0x8805},
	{0x0000, 0x8800},
	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */

	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */
	/* READ { 0, 0x0001, 0x8802 } ->
		0000: 10  */
	{0x0010, 0x8802},
	{0x0039, 0x8801},
	{0x0013, 0x8805},
	{0x0000, 0x8800},
	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */

	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */
	/* READ { 0, 0x0001, 0x8802 } ->
		0000: 10  */
	{0x0010, 0x8802},
	{0x003b, 0x8801},
	{0x000c, 0x8805},
	{0x0000, 0x8800},
	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */

	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */
	/* READ { 0, 0x0001, 0x8802 } ->
		0000: 10  */
	{0x0010, 0x8802},
	{0x0035, 0x8801},
	{0x0028, 0x8805},
	{0x0000, 0x8800},
	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */

	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */
	/* READ { 0, 0x0001, 0x8802 } ->
		0000: 10  */
	{0x0010, 0x8802},
	{0x0009, 0x8801},
	{0x0042, 0x8805},
	{0x0001, 0x8800},
	/* READ { 0, 0x0001, 0x8803 } ->
		0000: 00  */

	{0x0050, 0x8703},
	{0x0002, 0x8704},	/* External input CKIx1 */
	{0x0001, 0x870C},	/* Select CKOx2 output */
	{0x009A, 0x8600},	/* Line memory Read Counter (L) */
	{0x0001, 0x8606},  /* 1 Line memory Read Counter (H) Result: (d)410 */
	{0x0023, 0x8601},
	{0x0010, 0x8602},
	{0x000A, 0x8603},
	{0x009A, 0x8600},
	{0x0001, 0x865B},	/* 1 Horizontal Offset for Valid Pixel(L) */
	{0x0003, 0x865C},	/* Vertical offset for valid lines (L) */
	{0x0058, 0x865D},	/* Horizontal valid pixels window (L) */
	{0x0048, 0x865E},	/* Vertical valid lines window (L) */
	{0x0000, 0x865F},

	{0x0006, 0x8660},
		    /* Enable nibble data input, select nibble input order */

	{0x0013, 0x8608},	/* A11 Coeficients for color correction */
	{0x0028, 0x8609},
		    /* Note: these values are confirmed at the end of array */
	{0x0005, 0x860A},	/* ... */
	{0x0025, 0x860B},
	{0x00E1, 0x860C},
	{0x00FA, 0x860D},
	{0x00F4, 0x860E},
	{0x00E8, 0x860F},
	{0x0025, 0x8610},	/* A33 Coef. */
	{0x00FC, 0x8611},	/* White balance offset: R */
	{0x0001, 0x8612},	/* White balance offset: Gr */
	{0x00FE, 0x8613},	/* White balance offset: B */
	{0x0000, 0x8614},	/* White balance offset: Gb */

	{0x0064, 0x8651},	/* R gain for white balance (L) */
	{0x0040, 0x8652},	/* Gr gain for white balance (L) */
	{0x0066, 0x8653},	/* B gain for white balance (L) */
	{0x0040, 0x8654},	/* Gb gain for white balance (L) */
	{0x0001, 0x863F},	/* Enable fixed gamma correction */

	{0x00A1, 0x8656},	/* Size - Window1: 256x256, Window2: 128x128 */
	/* UV division: UV no change, Enable New edge enhancement */
	{0x0018, 0x8657},	/* Edge gain high threshold */
	{0x0020, 0x8658},	/* Edge gain low threshold */
	{0x000A, 0x8659},	/* Edge bandwidth high threshold */
	{0x0005, 0x865A},	/* Edge bandwidth low threshold */
	{0x0064, 0x8607},	/* UV filter enable */

	{0x0016, 0x8660},
	{0x0000, 0x86B0},	/* Bad pixels compensation address */
	{0x00DC, 0x86B1},	/* X coord for bad pixels compensation (L) */
	{0x0000, 0x86B2},
	{0x0009, 0x86B3},	/* Y coord for bad pixels compensation (L) */
	{0x0000, 0x86B4},

	{0x0001, 0x86B0},
	{0x00F5, 0x86B1},
	{0x0000, 0x86B2},
	{0x00C6, 0x86B3},
	{0x0000, 0x86B4},

	{0x0002, 0x86B0},
	{0x001C, 0x86B1},
	{0x0001, 0x86B2},
	{0x00D7, 0x86B3},
	{0x0000, 0x86B4},

	{0x0003, 0x86B0},
	{0x001C, 0x86B1},
	{0x0001, 0x86B2},
	{0x00D8, 0x86B3},
	{0x0000, 0x86B4},

	{0x0004, 0x86B0},
	{0x001D, 0x86B1},
	{0x0001, 0x86B2},
	{0x00D8, 0x86B3},
	{0x0000, 0x86B4},
	{0x001E, 0x8660},

	/* READ { 0, 0x0000, 0x8608 } ->
		0000: 13  */
	/* READ { 0, 0x0000, 0x8609 } ->
		0000: 28  */
	/* READ { 0, 0x0000, 0x8610 } ->
		0000: 05  */
	/* READ { 0, 0x0000, 0x8611 } ->
		0000: 25  */
	/* READ { 0, 0x0000, 0x8612 } ->
		0000: e1  */
	/* READ { 0, 0x0000, 0x8613 } ->
		0000: fa  */
	/* READ { 0, 0x0000, 0x8614 } ->
		0000: f4  */
	/* READ { 0, 0x0000, 0x8615 } ->
		0000: e8  */
	/* READ { 0, 0x0000, 0x8616 } ->
		0000: 25  */
	{}
};

static int reg_write(struct usb_device *dev,
			__u16 index, __u16 value)
{
	int ret;

	ret = usb_control_msg(dev,
			usb_sndctrlpipe(dev, 0),
			0,		/* request */
			USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			value, index, NULL, 0, 500);
	PDEBUG(D_USBO, "reg write i:0x%04x = 0x%02x",
		index, value);
	if (ret < 0)
		PDEBUG(D_ERR|D_USBO, "reg write: error %d", ret);
	return ret;
}

/* read 1 byte */
/* returns: negative is error, pos or zero is data */
static int reg_read(struct gspca_dev *gspca_dev,
			__u16 index)	/* wIndex */
{
	int ret;

	ret = usb_control_msg(gspca_dev->dev,
			usb_rcvctrlpipe(gspca_dev->dev, 0),
			0,			/* register */
			USB_DIR_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			0,		/* value */
			index,
			gspca_dev->usb_buf, 1,
			500);			/* timeout */
	PDEBUG(D_USBI, "reg read i:%04x --> %02x",
		index, gspca_dev->usb_buf[0]);
	if (ret < 0) {
		PDEBUG(D_ERR|D_USBI, "reg_read err %d", ret);
		return ret;
	}
	return gspca_dev->usb_buf[0];
}

static int write_vector(struct gspca_dev *gspca_dev,
			const __u16 data[][3])
{
	struct usb_device *dev = gspca_dev->dev;
	int ret, i = 0;

	while (data[i][1] != 0) {
		ret = reg_write(dev, data[i][1], data[i][0]);
		if (ret < 0)
			return ret;
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
	int data1, data2;

	/* Read from global register the USB product and vendor IDs, just to
	 * prove that we can communicate with the device.  This works, which
	 * confirms at we are communicating properly and that the device
	 * is a 508. */
	data1 = reg_read(gspca_dev, 0x8104);
	data2 = reg_read(gspca_dev, 0x8105);
	PDEBUG(D_PROBE, "Webcam Vendor ID: 0x%02x%02x", data2, data1);

	data1 = reg_read(gspca_dev, 0x8106);
	data2 = reg_read(gspca_dev, 0x8107);
	PDEBUG(D_PROBE, "Webcam Product ID: 0x%02x%02x", data2, data1);

	data1 = reg_read(gspca_dev, 0x8621);
	PDEBUG(D_PROBE, "Window 1 average luminance: %d", data1);

	cam = &gspca_dev->cam;
	cam->epaddr = 0x01;
	cam->cam_mode = sif_mode;
	cam->nmodes = ARRAY_SIZE(sif_mode);

	sd->subtype = id->driver_info;
	sd->brightness = BRIGHTNESS_DEF;

	switch (sd->subtype) {
	case ViewQuestVQ110:
		if (write_vector(gspca_dev, spca508_init_data))
			return -1;
		break;
	default:
/*	case MicroInnovationIC200: */
/*	case IntelEasyPCCamera: */
		if (write_vector(gspca_dev, spca508cs110_init_data))
			return -1;
		break;
	case HamaUSBSightcam:
		if (write_vector(gspca_dev, spca508_sightcam_init_data))
			return -1;
		break;
	case HamaUSBSightcam2:
		if (write_vector(gspca_dev, spca508_sightcam2_init_data))
			return -1;
		break;
	case CreativeVista:
		if (write_vector(gspca_dev, spca508_vista_init_data))
			return -1;
		break;
	}
	return 0;			/* success */
}

/* this function is called at probe and resume time */
static int sd_init(struct gspca_dev *gspca_dev)
{
/*	write_vector(gspca_dev, spca508_open_data); */
	return 0;
}

static int sd_start(struct gspca_dev *gspca_dev)
{
	int mode;

	mode = gspca_dev->cam.cam_mode[(int) gspca_dev->curr_mode].priv;
	reg_write(gspca_dev->dev, 0x8500, mode);
	switch (mode) {
	case 0:
	case 1:
		reg_write(gspca_dev->dev, 0x8700, 0x28);	/* clock */
		break;
	default:
/*	case 2: */
/*	case 3: */
		reg_write(gspca_dev->dev, 0x8700, 0x23);	/* clock */
		break;
	}
	reg_write(gspca_dev->dev, 0x8112, 0x10 | 0x20);
	return 0;
}

static void sd_stopN(struct gspca_dev *gspca_dev)
{
	/* Video ISO disable, Video Drop Packet enable: */
	reg_write(gspca_dev->dev, 0x8112, 0x20);
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
		data += SPCA508_OFFSET_DATA;
		len -= SPCA508_OFFSET_DATA;
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

	/* MX seem contrast */
	reg_write(gspca_dev->dev, 0x8651, brightness);
	reg_write(gspca_dev->dev, 0x8652, brightness);
	reg_write(gspca_dev->dev, 0x8653, brightness);
	reg_write(gspca_dev->dev, 0x8654, brightness);
}

static void getbrightness(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;

	sd->brightness = reg_read(gspca_dev, 0x8651);
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
	.pkt_scan = sd_pkt_scan,
};

/* -- module initialisation -- */
static const __devinitdata struct usb_device_id device_table[] = {
	{USB_DEVICE(0x0130, 0x0130), .driver_info = HamaUSBSightcam},
	{USB_DEVICE(0x041e, 0x4018), .driver_info = CreativeVista},
	{USB_DEVICE(0x0461, 0x0815), .driver_info = MicroInnovationIC200},
	{USB_DEVICE(0x0733, 0x0110), .driver_info = ViewQuestVQ110},
	{USB_DEVICE(0x0af9, 0x0010), .driver_info = HamaUSBSightcam},
	{USB_DEVICE(0x0af9, 0x0011), .driver_info = HamaUSBSightcam2},
	{USB_DEVICE(0x8086, 0x0110), .driver_info = IntelEasyPCCamera},
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
