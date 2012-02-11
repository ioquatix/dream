//
//  Client/Display/Cocoa/DWindowDelegate.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 1/03/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#import "DWindowDelegate.h"

@implementation DWindowDelegate

@synthesize displayContext = _window_context;

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	using namespace Dream::Client::Display;

	if (_window_context) {
		EventInput ipt(EventInput::EXIT);
		
		_window_context->process(ipt);				
	}
	
	return NSTerminateCancel;
}

- (BOOL)windowShouldClose:(id)sender
{
	using namespace Dream::Client::Display;
	
	if (_window_context) {
		EventInput ipt(EventInput::EXIT);
		
		_window_context->process(ipt);
	}
	
	return NO;
}

- (void)windowWillClose:(NSNotification *)notification {
	_window_context->stop();
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
	
}

- (void)windowDidResignKey:(NSNotification *)notification {
	NSWindow * window = (NSWindow *)[notification object];
	
	[window makeFirstResponder:window];
}

@end
