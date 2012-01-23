//
//  Client/Display/Cocoa/DApplicationDelegate.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 15/09/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "../Application.h"

@interface DApplicationDelegate : NSObject {
	Dream::Client::Display::IApplication * application;
}

@property(nonatomic,assign) Dream::Client::Display::IApplication * application;

@end
