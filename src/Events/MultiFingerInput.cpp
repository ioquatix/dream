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
	
		MultiFingerInput::MultiFingerInput () : _top(0)
		{
			
		}
		
		ButtonT MultiFingerInput::allocate_button ()
		{
			if (_free_buttons.size()) {
				ButtonT free_button = _free_buttons.back();
				_free_buttons.pop_back();
				return free_button;
			} else {
				_top += 1;
				return _top;
			}
		}
		
		void MultiFingerInput::release_button (ButtonT button)
		{
			if (_top == button) {
				_top -= 1;
			} else {
				_free_buttons.push_back(button);
			}
		}
		
		const FingerTracking & MultiFingerInput::begin_motion (FingerID finger, Vec3 position)
		{
			FingerTracking ft;
			ft.button = allocate_button();
			ft.position = position;
			ft.motion.zero();
			
			//std::cerr << " Begin Tracking Finger : " << ft.button << " @ " << ft.position << std::endl;
			
			return (_fingers[finger] = ft);
		}
		
		const FingerTracking & MultiFingerInput::update_motion (FingerID finger, Vec3 position)
		{
			FingersMap::iterator it = _fingers.find(finger);
			ensure(it != _fingers.end());
			
			FingerTracking & ft = it->second;
			ft.motion = ft.position - position;
			ft.position = position;
			
			//std::cerr << "Update Tracking Finger : " << ft.button << " @ " << ft.position << std::endl;
			
			return ft;
		}
		
		const FingerTracking MultiFingerInput::finish_motion (FingerID finger, Vec3 position)
		{
			FingersMap::iterator it = _fingers.find(finger);
			ensure(it != _fingers.end());
			
			FingerTracking ft = it->second;
			ft.motion = ft.position - position;
			ft.position = position;
			
			release_button(ft.button);
			_fingers.erase(it);
			
			//std::cerr << "Finish Tracking Finger : " << ft.button << " @ " << ft.position << std::endl;
			
			return ft;
		}
					
	}
}
