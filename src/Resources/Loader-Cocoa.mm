/*
 *  Loader-Mac.cpp
 *  Dream
 *
 *  Created by Samuel Williams on 7/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

// Include quickdraw for Cocoa
#define __INCLUDE_QD__

#import <Cocoa/Cocoa.h>

#include "Loader.h"
#include <boost/pool/detail/singleton.hpp>

namespace Dream {
	namespace Resources {
		using boost::details::pool::singleton_default;
		typedef singleton_default<Path> WorkingPath;
		
		Path Loader::workingPath () {
			// Cache the path result
			if (WorkingPath::instance().empty()) {
				NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
				// I don't think this is the right way to handle resource paths.
				// One main resource search path is not enough - need to implement
				// a list of paths.
				NSString *resPath = [[NSBundle mainBundle] resourcePath];
				
				WorkingPath::instance() = Path([resPath UTF8String]);
				[pool release];
			}
			
			return WorkingPath::instance();
		}
	}
}

