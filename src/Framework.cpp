//
//  Framework.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 17/04/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

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
