//
//  Client/Graphics/Graphics.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 31/12/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

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
#elif TARGET_OS_ANDROID
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>

	#define DREAM_OPENGLES2
#endif

namespace Dream
{
	namespace Client
	{
		namespace Graphics
		{
			using namespace Dream::Numerics;
			using namespace Dream::Client::Display;
			
			void check_error();
			
			template <typename TypeT>
			struct GLTypeTraits {};
			
#define GL_TYPE_TRAITS(type, name) template <> struct GLTypeTraits<type> { enum { TYPE = name }; };
			
			GL_TYPE_TRAITS(GLbyte, GL_BYTE)
			GL_TYPE_TRAITS(GLubyte, GL_UNSIGNED_BYTE)
			GL_TYPE_TRAITS(GLshort, GL_SHORT)
			GL_TYPE_TRAITS(GLushort, GL_UNSIGNED_SHORT)
			GL_TYPE_TRAITS(GLint, GL_INT)
			GL_TYPE_TRAITS(GLuint, GL_UNSIGNED_INT)
			GL_TYPE_TRAITS(GLfloat, GL_FLOAT)
		}
	}
}

#endif
