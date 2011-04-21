//
//  CocoaContextDelegate.m
//  Dream
//
//  Created by Samuel Williams on 1/03/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

#import "CocoaContextDelegate.h"


@implementation CocoaContextDelegate

@synthesize inputHandler = _inputHandler;

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	using namespace Dream::Client::Display;

	if (_inputHandler) {
		EventInput ipt(EventInput::EXIT);
		
		_inputHandler->event(ipt);				
	}
	
	return NSTerminateCancel;
}

- (BOOL)windowShouldClose:(id)sender
{
	using namespace Dream::Client::Display;
	
	if (_inputHandler) {
		EventInput ipt(EventInput::EXIT);
		
		_inputHandler->event(ipt);
	}
	
	return NO;
}

- (NSSize)windowWillResize:(NSWindow *)sender toSize:(NSSize)frameSize
{
	using namespace Dream::Client::Display;
	
	if (_inputHandler) {
		Vec2u oldSize, newSize;
		
		NSSize windowSize = [sender frame].size;
		
		oldSize[0] = windowSize.width;
		oldSize[1] = windowSize.height;
		
		newSize[0] = frameSize.width;
		newSize[1] = frameSize.height;
		
		ResizeInput ipt(oldSize, newSize);
		
		_inputHandler->resize(ipt);
	}
	
	return frameSize;
}

@end
