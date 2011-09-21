//
//  CocoaContextDelegate.h
//  Dream
//
//  Created by Samuel Williams on 1/03/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "DScreenManager.h"
#include "Context.h"

@class DScreenManager;

@interface DWindowDelegate : NSObject <NSWindowDelegate> {
	DScreenManager * screenManager;
	
	Dream::Client::Display::IInputHandler * _inputHandler;
}

@property(nonatomic,assign) Dream::Client::Display::IInputHandler * inputHandler;
@property(nonatomic,retain) DScreenManager * screenManager;

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender;
- (BOOL)windowShouldClose:(id)sender;

- (void)toggleFullScreen:(id)sender;

@end
