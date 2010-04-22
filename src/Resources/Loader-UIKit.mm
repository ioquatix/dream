/*
 *  Loader-UIKit2.mm
 *  Dream
 *
 *  Created by Samuel Williams on 26/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

// Include quickdraw for Cocoa
#define __INCLUDE_QD__

#import <UIKit/UIKit.h>

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
				
				NSString *resPath = [[NSBundle mainBundle] resourcePath];
				
				WorkingPath::instance() = Path([resPath UTF8String]);
				[pool release];
			}
			
			return WorkingPath::instance();
		}
	}
}

