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
******************************************************************************
*
*   Copyright (C) 1997-2004, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
*  FILE NAME : putil.h
*
*   Date        Name        Description
*   05/14/98    nos         Creation (content moved here from utypes.h).
*   06/17/99    erm         Added IEEE_754
*   07/22/98    stephen     Added IEEEremainder, max, min, trunc
*   08/13/98    stephen     Added isNegativeInfinity, isPositiveInfinity
*   08/24/98    stephen     Added longBitsFromDouble
*   03/02/99    stephen     Removed openFile().  Added AS400 support.
*   04/15/99    stephen     Converted to C
*   11/15/99    helena      Integrated S/390 changes for IEEE support.
*   01/11/00    helena      Added u_getVersion.
******************************************************************************
*/

#ifndef PUTIL_H
#define PUTIL_H

#include "unicode/utypes.h"

/* Define this to 1 if your platform supports IEEE 754 floating point,
   to 0 if it does not. */
#ifndef IEEE_754
#   define IEEE_754 1
#endif

/*==========================================================================*/
/* Platform utilities                                                       */
/*==========================================================================*/

/**
 * Platform utilities isolates the platform dependencies of the
 * libarary.  For each platform which this code is ported to, these
 * functions may have to be re-implemented.
 */

/**
 * Return the ICU data directory. 
 * The data directory is where common format ICU data files (.dat files)
 *   are loaded from.  Note that normal use of the built-in ICU
 *   facilities does not require loading of an external data file;
 *   unless you are adding custom data to ICU, the data directory
 *   does not need to be set.
 *
 * The data directory is determined as follows:
 *    If u_setDataDirectory() has been called, that is it, otherwise
 *    if the ICU_DATA environment variable is set, use that, otherwise
 *    If a data directory was specifed at ICU build time
 *      (#define ICU_DATA_DIR "path"), use that,
 *    otherwise no data directory is available.
 *
 * @return the data directory, or an empty string ("") if no data directory has
 *         been specified.
 *   
 * @stable ICU 2.0
 */
U_STABLE const char* U_EXPORT2 u_getDataDirectory(void);

/** 
 * Set the ICU data directory. 
 * The data directory is where common format ICU data files (.dat files)
 *   are loaded from.  Note that normal use of the built-in ICU
 *   facilities does not require loading of an external data file;
 *   unless you are adding custom data to ICU, the data directory
 *   does not need to be set.
 *
 * This function should be called at most once in a process, before the
 * first ICU operation (e.g., u_init()) that will require the loading of an
 * ICU data file.
 * This function is not thread-safe. Use it before calling ICU APIs from
 * multiple threads.
 *
 * @param directory The directory to be set.
 *
 * @see u_init
 * @stable ICU 2.0
 */
U_STABLE void U_EXPORT2 u_setDataDirectory(const char *directory);

/**
 * Please use ucnv_getDefaultName() instead.
 * Return the default codepage for this platform and locale.
 * This function can call setlocale() on Unix platforms. Please read the
 * platform documentation on setlocale() before calling this function.
 * @return the default codepage for this platform 
 * @internal
 */
U_INTERNAL const char*  U_EXPORT2 uprv_getDefaultCodepage(void);

/**
 * Please use uloc_getDefault() instead.
 * Return the default locale ID string by querying ths system, or
 *     zero if one cannot be found. 
 * This function can call setlocale() on Unix platforms. Please read the
 * platform documentation on setlocale() before calling this function.
 * @return the default locale ID string
 * @internal
 */
U_INTERNAL const char*  U_EXPORT2 uprv_getDefaultLocaleID(void);

/**
 * Filesystem file and path separator characters.
 * Example: '/' and ':' on Unix, '\\' and ';' on Windows.
 * @stable ICU 2.0
 */
#ifdef XP_MAC
#   define U_FILE_SEP_CHAR ':'
#   define U_FILE_ALT_SEP_CHAR ':'
#   define U_PATH_SEP_CHAR ';'
#   define U_FILE_SEP_STRING ":"
#   define U_FILE_ALT_SEP_STRING ":"
#   define U_PATH_SEP_STRING ";"
#elif defined(WIN32) || defined(OS2)
#   define U_FILE_SEP_CHAR '\\'
#   define U_FILE_ALT_SEP_CHAR '/'
#   define U_PATH_SEP_CHAR ';'
#   define U_FILE_SEP_STRING "\\"
#   define U_FILE_ALT_SEP_STRING "/"
#   define U_PATH_SEP_STRING ";"
#else
#   define U_FILE_SEP_CHAR '/'
#   define U_FILE_ALT_SEP_CHAR '/'
#   define U_PATH_SEP_CHAR ':'
#   define U_FILE_SEP_STRING "/"
#   define U_FILE_ALT_SEP_STRING "/"
#   define U_PATH_SEP_STRING ":"
#endif

/**
 * Convert char characters to UChar characters.
 * This utility function is useful only for "invariant characters"
 * that are encoded in the platform default encoding.
 * They are a small, constant subset of the encoding and include
 * just the latin letters, digits, and some punctuation.
 * For details, see U_CHARSET_FAMILY.
 *
 * @param cs Input string, points to <code>length</code>
 *           character bytes from a subset of the platform encoding.
 * @param us Output string, points to memory for <code>length</code>
 *           Unicode characters.
 * @param length The number of characters to convert; this may
 *               include the terminating <code>NUL</code>.
 *
 * @see U_CHARSET_FAMILY
 * @stable ICU 2.0
 */
U_STABLE void U_EXPORT2
u_charsToUChars(const char *cs, UChar *us, int32_t length);

/**
 * Convert UChar characters to char characters.
 * This utility function is useful only for "invariant characters"
 * that can be encoded in the platform default encoding.
 * They are a small, constant subset of the encoding and include
 * just the latin letters, digits, and some punctuation.
 * For details, see U_CHARSET_FAMILY.
 *
 * @param us Input string, points to <code>length</code>
 *           Unicode characters that can be encoded with the
 *           codepage-invariant subset of the platform encoding.
 * @param cs Output string, points to memory for <code>length</code>
 *           character bytes.
 * @param length The number of characters to convert; this may
 *               include the terminating <code>NUL</code>.
 *
 * @see U_CHARSET_FAMILY
 * @stable ICU 2.0
 */
U_STABLE void U_EXPORT2
u_UCharsToChars(const UChar *us, char *cs, int32_t length);

#endif