//
//  Client/Display/Cocoa/DWindowDelegate.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 1/03/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "DScreenManager.h"
#include "Context.h"

@class DScreenManager;

@interface DWindowDelegate : NSObject <NSWindowDelegate> {
	DScreenManager * _screen_manager;
	
	Dream::Client::Display::IInputHandler * _input_handler;
}

@property(nonatomic,assign) Dream::Client::Display::IInputHandler * inputHandler;
@property(nonatomic,retain) DScreenManager * screenManager;

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender;
- (BOOL)windowShouldClose:(id)sender;

- (void)toggleFullScreen:(id)sender;

@end
