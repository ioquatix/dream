/*
 *  Framework.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 17/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Revision.h"

namespace Dream
{
	const char * buildDate ()
	{
		return BUILD_DATE;
	}
	
	const char * buildPlatform ()
	{
		return BUILD_PLATFORM;
	}
	
	const char * buildRevision ()
	{
		return SCM_SHORT_ID;
	}
}
