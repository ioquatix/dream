//
//  Client/Display/Cocoa/DOpenGLView.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 14/09/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#import <AppKit/AppKit.h>
#include "Context.h"

#include "../../../Events/MultiFingerInput.h"

@interface DOpenGLView : NSOpenGLView {
	Dream::Client::Display::Cocoa::ViewContext * _display_context;

	Dream::Client::Display::MultiFingerInput * _multi_finger_input;

	BOOL _skip_next_motion_event;
}

@property (nonatomic, assign) Dream::Client::Display::Cocoa::ViewContext * displayContext;

- (void) warpCursorToCenter;

- (BOOL) handleEvent:(NSEvent *)event;

@end
