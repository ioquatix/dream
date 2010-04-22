//
//  DreamAppDelegate.mm
//  Death Ball Touch
//
//  Created by Samuel Williams on 18/04/09.
//  Copyright Orion Transfer Ltd 2009. All rights reserved.
//

#import "DreamAppDelegate.h"
#import "DreamView.h"

#include "Context.h"

@implementation DreamAppDelegate

@synthesize window;
@synthesize view;

- (id)init {
	if (self = [super init]) {
		window = nil;
		view = nil;
	}
	
	return self;
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {
	//glView.animationInterval = 1.0 / 60.0;
	//[glView startAnimation];
	
	[application setStatusBarHidden:YES];
	
	Dream::Client::Display::UIKitContext::runApplicationCallback();
	
	exit(0);
}


- (void)applicationWillResignActive:(UIApplication *)application {
	//glView.animationInterval = 1.0 / 5.0;
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
	//glView.animationInterval = 1.0 / 60.0;
}

- (void)applicationWillTerminate:(UIApplication *)application {
	//SDL_SendQuit();
	// hack to prevent automatic termination.  See SDL_uikitevents.m for details
	//longjmp(*(jump_env()), 1);
}

- (void)dealloc {
	[window release];
	[view release];
	
	[super dealloc];
}

@end
