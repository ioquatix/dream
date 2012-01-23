//
//  Client/Display/Cocoa/DScreenManager.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 8/06/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface DScreenManager : NSObject {
@private
    NSWindow * _partial_screen_window;
	NSWindow * _full_screen_window;
	
	NSScreen * _main_screen;
}

// This property must be connected to the initial window.
@property(retain,readwrite) NSWindow * partialScreenWindow;

// This provides access to the full screen window if available.
@property(retain,readonly) NSWindow * fullScreenWindow;

// This property can remain nil, or be set. If nil, the
// main screen will be determined by [self current_screen].
@property(retain,readwrite) NSScreen * mainScreen;

// This property controls whether the view is full_screen or not.
@property(readwrite) BOOL fullScreen;

// Returns the content view regardless of what window it is currently on.
@property(readonly) NSView * contentView;

// Returns the current screen depending on the location of partial_screen_window, or full_screen_window, depending on which is currently visible.
- (NSScreen*) currentScreen;

// These functions provide direct access to the full_screen process - but you should typically use the functions provided above (e.g. [screen_manager set_full_screen:YES])
- (void) acquireFullScreen: (NSScreen*)screen;
- (void) releaseFullScreen;

// Helper for menu item, etc.
- (IBAction) toggleFullScreen:(id)sender;

@end
