/*
 *  Resources/Loader-Generic.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 7/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "Loadable.h"

namespace Dream {
	namespace Resources {
		Path applicationWorkingPath () {
			return Path::currentWorkingDirectory();
		}
	}
}