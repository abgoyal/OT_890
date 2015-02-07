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

/* linux/drivers/hwmon/mt6516/ami304.c
 *
 * (C) Copyright 2008 
 * MediaTek <www.mediatek.com>
 *
 * AMI304 driver for MT6516
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
#ifndef __AMI304_H__
#define __AMI304_H__
/******************************************************************************
 * Function Configuration
******************************************************************************/
#define AMI304_TEST_MODE
/******************************************************************************
 * Definition
******************************************************************************/
#define AMI_TAG					"<AMI304> "
#define AMI_DEV_NAME		    "AMI304"
#define AMI_FUN(f)				printk(KERN_INFO AMI_TAG"%s\n", __FUNCTION__)
#define AMI_ERR(fmt, args...)	printk(KERN_ERR AMI_TAG"%s %d : "fmt, __FUNCTION__, __LINE__, ##args)
#define AMI_LOG(fmt, args...)	printk(KERN_INFO AMI_TAG fmt, ##args)
#define AMI_VER(fmt, args...)   ((void)0)
#define AMI304_I2C_ADDR         0x0F
#define AMI304_WR_SLAVE_ADDR	0x1E    // (0x0F << 1)
#define AMI304_RD_SLAVE_ADDR	0x1F    // (0x0F << 1) + 1
#define AMI304_AXES_NUM		    3
#define AMI304_DATA_LEN		    0x06	/*data length including X,Y,Z*/
#define AMI304_OFFSET_LEN       0x06
#define AMI304_ADDR_MAX         0xEB
#define AMI304_AXIS_X			0
#define AMI304_AXIS_Y			1
#define AMI304_AXIS_Z			2
#define AMI304_SENSITIVITY      1024


/*Register definition*/
#define AMI304_REG_WIA          0x0F
#define AMI304_REG_DATAX        0x10    // 2    
#define AMI304_REG_DATAY        0x12    // 2
#define AMI304_REG_DATAZ        0x14    // 2
#define AMI304_REG_INS1         0x16
#define AMI304_REG_STAT1        0x18
#define AMI304_REG_INL          0x1A
#define AMI304_REG_CNTL1        0x1B
#define AMI304_REG_CNTL2        0x1C
#define AMI304_REG_CNTL3        0x1D
#define AMI304_REG_INC1         0x1E
#define AMI304_REG_B0X          0x20    // 2
#define AMI304_REG_B0Y          0x22    // 2
#define AMI304_REG_B0Z          0x24    // 2
#define AMI304_REG_ITHR1        0x26    // 2 
#define AMI304_REG_PRET         0x30
#define AMI304_REG_CNTL4        0x5C    // 2 
#define AMI304_REG_TEMP         0x60    // 2
#define AMI304_REG_DELAYX       0x68
#define AMI304_REG_DELAYY       0x6E
#define AMI304_REG_DELAYZ       0x74
#define AMI304_REG_OFFX         0x6C    // 2 
#define AMI304_REG_OFFY         0x72    // 2 
#define AMI304_REG_OFFZ         0x78    // 2 
#define AMI304_REG_VER          0xE8    // 2
#define AMI304_REG_SN           0xEA    // 2 


// For "Interrupt source" register
#define INS1_PXSI       (1<<7)  // Interrupt event occurrs in X-axis
#define INS1_PYSI       (1<<6)  // Interrupt event occurrs in Y-axis
#define INS1_PZSI       (1<<5)  // Interrupt event occurrs in Z-axis
#define INS1_NXSI       (1<<4)  
#define INS1_NYSI       (1<<3)  
#define INS1_NZSI       (1<<2)  
#define INS1_MROI       (1<<1)  // Out of measurement range

// For "Status" register
#define STA1_DRDY       (1<<6)  // This bit is output to the DRDY to inform the preparation status of the measuring data
#define STA1_DOR        (1<<5)  // The measurement results have not been read out during inactivity time
#define STA1_INT        (1<<4)  // This bit is output to the INT to inform the Interrupt event

// For "Control Setting1" register
#define CNTL1_PC1       (1<<7)  // Power mode bit
#define CNTL1_ODR1      (1<<4)  // Sample rate bit
#define CNTL1_FS1       (1<<1)  // Measurement mode bit

#define CNTL1_PC1_STDBY (0<<7)  // Set Power mode to stand-by
#define CNTL1_PC1_ACT   (1<<7)  // Set Power mode to active state

#define CNTL1_ODR1_10   (0<<4)  // Set out data rate to 10Hz
#define CNTL1_ODR1_20   (1<<4)  // Set out data rate to 20Hz

#define CNTL1_FS1_NORM  (0<<1)  // Set measurement to normal mode
#define CNTL1_FS1_FORCE (1<<1)  // Set measurement to force mode

#define CNTL1_DEFAULT   (CNTL1_PC1_ACT|CNTL1_ODR1_20|CNTL1_FS1_NORM)

// For "Control Setting2" register
#define CNTL2_IEN       (1<<4)  // Enable INT
#define CNTL2_DREN      (1<<3)  // Enable DEDY
#define CNTL2_DRP       (1<<2)  // Set DRDY active level is low level

// For "Control Setting3" register
#define CNTL3_SRST      (1<<7)  // Soft reset
#define CNTL3_FORCE     (1<<6)  // Start immediately measurement period
#define CNTL3_B0_LO     (1<<4)  // Load the B0 from OTP-ROM to RAM

// For "Interrupt control 1" register
#define INC1_XIEN       (1<<7)  // Enable X-axis Interrupt
#define INC1_YIEN       (1<<6)  // Enalbe Y-axis Interrupt
#define INC1_ZEN        (1<<5)  // Enalbe Z-axis Interrupt
#define INC1_IEA        (1<<3)  // Set Interrupt active level to low
#define INC1_IEL        (1<<2)  // Set interrupt signal to edge trigger

// For "Control Setting 4" register
#define CNTL4_AB        (1<<8)
#define CNTL4_AB_COMP   (0<<8)  // Compass measurement mode
#define CNTL4_AB_GYRO   (1<<8)  // Gyro measurement mode

/******************************************************************************
 * Hardware depenedent configuration
******************************************************************************/
/*In MT6516, the interrupt is triggered through EINT13*/
#define GPIO_MSESNOR_RDY_PIN                GPIO119
#define GPIO_MSESNOR_RDY_PIN_M_GPIO         GPIO_MODE_00

#define GPIO_MSESNOR_INT_PIN                GPIO90      //EINT13
#define GPIO_MSESNOR_INT_PIN_M_GPIO         GPIO_MODE_00
#define GPIO_MSESNOR_INT_PIN_M_EINT         GPIO_MODE_03

#define EINT_MSENSOR_INT                    13

#endif  

