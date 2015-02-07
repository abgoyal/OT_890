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
*  permission of MediaTek Inc. (C) 2008
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
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   ccci_ch.h
 *
 * Project:
 * --------
 *   YuSu
 *
 * Description:
 * ------------
 *   MT6516 CCCI channel definitions
 *
 * Author:
 * -------
 *   CC Hwang (mtk00702)
 *
 ****************************************************************************/

#if defined(X_DEF_CH)
#define CCCI_CH_X(a, b) a = b,
#endif  /* X_DEF_CH */

#if defined(X_DEF_CTRL)
#define CCCI_CH_X(a, b) __ATTR(a, 0644, NULL, NULL),
#endif  /* X_DEF_CTRL */

#if defined(X_DEF_ATTR)
#define CCCI_CH_X(a, b) &(ccci_ctrl[a].attr),
#endif  /* X_DEF_ATTR */

CCCI_CH_X(CCCI_CONTROL_RX, 0)
CCCI_CH_X(CCCI_CONTROL_TX, 1)
CCCI_CH_X(CCCI_SYSTEM_RX, 2)
CCCI_CH_X(CCCI_SYSTEM_TX, 3)
CCCI_CH_X(CCCI_PCM_RX, 4)
CCCI_CH_X(CCCI_PCM_TX, 5)
CCCI_CH_X(CCCI_UART1_RX, 6)
CCCI_CH_X(CCCI_UART1_RX_ACK, 7)
CCCI_CH_X(CCCI_UART1_TX, 8)
CCCI_CH_X(CCCI_UART1_TX_ACK, 9)
CCCI_CH_X(CCCI_UART2_RX, 10)
CCCI_CH_X(CCCI_UART2_RX_ACK, 11)
CCCI_CH_X(CCCI_UART2_TX, 12)
CCCI_CH_X(CCCI_UART2_TX_ACK, 13)
CCCI_CH_X(CCCI_FS_RX, 14)
CCCI_CH_X(CCCI_FS_TX, 15)
CCCI_CH_X(CCCI_PMIC_RX, 16)
CCCI_CH_X(CCCI_PMIC_TX, 17)
CCCI_CH_X(CCCI_UEM_RX, 18)
CCCI_CH_X(CCCI_UEM_TX, 19)
CCCI_CH_X(CCCI_CCMNI1_RX, 20)
CCCI_CH_X(CCCI_CCMNI1_RX_ACK, 21)
CCCI_CH_X(CCCI_CCMNI1_TX, 22)
CCCI_CH_X(CCCI_CCMNI1_TX_ACK, 23)
CCCI_CH_X(CCCI_CCMNI2_RX, 24)
CCCI_CH_X(CCCI_CCMNI2_RX_ACK, 25)
CCCI_CH_X(CCCI_CCMNI2_TX, 26)
CCCI_CH_X(CCCI_CCMNI2_TX_ACK, 27)
CCCI_CH_X(CCCI_CCMNI3_RX, 28)
CCCI_CH_X(CCCI_CCMNI3_RX_ACK, 29)
CCCI_CH_X(CCCI_CCMNI3_TX, 30)
CCCI_CH_X(CCCI_CCMNI3_TX_ACK, 31)

#undef CCCI_CH_X