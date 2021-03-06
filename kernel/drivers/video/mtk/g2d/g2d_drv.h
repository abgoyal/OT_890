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

#ifndef __G2D_DRV_H__
#define __G2D_DRV_H__

#include <mach/mt6516_typedefs.h>

#define NO_FLOAT  (1)

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------
//  Utility Macros
// ---------------------------------------------------------------------------

#define G2D_CHECK_RET(expr)             \
    do {                                \
        G2D_STATUS ret = (expr);        \
        ASSERT(G2D_STATUS_OK == ret);   \
    } while (0)

// ---------------------------------------------------------------------------
//  Enumerations
// ---------------------------------------------------------------------------

typedef enum
{	
   G2D_STATUS_OK = 0,

   G2D_STATUS_ERROR,
   G2D_STATUS_INVALID_PARAMETER,
   G2D_STATUS_INSUFFICIENT_MEMORY,
} G2D_STATUS;


typedef enum {
   G2D_STATE_IDLE = 0,
   G2D_STATE_BUSY,
} G2D_STATE;


typedef enum
{
    G2D_IDX_COLOR_1BPP = 0,
    G2D_IDX_COLOR_2BPP = 1,
    G2D_IDX_COLOR_4BPP = 2,
    G2D_IDX_COLOR_8BPP = 3,
} G2D_INDEX_COLOR_FORMAT;


typedef enum
{
    G2D_COLOR_INDEX    = 0,
    G2D_COLOR_8BPP     = 0,
    G2D_COLOR_RGB565   = 1,
    G2D_COLOR_ARGB8888 = 2,
    G2D_COLOR_RGB888   = 3,
    G2D_COLOR_ARGB4444 = 5,
    G2D_COLOR_UNKOWN   = 6,
} G2D_COLOR_FORMAT;


typedef enum
{
    G2D_ROTATE_HFLIP_90  = 0,
    G2D_ROTATE_90        = 1,
    G2D_ROTATE_270       = 2,
    G2D_ROTATE_HFLIP_270 = 3,
    G2D_ROTATE_180       = 4,
    G2D_ROTATE_HFLIP_0   = 5,
    G2D_ROTATE_HFLIP_180 = 6,
    G2D_ROTATE_0         = 7,
} G2D_ROTATE;


typedef enum
{
    G2D_DIR_LOWER_RIGHT = 0,
    G2D_DIR_LOWER_LEFT  = 1,
    G2D_DIR_UPPER_RIGHT = 2,
    G2D_DIR_UPPER_LEFT  = 3,
} G2D_DIRECTION;


typedef enum
{
    G2D_FIRE_POLYGON_FILL  = 0x6,
    G2D_FIRE_RECT_FILL     = 0x8,
    G2D_FIRE_BLT           = 0x9,
    G2D_FIRE_ALPHA_BLT     = 0xA,
    G2D_FIRE_ROP_BLT       = 0xB,
    G2D_FIRE_FONT_DRAWING  = 0xC,
} G2D_FIRE_MODE;


typedef enum
{
    G2D_ROP4_BLACKNESS    = 0x0000,
    G2D_ROP4_WHITENESS    = 0xFFFF,
    G2D_ROP4_PATCOPY      = 0xF0F0,
    G2D_ROP4_PATINVERT    = 0x5A5A,
    G2D_ROP4_DSTINVERT    = 0x5555,
    G2D_ROP4_DSTCOPY      = 0xAAAA,
    G2D_ROP4_SRCCOPY      = 0xCCCC,
    G2D_ROP4_SRCINVERT    = 0x6666,
    G2D_ROP4_SRCAND       = 0x8888,
    G2D_ROP4_SRCPAINT     = 0xEEEE,
} G2D_ROP4_CODE;

// ---------------------------------------------------------------------------
//  Constant Definitions
// ---------------------------------------------------------------------------

typedef enum
{
    G2D_TILT_END = 0xFF,
} G2D_CONSTANTS;

// ---------------------------------------------------------------------------
//  Data Structures
// ---------------------------------------------------------------------------

typedef struct
{
    INT32  x;
    INT32  y;
} G2D_COORD;


typedef struct
{
    INT32  x;
    INT32  y;
    UINT32 width;
    UINT32 height;
} G2D_RECT;


#if NO_FLOAT
typedef INT32 S9_16;
#endif

