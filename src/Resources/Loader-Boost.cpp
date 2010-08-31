/*
 *  Resources/Loader-Boost.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 7/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <Loadable.h>

namespace Dream {
	namespace Resources {
		Path applicationWorkingPath () {
			// Boost API
			return boost::filesystem::initial_path();
		}
	}
}