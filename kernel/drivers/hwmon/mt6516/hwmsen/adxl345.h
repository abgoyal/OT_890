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
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/* linux/drivers/hwmon/adxl345.c
 *
 * (C) Copyright 2008 
 * MediaTek <www.mediatek.com>
 *
 * ADXL345 driver for MT6516
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __ADXL345_H
#define __ADXL345_H
 
/******************************************************************************
 * Function Configuration
******************************************************************************/
#define ADXL345_TEST_MODE
/******************************************************************************
 * Definition
******************************************************************************/
#define ADX_TAG					"<ADXL345> "
#define ADX_DEV_NAME		    "ADXL345"
#define ADX_FUN(f)				printk(KERN_INFO ADX_TAG"%s\n", __FUNCTION__)
#define ADX_ERR(fmt, args...)	printk(KERN_ERR ADX_TAG"%s %d : "fmt, __FUNCTION__, __LINE__, ##args)
#define ADX_LOG(fmt, args...)	printk(KERN_INFO ADX_TAG fmt, ##args)
#define ADX_VER(fmt, args...)   ((void)0)
#define ADXL345_WR_SLAVE_ADDR	0xA6
#define ADXL345_RD_SLAVE_ADDR	0xA7
#define ADXL345_AXES_NUM		3
#define ADXL345_DATA_LEN		0x06	/*data length including X,Y,Z*/
#define ADXL345_ADDR_MAX        0x39
#define ADXL345_AXIS_X			0
#define ADXL345_AXIS_Y			1
#define ADXL345_AXIS_Z			2
/*position in the sysfs attribute array*/
#define ADXL345_ATTR_X_OFFSET	0
#define ADXL345_ATTR_Y_OFFSET	1
#define ADXL345_ATTR_Z_OFFSET	2
#define ADXL345_ATTR_DATA		3
#define ADXL345_ATTR_REGS       4
/*In MT6516, the interrupt is triggered through EINT9/EINT3*/
#define ADXL345_INT1_NUM        9
#define ADXL345_INT2_NUM        3

/*Register definition*/
#define ADXL345_REG_DEVID		        0x00
#define ADXL345_REG_THRESH_TAP          0x1D
#define ADXL345_REG_OFSX                0x1E
#define ADXL345_REG_OFSY                0x1F
#define ADXL345_REG_OFSZ                0x20
#define ADXL345_REG_DUR                 0x21
#define ADXL345_REG_LATENT              0x22
#define ADXL345_REG_WINDOW              0x23
#define ADXL345_REG_THRESH_ACT          0x24
#define ADXL345_REG_THRESH_INACT        0x25
#define ADXL345_REG_TIME_INACT          0x26
#define ADXL345_REG_ACT_INACT_CTL       0x27
#define ADXL345_REG_THRESH_FF           0x28
#define ADXL345_REG_TIME_FF             0x29
#define ADXL345_REG_TAP_AXES            0x2A
#define ADXL345_REG_ACT_TAP_STATUS      0x2B
#define ADXL345_REG_BW_RATE             0x2C
#define ADXL345_REG_POWER_CTL           0x2D
#define ADXL345_REG_INT_ENABLE          0x2E
#define ADXL345_REG_INT_MAP             0x2F
#define ADXL345_REG_INT_SOURCE          0x30
#define ADXL345_REG_DATA_FORMAT         0x31
#define ADXL345_REG_DATAX0              0x32
#define ADXL345_REG_DATAX1              0x33
#define ADXL345_REG_DATAY0              0x34
#define ADXL345_REG_DATAY1              0x35
#define ADXL345_REG_DATAZ0              0x36
#define ADXL345_REG_DATAZ1              0x37
#define ADXL345_REG_FIFO_CTL            0x38
#define ADXL345_REG_FIFO_STATUS         0x39

/* Bit values in ACT_INACT_CTL*/ 
#define ADXL345_INACT_Z_ENABLE            0x01 
#define ADXL345_INACT_Z_DISABLE           0x00 
#define ADXL345_INACT_Y_ENABLE            0x02 
#define ADXL345_INACT_Y_DISABLE           0x00 
#define ADXL345_INACT_X_ENABLE            0x04 
#define ADXL345_INACT_X_DISABLE           0x00 
#define ADXL345_INACT_AC                  0x08 
#define ADXL345_INACT_DC                  0x00 
#define ADXL345_ACT_Z_ENABLE              0x10 
#define ADXL345_ACT_Z_DISABLE             0x00 
#define ADXL345_ACT_Y_ENABLE              0x20 
#define ADXL345_ACT_Y_DISABLE             0x00 
#define ADXL345_ACT_X_ENABLE              0x40 
#define ADXL345_ACT_X_DISABLE             0x00 
#define ADXL345_ACT_AC                    0x80 
#define ADXL345_ACT_DC                    0x00 

/* Bit values in TAP_AXES*/ 
#define ADXL345_TAP_Z_ENABLE              0x01 
#define ADXL345_TAP_Z_DISABLE             0x00 
#define ADXL345_TAP_Y_ENABLE              0x02 
#define ADXL345_TAP_Y_DISABLE             0x00 
#define ADXL345_TAP_X_ENABLE              0x04 
#define ADXL345_TAP_X_DISABLE             0x00 
#define ADXL345_TAP_SUPPRESS              0x08 
           
/* Bit values in ACT_TAP_STATUS */ 
#define ADXL345_TAP_Z_SOURCE              0x01 
#define ADXL345_TAP_Y_SOURCE              0x02 
#define ADXL345_TAP_X_SOURCE              0x04 
#define ADXL345_STAT_ASLEEP               0x08 
#define ADXL345_ACT_Z_SOURCE              0x10 
#define ADXL345_ACT_Y_SOURCE              0x20 
#define ADXL345_ACT_X_SOURCE              0x40 
           
