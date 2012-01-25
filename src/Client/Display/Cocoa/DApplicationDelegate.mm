//
//  Client/Display/Cocoa/DApplicationDelegate.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 15/09/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#import "DApplicationDelegate.h"

@implementation DApplicationDelegate

@synthesize application = _application;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	_application->delegate()->application_did_finish_launching(_application);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
	_application->delegate()->application_will_terminate(_application);
	
	{
		using namespace Dream::Events::Logging;
		
		logger()->log(LOG_DEBUG, "Application terminating.");
	}
}

- (void)applicationDidBecomeActive:(NSNotification *)aNotification
{
	_application->delegate()->application_did_enter_foreground(_application);
}

- (void)applicationWillResignActive:(NSNotification *)aNotification
{
	_application->delegate()->application_will_enter_background(_application);
}

@end
