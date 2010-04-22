/*
 *  Client/Display/MultiFingerInput.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 29/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_MULTIFINGERINPUT_H
#define _DREAM_CLIENT_DISPLAY_MULTIFINGERINPUT_H

// This is a private header, and should not be used as public API.

#include "../../Numerics/Vector.h"
#include "Input.h"

#include <map>
#include <vector>

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			using Dream::Numerics::Vec3;
			
			typedef intptr_t FingerID;
			
			struct FingerTracking
			{
				Vec3 position;
				Vec3 motion;
				
				ButtonT button;
			};
			
			class MultiFingerInput
			{
			protected:				
				typedef std::map<FingerID, FingerTracking> FingersMap;
				FingersMap m_fingers;
				
				std::vector<ButtonT> m_freeButtons;
				ButtonT m_top;

				ButtonT allocateButton ();
				void releaseButton(ButtonT);
				
			public:
				MultiFingerInput ();
				
				const FingerTracking & beginMotion (FingerID finger, Vec3 position);
				const FingerTracking & updateMotion (FingerID finger, Vec3 position);
				const FingerTracking finishMotion (FingerID finger, Vec3 position);
				
			};
			
		}
	}
}

#endif
