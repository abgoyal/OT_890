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
 * This file is part of the line box implementation for KDE.
 *
 * Copyright (C) 2003, 2006, 2007, 2008 Apple Inc. All rights reserved.
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

#ifndef RootInlineBox_h
#define RootInlineBox_h

#include "BidiContext.h"
#include "InlineFlowBox.h"

namespace WebCore {

class EllipsisBox;
class HitTestResult;

struct BidiStatus;
struct GapRects;

class RootInlineBox : public InlineFlowBox {
public:
    RootInlineBox(RenderObject* obj)
        : InlineFlowBox(obj)
        , m_overflow(0)
        , m_lineBreakObj(0)
        , m_lineBreakPos(0)
    {
    }

    virtual bool isRootInlineBox() const { return true; }

    virtual void destroy(RenderArena*);
    void detachEllipsisBox(RenderArena*);

    RootInlineBox* nextRootBox() { return static_cast<RootInlineBox*>(m_nextLine); }
    RootInlineBox* prevRootBox() { return static_cast<RootInlineBox*>(m_prevLine); }

    virtual void adjustPosition(int dx, int dy);

    virtual int topOverflow() const { return m_overflow ? m_overflow->m_topOverflow : m_y; }
    virtual int bottomOverflow() const { return m_overflow ? m_overflow->m_bottomOverflow : m_y + m_renderer->style(m_firstLine)->font().height(); }
    virtual int leftOverflow() const { return m_overflow ? m_overflow->m_leftOverflow : m_x; }
    virtual int rightOverflow() const { return m_overflow ? m_overflow->m_rightOverflow : m_x + m_width; }

    virtual void setVerticalOverflowPositions(int top, int bottom);
    void setHorizontalOverflowPositions(int left, int right);

    virtual void setVerticalSelectionPositions(int top, int bottom);

    virtual RenderLineBoxList* rendererLineBoxes() const;

#if ENABLE(SVG)
    virtual void computePerCharacterLayoutInformation() { }
#endif

    RenderObject* lineBreakObj() const { return m_lineBreakObj; }
    BidiStatus lineBreakBidiStatus() const;
    void setLineBreakInfo(RenderObject*, unsigned breakPos, const BidiStatus&);

    unsigned lineBreakPos() const { return m_lineBreakPos; }
    void setLineBreakPos(unsigned p) { m_lineBreakPos = p; }

    int blockHeight() const { return m_blockHeight; }
    void setBlockHeight(int h) { m_blockHeight = h; }

    bool endsWithBreak() const { return m_endsWithBreak; }
    void setEndsWithBreak(bool b) { m_endsWithBreak = b; }

    void childRemoved(InlineBox* box);

    bool canAccommodateEllipsis(bool ltr, int blockEdge, int lineBoxEdge, int ellipsisWidth);
    void placeEllipsis(const AtomicString& ellipsisStr, bool ltr, int blockLeftEdge, int blockRightEdge, int ellipsisWidth, InlineBox* markupBox = 0);
    virtual int placeEllipsisBox(bool ltr, int blockLeftEdge, int blockRightEdge, int ellipsisWidth, bool& foundBox);

    EllipsisBox* ellipsisBox() const;

    void paintEllipsisBox(RenderObject::PaintInfo&, int tx, int ty) const;
    bool hitTestEllipsisBox(HitTestResult&, int x, int y, int tx, int ty, HitTestAction, bool);

    virtual void clearTruncation();

#if PLATFORM(MAC)
    void addHighlightOverflow();
    void paintCustomHighlight(RenderObject::PaintInfo&, int tx, int ty, const AtomicString& highlightType);
#endif

    virtual void paint(RenderObject::PaintInfo&, int tx, int ty);
    virtual bool nodeAtPoint(const HitTestRequest&, HitTestResult&, int, int, int, int);

    bool hasSelectedChildren() const { return m_hasSelectedChildren; }
    void setHasSelectedChildren(bool);

