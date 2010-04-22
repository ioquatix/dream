/*
 *  Client/Graphics/Graphics.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 31/12/08.
 *  Copyright 2008 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_GRAPHICS_H
#define _DREAM_CLIENT_GRAPHICS_GRAPHICS_H

#include "../../Numerics/Numerics.h"
#include "../Display/Renderer.h"

namespace Dream
{
	namespace Client
	{
		namespace Graphics
		{
			using namespace Dream::Numerics;
		
#ifdef DREAM_USE_OPENGL20
			using namespace Dream::Client::Display::OpenGL20;
#elif DREAM_USE_OPENGL31
			using namespace Dream::Client::Display::OpenGL31;
#elif DREAM_USE_OPENGLES11
			using namespace Dream::Client::Display::OpenGLES11;
#endif			
			
		}
	}
}

#endif
