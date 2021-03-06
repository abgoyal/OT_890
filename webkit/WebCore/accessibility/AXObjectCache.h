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
 * Copyright (C) 2003, 2006, 2007, 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AXObjectCache_h
#define AXObjectCache_h

#include "AccessibilityObject.h"
#include "EventHandler.h"
#include "Timer.h"
#include <limits.h>
#include <wtf/HashMap.h>
#include <wtf/HashSet.h>
#include <wtf/RefPtr.h>

#ifdef __OBJC__
@class WebCoreTextMarker;
#else
class WebCoreTextMarker;
#endif

namespace WebCore {

    class RenderObject;
    class String;
    class VisiblePosition;
    class AccessibilityObject;
    class Node;
    
    typedef unsigned AXID;

    struct TextMarkerData  {
        AXID axID;
        Node* node;
        int offset;
        EAffinity affinity;
    };

    class AXObjectCache {
    public:
        AXObjectCache();
        ~AXObjectCache();
        
        // to be used with render objects
        AccessibilityObject* getOrCreate(RenderObject*);
        
        // used for objects without backing elements
        AccessibilityObject* getOrCreate(AccessibilityRole);
        
        // will only return the AccessibilityObject if it already exists
        AccessibilityObject* get(RenderObject*);
        
        void remove(RenderObject*);
        void remove(AXID);

        void detachWrapper(AccessibilityObject*);
        void attachWrapper(AccessibilityObject*);
        void postNotification(RenderObject*, const String&, bool postToElement);
        void postPlatformNotification(AccessibilityObject*, const String&);
        void childrenChanged(RenderObject*);
        void selectedChildrenChanged(RenderObject*);
        void handleActiveDescendantChanged(RenderObject*);
        void handleAriaRoleChanged(RenderObject*);
        void handleFocusedUIElementChanged();
#if PLATFORM(GTK)
        void handleFocusedUIElementChangedWithRenderers(RenderObject*, RenderObject*);
#endif
        static void enableAccessibility() { gAccessibilityEnabled = true; }
        static void enableEnhancedUserInterfaceAccessibility() { gAccessibilityEnhancedUserInterfaceEnabled = true; }
        
        static bool accessibilityEnabled() { return gAccessibilityEnabled; }
        static bool accessibilityEnhancedUserInterfaceEnabled() { return gAccessibilityEnhancedUserInterfaceEnabled; }

        void removeAXID(AccessibilityObject*);
        bool isIDinUse(AXID id) const { return m_idsInUse.contains(id); }

        // Text marker utilities.
        static void textMarkerDataForVisiblePosition(TextMarkerData&, const VisiblePosition&);
        static VisiblePosition visiblePositionForTextMarkerData(TextMarkerData&);
                
    private:
        HashMap<AXID, RefPtr<AccessibilityObject> > m_objects;
        HashMap<RenderObject*, AXID> m_renderObjectMapping;
        static bool gAccessibilityEnabled;
        static bool gAccessibilityEnhancedUserInterfaceEnabled;
        
        HashSet<AXID> m_idsInUse;
        
        Timer<AXObjectCache> m_notificationPostTimer;
        Vector<pair<RefPtr<AccessibilityObject>, const String> > m_notificationsToPost;
        void notificationPostTimerFired(Timer<AXObjectCache>*);
        
        AXID getAXID(AccessibilityObject*);
        bool nodeIsAriaType(Node* node, String role);
    };

#if !HAVE(ACCESSIBILITY)
    inline void AXObjectCache::handleActiveDescendantChanged(RenderObject*) { }
    inline void AXObjectCache::handleAriaRoleChanged(RenderObject*) { }
    inline void AXObjectCache::handleFocusedUIElementChanged() { }
    inline void AXObjectCache::detachWrapper(AccessibilityObject*) { }
    inline void AXObjectCache::attachWrapper(AccessibilityObject*) { }
    inline void AXObjectCache::selectedChildrenChanged(RenderObject*) { }
    inline void AXObjectCache::postNotification(RenderObject*, const String&, bool postToElement) { }
    inline void AXObjectCache::postPlatformNotification(AccessibilityObject*, const String&) { }
#if PLATFORM(GTK)
    inline void AXObjectCache::handleFocusedUIElementChangedWithRenderers(RenderObject*, RenderObject*) { }
#endif
#endif

}

#endif
