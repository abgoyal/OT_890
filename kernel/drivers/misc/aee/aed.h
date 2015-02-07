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


#ifndef __aee_h
#define __aee_h

#include <linux/autoconf.h>
#include <linux/bug.h>

#ifdef __cplusplus
extern "C" {
#endif 

#define SOCKET_NAME_AED         "aee_aed"         // Android Exp Daemon
#define SOCKET_NAME_AES         "com.mtk.aee.aes" // Android Exp Service

#define AE_MAX_EXP_COUNT        10
#define AE_MAX_FILE_PATH_LEN    128
#define AE_ROOT_DIR             "/data/aee_exp"
#define AE_ROOT_STORAGE         "/sdcard"
#define AE_CUID_ROOT            AE_ROOT_STORAGE "/aee_exp"
#define AE_CUID_ROOT_BACKUP     "/data/aee_exp"
#define AE_DIR_PREFIX           "db."
#define AE_MAIN_LOG_FILE_NAME   "__exp_main.txt"
#define AE_DETAIL_FILE_NAME     "_exp_detail.txt"

#define AE_EE_DEVICE_PATH       "/dev/aed0"
#define AE_KE_DEVICE_PATH       "/dev/aed1"

#define AE_INVALID              0xAEEFF000
#define AE_NOT_AVAILABLE        0xAEE00000
#define AE_DEFAULT              0xAEE00001

typedef enum {
    AE_SUCC, 
    AE_FAIL
} AE_ERR;

typedef enum {
    AE_PASS_BY_MEM,
    AE_PASS_BY_FILE,
    AE_PASS_METHOD_END  
} AE_PASS_METHOD;

typedef enum {
    AE_DEFECT_EXCEPITON,
    AE_DEFECT_WARNING,
    AE_DEFECT_REMINDING,
    AE_DEFECT_ATTR_END
} AE_DEFECT_ATTR;

typedef enum {AE_REQ, AE_RSP, AE_IND, AE_CMD_TYPE_END}        AE_CMD_TYPE;
typedef enum {AE_KE, AE_NE, AE_JE, AE_EE, AE_EXP_CLASS_END}   AE_EXP_CLASS;

typedef enum  {
    AE_REQ_IDX,

    AE_REQ_CLASS,
    AE_REQ_TYPE,
    AE_REQ_PROCESS,
    AE_REQ_MODULE,
    AE_REQ_BACKTRACE,
    AE_REQ_DETAIL,    /* Content of response message rule:
                       *   if msg.arg1==AE_PASS_BY_FILE => msg->data=file path 
                       */
    
    AE_REQ_ROOT_LOG_DIR,
    AE_REQ_CURR_LOG_DIR,
    AE_REQ_DFLT_LOG_DIR,
    AE_REQ_MAIN_LOG_FILE_PATH,    

    AE_IND_EXP_RAISED, // exception event raised, indicate AED to notify users
                       //   arg = AE_EXP_CLASS
    AE_IND_LOG_STATUS, // arg = AE_ERR
    AE_IND_LOG_CLOSE,  // arg = AE_ERR

    AE_IND_ALUI_PRINT, // indicate AED to use ALUI to print the desired string
    AE_IND_ALUI_CLEAR, // indicate AED to clear ALUI

    AE_IND_WRN_RAISED, // warning event raised, indicate AED to notify users    
                       //   arg = AE_EXP_CLASS; 
                       //   if seq == 0 => len = file path length
                       //   if seq == 1 => len = UI string length
    AE_IND_REM_RAISED, // warning event raised, indicate AED to notify users    
                       //   arg = AE_EXP_CLASS; 
                       //   if seq == AE_PASS_METHOD => len = file path length
                       //   if seq == AE_PASS_MEM    => len = UI string length

    AE_REQ_SWITCH_DAL_BEEP, // arg: dal on|off, seq: beep on|off
    AE_CMD_ID_END
} AE_CMD_ID;

typedef struct {
    AE_CMD_TYPE    cmdType; // command type
    AE_CMD_ID      cmdId;   // command Id
    unsigned int   seq;     // sequence number for error checking
    unsigned int   arg;     // simple argument
    unsigned int   len;     // dynamic length argument
} AE_Msg;

/* Kernel IOCTL interface */
struct aee_dal_show {
  char msg[256];
  };

#define AEEIOCTL_DAL_SHOW      _IOW('p', 0x01, struct aee_dal_show) /* Show string on DAL layer  */
#define AEEIOCTL_DAL_CLEAN     _IO('p', 0x02) /* Clear DAL layer */
#define AEEIOCTL_BEEP          _IOW('p', 0x03, unsigned long) /* Beep (ms) */

#ifdef __cplusplus
}
#endif 

#endif
