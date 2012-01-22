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

#include "../Client.h"

#include "../../Numerics/Numerics.h"
#include "../Display/Scene.h"

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#define DREAM_OPENGLES2

#elif TARGET_OS_MAC
#include <OpenGL/gl3.h>

#define DREAM_OPENGL32
#endif

namespace Dream
{
	namespace Client
	{
		namespace Graphics
		{
			using namespace Dream::Numerics;
			using namespace Dream::Client::Display;
			
			void checkError();
		}
	}
}

#endif
