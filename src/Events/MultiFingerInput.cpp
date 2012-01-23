//
//  Events/MultiFingerInput.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 29/04/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#include "MultiFingerInput.h"
#include <iostream>

namespace Dream
{
	namespace Events
	{
	
		MultiFingerInput::MultiFingerInput () : m_top(0)
		{
			
		}
		
		ButtonT MultiFingerInput::allocateButton ()
		{
			if (m_freeButtons.size()) {
				ButtonT freeButton = m_freeButtons.back();
				m_freeButtons.pop_back();
				return freeButton;
			} else {
				m_top += 1;
				return m_top;
			}
		}
		
		void MultiFingerInput::releaseButton (ButtonT button)
		{
			if (m_top == button) {
				m_top -= 1;
			} else {
				m_freeButtons.push_back(button);
			}
		}
		
		const FingerTracking & MultiFingerInput::beginMotion (FingerID finger, Vec3 position)
		{
			FingerTracking ft;
			ft.button = allocateButton();
			ft.position = position;
			ft.motion.zero();
			
			//std::cerr << " Begin Tracking Finger : " << ft.button << " @ " << ft.position << std::endl;
			
			return (m_fingers[finger] = ft);
		}
		
		const FingerTracking & MultiFingerInput::updateMotion (FingerID finger, Vec3 position)
		{
			FingersMap::iterator it = m_fingers.find(finger);
			ensure(it != m_fingers.end());
			
			FingerTracking & ft = it->second;
			ft.motion = ft.position - position;
			ft.position = position;
			
			//std::cerr << "Update Tracking Finger : " << ft.button << " @ " << ft.position << std::endl;
			
			return ft;
		}
		
		const FingerTracking MultiFingerInput::finishMotion (FingerID finger, Vec3 position)
		{
			FingersMap::iterator it = m_fingers.find(finger);
			ensure(it != m_fingers.end());
			
			FingerTracking ft = it->second;
			ft.motion = ft.position - position;
			ft.position = position;
			
			releaseButton(ft.button);
			m_fingers.erase(it);
			
			//std::cerr << "Finish Tracking Finger : " << ft.button << " @ " << ft.position << std::endl;
			
			return ft;
		}
					
	}
}
