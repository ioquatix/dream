//
//  Events/Fader.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 31/12/10.
//  Copyright (c) 2010 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_EVENTS_FADER_H
#define _DREAM_EVENTS_FADER_H

#include "../Numerics/Numerics.h"
#include "../Numerics/Interpolate.h"
#include "Source.h"

namespace Dream
{
	namespace Events
	{
		using namespace Dream::Numerics;
		
		class IKnob
		{
			public:
				virtual ~IKnob ();
				virtual void update (RealT time) abstract;
		};
		
		/**
			A fader object updates a knob with a time value linearly interpolated from 0.0 to 1.0. At the end, it calls
			the finish callback if it is specified.
			
			It is used for providing time based animation/interpolation, and by reference counting can be used in a set
			and forget fashion.
		*/
		class Fader : public Object, implements ITimerSource
		{
			public:
				typedef std::function<void (PTR(Fader) fader)> FinishCallbackT;
			
			protected:
				Shared<IKnob> m_knob;

				int m_count, m_steps;
				TimeT m_increment;
				
				bool m_finished;
				
				FinishCallbackT m_finishCallback;
			
			public:
				Fader(Shared<IKnob> knob, int steps, TimeT increment);
				virtual ~Fader ();
				
				void cancel ();
				bool finished () { return m_finished; }
				
				void setFinishCallback (FinishCallbackT finishCallback);
				
				virtual bool repeats () const;
				virtual TimeT nextTimeout (const TimeT & lastTimeout, const TimeT & currentTime) const;
				virtual void processEvents (Loop *, Event);
		};

	}
}

#endif
