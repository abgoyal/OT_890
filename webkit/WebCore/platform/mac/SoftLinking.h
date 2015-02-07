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

#import <wtf/Assertions.h>
#import <dlfcn.h>

#define SOFT_LINK_LIBRARY(lib) \
    static void* lib##Library() \
    { \
        static void* dylib = dlopen("/usr/lib/" #lib ".dylib", RTLD_NOW); \
        ASSERT(dylib); \
        return dylib; \
    }

#define SOFT_LINK_FRAMEWORK(framework) \
    static void* framework##Library() \
    { \
        static void* frameworkLibrary = dlopen("/System/Library/Frameworks/" #framework ".framework/" #framework, RTLD_NOW); \
        ASSERT(frameworkLibrary); \
        return frameworkLibrary; \
    }

#define SOFT_LINK(framework, functionName, resultType, parameterDeclarations, parameterNames) \
    static resultType init##functionName parameterDeclarations; \
    static resultType (*softLink##functionName) parameterDeclarations = init##functionName; \
    \
    static resultType init##functionName parameterDeclarations \
    { \
        softLink##functionName = (resultType (*) parameterDeclarations) dlsym(framework##Library(), #functionName); \
        ASSERT(softLink##functionName); \
        return softLink##functionName parameterNames; \
    }\
    \
    inline resultType functionName parameterDeclarations \
    {\
        return softLink##functionName parameterNames; \
    }

#define SOFT_LINK_CLASS(framework, className) \
    static Class init##className(); \
    static Class (*get##className##Class)() = init##className; \
    static Class class##className; \
    \
    static Class className##Function() \
    { \
        return class##className; \
    }\
    \
    static Class init##className() \
    { \
        framework##Library(); \
        class##className = objc_getClass(#className); \
        ASSERT(class##className); \
        get##className##Class = className##Function; \
        return class##className; \
    }

#define SOFT_LINK_POINTER(framework, name, type) \
    static type init##name(); \
    static type (*get##name)() = init##name; \
    static type pointer##name; \
    \
    static type name##Function() \
    { \
        return pointer##name; \
    }\
    \
    static type init##name() \
    { \
        void** pointer = static_cast<void**>(dlsym(framework##Library(), #name)); \
        ASSERT(pointer); \
        pointer##name = static_cast<type>(*pointer); \
        get##name = name##Function; \
        return pointer##name; \
    }

#define SOFT_LINK_CONSTANT(framework, name, type) \
    static type init##name(); \
    static type (*get##name)() = init##name; \
    static type constant##name; \
    \
    static type name##Function() \
    { \
        return constant##name; \
    }\
    \
    static type init##name() \
    { \
        void* constant = dlsym(framework##Library(), #name); \
        ASSERT(constant); \
        constant##name = *static_cast<type*>(constant); \
        get##name = name##Function; \
        return constant##name; \
    }
