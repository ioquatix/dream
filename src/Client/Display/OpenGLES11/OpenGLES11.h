/*
 *  Client/Display/OpenGLES11/OpenGLES11.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 04/08/07.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_OPENGLES11_OPENGLES11_H
#define _DREAM_CLIENT_DISPLAY_OPENGLES11_OPENGLES11_H

#ifndef DREAM_USE_OPENGLES11
#error This module is only for use with OpenGLES 1.1+
#endif

#include "../../../Numerics/Numerics.h"
#include "../../../Numerics/Quaternion.h"
#include "../../../Numerics/Vector.h"

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>

namespace Dream
{
	namespace Client
	{
		namespace Display 
		{
			namespace OpenGLES11
			{
				using namespace Dream::Numerics;

				template <typename t>
				struct GLTypeTraits {};
				
#define GL_TYPE_TRAITS(type, name) template <> struct GLTypeTraits<type> { enum { TYPE = name }; };
				
				GL_TYPE_TRAITS(GLbyte, GL_BYTE)
				GL_TYPE_TRAITS(GLubyte, GL_UNSIGNED_BYTE)
				GL_TYPE_TRAITS(GLshort, GL_SHORT)
				GL_TYPE_TRAITS(GLushort, GL_UNSIGNED_SHORT)
				GL_TYPE_TRAITS(GLfloat, GL_FLOAT)
				GL_TYPE_TRAITS(GLfixed, GL_FIXED)
				
				void glRotate (const Quaternion<float> &);
				void glTranslate (const Vector<3, float> &);
				
				/// Emulated by calling glColor4fv
				void glColor (const Vector<3, GLfloat> & v);
				
				/// Better to use this method. OpenGLES11 only supports glColor4fv
				void glColor (const Vector<4, GLfloat> & v);
				
				// Make it slightly easier to work with Point Sprites
				const GLenum GL_POINT_SPRITE = GL_POINT_SPRITE_OES;
				const GLenum GL_COORD_REPLACE = GL_COORD_REPLACE_OES;
			}		
		}
	}
}

#endif