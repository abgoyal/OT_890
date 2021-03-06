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

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2007
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


#ifndef __PDN_SW_H__
#define __PDN_SW_H__
#include "mt3351_pll.h"

// PDNCONA
// PDNSETA
// PDNCLRa
typedef enum
{
    PDN_PERI_DMA	=	0,
    PDN_PERI_USB	=	1,
    PDN_PERI_RESERVED0	=	2,
    PDN_PERI_SPI	=	3,
    PDN_PERI_GPT	=	4,
    PDN_PERI_UART0	=	5,
    PDN_PERI_UART1	=	6,
    PDN_PERI_UART2	=	7,
    PDN_PERI_UART3	=	8,
    PDN_PERI_UART4	=	9,
    PDN_PERI_PWM	=	10,
    PDN_PERI_PWM0CLK	=	11,
    PDN_PERI_PWM1CLK	=	12,
    PDN_PERI_PWM2CLK	=	13,
    PDN_PERI_MSDC0	=	14,
    PDN_PERI_MSDC1	=	15,
    PDN_PERI_MSDC2	=	16,
    PDN_PERI_NFI	=	17,
    PDN_PERI_IRDA	=	18,
    PDN_PERI_I2C	=	19,
    PDN_PERI_AUXADC	=	20,
    PDN_PERI_TOUCH	=	21,
    PDN_PERI_SYSROM	=	22,
    PDN_PERI_KEYPAD	=	23
} PDNCONA_MODE;
// PDNCONB
// PDNSETB
// PDNCLRB
typedef enum
{
    PDN_MM_GMC	=		0,
    PDN_MM_G2D		=	1,
    PDN_MM_GCMQ		=	2,
    PDN_MM_RESERVED0 =	3,
    PDN_MM_IMAGEDMA	=	4,
    PDN_MM_IMAGEPROCESSOR	=	5,
    PDN_MM_JPEG		=	6,
    PDN_MM_DCT		=	7,
    PDN_MM_ISP		=	8,
    PDN_MM_PRZ		=	9,
    PDN_MM_CRZ		=	10,
    PDN_MM_DRZ		=	11,
    PDN_MM_SPI		=	12,
    PDN_MM_ASM		=	13,
    PDN_MM_I2S		=	14,
    PDN_MM_RESIZE_LB=	15,
    PDN_MM_LCD		=	16,
    PDN_MM_DPI		=	17,
    PDN_MM_VFE		=	18,
    PDN_MM_AFE		=	19,
    PDN_MM_BLS		=	20
} PDNCONB_MODE;

/*******************************************************************************
 * Function declaration
 *******************************************************************************/

kal_bool PDN_Get_Peri_Status (PDNCONA_MODE mode);
kal_bool PDN_Get_MM_Status (PDNCONB_MODE mode);
void PDN_Power_CONA_DOWN(PDNCONA_MODE mode, kal_bool enable);
void PDN_Power_CONB_DOWN(PDNCONB_MODE mode, kal_bool enable);
void PDN_Power_DSP(kal_bool enable);
void APLL_set_CLK(APLL_FREQ APLL_CLK);
void Set_CLK_DLY(UINT8 delay);
void ARM_CLK_GATE_Enable(kal_bool enable);

#endif /* __PDN_SW_H__ */

