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
    Copyright (C) 1999 Lars Knoll (knoll@mpi-hd.mpg.de)
    Copyright (C) 2006 Alexey Proskuryakov (ap@nypop.com)
    Copyright (C) 2006, 2008 Apple Inc. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef TextResourceDecoder_h
#define TextResourceDecoder_h

#include "TextEncoding.h"

namespace WebCore {

class TextResourceDecoder : public RefCounted<TextResourceDecoder> {
public:
    enum EncodingSource {
        DefaultEncoding,
        AutoDetectedEncoding,
        EncodingFromXMLHeader,
        EncodingFromMetaTag,
        EncodingFromCSSCharset,
        EncodingFromHTTPHeader,
        UserChosenEncoding,
        EncodingFromParentFrame
    };

    static PassRefPtr<TextResourceDecoder> create(const String& mimeType, const TextEncoding& defaultEncoding = TextEncoding(), bool usesEncodingDetector = false)
    {
        return adoptRef(new TextResourceDecoder(mimeType, defaultEncoding, usesEncodingDetector));
    }
    ~TextResourceDecoder();

    void setEncoding(const TextEncoding&, EncodingSource);
    const TextEncoding& encoding() const { return m_encoding; }

    String decode(const char* data, size_t length);
    String flush();

    void setHintEncoding(const TextResourceDecoder* hintDecoder)
    {
        // hintEncoding is for use with autodetection, which should be 
        // only invoked when hintEncoding comes from auto-detection.
        if (hintDecoder && hintDecoder->m_source == AutoDetectedEncoding)
            m_hintEncoding = hintDecoder->encoding().name();
    }
   
    void useLenientXMLDecoding() { m_useLenientXMLDecoding = true; }
    bool sawError() const { return m_sawError; }

private:
    TextResourceDecoder(const String& mimeType, const TextEncoding& defaultEncoding,
                        bool usesEncodingDetector);

    enum ContentType { PlainText, HTML, XML, CSS }; // PlainText only checks for BOM.
    static ContentType determineContentType(const String& mimeType);
    static const TextEncoding& defaultEncoding(ContentType, const TextEncoding& defaultEncoding);

    size_t checkForBOM(const char*, size_t);
    bool checkForCSSCharset(const char*, size_t, bool& movedDataToBuffer);
    bool checkForHeadCharset(const char*, size_t, bool& movedDataToBuffer);
    void detectJapaneseEncoding(const char*, size_t);
    bool shouldAutoDetect() const;

    ContentType m_contentType;
    TextEncoding m_encoding;
    OwnPtr<TextCodec> m_codec;
    EncodingSource m_source;
    const char* m_hintEncoding;
    Vector<char> m_buffer;
    bool m_checkedForBOM;
    bool m_checkedForCSSCharset;
    bool m_checkedForHeadCharset;
    bool m_useLenientXMLDecoding; // Don't stop on XML decoding errors.
    bool m_sawError;
    bool m_usesEncodingDetector;
};

}

#endif
