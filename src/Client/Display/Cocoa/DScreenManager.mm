//
//  Client/Display/Cocoa/DScreenManager.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 8/06/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#import "DScreenManager.h"

@implementation DScreenManager

@synthesize partialScreenWindow = _partial_screen_window, fullScreenWindow = _full_screen_window;
@dynamic mainScreen, fullScreen, contentView;

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (NSView*)contentView {
	if (_full_screen_window) {
		return [_full_screen_window contentView];
	} else {
		return [_partial_screen_window contentView];
	}
}

- (NSScreen*) mainScreen {
	return [[_main_screen retain] autorelease];
}

- (void) setMainScreen:(NSScreen *)main_screen {
	// If both nil or both the same pointer - object is identical
	if (_main_screen == main_screen) {
		return;
	}
	
	[_main_screen release];
	_main_screen = [main_screen retain];
	
	// Cycle full screen because the screen may have changed - this could perhaps be improved to reduce mode switching, etc.
	if ([self fullScreen]) {
		[self setFullScreen:NO];
		[self setFullScreen:YES];
	}
}

- (BOOL) fullScreen {
	if (_full_screen_window) {
		return YES;
	} else {
		return NO;
	}
}

- (void) setFullScreen:(BOOL)fullScreen {
	if (fullScreen && _full_screen_window == nil) {
		// If we requested full screen and there is no full screenw window, we need to acquire it.
		[self acquireFullScreen:[self currentScreen]];
	} else if (!fullScreen && _full_screen_window != nil) {
		[self releaseFullScreen];
	}
}

// Returns the current screen depending on the location of _partial_screen_window,
// or _full_screen_window, depending on which is currently visible.
- (NSScreen*) currentScreen {
	if (_partial_screen_window) {
		if (_full_screen_window) {
			return [_full_screen_window deepestScreen];
		} else {
			return [_partial_screen_window deepestScreen];
		}
	} else {
		return [NSScreen mainScreen];
	}
}

- (void) acquireFullScreen: (NSScreen*)screen {
	// If the full screen window is already allocated, we are done.
	if (_full_screen_window) return;
	
	NSRect frame = [screen frame];
	
	// Instantiate new borderless window:
	_full_screen_window = [[NSWindow alloc] initWithContentRect:frame styleMask:NSBorderlessWindowMask backing:NSBackingStoreBuffered defer:YES];
	
	if (_full_screen_window != nil) {
		// Close the existing partial window:
		[_partial_screen_window setAcceptsMouseMovedEvents:NO];
		[_partial_screen_window setReleasedWhenClosed:NO];
		[_partial_screen_window close];

		// Set the options for our new fullscreen window, we retrieve some details from _partial_screen_window:
		[_full_screen_window setTitle:[_partial_screen_window title]];
		[_full_screen_window setOpaque:YES];
		[_full_screen_window setReleasedWhenClosed:YES];
		[_full_screen_window setAcceptsMouseMovedEvents:YES];
		[_full_screen_window setBackgroundColor:[NSColor blackColor]];
		
		[_full_screen_window setLevel:NSMainMenuWindowLevel+1];
		
		// The order of this operation might cause flickering - need to test?
		[_full_screen_window setContentView:[_partial_screen_window contentView]];
		
		[_full_screen_window makeKeyAndOrderFront:self];
		[_full_screen_window makeFirstResponder:[_full_screen_window contentView]];
	} else {
		NSLog(@"Error: could not allocate fullscreen window!");
	}
}

- (void) releaseFullScreen {
	if (!_full_screen_window) return;
	
	[_partial_screen_window setAcceptsMouseMovedEvents:YES];
	
	[_full_screen_window setReleasedWhenClosed:NO];
	[_full_screen_window close];
	
	[_partial_screen_window setContentView:[_full_screen_window contentView]];
	[_partial_screen_window makeKeyAndOrderFront:self];
	
	[_full_screen_window release];
	_full_screen_window = nil;
}

- (IBAction) toggleFullScreen:(id)sender {
	[self setFullScreen:![self fullScreen]];
}

@end