/* Bit values in BW_RATE, expresed as output data rate */ 
#define ADXL345_RATE_3200                 0x0f 
#define ADXL345_RATE_1600                 0x0e 
#define ADXL345_RATE_800                  0x0d 
#define ADXL345_RATE_400                  0x0c 
#define ADXL345_RATE_200                  0x0b 
#define ADXL345_RATE_100                  0x0a 
#define ADXL345_RATE_50                   0x09 
#define ADXL345_RATE_25                   0x08 
#define ADXL345_RATE_12_5                 0x07 
#define ADXL345_RATE_6_25                 0x06 
#define ADXL345_RATE_3_125                0x05 
#define ADXL345_RATE_1_563                0x04 
#define ADXL345_RATE__782                 0x03 
#define ADXL345_RATE__39                  0x02 
#define ADXL345_RATE__195                 0x01 
#define ADXL345_RATE__098                 0x00 

/* Expressed as output bandwidth 
   Use either the bandwidth or rate code, 
   whichever is more appropriate for your application */ 
#define ADXL345_BW_1600                   0x0f 
#define ADXL345_BW_800                    0x0e 
#define ADXL345_BW_400                    0x0d 
#define ADXL345_BW_200                    0x0c 
#define ADXL345_BW_100                    0x0b 
#define ADXL345_BW_50                     0x0a 
#define ADXL345_BW_25                     0x09 
#define ADXL345_BW_12_5                   0x08 
#define ADXL345_BW_6_25                   0x07 
#define ADXL345_BW_3_125                  0x06 
#define ADXL345_BW_1_563                  0x05 
#define ADXL345_BW__782                   0x04 
#define ADXL345_BW__39                    0x03 
#define ADXL345_BW__195                   0x02 
#define ADXL345_BW__098                   0x01 
#define ADXL345_BW__048                   0x00 
#define ADXL345_LOW_POWER                 0x08 
#define ADXL345_LOW_NOISE                 0x00 

/* Bit values in POWER_CTL*/ 
#define ADXL345_WAKEUP_8HZ                0x00 
#define ADXL345_WAKEUP_4HZ                0x01 
#define ADXL345_WAKEUP_2HZ                0x02 
#define ADXL345_WAKEUP_1HZ                0x03 
#define ADXL345_SLEEP                     0x04 
#define ADXL345_MEASURE                   0x08 
#define ADXL345_STANDBY                   0x00 
#define ADXL345_AUTO_SLEEP                0x10 
#define ADXL345_ACT_INACT_SERIAL          0x20 
#define ADXL345_ACT_INACT_CONCURRENT      0x00 
#define ADXL345_POWER_CTL_DEFAULT         (ADXL345_WAKEUP_1HZ | ADXL345_ACT_INACT_SERIAL)
 
/* Bit values in INT_ENABLE, INT_MAP, and INT_SOURCE are identical. 
   Use these bit values to read or write any of these registers.*/ 
#define ADXL345_INT_OVERRUN               0x01 
#define ADXL345_INT_WATERMARK             0x02 
#define ADXL345_INT_FREEFALL              0x04 
#define ADXL345_INT_INACTIVITY            0x08 
#define ADXL345_INT_ACTIVITY              0x10 
#define ADXL345_INT_DOUBLETAP             0x20 
#define ADXL345_INT_SINGLETAP             0x40 
#define ADXL345_INT_DATAREADY             0x80 
#define ADXL345_INT_DISABLE               0x00
#define ADXL345_INT_DEFAULT               (ADXL345_INT_DATAREADY|ADXL345_INT_WATERMARK|ADXL345_INT_OVERRUN)
 
/* Bit values in DATA_FORMAT
   Register values read in DATAX0 through DATAZ1 are dependent on the  
   value specified in data format.  Customer code will need to interpret 
   the data as desired.*/ 
#define ADXL345_RANGE_2G                  0x00 
#define ADXL345_RANGE_4G                  0x01 
#define ADXL345_RANGE_8G                  0x02 
#define ADXL345_RANGE_16G                 0x03 
#define ADXL345_DATA_JUST_RIGHT           0x00 
#define ADXL345_DATA_JUST_LEFT            0x04 
#define ADXL345_10BIT                     0x00 
#define ADXL345_FULL_RESOLUTION           0x08 
#define ADXL345_INT_LOW                   0x20 
#define ADXL345_INT_HIGH                  0x00 
#define ADXL345_SPI3WIRE                  0x40 
#define ADXL345_SPI4WIRE                  0x00 
#define ADXL345_SELFTEST                  0x80 
#define ADXL345_DATA_FORMAT_DEFAULT       (ADXL345_FULL_RESOLUTION|ADXL345_RANGE_16G)
      
/* Bit values in FIFO_CTL                                                
   The low bits are a value 0 to 31 used for the watermark or the number 
   of pre-trigger samples when in triggered mode */ 
#define ADXL345_TRIGGER_INT1              0x00 
#define ADXL345_TRIGGER_INT2              0x20 
#define ADXL345_FIFO_MODE_BYPASS          0x00 
#define ADXL345_FIFO_MODE_FIFO            0x40 
#define ADXL345_FIFO_MODE_STREAM          0x80 
#define ADXL345_FIFO_MODE_TRIGGER         0xc0 
#define ADXL345_FIFO_CTL_DEFAULT          (ADXL345_FIFO_MODE_STREAM | 0x10)
 
/* Bit values in FIFO_STATUS                                             
   The low bits are a value 0 to 32 showing the number of entries 
   currently available in the FIFO buffer */ 
#define ADXL345_FIFO_TRIGGERED            0x80 
 
#endif /*__ADXL345_H*/
