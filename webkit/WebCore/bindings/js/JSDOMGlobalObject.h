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
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
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

#ifndef JSDOMGlobalObject_h
#define JSDOMGlobalObject_h

#include <runtime/JSGlobalObject.h>

namespace WebCore {

    class Document;
    class Event;
    class JSLazyEventListener;
    class JSEventListener;
    class ScriptExecutionContext;

    typedef HashMap<const JSC::ClassInfo*, RefPtr<JSC::Structure> > JSDOMStructureMap;
    typedef HashMap<const JSC::ClassInfo*, JSC::JSObject*> JSDOMConstructorMap;

    class JSDOMGlobalObject : public JSC::JSGlobalObject {
        typedef JSC::JSGlobalObject Base;
    protected:
        struct JSDOMGlobalObjectData;

        JSDOMGlobalObject(PassRefPtr<JSC::Structure>, JSDOMGlobalObjectData*, JSC::JSObject* thisValue);
        virtual ~JSDOMGlobalObject();

    public:
        JSDOMStructureMap& structures() { return d()->structures; }
        JSDOMConstructorMap& constructors() const { return d()->constructors; }

        virtual ScriptExecutionContext* scriptExecutionContext() const = 0;

        // Finds a wrapper of a GC-unprotected JS EventListener, returns 0 if no existing one.
        JSEventListener* findJSEventListener(JSC::JSValue);

        // Finds or creates a wrapper of a JS EventListener. JS EventListener object is *NOT* GC-protected.
        PassRefPtr<JSEventListener> findOrCreateJSEventListener(JSC::JSValue);

        // Creates a GC-protected JS EventListener for an "onXXX" event attribute.
        // These listeners cannot be removed through the removeEventListener API.
        PassRefPtr<JSEventListener> createJSAttributeEventListener(JSC::JSValue);

        typedef HashMap<JSC::JSObject*, JSEventListener*> JSListenersMap;

        JSListenersMap& jsEventListeners();

        // Make binding code generation easier.
        JSDOMGlobalObject* globalObject() { return this; }

        void setCurrentEvent(Event*);
        Event* currentEvent() const;

        virtual void markChildren(JSC::MarkStack&);

    protected:
        struct JSDOMGlobalObjectData : public JSC::JSGlobalObject::JSGlobalObjectData {
            JSDOMGlobalObjectData();

            JSDOMStructureMap structures;
            JSDOMConstructorMap constructors;

            JSDOMGlobalObject::JSListenersMap jsEventListeners;

            Event* evt;
        };

    private:
        JSDOMGlobalObjectData* d() const { return static_cast<JSDOMGlobalObjectData*>(JSC::JSVariableObject::d); }
    };

    template<class ConstructorClass>
    inline JSC::JSObject* getDOMConstructor(JSC::ExecState* exec, const JSDOMGlobalObject* globalObject)
    {
        if (JSC::JSObject* constructor = globalObject->constructors().get(&ConstructorClass::s_info))
            return constructor;
        JSC::JSObject* constructor = new (exec) ConstructorClass(exec, const_cast<JSDOMGlobalObject*>(globalObject));
        ASSERT(!globalObject->constructors().contains(&ConstructorClass::s_info));
        globalObject->constructors().set(&ConstructorClass::s_info, constructor);
        return constructor;
    }

    JSDOMGlobalObject* toJSDOMGlobalObject(Document*);
    JSDOMGlobalObject* toJSDOMGlobalObject(ScriptExecutionContext*);

} // namespace WebCore

#endif // JSDOMGlobalObject_h
