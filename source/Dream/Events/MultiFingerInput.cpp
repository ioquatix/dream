//
//  Events/MultiFingerInput.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 29/04/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#include "MultiFingerInput.h"

#include "Logger.h"

namespace Dream
{
	namespace Events
	{
		static const bool DEBUG = false;

		using namespace Logging;

		MultiFingerInput::MultiFingerInput () : _top(0)
		{
		}

		ButtonT MultiFingerInput::allocate_button ()
		{
			if (_free_buttons.size() > 0) {
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

		const FingerTracking & MultiFingerInput::begin_motion(FingerID finger, Vec3 position)
		{
			FingerTracking ft;
			ft.button = allocate_button();
			ft.position = position;
			ft.motion = 0;
			
			if (DEBUG) logger()->log(LOG_DEBUG, LogBuffer() << "Begin motion for finger: " << finger);

			return (_fingers[finger] = ft);
		}

		const FingerTracking & MultiFingerInput::update_motion(FingerID finger, Vec3 position)
		{
			if (DEBUG) logger()->log(LOG_DEBUG, LogBuffer() << "Update motion for finger: " << finger);

			FingersMap::iterator it = _fingers.find(finger);
			DREAM_ASSERT(it != _fingers.end());

			FingerTracking & ft = it->second;
			ft.motion = ft.position - position;
			ft.position = position;

			return ft;
		}

		const FingerTracking MultiFingerInput::finish_motion(FingerID finger, Vec3 position)
		{
			if (DEBUG) logger()->log(LOG_DEBUG, LogBuffer() << "Finish motion for finger: " << finger);

			FingersMap::iterator it = _fingers.find(finger);
			DREAM_ASSERT(it != _fingers.end());

			FingerTracking ft = it->second;
			ft.motion = ft.position - position;
			ft.position = position;

			release_button(ft.button);
			_fingers.erase(it);

			return ft;
		}
	}
}
