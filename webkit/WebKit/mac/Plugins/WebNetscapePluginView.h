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
 * Copyright (C) 2005, 2007 Apple Inc. All rights reserved.
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

#if ENABLE(NETSCAPE_PLUGIN_API)

#import "WebBaseNetscapePluginView.h"

#import "WebNetscapeContainerCheckPrivate.h"
#import <WebKit/npfunctions.h>
#import <WebKit/npapi.h>
#import <wtf/HashMap.h>
#import <wtf/HashSet.h>
#import <wtf/OwnPtr.h>

@class WebDataSource;
@class WebFrame;
@class WebNetscapePluginPackage;
@class WebView;

class PluginTimer;
class WebNetscapePluginStream;
class WebNetscapePluginEventHandler;

typedef union PluginPort {
#ifndef NP_NO_QUICKDRAW
    NP_Port qdPort;
#endif        
    NP_CGContext cgPort;
} PluginPort;

// Because the Adobe 7.x Acrobat plug-in has a hard coded check for a view named 
// "WebNetscapePluginDocumentView", this class must retain the old name in order 
// for the plug-in to function correctly. (rdar://problem/4699455)
#define WebNetscapePluginView WebNetscapePluginDocumentView

@interface WebNetscapePluginView : WebBaseNetscapePluginView<WebPluginManualLoader, WebPluginContainerCheckController>
{
    RefPtr<WebNetscapePluginStream> _manualStream;
#ifndef BUILDING_ON_TIGER
    RetainPtr<CALayer> _pluginLayer;
#endif
    unsigned _dataLengthReceived;
    RetainPtr<NSError> _error;
        
    unsigned argsCount;
    char **cAttributes;
    char **cValues;
        
    NPP plugin;
    NPWindow window;
    NPWindow lastSetWindow;
    PluginPort nPort;
    PluginPort lastSetPort;
    NPDrawingModel drawingModel;
    NPEventModel eventModel;
    
#ifndef NP_NO_QUICKDRAW
    // This is only valid when drawingModel is NPDrawingModelQuickDraw
    GWorldPtr offscreenGWorld;
#endif

    OwnPtr<WebNetscapePluginEventHandler> _eventHandler;
    
    BOOL inSetWindow;
    BOOL shouldStopSoon;

    uint32 currentTimerID;
    HashMap<uint32, PluginTimer*>* timers;

    unsigned pluginFunctionCallDepth;
    
    int32 specifiedHeight;
    int32 specifiedWidth;
            
    HashSet<RefPtr<WebNetscapePluginStream> > streams;
    RetainPtr<NSMutableDictionary> _pendingFrameLoads;
    
    BOOL _isFlash;
    BOOL _isSilverlight;
    
    NSMutableDictionary *_containerChecksInProgress;
    uint32 _currentContainerCheckRequestID;
}

+ (WebNetscapePluginView *)currentPluginView;


- (id)initWithFrame:(NSRect)r
      pluginPackage:(WebNetscapePluginPackage *)thePluginPackage
                URL:(NSURL *)URL
            baseURL:(NSURL *)baseURL
           MIMEType:(NSString *)MIME
      attributeKeys:(NSArray *)keys
    attributeValues:(NSArray *)values
       loadManually:(BOOL)loadManually
            element:(PassRefPtr<WebCore::HTMLPlugInElement>)element;


- (NPP)plugin;

- (void)disconnectStream:(WebNetscapePluginStream*)stream;

// Returns the NPObject that represents the plugin interface.
// The return value is expected to be retained.
- (NPObject *)createPluginScriptableObject;

// -willCallPlugInFunction must be called before calling any of the NPP_* functions for this view's plugin.
// This is necessary to ensure that plug-ins are not destroyed while WebKit calls into them.  Some plug-ins (Flash
// at least) are written with the assumption that nothing they do in their plug-in functions can cause NPP_Destroy()
// to be called.  Unfortunately, this is not true, especially if the plug-in uses NPN_Invoke() to execute a
// document.write(), which clears the document and destroys the plug-in.
// See <rdar://problem/4480737>.
- (void)willCallPlugInFunction;

// -didCallPlugInFunction should be called after returning from a plug-in function.  It should be called exactly
// once for every call to -willCallPlugInFunction.
// See <rdar://problem/4480737>.
- (void)didCallPlugInFunction;

- (void)handleMouseMoved:(NSEvent *)event;
- (uint32)checkIfAllowedToLoadURL:(const char*)urlCString frame:(const char*)frameNameCString callbackFunc:(void (*)(NPP npp, uint32 checkID, NPBool allowed, void* context))callbackFunc context:(void*)context;
- (void)cancelCheckIfAllowedToLoadURL:(uint32)checkID;

@end

@interface WebNetscapePluginView (WebInternal)
- (BOOL)sendEvent:(void*)event isDrawRect:(BOOL)eventIsDrawRect;
- (NPEventModel)eventModel;

- (NPError)loadRequest:(NSURLRequest *)request inTarget:(NSString *)target withNotifyData:(void *)notifyData sendNotification:(BOOL)sendNotification;
- (NPError)getURLNotify:(const char *)URL target:(const char *)target notifyData:(void *)notifyData;
- (NPError)getURL:(const char *)URL target:(const char *)target;
- (NPError)postURLNotify:(const char *)URL target:(const char *)target len:(UInt32)len buf:(const char *)buf file:(NPBool)file notifyData:(void *)notifyData;
- (NPError)postURL:(const char *)URL target:(const char *)target len:(UInt32)len buf:(const char *)buf file:(NPBool)file;
- (NPError)newStream:(NPMIMEType)type target:(const char *)target stream:(NPStream**)stream;
- (NPError)write:(NPStream*)stream len:(SInt32)len buffer:(void *)buffer;
- (NPError)destroyStream:(NPStream*)stream reason:(NPReason)reason;
- (void)status:(const char *)message;
- (const char *)userAgent;
- (void)invalidateRect:(NPRect *)invalidRect;
- (void)invalidateRegion:(NPRegion)invalidateRegion;
- (void)forceRedraw;
- (NPError)getVariable:(NPNVariable)variable value:(void *)value;
- (NPError)setVariable:(NPPVariable)variable value:(void *)value;
- (uint32)scheduleTimerWithInterval:(uint32)interval repeat:(NPBool)repeat timerFunc:(void (*)(NPP npp, uint32 timerID))timerFunc;
- (void)unscheduleTimer:(uint32)timerID;
- (NPError)popUpContextMenu:(NPMenu *)menu;
- (NPError)getVariable:(NPNURLVariable)variable forURL:(const char*)url value:(char**)value length:(uint32*)length;
- (NPError)setVariable:(NPNURLVariable)variable forURL:(const char*)url value:(const char*)value length:(uint32)length;
- (NPError)getAuthenticationInfoWithProtocol:(const char*) protocol host:(const char*)host port:(int32)port scheme:(const char*)scheme realm:(const char*)realm
                                    username:(char**)username usernameLength:(uint32*)usernameLength 
                                    password:(char**)password passwordLength:(uint32*)passwordLength;
- (char*)resolveURL:(const char*)url forTarget:(const char*)target;
@end

WKNBrowserContainerCheckFuncs *browserContainerCheckFuncs();

#endif

