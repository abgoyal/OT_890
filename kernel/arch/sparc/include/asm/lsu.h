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

#ifndef _SPARC64_LSU_H
#define _SPARC64_LSU_H

#include <linux/const.h>

/* LSU Control Register */
#define LSU_CONTROL_PM _AC(0x000001fe00000000,UL) /* Phys-watchpoint byte mask*/
#define LSU_CONTROL_VM _AC(0x00000001fe000000,UL) /* Virt-watchpoint byte mask*/
#define LSU_CONTROL_PR _AC(0x0000000001000000,UL) /* Phys-rd watchpoint enable*/
#define LSU_CONTROL_PW _AC(0x0000000000800000,UL) /* Phys-wr watchpoint enable*/
#define LSU_CONTROL_VR _AC(0x0000000000400000,UL) /* Virt-rd watchpoint enable*/
#define LSU_CONTROL_VW _AC(0x0000000000200000,UL) /* Virt-wr watchpoint enable*/
#define LSU_CONTROL_FM _AC(0x00000000000ffff0,UL) /* Parity mask enables.     */
#define LSU_CONTROL_DM _AC(0x0000000000000008,UL) /* Data MMU enable.         */
#define LSU_CONTROL_IM _AC(0x0000000000000004,UL) /* Instruction MMU enable.  */
#define LSU_CONTROL_DC _AC(0x0000000000000002,UL) /* Data cache enable.       */
#define LSU_CONTROL_IC _AC(0x0000000000000001,UL) /* Instruction cache enable.*/

#endif /* !(_SPARC64_LSU_H) */
