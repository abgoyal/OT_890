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
 * Copyright (C) 2008 Apple Inc. All rights reserved.
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

#ifndef JITStubCall_h
#define JITStubCall_h

#include <wtf/Platform.h>

#if ENABLE(JIT)

namespace JSC {

    class JITStubCall {
    public:
        JITStubCall(JIT* jit, JSObject* (JIT_STUB *stub)(STUB_ARGS_DECLARATION))
            : m_jit(jit)
            , m_stub(reinterpret_cast<void*>(stub))
            , m_returnType(Cell)
            , m_stackIndex(stackIndexStart)
        {
        }

        JITStubCall(JIT* jit, JSPropertyNameIterator* (JIT_STUB *stub)(STUB_ARGS_DECLARATION))
            : m_jit(jit)
            , m_stub(reinterpret_cast<void*>(stub))
            , m_returnType(Cell)
            , m_stackIndex(stackIndexStart)
        {
        }

        JITStubCall(JIT* jit, void* (JIT_STUB *stub)(STUB_ARGS_DECLARATION))
            : m_jit(jit)
            , m_stub(reinterpret_cast<void*>(stub))
            , m_returnType(VoidPtr)
            , m_stackIndex(stackIndexStart)
        {
        }

        JITStubCall(JIT* jit, int (JIT_STUB *stub)(STUB_ARGS_DECLARATION))
            : m_jit(jit)
            , m_stub(reinterpret_cast<void*>(stub))
            , m_returnType(Int)
            , m_stackIndex(stackIndexStart)
        {
        }

        JITStubCall(JIT* jit, bool (JIT_STUB *stub)(STUB_ARGS_DECLARATION))
            : m_jit(jit)
            , m_stub(reinterpret_cast<void*>(stub))
            , m_returnType(Int)
            , m_stackIndex(stackIndexStart)
        {
        }

        JITStubCall(JIT* jit, void (JIT_STUB *stub)(STUB_ARGS_DECLARATION))
            : m_jit(jit)
            , m_stub(reinterpret_cast<void*>(stub))
            , m_returnType(Void)
            , m_stackIndex(stackIndexStart)
        {
        }

#if USE(JSVALUE32_64)
        JITStubCall(JIT* jit, EncodedJSValue (JIT_STUB *stub)(STUB_ARGS_DECLARATION))
            : m_jit(jit)
            , m_stub(reinterpret_cast<void*>(stub))
            , m_returnType(Value)
            , m_stackIndex(stackIndexStart)
        {
        }
#endif

        // Arguments are added first to last.

        void skipArgument()
        {
            m_stackIndex += stackIndexStep;
        }

        void addArgument(JIT::Imm32 argument)
        {
            m_jit->poke(argument, m_stackIndex);
            m_stackIndex += stackIndexStep;
        }

        void addArgument(JIT::ImmPtr argument)
        {
            m_jit->poke(argument, m_stackIndex);
            m_stackIndex += stackIndexStep;
        }

        void addArgument(JIT::RegisterID argument)
        {
            m_jit->poke(argument, m_stackIndex);
            m_stackIndex += stackIndexStep;
        }

        void addArgument(const JSValue& value)
        {
            m_jit->poke(JIT::Imm32(value.payload()), m_stackIndex);
            m_jit->poke(JIT::Imm32(value.tag()), m_stackIndex + 1);
            m_stackIndex += stackIndexStep;
        }

        void addArgument(JIT::RegisterID tag, JIT::RegisterID payload)
        {
            m_jit->poke(payload, m_stackIndex);
            m_jit->poke(tag, m_stackIndex + 1);
            m_stackIndex += stackIndexStep;
        }

#if USE(JSVALUE32_64)
        void addArgument(unsigned srcVirtualRegister)
        {
            if (m_jit->m_codeBlock->isConstantRegisterIndex(srcVirtualRegister)) {
                addArgument(m_jit->getConstantOperand(srcVirtualRegister));
                return;
            }

            m_jit->emitLoad(srcVirtualRegister, JIT::regT1, JIT::regT0);
            addArgument(JIT::regT1, JIT::regT0);
        }

        void getArgument(size_t argumentNumber, JIT::RegisterID tag, JIT::RegisterID payload)
        {
            size_t stackIndex = stackIndexStart + (argumentNumber * stackIndexStep);
            m_jit->peek(payload, stackIndex);
            m_jit->peek(tag, stackIndex + 1);
        }
#else
        void addArgument(unsigned src, JIT::RegisterID scratchRegister) // src is a virtual register.
        {
            if (m_jit->m_codeBlock->isConstantRegisterIndex(src))
                addArgument(JIT::ImmPtr(JSValue::encode(m_jit->m_codeBlock->getConstant(src))));
            else {
                m_jit->loadPtr(JIT::Address(JIT::callFrameRegister, src * sizeof(Register)), scratchRegister);
                addArgument(scratchRegister);
            }
            m_jit->killLastResultRegister();
        }
#endif

        JIT::Call call()
        {
#if ENABLE(OPCODE_SAMPLING)
            if (m_jit->m_bytecodeIndex != (unsigned)-1)
                m_jit->sampleInstruction(m_jit->m_codeBlock->instructions().begin() + m_jit->m_bytecodeIndex, true);
#endif

            m_jit->restoreArgumentReference();
            JIT::Call call = m_jit->call();
            m_jit->m_calls.append(CallRecord(call, m_jit->m_bytecodeIndex, m_stub));

#if ENABLE(OPCODE_SAMPLING)
            if (m_jit->m_bytecodeIndex != (unsigned)-1)
                m_jit->sampleInstruction(m_jit->m_codeBlock->instructions().begin() + m_jit->m_bytecodeIndex, false);
#endif

#if USE(JSVALUE32_64)
            m_jit->unmap();
#else
            m_jit->killLastResultRegister();
#endif
            return call;
        }

#if USE(JSVALUE32_64)
        JIT::Call call(unsigned dst) // dst is a virtual register.
        {
            ASSERT(m_returnType == Value || m_returnType == Cell);
            JIT::Call call = this->call();
            if (m_returnType == Value)
                m_jit->emitStore(dst, JIT::regT1, JIT::regT0);
            else
                m_jit->emitStoreCell(dst, JIT::returnValueRegister);
            return call;
        }
#else
        JIT::Call call(unsigned dst) // dst is a virtual register.
        {
            ASSERT(m_returnType == VoidPtr || m_returnType == Cell);
            JIT::Call call = this->call();
            m_jit->emitPutVirtualRegister(dst);
            return call;
        }
#endif

        JIT::Call call(JIT::RegisterID dst) // dst is a machine register.
        {
#if USE(JSVALUE32_64)
            ASSERT(m_returnType == Value || m_returnType == VoidPtr || m_returnType == Int || m_returnType == Cell);
#else
            ASSERT(m_returnType == VoidPtr || m_returnType == Int || m_returnType == Cell);
#endif
            JIT::Call call = this->call();
            if (dst != JIT::returnValueRegister)
                m_jit->move(JIT::returnValueRegister, dst);
            return call;
        }

    private:
        static const size_t stackIndexStep = sizeof(EncodedJSValue) == 2 * sizeof(void*) ? 2 : 1;
        static const size_t stackIndexStart = 1; // Index 0 is reserved for restoreArgumentReference().

        JIT* m_jit;
        void* m_stub;
        enum { Void, VoidPtr, Int, Value, Cell } m_returnType;
        size_t m_stackIndex;
    };
}

#endif // ENABLE(JIT)

#endif // JITStubCall_h
