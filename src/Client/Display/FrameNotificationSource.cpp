/*
 *  Client/Display/FrameNotificationSource.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 29/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "FrameNotificationSource.h"

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			FrameNotificationSource::FrameNotificationSource (FrameCallbackT callback) : m_callback(callback), m_processed(true)
			{
				
			}

			FrameNotificationSource::~FrameNotificationSource ()
			{
				
			}

			void FrameNotificationSource::frameCallback (Events::Loop * loop, TimeT at)
			{
				if (m_processed) {
					m_processed = false;
				
					m_frameUpdateAt = at;
					loop->postNotification(this, true);
				} else {
					//std::cerr << __func__ << ": Multiple unprocessed frame callbacks!" << std::endl;
				}
			}
			
			void FrameNotificationSource::setCallback (FrameCallbackT callback)
			{
				m_callback = callback;
			}

			void FrameNotificationSource::processEvents (Events::Loop * loop, Events::Event event)
			{
				m_processed = true;
				
				// If the frame update time is still in the future, we
				// need to begin rendering the frame.
				if (m_frameUpdateAt > systemTime()) {
					m_callback(m_frameUpdateAt);
				} else {
					//std::cerr << __func__ << ": Frame notification too late" << std::endl;
				}
			}

		}
	}
}
