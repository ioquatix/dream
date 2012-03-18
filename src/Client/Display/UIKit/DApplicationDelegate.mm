//
//  Client/Display/UIKit/DApplicationDelegate.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 18/04/09.
//  Copyright Samuel Williams 2009. All rights reserved.
//

#import "DApplicationDelegate.h"
#import "DOpenGLView.h"

#include "Context.h"

Dream::Client::Display::IApplication * global_application = NULL;

@implementation DApplicationDelegate

+ (void) startWithApplication: (Dream::Client::Display::IApplication*)application {
	global_application = application;
	
	int argc = 0;
	char ** argv = {NULL};
	
	UIApplicationMain(argc, argv, nil, @"DApplicationDelegate");
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {
	// [[NSFileManager defaultManager] changeCurrentDirectoryPath: [[NSBundle mainBundle] resourcePath]];
	//[application setStatusBarHidden:YES];

	//[self performSelector:@selector(runApplicationCallback:) withObject:nil afterDelay:0.0];
	
	global_application->delegate()->application_did_finish_launching(global_application);
}

- (void)applicationWillResignActive:(UIApplication *)application {
	global_application->delegate()->application_will_enter_background(global_application);
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
	global_application->delegate()->application_did_enter_foreground(global_application);
}

- (void)applicationWillTerminate:(UIApplication *)application {
	global_application->delegate()->application_did_finish_launching(global_application);
}

@end
