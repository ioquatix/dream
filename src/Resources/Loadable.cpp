/*
 *  Resources/Loadable.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 4/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "Loadable.h"

namespace Dream {
	namespace Resources {
		
		LoadError::LoadError (StringT what) : m_what(what) {
			
		}
		
		LoadError::~LoadError () throw () {
			
		}
		
		const char * LoadError::what () const noexcept {
			return m_what.c_str();
		}
		
	}
}