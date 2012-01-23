//
//  Renderer/Camera.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 9/06/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_RENDERER_CAMERA_H
#define _DREAM_RENDERER_CAMERA_H

#include "Renderer.h"
#include "../Numerics/Matrix.h"
#include "../Events/Input.h"

namespace Dream
{
	namespace Renderer
	{
		using namespace Dream::Events;
		
		class ICamera : implements IObject, implements IInputHandler {
		public:				
			virtual ~ICamera ();
			
			virtual Mat44 viewMatrix() const abstract;
		};			
	}
}

#endif
