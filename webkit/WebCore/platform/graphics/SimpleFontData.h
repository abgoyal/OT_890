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
 * This file is part of the internal font implementation.
 *
 * Copyright (C) 2006, 2008 Apple Inc. All rights reserved.
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

#ifndef SimpleFontData_h
#define SimpleFontData_h

#include "FontData.h"
#include "FontPlatformData.h"
#include "GlyphPageTreeNode.h"
#include "GlyphWidthMap.h"
#include <wtf/OwnPtr.h>

#if USE(ATSUI)
typedef struct OpaqueATSUStyle* ATSUStyle;
#endif

#if PLATFORM(WIN)
#include <usp10.h>
#endif

#if PLATFORM(CAIRO)
#include <cairo.h>
#endif

#if PLATFORM(QT)
#include <QFont>
#endif

namespace WebCore {

class FontDescription;
class FontPlatformData;
class SharedBuffer;
class SVGFontData;
class WidthMap;

enum Pitch { UnknownPitch, FixedPitch, VariablePitch };

class SimpleFontData : public FontData {
public:
    SimpleFontData(const FontPlatformData&, bool customFont = false, bool loading = false, SVGFontData* data = 0);
    virtual ~SimpleFontData();

public:
    const FontPlatformData& platformData() const { return m_platformData; }
    SimpleFontData* smallCapsFontData(const FontDescription& fontDescription) const;

    // vertical metrics
    int ascent() const { return m_ascent; }
    int descent() const { return m_descent; }
    int lineSpacing() const { return m_lineSpacing; }
    int lineGap() const { return m_lineGap; }
    float maxCharWidth() const { return m_maxCharWidth; }
    float avgCharWidth() const { return m_avgCharWidth; }
    float xHeight() const { return m_xHeight; }
    unsigned unitsPerEm() const { return m_unitsPerEm; }

    float widthForGlyph(Glyph) const;
    float platformWidthForGlyph(Glyph) const;

    float spaceWidth() const { return m_spaceWidth; }
    float adjustedSpaceWidth() const { return m_adjustedSpaceWidth; }

#if PLATFORM(CG) || PLATFORM(CAIRO)
    float syntheticBoldOffset() const { return m_syntheticBoldOffset; }
#endif

    Glyph spaceGlyph() const { return m_spaceGlyph; }

    virtual const SimpleFontData* fontDataForCharacter(UChar32) const;
    virtual bool containsCharacters(const UChar*, int length) const;

    void determinePitch();
    Pitch pitch() const { return m_treatAsFixedPitch ? FixedPitch : VariablePitch; }

#if ENABLE(SVG_FONTS)
    SVGFontData* svgFontData() const { return m_svgFontData.get(); }
    bool isSVGFont() const { return m_svgFontData; }
#else
    bool isSVGFont() const { return false; }
#endif

    virtual bool isCustomFont() const { return m_isCustomFont; }
    virtual bool isLoading() const { return m_isLoading; }
    virtual bool isSegmented() const;

    const GlyphData& missingGlyphData() const { return m_missingGlyphData; }

#ifndef NDEBUG
    virtual String description() const;
#endif

#if PLATFORM(MAC)
    NSFont* getNSFont() const { return m_platformData.font(); }
#endif

#if USE(CORE_TEXT)
    CTFontRef getCTFont() const;
    CFDictionaryRef getCFStringAttributes() const;
#endif

#if USE(ATSUI)
    void checkShapesArabic() const;
    bool shapesArabic() const
    {
        if (!m_checkedShapesArabic)
            checkShapesArabic();
        return m_shapesArabic;
    }
#endif

#if PLATFORM(QT)
    QFont getQtFont() const { return m_platformData.font(); }
#endif

#if PLATFORM(WIN)
    bool isSystemFont() const { return m_isSystemFont; }
    SCRIPT_FONTPROPERTIES* scriptFontProperties() const;
    SCRIPT_CACHE* scriptCache() const { return &m_scriptCache; }

    static void setShouldApplyMacAscentHack(bool);
    static bool shouldApplyMacAscentHack();
#endif

#if PLATFORM(CAIRO)
    void setFont(cairo_t*) const;
#endif

#if PLATFORM(WX)
    wxFont* getWxFont() const { return m_platformData.font(); }
#endif

private:
    void platformInit();
    void platformGlyphInit();
    void platformCharWidthInit();
    void platformDestroy();
    
    void initCharWidths();

    void commonInit();

#if PLATFORM(WIN)
    void initGDIFont();
    void platformCommonDestroy();
    float widthForGDIGlyph(Glyph glyph) const;
#endif

    int m_ascent;
    int m_descent;
    int m_lineSpacing;
    int m_lineGap;
    float m_maxCharWidth;
    float m_avgCharWidth;
    float m_xHeight;
    unsigned m_unitsPerEm;

    FontPlatformData m_platformData;

    mutable GlyphWidthMap m_glyphToWidthMap;

    bool m_treatAsFixedPitch;

#if ENABLE(SVG_FONTS)
    OwnPtr<SVGFontData> m_svgFontData;
#endif

    bool m_isCustomFont;  // Whether or not we are custom font loaded via @font-face
    bool m_isLoading; // Whether or not this custom font is still in the act of loading.

    Glyph m_spaceGlyph;
    float m_spaceWidth;
    float m_adjustedSpaceWidth;

    GlyphData m_missingGlyphData;

    mutable SimpleFontData* m_smallCapsFontData;

#if PLATFORM(CG) || PLATFORM(CAIRO)
    float m_syntheticBoldOffset;
#endif

#ifdef BUILDING_ON_TIGER
public:
    void* m_styleGroup;

private:
#endif

#if USE(ATSUI)
public:
    mutable ATSUStyle m_ATSUStyle;
    mutable bool m_ATSUStyleInitialized;
    mutable bool m_ATSUMirrors;
    mutable bool m_checkedShapesArabic;
    mutable bool m_shapesArabic;

private:
#endif

#if USE(CORE_TEXT)
    mutable RetainPtr<CTFontRef> m_CTFont;
    mutable RetainPtr<CFDictionaryRef> m_CFStringAttributes;
#endif

#if PLATFORM(WIN)
    bool m_isSystemFont;
    mutable SCRIPT_CACHE m_scriptCache;
    mutable SCRIPT_FONTPROPERTIES* m_scriptFontProperties;
#endif
};
    
    
#if !PLATFORM(QT)
ALWAYS_INLINE float SimpleFontData::widthForGlyph(Glyph glyph) const
{
    float width = m_glyphToWidthMap.widthForGlyph(glyph);
    if (width != cGlyphWidthUnknown)
        return width;
    
    width = platformWidthForGlyph(glyph);
    m_glyphToWidthMap.setWidthForGlyph(glyph, width);
    
    return width;
}
#endif

} // namespace WebCore

#endif // SimpleFontData_h
