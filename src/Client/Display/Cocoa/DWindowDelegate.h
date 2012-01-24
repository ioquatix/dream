//
//  Client/Display/Cocoa/DWindowDelegate.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 1/03/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "Context.h"

@class DScreenManager;

@interface DWindowDelegate : NSObject <NSWindowDelegate> {
	Dream::Client::Display::IInputHandler * _input_handler;
}

@property(nonatomic,assign) Dream::Client::Display::IInputHandler * inputHandler;

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender;
- (BOOL)windowShouldClose:(id)sender;

/*
- (void)windowWillEnterFullScreen:(NSNotification *)notification;
- (void)windowDidEnterFullScreen:(NSNotification *)notification;
- (void)windowWillExitFullScreen:(NSNotification *)notification;
- (void)windowDidExitFullScreen:(NSNotification *)notification;
*/

@end