typedef struct
{
#if NO_FLOAT
    S9_16 a;
    S9_16 r;
    S9_16 g;
    S9_16 b;
#else
    FLOAT a;
    FLOAT r;
    FLOAT g;
    FLOAT b;
#endif    
} G2D_GRADIENT;

// ---------------------------------------------------------------------------
//  Callback Function Declarations
// ---------------------------------------------------------------------------

typedef void (*G2D_CALLBACK)(void);

// ---------------------------------------------------------------------------
//  G2D Driver API
// ---------------------------------------------------------------------------

G2D_STATUS G2D_Init(void);
G2D_STATUS G2D_Deinit(void);

G2D_STATUS G2D_PowerOn(void);
G2D_STATUS G2D_PowerOff(void);

G2D_STATE  G2D_GetStatus(void);
G2D_STATUS G2D_WaitForNotBusy(void);

G2D_STATUS G2D_EnableCommandQueue(BOOL bEnable);
G2D_STATUS G2D_SetCallbackFunction(G2D_CALLBACK pfCallback);

G2D_STATUS G2D_EnableClipping(BOOL bEnable);
G2D_STATUS G2D_SetClippingWindow(INT32 i4X, INT32 i4Y, UINT32 u4Width, UINT32 u4Height);

G2D_STATUS G2D_SetRotateMode(G2D_ROTATE eRotation);
G2D_STATUS G2D_SetDirection(G2D_DIRECTION eDirection);

G2D_STATUS G2D_SetSrcColorKey(BOOL bEnable, UINT32 u4ColorKey);
G2D_STATUS G2D_SetDstColorKey(BOOL bEnable, UINT32 u4ColorKey);
G2D_STATUS G2D_SetColorReplace(BOOL bEnable, UINT32 u4AvdColor, UINT32 u4RepColor);

G2D_STATUS G2D_SetDstBuffer(UINT8 *pAddr, UINT32 u4Width, UINT32 u4Height, G2D_COLOR_FORMAT eFormat);
G2D_STATUS G2D_SetDstRect(UINT32 u4X, UINT32 u4Y, UINT32 u4Width, UINT32 u4Height);

G2D_STATUS G2D_SetSrcBuffer(UINT8 *pAddr, UINT32 u4Width, UINT32 u4Height, G2D_COLOR_FORMAT eFormat);
G2D_STATUS G2D_SetSrcRect(UINT32 u4X, UINT32 u4Y, UINT32 u4Width, UINT32 u4Height);

G2D_STATUS G2D_SetPatBuffer(UINT8 *pAddr, UINT32 u4Width, UINT32 u4Height, G2D_COLOR_FORMAT eFormat);
G2D_STATUS G2D_SetPatRect(UINT32 u4X, UINT32 u4Y, UINT32 u4Width, UINT32 u4Height);
G2D_STATUS G2D_SetPatOffset(INT32 i4XOffset, INT32 i4YOffset);

G2D_STATUS G2D_SetMskBuffer(UINT8 *pAddr, UINT32 u4Width, UINT32 u4Height);
G2D_STATUS G2D_SetMskRect(UINT32 u4X, UINT32 u4Y, UINT32 u4Width, UINT32 u4Height);

G2D_STATUS G2D_SetStretchBitblt(BOOL bEnable, UINT16 u2ScaleUp, UINT16 u2ScaleDown);
G2D_STATUS G2D_SetItalicBitblt(BOOL bEnable, const UINT8 *pTiltValues);

/// Fire Functions

G2D_STATUS G2D_StartRectangleFill(UINT32 u4FilledColor,
                                  G2D_GRADIENT *pGradientX,
                                  G2D_GRADIENT *pGradientY);
G2D_STATUS G2D_StartBitblt(void);
G2D_STATUS G2D_StartRopBitblt(UINT16 u2Rop4Code);
G2D_STATUS G2D_StartAlphaBitblt(UINT8 ucAlphaValue, BOOL isPremultipledAlpha);

G2D_STATUS G2D_StartFontDrawing(BOOL bEnableBgColor,
                                UINT32 u4FgColor, UINT32 u4BgColor,
                                UINT8 *pFontBitmap, UINT32 u4StartBit);

/// Profiling Function

UINT32 G2D_GetLastOpDuration(void);

/// Benchmarking Function
void G2D_Perf_Test(void *info);


#ifdef __cplusplus
}
#endif

#endif  // __G2D_DRV_H__
