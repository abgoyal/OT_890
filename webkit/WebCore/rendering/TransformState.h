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
 * Copyright (C) 2009 Apple Inc.  All rights reserved.
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

#ifndef TransformState_h
#define TransformState_h

#include "FloatPoint.h"
#include "FloatQuad.h"
#include "IntSize.h"
#include "TransformationMatrix.h"
#include <wtf/Noncopyable.h>
#include <wtf/PassRefPtr.h>
#include <wtf/OwnPtr.h>
#include <wtf/RefCounted.h>

namespace WebCore {

class TransformState : public Noncopyable {
public:
    enum TransformDirection { ApplyTransformDirection, UnapplyInverseTransformDirection };
    enum TransformAccumulation { FlattenTransform, AccumulateTransform };

    // If quad is non-null, it will be mapped
    TransformState(TransformDirection mappingDirection, const FloatPoint& p, const FloatQuad* quad = 0)
        : m_lastPlanarPoint(p)
        , m_accumulatingTransform(false)
        , m_mapQuad(quad != 0)
        , m_direction(mappingDirection)
    {
        if (quad)
            m_lastPlanarQuad = *quad;
    }
    
    void move(const IntSize& s, TransformAccumulation accumulate = FlattenTransform)
    {
        move(s.width(), s.height(), accumulate);
    }
    
    void move(int x, int y, TransformAccumulation = FlattenTransform);
    void applyTransform(const TransformationMatrix& transformFromContainer, TransformAccumulation = FlattenTransform);
    void flatten();

    // Return the coords of the point or quad in the last flattened layer
    FloatPoint lastPlanarPoint() const { return m_lastPlanarPoint; }
    FloatQuad lastPlanarQuad() const { return m_lastPlanarQuad; }

    // Return the point or quad mapped through the current transform
    FloatPoint mappedPoint() const;
    FloatQuad mappedQuad() const;

private:
    void flattenWithTransform(const TransformationMatrix&);
    
    FloatPoint m_lastPlanarPoint;
    FloatQuad m_lastPlanarQuad;

    // We only allocate the transform if we need to
    OwnPtr<TransformationMatrix> m_accumulatedTransform;
    bool m_accumulatingTransform;
    bool m_mapQuad;
    TransformDirection m_direction;
};

class HitTestingTransformState : public RefCounted<HitTestingTransformState> {
public:
    static PassRefPtr<HitTestingTransformState> create(const FloatPoint& p, const FloatQuad& quad)
    {
        return adoptRef(new HitTestingTransformState(p, quad));
    }

    static PassRefPtr<HitTestingTransformState> create(const HitTestingTransformState& other)
    {
        return adoptRef(new HitTestingTransformState(other));
    }

    enum TransformAccumulation { FlattenTransform, AccumulateTransform };
    void translate(int x, int y, TransformAccumulation);
    void applyTransform(const TransformationMatrix& transformFromContainer, TransformAccumulation);

    FloatPoint mappedPoint() const;
    FloatQuad mappedQuad() const;
    void flatten();

    FloatPoint m_lastPlanarPoint;
    FloatQuad m_lastPlanarQuad;
    TransformationMatrix m_accumulatedTransform;
    bool m_accumulatingTransform;

private:
    HitTestingTransformState(const FloatPoint& p, const FloatQuad& quad)
        : m_lastPlanarPoint(p)
        , m_lastPlanarQuad(quad)
        , m_accumulatingTransform(false)
    {
    }
    
    HitTestingTransformState(const HitTestingTransformState& other)
        : RefCounted<HitTestingTransformState>()
        , m_lastPlanarPoint(other.m_lastPlanarPoint)
        , m_lastPlanarQuad(other.m_lastPlanarQuad)
        , m_accumulatedTransform(other.m_accumulatedTransform)
        , m_accumulatingTransform(other.m_accumulatingTransform)
    {
    }
    
    void flattenWithTransform(const TransformationMatrix&);
};

} // namespace WebCore

#endif // TransformState_h
