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
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 *           (C) 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
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
 *
 */

#ifndef EventTarget_h
#define EventTarget_h

#include <wtf/Forward.h>

namespace WebCore {

    class AbstractWorker;
    class AtomicString;
    class DedicatedWorkerContext;
    class DOMApplicationCache;
    class DOMWindow;
    class Event;
    class EventListener;
    class MessagePort;
    class Node;
    class SVGElementInstance;
    class ScriptExecutionContext;
    class SharedWorker;
    class SharedWorkerContext;
    class Worker;
    class XMLHttpRequest;
    class XMLHttpRequestUpload;

    typedef int ExceptionCode;

    class EventTarget {
    public:
        virtual MessagePort* toMessagePort();
        virtual Node* toNode();
        virtual DOMWindow* toDOMWindow();
        virtual XMLHttpRequest* toXMLHttpRequest();
        virtual XMLHttpRequestUpload* toXMLHttpRequestUpload();
#if ENABLE(OFFLINE_WEB_APPLICATIONS)
        virtual DOMApplicationCache* toDOMApplicationCache();
#endif
#if ENABLE(SVG)
        virtual SVGElementInstance* toSVGElementInstance();
#endif
#if ENABLE(WORKERS)
        virtual Worker* toWorker();
        virtual DedicatedWorkerContext* toDedicatedWorkerContext();
#endif

#if ENABLE(SHARED_WORKERS)
        virtual SharedWorker* toSharedWorker();
        virtual SharedWorkerContext* toSharedWorkerContext();
#endif

        virtual ScriptExecutionContext* scriptExecutionContext() const = 0;

        virtual void addEventListener(const AtomicString& eventType, PassRefPtr<EventListener>, bool useCapture) = 0;
        virtual void removeEventListener(const AtomicString& eventType, EventListener*, bool useCapture) = 0;
        virtual bool dispatchEvent(PassRefPtr<Event>, ExceptionCode&) = 0;

        void ref() { refEventTarget(); }
        void deref() { derefEventTarget(); }

        // Handlers to do/undo actions on the target node before an event is dispatched to it and after the event
        // has been dispatched.  The data pointer is handed back by the preDispatch and passed to postDispatch.
        virtual void* preDispatchEventHandler(Event*) { return 0; }
        virtual void postDispatchEventHandler(Event*, void* /*dataFromPreDispatch*/) { }

    protected:
        virtual ~EventTarget();

    private:
        virtual void refEventTarget() = 0;
        virtual void derefEventTarget() = 0;
    };

    void forbidEventDispatch();
    void allowEventDispatch();

#ifndef NDEBUG
    bool eventDispatchForbidden();
#else
    inline void forbidEventDispatch() { }
    inline void allowEventDispatch() { }
#endif

}

#endif
