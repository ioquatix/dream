//
//  Resources/Loader-UIKit.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 26/04/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#include "Loader.h"

#import <UIKit/UIKit.h>

namespace Dream {
	namespace Resources {
		Shared<Path> _working_path;
		
		Path application_working_path () {
			// Cache the path result
			if (!_working_path) {
				NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
				// I don't think this is the right way to handle resource paths.
				// One main resource search path is not enough - need to implement
				// a list of paths.
				NSString * resource_path = [[NSBundle mainBundle] resourcePath];
				
				_working_path = new Path([resource_path UTF8String]);
				[pool release];
			}
			
			return *_working_path;
		}
	}
}
