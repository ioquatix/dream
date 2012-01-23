//
//  Core/Timer-CoreVideo.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 20/04/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#include "Timer.h"

#include <CoreVideo/CVHostTime.h>

namespace Dream {
	namespace Core {
		TimeT systemTime () {
			return CVGetCurrentHostTime() / CVGetHostClockFrequency();
		}
	}
}
