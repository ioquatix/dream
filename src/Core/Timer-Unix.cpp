/*
 *  Core/Timer-Unix.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 20/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Timer.h"

#include <ctime>
#include <sys/time.h>

namespace Dream {
	namespace Core {		
		TimeT systemTime () {
			struct timeval t;
			gettimeofday (&t, (struct timezone*)0);
			return ((TimeT)t.tv_sec) + ((TimeT)t.tv_usec / 1000000.0);
		}		
	}
}