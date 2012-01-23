//
//  Resources/Loader-Generic.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 7/05/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "Loadable.h"

namespace Dream {
	namespace Resources {
		Path application_working_path () {
			return Path::current_working_directory();
		}
	}
}
