//
//  DApplicationDelegate.h
//  Dream
//
//  Created by Samuel Williams on 15/09/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "../Application.h"

@interface DApplicationDelegate : NSObject {
	Dream::Client::Display::IApplication * application;
}

@property(nonatomic,assign) Dream::Client::Display::IApplication * application;

@end
