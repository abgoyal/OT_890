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
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef WorkerContextExecutionProxy_h
#define WorkerContextExecutionProxy_h

#if ENABLE(WORKERS)

#include "ScriptValue.h"
#include "V8EventListenerList.h"
#include "V8Index.h"
#include <v8.h>
#include <wtf/OwnPtr.h>
#include <wtf/Vector.h>

namespace WebCore {

    class Event;
    class EventTarget;
    class V8EventListener;
    class V8WorkerContextEventListener;
    class WorkerContext;

    struct WorkerContextExecutionState {
        WorkerContextExecutionState() : hadException(false), lineNumber(0) { }

        bool hadException;
        ScriptValue exception;
        String errorMessage;
        int lineNumber;
        String sourceURL;
    };

    class WorkerContextExecutionProxy {
    public:
        WorkerContextExecutionProxy(WorkerContext*);
        ~WorkerContextExecutionProxy();

        void removeEventListener(V8EventListener*);

        // Finds/creates event listener wrappers.
        PassRefPtr<V8EventListener> findOrCreateEventListener(v8::Local<v8::Value> listener, bool isInline, bool findOnly);
        PassRefPtr<V8EventListener> findOrCreateObjectEventListener(v8::Local<v8::Value> object, bool isInline, bool findOnly);
        PassRefPtr<V8EventListener> findOrCreateEventListenerHelper(v8::Local<v8::Value> object, bool isInline, bool findOnly, bool createObjectEventListener);

        // Track the event so that we can detach it from the JS wrapper when a worker
        // terminates. This is needed because we need to be able to dispose these
        // events and releases references to their event targets: WorkerContext.
        void trackEvent(Event*);

        // Evaluate a script file in the current execution environment.
        ScriptValue evaluate(const String& script, const String& fileName, int baseLine, WorkerContextExecutionState*);

        // Returns a local handle of the context.
        v8::Local<v8::Context> context() { return v8::Local<v8::Context>::New(m_context); }

        // Returns WorkerContext object.
        WorkerContext* workerContext() { return m_workerContext; }

        // Returns WorkerContextExecutionProxy object of the currently executing context. 0 will be returned if the current executing context is not the worker context.
        static WorkerContextExecutionProxy* retrieve();

        // We have to keep all these conversion functions here before WorkerContextExecutionProxy is refactor-ed.
        template<typename T>
        static v8::Handle<v8::Value> convertToV8Object(V8ClassIndex::V8WrapperType type, PassRefPtr<T> impl)
        {
            return convertToV8Object(type, impl.get());
        }
        static v8::Handle<v8::Value> convertToV8Object(V8ClassIndex::V8WrapperType, void* impl);
        static v8::Handle<v8::Value> convertEventToV8Object(Event*);
        static v8::Handle<v8::Value> convertEventTargetToV8Object(EventTarget*);
        static v8::Handle<v8::Value> convertWorkerContextToV8Object(WorkerContext*);

    private:
        void initV8IfNeeded();
        void initContextIfNeeded();
        void dispose();

        // Run an already compiled script.
        v8::Local<v8::Value> runScript(v8::Handle<v8::Script>);

        static v8::Local<v8::Object> toV8(V8ClassIndex::V8WrapperType descriptorType, V8ClassIndex::V8WrapperType cptrType, void* impl);

        static bool forgetV8EventObject(Event*);

        WorkerContext* m_workerContext;
        v8::Persistent<v8::Context> m_context;
        int m_recursion;

        OwnPtr<V8EventListenerList> m_listeners;
        Vector<Event*> m_events;
    };

} // namespace WebCore

#endif // ENABLE(WORKERS)

#endif // WorkerContextExecutionProxy_h
