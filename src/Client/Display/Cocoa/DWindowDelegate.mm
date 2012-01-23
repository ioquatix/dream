//
//  Client/Display/Cocoa/DWindowDelegate.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 1/03/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#import "DWindowDelegate.h"

@implementation DWindowDelegate

@synthesize inputHandler = _input_handler;

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


@end
