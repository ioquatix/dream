/*
 *  Events/Console.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 13/12/08.
 *  Copyright 2008 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_EVENTS_CONSOLE_H
#define _DREAM_EVENTS_CONSOLE_H

#include "Events.h"

namespace Dream
{
	namespace Events
	{
		
		/// This function is used to reopen stdin so that
		/// other functions can interact with them as expected.
		/// NB. This is a terrible terrible hack. :(
		void reopenStandardFileDescriptorsAsPipes ();
		
	}
}

#endif