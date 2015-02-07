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

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

 
#ifndef _MTK_DVT_TEST_ADC_HW_H
#define _MTK_DVT_TEST_ADC_HW_H


#define AUXADC_CON0                     (AUXADC_BASE + 0x000)
#define AUXADC_CON1                     (AUXADC_BASE + 0x004)
#define AUXADC_CON2                     (AUXADC_BASE + 0x008)
#define AUXADC_CON3                     (AUXADC_BASE + 0x00C)

#define AUXADC_DAT0                     (AUXADC_BASE + 0x010)
#define AUXADC_DAT1                     (AUXADC_BASE + 0x014)
#define AUXADC_DAT2                     (AUXADC_BASE + 0x018)
#define AUXADC_DAT3                     (AUXADC_BASE + 0x01C)
#define AUXADC_DAT4                     (AUXADC_BASE + 0x020)
#define AUXADC_DAT5                     (AUXADC_BASE + 0x024)
#define AUXADC_DAT6                     (AUXADC_BASE + 0x028)
#define AUXADC_DAT7                     (AUXADC_BASE + 0x02C)
#define AUXADC_DAT8                     (AUXADC_BASE + 0x030)
#define AUXADC_DAT9                     (AUXADC_BASE + 0x034)

#define AUXADC_DET_VOLT                 (AUXADC_BASE + 0x060)
#define AUXADC_DET_SEL                  (AUXADC_BASE + 0x064)
#define AUXADC_DET_PERIOD               (AUXADC_BASE + 0x068)
#define AUXADC_DET_DEBT                 (AUXADC_BASE + 0x06C)

//#define AUXADC_QBIT                    	 (AUXADC_BASE + 0x058)
//#define AUXADC_TDMA0_CNT                (AUXADC_BASE + 0x05C)
//#define AUXADC_TDMA1_CNT                (AUXADC_BASE + 0x060)


//-----------------------------------------------------------------------------

/*AUXADC_SYNC on AUXADC_CON0*/
#define AUXADC_SYNC_CHAN(_line)         (0x0001<<_line)   /*Time event 1*/

/*AUXADC_IMM on AUXADC_CON1*/
#define AUXADC_IMM_CHAN(_line)          (0x0001<<_line)

/*AUXADC_SYN on AUXADC_CON2*/
#define AUXADC_SYN_BIT                  (0x0001)         /*Time event 0*/

/*AUXADC_CON3*/
#define AUXADC_CON3_STATUS_mask         (0x0001)
    #define AUXADC_STATUS_BUSY          (0x01)
    #define AUXADC_STATUS_IDLE          (0x00) 
//#define AUXADC_CON_CALI_MASK          (0x007c)
//#define AUXADC_CON_TESTDACMON         (0x0080)
#define AUXADC_CON3_AUTOCLR0_mask       (0x0100)
#define AUXADC_CON3_AUTOCLR0_offset     (8)
#define AUXADC_CON3_AUTOCLR1_mask       (0x0200)
#define AUXADC_CON3_AUTOCLR1_offset     (9)
#define AUXADC_CON3_PUWAIT_EN_mask      (0x0800)
#define AUXADC_CON3_PUWAIT_EN_offset    (11)
#define AUXADC_CON3_AUTOSET_mask        (0x8000)
#define AUXADC_CON3_AUTOSET_offset      (15)

/*AUXADC_DET_CTL*/
#define AUXADC_VOLT_INV_mask            (0x03FF)
#define AUXADC_VOLT_THRESHOLD_mask      (0x8000)
#define AUXADC_VOLT_THRESHOLD_offset    (15)

/*AUXADC_QBIT*/
#define AUXADC_QBIT_RATE_CNT2           (0x0010)
#define AUXADC_QBIT_RATE_CNT1           (0x0008)
#define AUXADC_QBIT_RATE_CNT0           (0x0000)

/*AUXADC_TDMA1_CNT*/
#define AUXADC_TDMA1_FORCETIMER_mask    (0x0200)
#define AUXADC_TDMA1_FORCETIMER_offset   (9)
#define AUXADC_TDMA1_ENABLECOUTER_mask  (0x0100)
#define AUXADC_TDMA1_ENABLECOUTER_offset (8)
#define AUXADC_TDMA1_TIMERCOUNT_mask    (0x00FF)

/*AUXADC_TDMA0_CNT*/
#define AUXADC_TDMA0_FORCETIMER_mask    (0x0200)
#define AUXADC_TDMA0_FORCETIMER_offset   (9)
#define AUXADC_TDMA0_ENABLECOUTER_mask  (0x0100)
#define AUXADC_TDMA0_ENABLECOUTER_offset (8)
#define AUXADC_TDMA0_TIMERCOUNT_mask    (0x00FF)


#endif   /*_MTK_DVT_TEST_ADC_HW_H*/

