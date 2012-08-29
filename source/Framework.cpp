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
	const char * build_date ()
	{
		return BUILD_DATE;
	}

	const char * build_platform ()
	{
		return BUILD_PLATFORM;
	}

	const char * build_revision ()
	{
		return SCM_SHORT_ID;
	}
}
