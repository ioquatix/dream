/*
 *  Client/Display/UIKitContext/DreamApplicationDelegate.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 18/04/09.
 *  Copyright Orion Transfer Ltd 2009. All rights reserved.
 *
 */

// This is a private header, and should not be used as public API.

#import <UIKit/UIKit.h>
#include "Application.h"

@class DreamView;

@interface DreamApplicationDelegate : NSObject<UIApplicationDelegate> 
{
    UIWindow * window;
    DreamView * view;
}

+ (void) start;

@property (nonatomic, retain) IBOutlet UIWindow * window;
@property (nonatomic, retain) IBOutlet DreamView * view;

@end
