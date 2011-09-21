//
//  DApplicationDelegate.mm
//  Death Ball Touch
//
//  Created by Samuel Williams on 18/04/09.
//  Copyright Orion Transfer Ltd 2009. All rights reserved.
//

#import "DApplicationDelegate.h"
#import "DOpenGLView.h"

#include "Context.h"

Dream::Client::Display::IApplication * g_application = NULL;

@implementation DApplicationDelegate

+ (void) startWithApplication: (Dream::Client::Display::IApplication*)application {
	g_application = application;
	
	int argc = 0;
	char ** argv = {NULL};
	
	UIApplicationMain(argc, argv, nil, @"DApplicationDelegate");
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {
	// [[NSFileManager defaultManager] changeCurrentDirectoryPath: [[NSBundle mainBundle] resourcePath]];
	//[application setStatusBarHidden:YES];

	//[self performSelector:@selector(runApplicationCallback:) withObject:nil afterDelay:0.0];
	
	g_application->delegate()->applicationDidFinishLaunching(g_application);
}

- (void)applicationWillResignActive:(UIApplication *)application {
	g_application->delegate()->applicationWillEnterBackground(g_application);
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
	g_application->delegate()->applicationDidEnterForeground(g_application);
}

- (void)applicationWillTerminate:(UIApplication *)application {
	g_application->delegate()->applicationDidFinishLaunching(g_application);
}

@end
