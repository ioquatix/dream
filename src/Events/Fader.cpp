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
			: m_knob(knob), m_count(0), m_steps(steps), m_increment(increment), m_finished(false)
		{
		
		}
		
		Fader::~Fader ()
		{
		
		}
		
		void Fader::setFinishCallback (FinishCallbackT finishCallback)
		{
			m_finishCallback = finishCallback;
		}
		
		void Fader::cancel ()
		{
			m_finished = true;
		}
		
		bool Fader::repeats () const
		{
			if (m_finished) return false;
			
			return m_count < m_steps;
		}
		
		TimeT Fader::nextTimeout (const TimeT & lastTimeout, const TimeT & currentTime) const
		{
			return lastTimeout + m_increment;
		}
		
		void Fader::processEvents (Loop *, Event event)
		{
			if (event == TIMEOUT) {
				m_count += 1;
				
				RealT time = RealT(m_count) / RealT(m_steps);
				
				m_knob->update(time);
				
				if (m_count == m_steps && m_finishCallback) {
					m_finished = true;
					m_finishCallback(this);
				}
			}
		}
		
	}
}
