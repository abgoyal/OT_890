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
 * Copyright (C) 2008 Matt Lilek <webkit@mattlilek.com>
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef InspectorResource_h
#define InspectorResource_h

#include "HTTPHeaderMap.h"
#include "KURL.h"
#include "ScriptObject.h"
#include "ScriptState.h"
#include "ScriptString.h"

#include <wtf/CurrentTime.h>
#include <wtf/OwnPtr.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/RefPtr.h>

namespace WebCore {

    class CachedResource;
    class DocumentLoader;
    class InspectorFrontend;
    class Frame;
    class ResourceResponse;

    struct ResourceRequest;

    class InspectorResource : public RefCounted<InspectorResource> {
    public:

        // Keep these in sync with WebInspector.Resource.Type
        enum Type {
            Doc,
            Stylesheet,
            Image,
            Font,
            Script,
            XHR,
            Media,
            Other
        };

        static PassRefPtr<InspectorResource> create(long long identifier, DocumentLoader* loader)
        {
            return adoptRef(new InspectorResource(identifier, loader));
        }

        static PassRefPtr<InspectorResource> createCached(long long identifier, DocumentLoader*, const CachedResource*);

        ~InspectorResource();

        void createScriptObject(InspectorFrontend* frontend);
        void updateScriptObject(InspectorFrontend* frontend);
        void releaseScriptObject(InspectorFrontend* frontend, bool callRemoveResource);

        void updateRequest(const ResourceRequest&);
        void updateResponse(const ResourceResponse&);

        void setXMLHttpResponseText(const ScriptString& data);

        String sourceString() const;
        bool isSameLoader(DocumentLoader* loader) const { return loader == m_loader; }
        void markMainResource() { m_isMainResource = true; }
        long long identifier() const { return m_identifier; }
        String requestURL() const { return m_requestURL.string(); }
        Frame* frame() const { return m_frame.get(); }
        const String& mimeType() const { return m_mimeType; }
        void startTiming();
        void markResponseReceivedTime();
        void endTiming();

        void markFailed();
        void addLength(int lengthReceived);

    private:
        enum ChangeType {
            NoChange = 0,
            RequestChange = 1,
            ResponseChange = 2,
            TypeChange = 4,
            LengthChange = 8,
            CompletionChange = 16,
            TimingChange = 32
        };

        class Changes {
        public:
            Changes() : m_change(NoChange) {}

            inline bool hasChange(ChangeType change) { return (m_change & change) || !(m_change + change); }
            inline void set(ChangeType change)
            {
                m_change = static_cast<ChangeType>(static_cast<unsigned>(m_change) | static_cast<unsigned>(change));            
            }
            inline void clear(ChangeType change)
            {
                m_change = static_cast<ChangeType>(static_cast<unsigned>(m_change) & ~static_cast<unsigned>(change));
            }

            inline void setAll() { m_change = static_cast<ChangeType>(63); }
            inline void clearAll() { m_change = NoChange; }

        private:
            ChangeType m_change;
        };

        InspectorResource(long long identifier, DocumentLoader*);
        Type type() const;

        long long m_identifier;
        RefPtr<DocumentLoader> m_loader;
        RefPtr<Frame> m_frame;
        KURL m_requestURL;
        HTTPHeaderMap m_requestHeaderFields;
        HTTPHeaderMap m_responseHeaderFields;
        String m_mimeType;
        String m_suggestedFilename;
        bool m_scriptObjectCreated;
        long long m_expectedContentLength;
        bool m_cached;
        bool m_finished;
        bool m_failed;
        int m_length;
        int m_responseStatusCode;
        double m_startTime;
        double m_responseReceivedTime;
        double m_endTime;
        ScriptString m_xmlHttpResponseText;
        Changes m_changes;
        bool m_isMainResource;
    };

} // namespace WebCore

#endif // InspectorResource_h
