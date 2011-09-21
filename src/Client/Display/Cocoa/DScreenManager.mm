//
//  DScreenManager.m
//  Dream
//
//  Created by Samuel Williams on 8/06/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

#import "DScreenManager.h"

@implementation DScreenManager

@synthesize partialScreenWindow, fullScreenWindow;
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
	if (fullScreenWindow) {
		return [fullScreenWindow contentView];
	} else {
		return [partialScreenWindow contentView];
	}
}

- (NSScreen*) mainScreen {
	return [[mainScreen retain] autorelease];
}

- (void) setMainScreen:(NSScreen *)_mainScreen {
	// If both nil or both the same pointer - object is identical
	if (mainScreen == _mainScreen) {
		return;
	}
	
	[mainScreen release];
	mainScreen = [_mainScreen retain];
	
	// Cycle full screen because the screen may have changed - this could perhaps be improved to reduce mode switching, etc.
	if ([self fullScreen]) {
		[self setFullScreen:NO];
		[self setFullScreen:YES];
	}
}

- (BOOL) fullScreen {
	if (fullScreenWindow) {
		return YES;
	} else {
		return NO;
	}
}

- (void) setFullScreen:(BOOL)fullScreen {
	if (fullScreen && fullScreenWindow == nil) {
		// If we requested full screen and there is no full screenw window, we need to acquire it.
		[self acquireFullScreen:[self currentScreen]];
	} else if (!fullScreen && fullScreenWindow != nil) {
		[self releaseFullScreen];
	}
}

// Returns the current screen depending on the location of partialScreenWindow,
// or fullScreenWindow, depending on which is currently visible.
- (NSScreen*) currentScreen {
	if (partialScreenWindow) {
		if (fullScreenWindow) {
			return [fullScreenWindow deepestScreen];
		} else {
			return [partialScreenWindow deepestScreen];
		}
	} else {
		return [NSScreen mainScreen];
	}
}

- (void) acquireFullScreen: (NSScreen*)screen {
	// If the full screen window is already allocated, we are done.
	if (fullScreenWindow) return;
	
	NSRect frame = [screen frame];
	
	// Instantiate new borderless window:
	fullScreenWindow = [[NSWindow alloc] initWithContentRect:frame styleMask:NSBorderlessWindowMask backing:NSBackingStoreBuffered defer:YES];
	
	if (fullScreenWindow != nil) {
		// Close the existing partial window:
		[partialScreenWindow setAcceptsMouseMovedEvents:NO];
		[partialScreenWindow setReleasedWhenClosed:NO];
		[partialScreenWindow close];

		// Set the options for our new fullscreen window, we retrieve some details from partialScreenWindow:
		[fullScreenWindow setTitle:[partialScreenWindow title]];
		[fullScreenWindow setOpaque:YES];
		[fullScreenWindow setReleasedWhenClosed:YES];
		[fullScreenWindow setAcceptsMouseMovedEvents:YES];
		[fullScreenWindow setBackgroundColor:[NSColor blackColor]];
		
		[fullScreenWindow setLevel:NSMainMenuWindowLevel+1];
		
		// The order of this operation might cause flickering - need to test?
		[fullScreenWindow setContentView:[partialScreenWindow contentView]];
		
		[fullScreenWindow makeKeyAndOrderFront:self];
		[fullScreenWindow makeFirstResponder:[fullScreenWindow contentView]];
	} else {
		NSLog(@"Error: could not allocate fullscreen window!");
	}
}

- (void) releaseFullScreen {
	if (!fullScreenWindow) return;
	
	[partialScreenWindow setAcceptsMouseMovedEvents:YES];
	
	[fullScreenWindow setReleasedWhenClosed:NO];
	[fullScreenWindow close];
	
	[partialScreenWindow setContentView:[fullScreenWindow contentView]];
	[partialScreenWindow makeKeyAndOrderFront:self];
	
	[fullScreenWindow release];
	fullScreenWindow = nil;
}

- (IBAction) toggleFullScreen:(id)sender {
	[self setFullScreen:![self fullScreen]];
}

@end
