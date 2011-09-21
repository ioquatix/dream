//
//  DApplicationDelegate.m
//  Dream
//
//  Created by Samuel Williams on 15/09/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
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

@end
