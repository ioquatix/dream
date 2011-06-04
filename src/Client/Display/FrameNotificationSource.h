/*
 *  Client/Display/FrameNotificationSource.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 29/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_FRAMENOTIFICATIONSOURCE_H
#define _DREAM_CLIENT_DISPLAY_FRAMENOTIFICATIONSOURCE_H

// This is a private header, and should not be used as public API.

#include "../../Events/Loop.h"
#include "Context.h"

namespace Dream {
	namespace Client {
		namespace Display {
			
			class FrameNotificationSource : public Object, implements Events::INotificationSource
			{
			protected:
				TimeT m_frameUpdateAt;
				FrameCallbackT m_callback;
				bool m_processed;
				
			public:
				/// Construct the notification with the specified callback.
				FrameNotificationSource (FrameCallbackT callback);
				virtual ~FrameNotificationSource ();
				
				/// Inform the event loop that the frame is needed at the given time.
				/// The best way to use this callback with IContext is to bind the Loop
				/// parameter into the callback.
				/// @sa Application::Application()
				void frameCallback (Events::Loop * loop, TimeT at);

				bool ignored ();
				
				/// This function should only be called from the same thread as the event loop.
				void setCallback (FrameCallbackT callback);
				
				virtual void processEvents (Events::Loop *, Events::Event);
			};
			
		}
	}
}

#endif