//
//  Client/Display/Cocoa/DApplicationDelegate.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 15/09/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#import "DApplicationDelegate.h"

@implementation DApplicationDelegate

@synthesize application;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	application->delegate()->applicationDidFinishLaunching(application);
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
	application->delegate()->applicationWillTerminate(application);
}

- (void)applicationDidBecomeActive:(NSNotification *)aNotification
{
	application->delegate()->applicationDidEnterForeground(application);
}

- (void)applicationWillResignActive:(NSNotification *)aNotification
{
	application->delegate()->applicationWillEnterBackground(application);
}

@end
