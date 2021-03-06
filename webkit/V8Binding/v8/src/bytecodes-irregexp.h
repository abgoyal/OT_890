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

// Copyright 2008-2009 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#ifndef V8_BYTECODES_IRREGEXP_H_
#define V8_BYTECODES_IRREGEXP_H_

namespace v8 {
namespace internal {


static const int BYTECODE_MASK = 0xff;
// The first argument is packed in with the byte code in one word, but so it
// has 24 bits, but it can be positive and negative so only use 23 bits for
// positive values.
static const unsigned int MAX_FIRST_ARG = 0x7fffffu;
static const int BYTECODE_SHIFT = 8;

#define BYTECODE_ITERATOR(V)                                                   \
V(BREAK,              0, 4)   /* bc8                                        */ \
V(PUSH_CP,            1, 4)   /* bc8 pad24                                  */ \
V(PUSH_BT,            2, 8)   /* bc8 pad24 offset32                         */ \
V(PUSH_REGISTER,      3, 4)   /* bc8 reg_idx24                              */ \
V(SET_REGISTER_TO_CP, 4, 8)   /* bc8 reg_idx24 offset32                     */ \
V(SET_CP_TO_REGISTER, 5, 4)   /* bc8 reg_idx24                              */ \
V(SET_REGISTER_TO_SP, 6, 4)   /* bc8 reg_idx24                              */ \
V(SET_SP_TO_REGISTER, 7, 4)   /* bc8 reg_idx24                              */ \
V(SET_REGISTER,       8, 8)   /* bc8 reg_idx24 value32                      */ \
V(ADVANCE_REGISTER,   9, 8)   /* bc8 reg_idx24 value32                      */ \
V(POP_CP,            10, 4)   /* bc8 pad24                                  */ \
V(POP_BT,            11, 4)   /* bc8 pad24                                  */ \
V(POP_REGISTER,      12, 4)   /* bc8 reg_idx24                              */ \
V(FAIL,              13, 4)   /* bc8 pad24                                  */ \
V(SUCCEED,           14, 4)   /* bc8 pad24                                  */ \
V(ADVANCE_CP,        15, 4)   /* bc8 offset24                               */ \
V(GOTO,              16, 8)   /* bc8 pad24 addr32                           */ \
V(LOAD_CURRENT_CHAR, 17, 8)   /* bc8 offset24 addr32                        */ \
V(LOAD_CURRENT_CHAR_UNCHECKED, 18, 4) /* bc8 offset24                       */ \
V(LOAD_2_CURRENT_CHARS, 19, 8) /* bc8 offset24 addr32                       */ \
V(LOAD_2_CURRENT_CHARS_UNCHECKED, 20, 4) /* bc8 offset24                    */ \
V(LOAD_4_CURRENT_CHARS, 21, 8) /* bc8 offset24 addr32                       */ \
V(LOAD_4_CURRENT_CHARS_UNCHECKED, 22, 4) /* bc8 offset24                    */ \
V(CHECK_4_CHARS,     23, 12)  /* bc8 pad24 uint32 addr32                    */ \
V(CHECK_CHAR,        24, 8)   /* bc8 pad8 uint16 addr32                     */ \
V(CHECK_NOT_4_CHARS, 25, 12)  /* bc8 pad24 uint32 addr32                    */ \
V(CHECK_NOT_CHAR,    26, 8)   /* bc8 pad8 uint16 addr32                     */ \
V(AND_CHECK_4_CHARS, 27, 16)  /* bc8 pad24 uint32 uint32 addr32             */ \
V(AND_CHECK_CHAR,    28, 12)  /* bc8 pad8 uint16 uint32 addr32              */ \
V(AND_CHECK_NOT_4_CHARS, 29, 16) /* bc8 pad24 uint32 uint32 addr32          */ \
V(AND_CHECK_NOT_CHAR, 30, 12) /* bc8 pad8 uint16 uint32 addr32              */ \
V(MINUS_AND_CHECK_NOT_CHAR, 31, 12) /* bc8 pad8 uc16 uc16 addr32            */ \
V(CHECK_LT,          32, 8)   /* bc8 pad8 uc16 addr32                       */ \
V(CHECK_GT,          33, 8)   /* bc8 pad8 uc16 addr32                       */ \
V(CHECK_NOT_BACK_REF, 34, 8)  /* bc8 reg_idx24 addr32                       */ \
V(CHECK_NOT_BACK_REF_NO_CASE, 35, 8) /* bc8 reg_idx24 addr32                */ \
V(CHECK_NOT_REGS_EQUAL, 36, 12) /* bc8 regidx24 reg_idx32 addr32            */ \
V(LOOKUP_MAP1,       37, 12)  /* bc8 pad8 start16 bit_map_addr32 addr32     */ \
V(LOOKUP_MAP2,       38, 96)  /* bc8 pad8 start16 half_nibble_map_addr32*   */ \
V(LOOKUP_MAP8,       39, 96)  /* bc8 pad8  start16 byte_map addr32*         */ \
V(LOOKUP_HI_MAP8,    40, 96)  /* bc8 start24 byte_map_addr32 addr32*        */ \
V(CHECK_REGISTER_LT, 41, 12)  /* bc8 reg_idx24 value32 addr32               */ \
V(CHECK_REGISTER_GE, 42, 12)  /* bc8 reg_idx24 value32 addr32               */ \
V(CHECK_REGISTER_EQ_POS, 43, 8) /* bc8 reg_idx24 addr32                     */ \
V(CHECK_AT_START,    44, 8)   /* bc8 pad24 addr32                           */ \
V(CHECK_NOT_AT_START, 45, 8)  /* bc8 pad24 addr32                           */ \
V(CHECK_GREEDY,      46, 8)   /* bc8 pad24 addr32                           */ \
V(ADVANCE_CP_AND_GOTO, 47, 8) /* bc8 offset24 addr32                        */

#define DECLARE_BYTECODES(name, code, length) \
  static const int BC_##name = code;
BYTECODE_ITERATOR(DECLARE_BYTECODES)
#undef DECLARE_BYTECODES

#define DECLARE_BYTECODE_LENGTH(name, code, length) \
  static const int BC_##name##_LENGTH = length;
BYTECODE_ITERATOR(DECLARE_BYTECODE_LENGTH)
#undef DECLARE_BYTECODE_LENGTH
} }

#endif  // V8_BYTECODES_IRREGEXP_H_
