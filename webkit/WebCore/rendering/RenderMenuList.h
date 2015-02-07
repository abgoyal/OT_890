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
 * This file is part of the select element renderer in WebCore.
 *
 * Copyright (C) 2006, 2007 Apple Inc. All rights reserved.
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

#ifndef RenderMenuList_h
#define RenderMenuList_h

#include "PopupMenuClient.h"
#include "RenderFlexibleBox.h"

#if PLATFORM(MAC)
#define POPUP_MENU_PULLS_DOWN 0
#else
#define POPUP_MENU_PULLS_DOWN 1
#endif

namespace WebCore {

class PopupMenu;
class RenderText;

class RenderMenuList : public RenderFlexibleBox, private PopupMenuClient {
public:
    RenderMenuList(Element*);
    virtual ~RenderMenuList();

public:
    bool popupIsVisible() const { return m_popupIsVisible; }
    void showPopup();
    void hidePopup();

    void setOptionsChanged(bool changed) { m_optionsChanged = changed; }

    String text() const;

private:
    virtual bool isMenuList() const { return true; }

    virtual void addChild(RenderObject* newChild, RenderObject* beforeChild = 0);
    virtual void removeChild(RenderObject*);
    virtual bool createsAnonymousWrapper() const { return true; }
    virtual bool canHaveChildren() const { return false; }

    virtual void updateFromElement();

    virtual bool hasControlClip() const { return true; }
    virtual IntRect controlClipRect(int tx, int ty) const;

    virtual const char* renderName() const { return "RenderMenuList"; }

    virtual void calcPrefWidths();

    virtual void styleDidChange(StyleDifference, const RenderStyle* oldStyle);

    // PopupMenuClient methods
    virtual String itemText(unsigned listIndex) const;
    virtual String itemToolTip(unsigned listIndex) const;
    virtual bool itemIsEnabled(unsigned listIndex) const;
    virtual PopupMenuStyle itemStyle(unsigned listIndex) const;
    virtual PopupMenuStyle menuStyle() const;
    virtual int clientInsetLeft() const;
    virtual int clientInsetRight() const;
    virtual int clientPaddingLeft() const;
    virtual int clientPaddingRight() const;
    virtual int listSize() const;
    virtual int selectedIndex() const;
    virtual bool itemIsSeparator(unsigned listIndex) const;
    virtual bool itemIsLabel(unsigned listIndex) const;
    virtual bool itemIsSelected(unsigned listIndex) const;
    virtual void setTextFromItem(unsigned listIndex);
    virtual bool valueShouldChangeOnHotTrack() const { return true; }
    virtual bool shouldPopOver() const { return !POPUP_MENU_PULLS_DOWN; }
    virtual void valueChanged(unsigned listIndex, bool fireOnChange = true);
    virtual FontSelector* fontSelector() const;
    virtual HostWindow* hostWindow() const;
    virtual PassRefPtr<Scrollbar> createScrollbar(ScrollbarClient*, ScrollbarOrientation, ScrollbarControlSize);

    virtual bool hasLineIfEmpty() const { return true; }

    Color itemBackgroundColor(unsigned listIndex) const;

    void createInnerBlock();
    void adjustInnerStyle();
    void setText(const String&);
    void setTextFromOption(int optionIndex);
    void updateOptionsWidth();

    RenderText* m_buttonText;
    RenderBlock* m_innerBlock;

    bool m_optionsChanged;
    int m_optionsWidth;

    RefPtr<PopupMenu> m_popup;
    bool m_popupIsVisible;
};

inline RenderMenuList* toRenderMenuList(RenderObject* object)
{ 
    ASSERT(!object || object->isMenuList());
    return static_cast<RenderMenuList*>(object);
}

// This will catch anyone doing an unnecessary cast.
void toRenderMenuList(const RenderMenuList*);

}

#endif
