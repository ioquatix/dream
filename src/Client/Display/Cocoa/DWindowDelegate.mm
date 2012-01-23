//
//  Client/Display/Cocoa/DWindowDelegate.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 1/03/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#import "DWindowDelegate.h"
#import "DScreenManager.h"

@implementation DWindowDelegate

@synthesize inputHandler = _inputHandler, screenManager;

- (id)init {
    self = [super init];
    if (self) {
		screenManager = [DScreenManager new];
    }
    return self;
}

- (void)dealloc {
    [screenManager release];
	
    [super dealloc];
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	using namespace Dream::Client::Display;

	if (_inputHandler) {
		EventInput ipt(EventInput::EXIT);
		
		_inputHandler->process(ipt);				
	}
	
	return NSTerminateCancel;
}

- (BOOL)windowShouldClose:(id)sender
{
	using namespace Dream::Client::Display;
	
	if (_inputHandler) {
		EventInput ipt(EventInput::EXIT);
		
		_inputHandler->process(ipt);
	}
	
	return NO;
}

- (void) toggleFullScreen:(id)sender {
	using namespace Dream::Client::Display;
	
	if (_inputHandler) {
		NSView * contentView = [screenManager contentView];
		
		//NSSize oldSize = [contentView bounds].size;
		
		[screenManager toggleFullScreen:sender];
		
		NSSize newSize = [contentView bounds].size;
		
		ResizeInput resizeInput(Vec2u(newSize.width, newSize.height));		
		_inputHandler->process(resizeInput);
	}
}

@end
