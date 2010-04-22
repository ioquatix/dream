/*
 *  Core/Timer-CoreVideo.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 20/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Timer.h"

#include <CoreVideo/CVHostTime.h>

namespace Dream {
	namespace Core {
		TimeT systemTime () {
			return CVGetCurrentHostTime() / CVGetHostClockFrequency();
		}
	}
}