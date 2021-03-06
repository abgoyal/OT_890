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
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Graham Dennis (graham.dennis@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef ContentData_h
#define ContentData_h

#include "PlatformString.h"
#include "RenderStyleConstants.h"
#include "StringImpl.h"
#include "StyleImage.h"
#include <wtf/Noncopyable.h>

namespace WebCore {

class CounterContent;

struct ContentData : Noncopyable {
public:
    ContentData()
        : m_type(CONTENT_NONE)
        , m_next(0)
    {
    }

    ~ContentData()
    {
        clear();
    }

    void clear();

    bool isCounter() const { return m_type == CONTENT_COUNTER; }
    bool isImage() const { return m_type == CONTENT_OBJECT; }
    bool isNone() const { return m_type == CONTENT_NONE; }
    bool isText() const { return m_type == CONTENT_TEXT; }

    StyleContentType type() const { return m_type; }

    bool dataEquivalent(const ContentData&) const;

    StyleImage* image() const { return m_content.m_image; }
    void setImage(PassRefPtr<StyleImage> image)
    {
        deleteContent();
        m_type = CONTENT_OBJECT;
        m_content.m_image = image.releaseRef();
    }

    StringImpl* text() const { return m_content.m_text; }
    void setText(PassRefPtr<StringImpl> text)
    {
        deleteContent();
        m_type = CONTENT_TEXT;
        m_content.m_text = text.releaseRef();
    }

    CounterContent* counter() const { return m_content.m_counter; }
    void setCounter(CounterContent* counter)
    {
        deleteContent();
        m_type = CONTENT_COUNTER;
        m_content.m_counter = counter;
    }

    ContentData* next() const { return m_next; }
    void setNext(ContentData* next)
    {
        m_next = next;
    }

private:
    void deleteContent();

    StyleContentType m_type;
    union {
        StyleImage* m_image;
        StringImpl* m_text;
        CounterContent* m_counter;
    } m_content;
    ContentData* m_next;
};

} // namespace WebCore

#endif // ContentData_h
