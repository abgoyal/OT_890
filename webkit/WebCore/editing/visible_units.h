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
 * Copyright (C) 2004 Apple Computer, Inc.  All rights reserved.
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

#ifndef visible_units_h
#define visible_units_h

#include "Document.h"
#include "TextAffinity.h"

namespace WebCore {

class VisiblePosition;

enum EWordSide { RightWordIfOnBoundary = false, LeftWordIfOnBoundary = true };

// words
VisiblePosition startOfWord(const VisiblePosition &, EWordSide = RightWordIfOnBoundary);
VisiblePosition endOfWord(const VisiblePosition &, EWordSide = RightWordIfOnBoundary);
VisiblePosition previousWordPosition(const VisiblePosition &);
VisiblePosition nextWordPosition(const VisiblePosition &);

// sentences
VisiblePosition startOfSentence(const VisiblePosition &);
VisiblePosition endOfSentence(const VisiblePosition &);
VisiblePosition previousSentencePosition(const VisiblePosition &);
VisiblePosition nextSentencePosition(const VisiblePosition &);

// lines
VisiblePosition startOfLine(const VisiblePosition &);
VisiblePosition endOfLine(const VisiblePosition &);
VisiblePosition previousLinePosition(const VisiblePosition &, int x);
VisiblePosition nextLinePosition(const VisiblePosition &, int x);
bool inSameLine(const VisiblePosition &, const VisiblePosition &);
bool inSameLogicalLine(const VisiblePosition &, const VisiblePosition &);
bool isStartOfLine(const VisiblePosition &);
bool isEndOfLine(const VisiblePosition &);
VisiblePosition logicalStartOfLine(const VisiblePosition &);
VisiblePosition logicalEndOfLine(const VisiblePosition &);

// paragraphs (perhaps a misnomer, can be divided by line break elements)
VisiblePosition startOfParagraph(const VisiblePosition&);
VisiblePosition endOfParagraph(const VisiblePosition&);
VisiblePosition startOfNextParagraph(const VisiblePosition&);
VisiblePosition previousParagraphPosition(const VisiblePosition &, int x);
VisiblePosition nextParagraphPosition(const VisiblePosition &, int x);
bool inSameParagraph(const VisiblePosition &, const VisiblePosition &);
bool isStartOfParagraph(const VisiblePosition &);
bool isEndOfParagraph(const VisiblePosition &);

// blocks (true paragraphs; line break elements don't break blocks)
VisiblePosition startOfBlock(const VisiblePosition &);
VisiblePosition endOfBlock(const VisiblePosition &);
bool inSameBlock(const VisiblePosition &, const VisiblePosition &);
bool isStartOfBlock(const VisiblePosition &);
bool isEndOfBlock(const VisiblePosition &);

// document
VisiblePosition startOfDocument(const Node*);
VisiblePosition endOfDocument(const Node*);
VisiblePosition startOfDocument(const VisiblePosition &);
VisiblePosition endOfDocument(const VisiblePosition &);
bool inSameDocument(const VisiblePosition &, const VisiblePosition &);
bool isStartOfDocument(const VisiblePosition &);
bool isEndOfDocument(const VisiblePosition &);

// editable content
VisiblePosition startOfEditableContent(const VisiblePosition&);
VisiblePosition endOfEditableContent(const VisiblePosition&);

} // namespace WebCore

#endif // VisiblePosition_h
