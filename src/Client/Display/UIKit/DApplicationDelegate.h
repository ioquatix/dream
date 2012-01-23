//
//  Client/Display/UIKit/DApplicationDelegate.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 18/04/09.
//  Copyright Samuel Williams 2009. All rights reserved.
//
//

// This is a private header, and should not be used as public API.

#import <UIKit/UIKit.h>
#include "Application.h"

@interface DApplicationDelegate : NSObject<UIApplicationDelegate> 
{
}

+ (void) startWithApplication: (Dream::Client::Display::IApplication*)application;

@end
