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

#ifndef V8_REGEXP_MACRO_ASSEMBLER_IRREGEXP_H_
#define V8_REGEXP_MACRO_ASSEMBLER_IRREGEXP_H_

namespace v8 {
namespace internal {

#ifndef V8_NATIVE_REGEXP

class RegExpMacroAssemblerIrregexp: public RegExpMacroAssembler {
 public:
  // Create an assembler. Instructions and relocation information are emitted
  // into a buffer, with the instructions starting from the beginning and the
  // relocation information starting from the end of the buffer. See CodeDesc
  // for a detailed comment on the layout (globals.h).
  //
  // If the provided buffer is NULL, the assembler allocates and grows its own
  // buffer, and buffer_size determines the initial buffer size. The buffer is
  // owned by the assembler and deallocated upon destruction of the assembler.
  //
  // If the provided buffer is not NULL, the assembler uses the provided buffer
  // for code generation and assumes its size to be buffer_size. If the buffer
  // is too small, a fatal error occurs. No deallocation of the buffer is done
  // upon destruction of the assembler.
  explicit RegExpMacroAssemblerIrregexp(Vector<byte>);
  virtual ~RegExpMacroAssemblerIrregexp();
  // The byte-code interpreter checks on each push anyway.
  virtual int stack_limit_slack() { return 1; }
  virtual void Bind(Label* label);
  virtual void AdvanceCurrentPosition(int by);  // Signed cp change.
  virtual void PopCurrentPosition();
  virtual void PushCurrentPosition();
  virtual void Backtrack();
  virtual void GoTo(Label* label);
  virtual void PushBacktrack(Label* label);
  virtual void Succeed();
  virtual void Fail();
  virtual void PopRegister(int register_index);
  virtual void PushRegister(int register_index,
                            StackCheckFlag check_stack_limit);
  virtual void AdvanceRegister(int reg, int by);  // r[reg] += by.
  virtual void SetRegister(int register_index, int to);
  virtual void WriteCurrentPositionToRegister(int reg, int cp_offset);
  virtual void ClearRegisters(int reg_from, int reg_to);
  virtual void ReadCurrentPositionFromRegister(int reg);
  virtual void WriteStackPointerToRegister(int reg);
  virtual void ReadStackPointerFromRegister(int reg);
  virtual void LoadCurrentCharacter(int cp_offset,
                                    Label* on_end_of_input,
                                    bool check_bounds = true,
                                    int characters = 1);
  virtual void CheckCharacter(uint32_t c, Label* on_equal);
  virtual void CheckCharacterAfterAnd(uint32_t c,
                                      uint32_t mask,
                                      Label* on_equal);
  virtual void CheckCharacterGT(uc16 limit, Label* on_greater);
  virtual void CheckCharacterLT(uc16 limit, Label* on_less);
  virtual void CheckGreedyLoop(Label* on_tos_equals_current_position);
  virtual void CheckAtStart(Label* on_at_start);
  virtual void CheckNotAtStart(Label* on_not_at_start);
  virtual void CheckNotCharacter(uint32_t c, Label* on_not_equal);
  virtual void CheckNotCharacterAfterAnd(uint32_t c,
                                         uint32_t mask,
                                         Label* on_not_equal);
  virtual void CheckNotCharacterAfterMinusAnd(uc16 c,
                                              uc16 minus,
                                              uc16 mask,
                                              Label* on_not_equal);
  virtual void CheckNotBackReference(int start_reg, Label* on_no_match);
  virtual void CheckNotBackReferenceIgnoreCase(int start_reg,
                                               Label* on_no_match);
  virtual void CheckNotRegistersEqual(int reg1, int reg2, Label* on_not_equal);
  virtual void CheckCharacters(Vector<const uc16> str,
                               int cp_offset,
                               Label* on_failure,
                               bool check_end_of_string);
  virtual void IfRegisterLT(int register_index, int comparand, Label* if_lt);
  virtual void IfRegisterGE(int register_index, int comparand, Label* if_ge);
  virtual void IfRegisterEqPos(int register_index, Label* if_eq);

  virtual IrregexpImplementation Implementation();
  virtual Handle<Object> GetCode(Handle<String> source);
 private:
  void Expand();
  // Code and bitmap emission.
  inline void EmitOrLink(Label* label);
  inline void Emit32(uint32_t x);
  inline void Emit16(uint32_t x);
  inline void Emit(uint32_t bc, uint32_t arg);
  // Bytecode buffer.
  int length();
  void Copy(Address a);

  // The buffer into which code and relocation info are generated.
  Vector<byte> buffer_;
  // The program counter.
  int pc_;
  // True if the assembler owns the buffer, false if buffer is external.
  bool own_buffer_;
  Label backtrack_;

  int advance_current_start_;
  int advance_current_offset_;
  int advance_current_end_;

  static const int kInvalidPC = -1;

  DISALLOW_IMPLICIT_CONSTRUCTORS(RegExpMacroAssemblerIrregexp);
};

#endif  // !V8_NATIVE_REGEXP

} }  // namespace v8::internal

#endif  // V8_REGEXP_MACRO_ASSEMBLER_IRREGEXP_H_
