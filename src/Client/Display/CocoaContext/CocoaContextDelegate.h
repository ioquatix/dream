//
//  CocoaContextDelegate.h
//  Dream
//
//  Created by Samuel Williams on 1/03/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "../Input.h"

@interface CocoaContextDelegate : NSObject <NSWindowDelegate> {
@private
    Dream::Client::Display::IInputHandler * _inputHandler;
}

@property(assign) Dream::Client::Display::IInputHandler * inputHandler;

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender;

- (BOOL)windowShouldClose:(id)sender;
- (NSSize)windowWillResize:(NSWindow *)sender toSize:(NSSize)frameSize;

//- (BOOL)windowShouldZoom:(NSWindow *)window toFrame:(NSRect)newFrame;

//- (void)windowDidBecomeKey:(NSNotification *)notification;
//- (void)windowDidResignKey:(NSNotification *)notification;
//- (void)windowDidBecomeMain:(NSNotification *)notification;
//- (void)windowDidResignMain:(NSNotification *)notification;

@end
