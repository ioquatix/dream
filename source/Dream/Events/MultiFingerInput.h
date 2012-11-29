//
//  Events/MultiFingerInput.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 29/04/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CLIENT_DISPLAY_MULTIFINGERINPUT_H
#define _DREAM_CLIENT_DISPLAY_MULTIFINGERINPUT_H

// This is a private header, and should not be used as public API.

#include "Input.h"

#include <map>
#include <vector>

#include <Euclid/Numerics/Vector.h>

namespace Dream
{
	namespace Events
	{
		using Euclid::Numerics::Vec3;

		typedef intptr_t FingerID;

		struct FingerTracking {
			Vec3 position;
			Vec3 motion;

			ButtonT button;
		};

		class MultiFingerInput {
		protected:
			typedef std::map<FingerID, FingerTracking> FingersMap;
			FingersMap _fingers;

			std::vector<ButtonT> _free_buttons;
			ButtonT _top;

			ButtonT allocate_button ();
			void release_button(ButtonT);

		public:
			MultiFingerInput ();

			const FingerTracking & begin_motion (FingerID finger, Vec3 position);
			const FingerTracking & update_motion (FingerID finger, Vec3 position);
			const FingerTracking finish_motion (FingerID finger, Vec3 position);
		};
	}
}

#endif
