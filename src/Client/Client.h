/*
 *  Client.h
 *  Dream
 *
 *  Created by Samuel Williams on 14/08/10.
 *  Copyright 2010 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_CLIENT_H
#define _DREAM_CLIENT_CLIENT_H

#ifdef __APPLE__
#include <TargetConditionals.h>
#elif __LINUX__
#define TARGET_OS_LINUX 1
#endif

namespace Dream
{
	/** The Client namespace is functionality which is specific to actual graphical applications.
	
	This includes such functionality such as graphics, audio, user interface, and associated areas.	
	*/
	namespace Client
	{
	}
}

#endif
