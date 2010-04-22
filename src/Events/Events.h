/*
 *  Events/Events.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 9/12/08.
 *  Copyright 2008 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_EVENTS_EVENTS_H
#define _DREAM_EVENTS_EVENTS_H

#include "../Framework.h"
#include "../Core/Timer.h"

namespace Dream
{
	/// Asynchronous event processing and delegation.
	/// Events and event loops typically help to serialize "asynchronious" events. Because this typically involves multiple threads and multiple run-loops, it
	/// is important to note that functions are not thread-safe (i.e. called from a different thread) unless explicity stated in the documentation. Functions
	/// which are thread-safe have specific mechanisms such as locks in order to provide a specific behaviour, so keep this in mind when using the APIs.
	namespace Events
	{
		using namespace Dream::Core;
		
		/// Event constants as used by Loop
		enum Event
		{
			READ_READY = 1,
			WRITE_READY = 2,
			TIMEOUT = 16,
			NOTIFICATION = 32
		};
	}
}

#endif