    virtual RenderObject::SelectionState selectionState();
    InlineBox* firstSelectedBox();
    InlineBox* lastSelectedBox();

    GapRects fillLineSelectionGap(int selTop, int selHeight, RenderBlock* rootBlock, int blockX, int blockY,
                                  int tx, int ty, const RenderObject::PaintInfo*);

    RenderBlock* block() const;

    int selectionTop();
    int selectionBottom() { return m_overflow ? m_overflow->m_selectionBottom : m_y + height(); }
    int selectionHeight() { return max(0, selectionBottom() - selectionTop()); }

    InlineBox* closestLeafChildForXPos(int x, bool onlyEditableLeaves = false);

    Vector<RenderBox*>& floats()
    {
        ASSERT(!isDirty());
        if (!m_overflow)
            m_overflow = new (m_renderer->renderArena()) Overflow(this);
        return m_overflow->floats;
    }

    Vector<RenderBox*>* floatsPtr() { ASSERT(!isDirty()); return m_overflow ? &m_overflow->floats : 0; }

    virtual void extractLineBoxFromRenderObject();
    virtual void attachLineBoxToRenderObject();
    virtual void removeLineBoxFromRenderObject();

protected:
    // Normally we are only as tall as the style on our block dictates, but we might have content
    // that spills out above the height of our font (e.g, a tall image), or something that extends further
    // below our line (e.g., a child whose font has a huge descent).
        
    // Allocated only when some of these fields have non-default values
    struct Overflow {
        Overflow(RootInlineBox* box) 
            : m_topOverflow(box->m_y)
            , m_bottomOverflow(box->m_y + box->height())
            , m_leftOverflow(box->m_x)
            , m_rightOverflow(box->m_x + box->m_width)
            , m_selectionTop(box->m_y)
            , m_selectionBottom(box->m_y + box->height())
        {
        }

        void destroy(RenderArena*);
        void* operator new(size_t, RenderArena*) throw();
        void operator delete(void*, size_t);
        
        int m_topOverflow;
        int m_bottomOverflow;
        int m_leftOverflow;
        int m_rightOverflow;
        int m_selectionTop;
        int m_selectionBottom;
        // Floats hanging off the line are pushed into this vector during layout. It is only
        // good for as long as the line has not been marked dirty.
        Vector<RenderBox*> floats;
    private:
        void* operator new(size_t) throw();
    };
    
    Overflow* m_overflow;

    // Where this line ended.  The exact object and the position within that object are stored so that
    // we can create an InlineIterator beginning just after the end of this line.
    RenderObject* m_lineBreakObj;
    unsigned m_lineBreakPos;
    RefPtr<BidiContext> m_lineBreakContext;

    // The height of the block at the end of this line.  This is where the next line starts.
    int m_blockHeight;

    WTF::Unicode::Direction m_lineBreakBidiStatusEor : 5;
    WTF::Unicode::Direction m_lineBreakBidiStatusLastStrong : 5;
    WTF::Unicode::Direction m_lineBreakBidiStatusLast : 5;
};

inline void RootInlineBox::setHorizontalOverflowPositions(int left, int right) 
{ 
    if (!m_overflow) {
        if (left == m_x && right == m_x + m_width)
            return;
        m_overflow = new (m_renderer->renderArena()) Overflow(this);       
    }
    m_overflow->m_leftOverflow = left; 
    m_overflow->m_rightOverflow = right; 
}

inline void RootInlineBox::setVerticalSelectionPositions(int top, int bottom) 
{ 
    if (!m_overflow) {
        const Font& font = m_renderer->style(m_firstLine)->font();
        if (top == m_y && bottom == m_y + font.height())
            return;
        m_overflow = new (m_renderer->renderArena()) Overflow(this);
    }
    m_overflow->m_selectionTop = top; 
    m_overflow->m_selectionBottom = bottom; 
}

} // namespace WebCore

#endif // RootInlineBox_h
