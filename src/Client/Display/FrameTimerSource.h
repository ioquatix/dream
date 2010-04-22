/*
 *  Client/Display/FrameTimerSource.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 29/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_FRAMETIMERSOURCE_H
#define _DREAM_CLIENT_DISPLAY_FRAMETIMERSOURCE_H

// This is a private header, and should not be used as public API.

#include "../../Events/Loop.h"
#include "Context.h"

namespace Dream
{
	namespace Client
	{
		namespace Display
		{

			class FrameTimerSource : public Object, IMPLEMENTS_NS(Events, TimerSource)
			{
				EXPOSE_CLASS(FrameTimerSource)
				
				class Class : public Object::Class, IMPLEMENTS_NS(Events, TimerSource::Class)
				{
					EXPOSE_CLASSTYPE
				};
				
			protected:
				bool m_cancelled;
				TimeT m_period;
				FrameCallbackT m_callback;
				
			public:
				FrameTimerSource (FrameCallbackT callback, TimeT period);
				
				virtual void processEvents (Events::Loop *, Events::Event);
				
				virtual bool repeats () const;
				virtual TimeT nextTimeout (const TimeT & lastTimeout, const TimeT & currentTime) const;
				
				void setPeriod (TimeT period);
				void setCallback (FrameCallbackT callback);
				
				void cancel ();
			};

		}
	}
}

#endif
