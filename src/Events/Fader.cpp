//
//  Events/Fader.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 31/12/10.
//  Copyright (c) 2010 Samuel Williams. All rights reserved.
//
//

#include "Fader.h"

namespace Dream
{
	namespace Events
	{
		
		IKnob::~IKnob ()
		{
		
		}
		
		
		
		Fader::Fader(Shared<IKnob> knob, int steps, TimeT increment)
			: _knob(knob), _count(0), _steps(steps), _increment(increment), _finished(false)
		{
		
		}
		
		Fader::~Fader ()
		{
		
		}
		
		void Fader::set_finish_callback (FinishCallbackT finish_callback)
		{
			_finish_callback = finish_callback;
		}
		
		void Fader::cancel ()
		{
			_finished = true;
		}
		
		bool Fader::repeats () const
		{
			if (_finished) return false;
			
			return _count < _steps;
		}
		
		TimeT Fader::next_timeout (const TimeT & last_timeout, const TimeT & current_time) const
		{
			return last_timeout + _increment;
		}
		
		void Fader::process_events (Loop *, Event event)
		{
			if (event == TIMEOUT) {
				_count += 1;
				
				RealT time = RealT(_count) / RealT(_steps);
				
				_knob->update(time);
				
				if (_count == _steps && _finish_callback) {
					_finished = true;
					_finish_callback(this);
				}
			}
		}
		
	}
}
