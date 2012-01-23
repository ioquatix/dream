//
//  Resources/Loadable.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 4/05/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "Loadable.h"

namespace Dream {
	namespace Resources {
		
		LoadError::LoadError (StringT what) : _what(what) {
			
		}
		
		LoadError::~LoadError () throw () {
			
		}
		
		const char * LoadError::what () const noexcept {
			return _what.c_str();
		}
		
	}
}
