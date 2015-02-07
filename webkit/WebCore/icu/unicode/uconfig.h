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
**********************************************************************
*   Copyright (C) 2002-2004, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  uconfig.h
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002sep19
*   created by: Markus W. Scherer
*/

#ifndef __UCONFIG_H__
#define __UCONFIG_H__

/*!
 * \file
 * \brief Switches for excluding parts of ICU library code modules.
 *
 * Allows to build partial, smaller libraries for special purposes.
 * By default, all modules are built.
 * The switches are fairly coarse, controlling large modules.
 * Basic services cannot be turned off.
 *
 * @stable ICU 2.4
 */

/**
 * \def UCONFIG_ONLY_COLLATION
 * This switch turns off modules that are not needed for collation.
 *
 * It does not turn off legacy conversion because that is necessary
 * for ICU to work on EBCDIC platforms (for the default converter).
 * If you want "only collation" and do not build for EBCDIC,
 * then you can #define UCONFIG_NO_LEGACY_CONVERSION 1 as well.
 *
 * @stable ICU 2.4
 */
#ifndef UCONFIG_ONLY_COLLATION
#   define UCONFIG_ONLY_COLLATION 0
#endif

#if UCONFIG_ONLY_COLLATION
    /* common library */
#   define UCONFIG_NO_BREAK_ITERATION 1
#   define UCONFIG_NO_IDNA 1

    /* i18n library */
#   if UCONFIG_NO_COLLATION
#       error Contradictory collation switches in uconfig.h.
#   endif
#   define UCONFIG_NO_FORMATTING 1
#   define UCONFIG_NO_TRANSLITERATION 1
#   define UCONFIG_NO_REGULAR_EXPRESSIONS 1
#endif

/* common library switches -------------------------------------------------- */

/**
 * \def UCONFIG_NO_CONVERSION
 * ICU will not completely build with this switch turned on.
 * This switch turns off all converters.
 *
 * @draft ICU 3.2
 */
#ifndef UCONFIG_NO_CONVERSION
#   define UCONFIG_NO_CONVERSION 0
#endif

#if UCONFIG_NO_CONVERSION
#   define UCONFIG_NO_LEGACY_CONVERSION 1
#endif

/**
 * \def UCONFIG_NO_LEGACY_CONVERSION
 * This switch turns off all converters except for
 * - Unicode charsets (UTF-7/8/16/32, CESU-8, SCSU, BOCU-1)
 * - US-ASCII
 * - ISO-8859-1
 *
 * Turning off legacy conversion is not possible on EBCDIC platforms
 * because they need ibm-37 or ibm-1047 default converters.
 *
 * @stable ICU 2.4
 */
#ifndef UCONFIG_NO_LEGACY_CONVERSION
#   define UCONFIG_NO_LEGACY_CONVERSION 0
#endif

/**
 * \def UCONFIG_NO_NORMALIZATION
 * This switch turns off normalization.
 * It implies turning off several other services as well, for example
 * collation and IDNA.
 *
 * @stable ICU 2.6
 */
#ifndef UCONFIG_NO_NORMALIZATION
#   define UCONFIG_NO_NORMALIZATION 0
#elif UCONFIG_NO_NORMALIZATION
    /* common library */
#   define UCONFIG_NO_IDNA 1

    /* i18n library */
#   if UCONFIG_ONLY_COLLATION
#       error Contradictory collation switches in uconfig.h.
#   endif
#   define UCONFIG_NO_COLLATION 1
#   define UCONFIG_NO_TRANSLITERATION 1
#endif

/**
 * \def UCONFIG_NO_BREAK_ITERATION
 * This switch turns off break iteration.
 *
 * @stable ICU 2.4
 */
#ifndef UCONFIG_NO_BREAK_ITERATION
#   define UCONFIG_NO_BREAK_ITERATION 0
#endif

/**
 * \def UCONFIG_NO_IDNA
 * This switch turns off IDNA.
 *
 * @stable ICU 2.6
 */
#ifndef UCONFIG_NO_IDNA
#   define UCONFIG_NO_IDNA 0
#endif

/* i18n library switches ---------------------------------------------------- */

/**
 * \def UCONFIG_NO_COLLATION
 * This switch turns off collation and collation-based string search.
 *
 * @stable ICU 2.4
 */
#ifndef UCONFIG_NO_COLLATION
#   define UCONFIG_NO_COLLATION 0
#endif

/**
 * \def UCONFIG_NO_FORMATTING
 * This switch turns off formatting and calendar/timezone services.
 *
 * @stable ICU 2.4
 */
#ifndef UCONFIG_NO_FORMATTING
#   define UCONFIG_NO_FORMATTING 0
#endif

/**
 * \def UCONFIG_NO_TRANSLITERATION
 * This switch turns off transliteration.
 *
 * @stable ICU 2.4
 */
#ifndef UCONFIG_NO_TRANSLITERATION
#   define UCONFIG_NO_TRANSLITERATION 0
#endif

/**
 * \def UCONFIG_NO_REGULAR_EXPRESSIONS
 * This switch turns off regular expressions.
 *
 * @stable ICU 2.4
 */
#ifndef UCONFIG_NO_REGULAR_EXPRESSIONS
#   define UCONFIG_NO_REGULAR_EXPRESSIONS 0
#endif

/**
 * \def UCONFIG_NO_SERVICE
 * This switch turns off service registration.
 *
 * @draft ICU 3.2
 */
#ifndef UCONFIG_NO_SERVICE
#   define UCONFIG_NO_SERVICE 0
#endif

#endif