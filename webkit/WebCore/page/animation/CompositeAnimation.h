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
 * Copyright (C) 2007 Apple Inc. All rights reserved.
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

#ifndef CompositeAnimation_h
#define CompositeAnimation_h

#include "AtomicString.h"

#include "ImplicitAnimation.h"
#include "KeyframeAnimation.h"
#include <wtf/HashMap.h>
#include <wtf/Noncopyable.h>

namespace WebCore {

class AnimationControllerPrivate;
class AnimationController;
class RenderObject;
class RenderStyle;

// A CompositeAnimation represents a collection of animations that are running
// on a single RenderObject, such as a number of properties transitioning at once.
class CompositeAnimation : public RefCounted<CompositeAnimation> {
public:
    static PassRefPtr<CompositeAnimation> create(AnimationControllerPrivate* animationController)
    {
        return adoptRef(new CompositeAnimation(animationController));
    };

    ~CompositeAnimation();
    
    void clearRenderer();

    PassRefPtr<RenderStyle> animate(RenderObject*, RenderStyle* currentStyle, RenderStyle* targetStyle);
    PassRefPtr<RenderStyle> getAnimatedStyle() const;

    double timeToNextService() const;
    
    AnimationControllerPrivate* animationController() const { return m_animationController; }

    void suspendAnimations();
    void resumeAnimations();
    bool isSuspended() const { return m_isSuspended; }
    
    bool hasAnimations() const  { return !m_transitions.isEmpty() || !m_keyframeAnimations.isEmpty(); }

    void setAnimating(bool);
    bool isAnimatingProperty(int property, bool isRunningNow) const;
    
    PassRefPtr<KeyframeAnimation> getAnimationForProperty(int property) const;

    void overrideImplicitAnimations(int property);
    void resumeOverriddenImplicitAnimations(int property);

    bool pauseAnimationAtTime(const AtomicString& name, double t);
    bool pauseTransitionAtTime(int property, double t);
    unsigned numberOfActiveAnimations() const;

private:
    CompositeAnimation(AnimationControllerPrivate* animationController)
        : m_animationController(animationController)
        , m_numStyleAvailableWaiters(0)
        , m_isSuspended(false)
    {
    }

    void updateTransitions(RenderObject*, RenderStyle* currentStyle, RenderStyle* targetStyle);
    void updateKeyframeAnimations(RenderObject*, RenderStyle* currentStyle, RenderStyle* targetStyle);
    
    typedef HashMap<int, RefPtr<ImplicitAnimation> > CSSPropertyTransitionsMap;
    typedef HashMap<AtomicStringImpl*, RefPtr<KeyframeAnimation> >  AnimationNameMap;

    AnimationControllerPrivate* m_animationController;
    CSSPropertyTransitionsMap m_transitions;
    AnimationNameMap m_keyframeAnimations;
    Vector<AtomicStringImpl*> m_keyframeAnimationOrderMap;
    unsigned m_numStyleAvailableWaiters;
    bool m_isSuspended;
};

} // namespace WebCore

#endif // CompositeAnimation_h
