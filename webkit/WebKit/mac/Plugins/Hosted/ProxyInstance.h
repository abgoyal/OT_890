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
 * Copyright (C) 2009 Apple Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#if USE(PLUGIN_HOST_PROCESS)

#ifndef ProxyInstance_h
#define ProxyInstance_h

#include <WebCore/runtime.h>
#include <WebCore/runtime_root.h>
#include <wtf/OwnPtr.h>
#include "WebKitPluginHostTypes.h"

namespace WebKit {

class ProxyClass;
class NetscapePluginInstanceProxy;
    
class ProxyInstance : public JSC::Bindings::Instance {
public:
    static PassRefPtr<ProxyInstance> create(PassRefPtr<JSC::Bindings::RootObject> rootObject, NetscapePluginInstanceProxy* instanceProxy, uint32_t objectID)
    {
        return adoptRef(new ProxyInstance(rootObject, instanceProxy, objectID));
    }
    ~ProxyInstance();

    JSC::Bindings::MethodList methodsNamed(const JSC::Identifier&);
    JSC::Bindings::Field* fieldNamed(const JSC::Identifier&);

    JSC::JSValue fieldValue(JSC::ExecState*, const JSC::Bindings::Field*) const;
    void setFieldValue(JSC::ExecState*, const JSC::Bindings::Field*, JSC::JSValue) const;
    
    void invalidate();
    
    uint32_t objectID() const { return m_objectID; }
    
private:
    ProxyInstance(PassRefPtr<JSC::Bindings::RootObject>, NetscapePluginInstanceProxy*, uint32_t objectID);
    
    virtual JSC::Bindings::Class *getClass() const;

    virtual JSC::JSValue invokeMethod(JSC::ExecState*, const JSC::Bindings::MethodList&, const JSC::ArgList& args);

    virtual bool supportsInvokeDefaultMethod() const;
    virtual JSC::JSValue invokeDefaultMethod(JSC::ExecState*, const JSC::ArgList&);

    virtual bool supportsConstruct() const;
    virtual JSC::JSValue invokeConstruct(JSC::ExecState*, const JSC::ArgList&);

    virtual JSC::JSValue defaultValue(JSC::ExecState*, JSC::PreferredPrimitiveType) const;
    virtual JSC::JSValue valueOf(JSC::ExecState*) const;
    
    virtual void getPropertyNames(JSC::ExecState*, JSC::PropertyNameArray&);

    JSC::JSValue stringValue(JSC::ExecState*) const;
    JSC::JSValue numberValue(JSC::ExecState*) const;
    JSC::JSValue booleanValue() const;
    
    JSC::JSValue invoke(JSC::ExecState*, InvokeType, uint64_t identifier, const JSC::ArgList& args);
    
    NetscapePluginInstanceProxy* m_instanceProxy;
    uint32_t m_objectID;
    JSC::Bindings::FieldMap m_fields;
    JSC::Bindings::MethodMap m_methods;
};
    
}

#endif // ProxyInstance_h
#endif // USE(PLUGIN_HOST_PROCESS)
