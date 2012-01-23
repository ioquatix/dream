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
				Shared<IKnob> _knob;

				int _count, _steps;
				TimeT _increment;
				
				bool _finished;
				
				FinishCallbackT _finish_callback;
			
			public:
				Fader(Shared<IKnob> knob, int steps, TimeT increment);
				virtual ~Fader ();
				
				void cancel ();
				bool finished () { return _finished; }
				
				void set_finish_callback (FinishCallbackT finish_callback);
				
				virtual bool repeats () const;
				virtual TimeT next_timeout (const TimeT & last_timeout, const TimeT & current_time) const;
				virtual void process_events (Loop *, Event);
		};

	}
}

#endif
