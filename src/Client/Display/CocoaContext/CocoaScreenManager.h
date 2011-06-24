//
//  CocoaScreenManager.h
//  Dream
//
//  Created by Samuel Williams on 8/06/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface CocoaScreenManager : NSObject {
@private
    NSWindow * partialScreenWindow;
	NSWindow * fullScreenWindow;
	
	NSScreen * mainScreen;
}

// This property must be connected to the initial window.
@property(retain,readwrite) NSWindow * partialScreenWindow;

// This provides access to the full screen window if available.
@property(retain,readonly) NSWindow * fullScreenWindow;

// This property can remain nil, or be set. If nil, the
// main screen will be determined by [self currentScreen].
@property(retain,readwrite) NSScreen * mainScreen;

// This property controls whether the view is fullScreen or not.
@property(readwrite) BOOL fullScreen;

// Returns the content view regardless of what window it is currently on.
@property(readonly) NSView * contentView;

// Returns the current screen depending on the location of partialScreenWindow, or fullScreenWindow, depending on which is currently visible.
- (NSScreen*) currentScreen;

// These functions provide direct access to the fullScreen process - but you should typically use the functions provided above (e.g. [screenManager setFullScreen:YES])
- (void) acquireFullScreen: (NSScreen*)screen;
- (void) releaseFullScreen;

// Helper for menu item, etc.
- (IBAction) toggleFullScreen:(id)sender;

@end
