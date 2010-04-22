/*
 *  Client/Display/FrameTimerSource.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 29/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "FrameTimerSource.h"

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			
			IMPLEMENT_CLASS(FrameTimerSource)
			
			FrameTimerSource::FrameTimerSource (FrameCallbackT callback, TimeT period) : m_cancelled(false), m_period(period), m_callback(callback)
			{
				
			}
			
			void FrameTimerSource::processEvents (Events::Loop *, Events::Event)
			{
				TimeT nextFrameTime = systemTime() + m_period;
				
				m_callback(nextFrameTime);
			}
			
			bool FrameTimerSource::repeats () const
			{
				return !m_cancelled;
			}
			
			TimeT FrameTimerSource::nextTimeout (const TimeT & lastTimeout, const TimeT & currentTime) const
			{
				TimeT nextFrame = lastTimeout + m_period;
				
				if (nextFrame < currentTime) {
					unsigned droppedFrames = Number<TimeT>::ceil((currentTime - nextFrame) / m_period);
					
					std::cerr << "Dropped " << droppedFrames << " frames!" << std::endl;
					
					nextFrame += m_period * droppedFrames;
				}
				
				return nextFrame;
			}

			void FrameTimerSource::setPeriod (TimeT period)
			{
				m_period = period;
			}
			
			void FrameTimerSource::setCallback (FrameCallbackT callback)
			{
				m_callback = callback;
			}
			
			void FrameTimerSource::cancel ()
			{
				m_cancelled = true;
			}
			
		}
	}
}
