//
//  Resources/Loader-Cocoa.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 7/05/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

// Include quickdraw for Cocoa
#define __INCLUDE_QD__

#import <Cocoa/Cocoa.h>
#include "Loadable.h"

namespace Dream {
	namespace Resources {
		Shared<Path> g_workingPath;
				
		Path applicationWorkingPath () {
			// Cache the path result
			if (!g_workingPath) {
				NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
				// I don't think this is the right way to handle resource paths.
				// One main resource search path is not enough - need to implement
				// a list of paths.
				NSString *resPath = [[NSBundle mainBundle] resourcePath];
				
				g_workingPath = new Path([resPath UTF8String]);
				[pool release];
			}
			
			return *g_workingPath;
		}
	}
}
