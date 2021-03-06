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
 * Copyright (C) 2007, 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SymbolTable_h
#define SymbolTable_h

#include "JSObject.h"
#include "UString.h"
#include <wtf/AlwaysInline.h>

namespace JSC {

    static ALWAYS_INLINE int missingSymbolMarker() { return std::numeric_limits<int>::max(); }

    // The bit twiddling in this class assumes that every register index is a
    // reasonably small positive or negative number, and therefore has its high
    // four bits all set or all unset.

    struct SymbolTableEntry {
        SymbolTableEntry()
            : m_bits(0)
        {
        }

        SymbolTableEntry(int index)
        {
            ASSERT(isValidIndex(index));
            pack(index, false, false);
        }

        SymbolTableEntry(int index, unsigned attributes)
        {
            ASSERT(isValidIndex(index));
            pack(index, attributes & ReadOnly, attributes & DontEnum);
        }
        
        bool isNull() const
        {
            return !m_bits;
        }

        int getIndex() const
        {
            return m_bits >> FlagBits;
        }

        unsigned getAttributes() const
        {
            unsigned attributes = 0;
            if (m_bits & ReadOnlyFlag)
                attributes |= ReadOnly;
            if (m_bits & DontEnumFlag)
                attributes |= DontEnum;
            return attributes;
        }

        void setAttributes(unsigned attributes)
        {
            pack(getIndex(), attributes & ReadOnly, attributes & DontEnum);
        }

        bool isReadOnly() const
        {
            return m_bits & ReadOnlyFlag;
        }

    private:
        static const unsigned ReadOnlyFlag = 0x1;
        static const unsigned DontEnumFlag = 0x2;
        static const unsigned NotNullFlag = 0x4;
        static const unsigned FlagBits = 3;

        void pack(int index, bool readOnly, bool dontEnum)
        {
            m_bits = (index << FlagBits) | NotNullFlag;
            if (readOnly)
                m_bits |= ReadOnlyFlag;
            if (dontEnum)
                m_bits |= DontEnumFlag;
        }
        
        bool isValidIndex(int index)
        {
            return ((index << FlagBits) >> FlagBits) == index;
        }

        int m_bits;
    };

    struct SymbolTableIndexHashTraits {
        typedef SymbolTableEntry TraitType;
        static SymbolTableEntry emptyValue() { return SymbolTableEntry(); }
        static const bool emptyValueIsZero = true;
        static const bool needsDestruction = false;
    };

    typedef HashMap<RefPtr<UString::Rep>, SymbolTableEntry, IdentifierRepHash, HashTraits<RefPtr<UString::Rep> >, SymbolTableIndexHashTraits> SymbolTable;

} // namespace JSC

#endif // SymbolTable_h
