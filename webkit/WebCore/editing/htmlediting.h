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
 * Copyright (C) 2004, 2006, 2008 Apple Inc. All rights reserved.
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

#ifndef htmlediting_h
#define htmlediting_h

#include <wtf/Forward.h>
#include "HTMLNames.h"
#include "ExceptionCode.h"

namespace WebCore {

class Document;
class Element;
class HTMLElement;
class Node;
class Position;
class Range;
class String;
class VisiblePosition;
class VisibleSelection;

Position rangeCompliantEquivalent(const Position&);
Position rangeCompliantEquivalent(const VisiblePosition&);
int lastOffsetForEditing(const Node*);
bool isAtomicNode(const Node*);
bool editingIgnoresContent(const Node*);
bool canHaveChildrenForEditing(const Node*);
Node* highestEditableRoot(const Position&);
VisiblePosition firstEditablePositionAfterPositionInRoot(const Position&, Node*);
VisiblePosition lastEditablePositionBeforePositionInRoot(const Position&, Node*);
int comparePositions(const Position&, const Position&);
int comparePositions(const VisiblePosition&, const VisiblePosition&);
Node* lowestEditableAncestor(Node*);
Position nextCandidate(const Position&);
Position nextVisuallyDistinctCandidate(const Position&);
Position previousCandidate(const Position&);
Position previousVisuallyDistinctCandidate(const Position&);
bool isEditablePosition(const Position&);
bool isRichlyEditablePosition(const Position&);
Element* editableRootForPosition(const Position&);
Element* unsplittableElementForPosition(const Position&);
bool isBlock(const Node*);
Node* enclosingBlock(Node*);

String stringWithRebalancedWhitespace(const String&, bool, bool);
const String& nonBreakingSpaceString();

//------------------------------------------------------------------------------------------

Position positionBeforeNode(const Node*);
Position positionAfterNode(const Node*);
VisiblePosition visiblePositionBeforeNode(Node*);
VisiblePosition visiblePositionAfterNode(Node*);
PassRefPtr<Range> createRange(PassRefPtr<Document>, const VisiblePosition& start, const VisiblePosition& end, ExceptionCode&);
PassRefPtr<Range> extendRangeToWrappingNodes(PassRefPtr<Range> rangeToExtend, const Range* maximumRange, const Node* rootNode);

PassRefPtr<Range> avoidIntersectionWithNode(const Range*, Node*);
VisibleSelection avoidIntersectionWithNode(const VisibleSelection&, Node*);

bool isSpecialElement(const Node*);
bool validBlockTag(const String&);

PassRefPtr<HTMLElement> createDefaultParagraphElement(Document*);
PassRefPtr<HTMLElement> createBreakElement(Document*);
PassRefPtr<HTMLElement> createOrderedListElement(Document*);
PassRefPtr<HTMLElement> createUnorderedListElement(Document*);
PassRefPtr<HTMLElement> createListItemElement(Document*);
PassRefPtr<HTMLElement> createHTMLElement(Document*, const QualifiedName&);
PassRefPtr<HTMLElement> createHTMLElement(Document*, const AtomicString&);

bool isTabSpanNode(const Node*);
bool isTabSpanTextNode(const Node*);
Node* tabSpanNode(const Node*);
Position positionBeforeTabSpan(const Position&);
PassRefPtr<Element> createTabSpanElement(Document*);
PassRefPtr<Element> createTabSpanElement(Document*, PassRefPtr<Node> tabTextNode);
PassRefPtr<Element> createTabSpanElement(Document*, const String& tabText);

bool isNodeRendered(const Node*);
bool isMailBlockquote(const Node*);
Node* nearestMailBlockquote(const Node*);
unsigned numEnclosingMailBlockquotes(const Position&);
int caretMinOffset(const Node*);
int caretMaxOffset(const Node*);

//------------------------------------------------------------------------------------------

bool isTableStructureNode(const Node*);
PassRefPtr<Element> createBlockPlaceholderElement(Document*);

bool isFirstVisiblePositionInSpecialElement(const Position&);
Position positionBeforeContainingSpecialElement(const Position&, Node** containingSpecialElement=0);
bool isLastVisiblePositionInSpecialElement(const Position&);
Position positionAfterContainingSpecialElement(const Position&, Node** containingSpecialElement=0);
Position positionOutsideContainingSpecialElement(const Position&, Node** containingSpecialElement=0);
Node* isLastPositionBeforeTable(const VisiblePosition&);
Node* isFirstPositionAfterTable(const VisiblePosition&);

Node* enclosingNodeWithTag(const Position&, const QualifiedName&);
Node* enclosingNodeOfType(const Position&, bool (*nodeIsOfType)(const Node*), bool onlyReturnEditableNodes = true);
Node* highestEnclosingNodeOfType(const Position&, bool (*nodeIsOfType)(const Node*));
Node* enclosingTableCell(const Position&);
Node* enclosingEmptyListItem(const VisiblePosition&);
Node* enclosingAnchorElement(const Position&);
bool isListElement(Node*);
HTMLElement* enclosingList(Node*);
HTMLElement* outermostEnclosingList(Node*);
HTMLElement* enclosingListChild(Node*);
bool canMergeLists(Element* firstList, Element* secondList);
Node* highestAncestor(Node*);
bool isTableElement(Node*);
bool isTableCell(const Node*);

bool lineBreakExistsAtPosition(const Position&);
bool lineBreakExistsAtVisiblePosition(const VisiblePosition&);

VisibleSelection selectionForParagraphIteration(const VisibleSelection&);

int indexForVisiblePosition(const VisiblePosition&);
bool isVisiblyAdjacent(const Position& first, const Position& second);
bool isNodeVisiblyContainedWithin(Node*, const Range*);
}

#endif
