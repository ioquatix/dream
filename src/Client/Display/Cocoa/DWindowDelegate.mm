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

@synthesize inputHandler = _input_handler, screenManager = _screen_manager;

- (id)init {
    self = [super init];
    if (self) {
		_screen_manager = [DScreenManager new];
    }
    return self;
}

- (void)dealloc {
    [_screen_manager release];
	
    [super dealloc];
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	using namespace Dream::Client::Display;

	if (_input_handler) {
		EventInput ipt(EventInput::EXIT);
		
		_input_handler->process(ipt);				
	}
	
	return NSTerminateCancel;
}

- (BOOL)windowShouldClose:(id)sender
{
	using namespace Dream::Client::Display;
	
	if (_input_handler) {
		EventInput ipt(EventInput::EXIT);
		
		_input_handler->process(ipt);
	}
	
	return NO;
}

- (void) toggleFullScreen:(id)sender {
	using namespace Dream::Client::Display;
	
	if (_input_handler) {
		NSView * contentView = [_screen_manager contentView];
		
		//NSSize oldSize = [contentView bounds].size;
		
		[_screen_manager toggleFullScreen:sender];
		
		NSSize newSize = [contentView bounds].size;
		
		ResizeInput resizeInput(Vec2u(newSize.width, newSize.height));		
		_input_handler->process(resizeInput);
	}
}

@end
