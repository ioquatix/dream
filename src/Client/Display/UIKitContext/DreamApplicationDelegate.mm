//
//  DreamApplicationDelegate.mm
//  Death Ball Touch
//
//  Created by Samuel Williams on 18/04/09.
//  Copyright Orion Transfer Ltd 2009. All rights reserved.
//

#import "DreamApplicationDelegate.h"
#import "DreamView.h"

#include "Context.h"

@implementation DreamApplicationDelegate

+ (void) start {
	UIApplicationMain(0, NULL, nil, @"DreamApplicationDelegate");
}

@synthesize window;
@synthesize view;

- (id)init {
	if (self = [super init]) {
		window = nil;
		view = nil;
	}
	
	return self;
}

- (void) runApplicationCallback: (id)sender {
	Dream::Client::Display::UIKitContext::runApplicationCallback();
	
	exit(0);
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {
	// [[NSFileManager defaultManager] changeCurrentDirectoryPath: [[NSBundle mainBundle] resourcePath]];
	[application setStatusBarHidden:YES];

	[self performSelector:@selector(runApplicationCallback:) withObject:nil afterDelay:0.0];
}

- (void)applicationWillResignActive:(UIApplication *)application {

}


- (void)applicationDidBecomeActive:(UIApplication *)application {

}

- (void)applicationWillTerminate:(UIApplication *)application {

}

- (void)dealloc {
	[window release];
	[view release];
	
	[super dealloc];
}

@end
