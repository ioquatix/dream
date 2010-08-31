/*
 *  Client/Display/OpenGL20/OpenGL20.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 04/08/07.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_OPENGL20_OPENGL20_H
#define _DREAM_CLIENT_DISPLAY_OPENGL20_OPENGL20_H

#include "../../../Numerics/Numerics.h"
#include "../../../Numerics/Quaternion.h"
#include "../../../Numerics/Vector.h"

#ifndef DREAM_USE_OPENGL20
#error This module is only for use with OpenGL 2.0+
#endif

#if defined(__APPLE__) || defined(MACOSX)
// Provides CGL... functions
#include <OpenGL/OpenGL.h>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif

namespace Dream
{
	namespace Client
	{
		namespace Display 
		{
			namespace OpenGL20
			{				
				using namespace Dream::Numerics;
				
				template <typename t>
				struct GLTypeTraits {};
				
#define GL_TYPE_TRAITS(type, name) template <> struct GLTypeTraits<type> { enum { TYPE = name }; };
				
				GL_TYPE_TRAITS(GLbyte, GL_BYTE)
				GL_TYPE_TRAITS(GLubyte, GL_UNSIGNED_BYTE)
				GL_TYPE_TRAITS(GLshort, GL_SHORT)
				GL_TYPE_TRAITS(GLushort, GL_UNSIGNED_SHORT)
				GL_TYPE_TRAITS(GLint, GL_INT)
				GL_TYPE_TRAITS(GLuint, GL_UNSIGNED_INT)
				GL_TYPE_TRAITS(GLfloat, GL_FLOAT)
				
				void glRotate (const Quaternion<float> &);
				void glTranslate (const Vector<3, float> &);
				
				void glVertex (const Vector<2, GLfloat> & v);
				void glVertex (const Vector<3, GLfloat> & v);
				
				void glColor (const Vector<3, GLfloat> & v);
				void glColor (const Vector<4, GLfloat> & v);
				
				void glTexCoord (const Vector<2, GLfloat> & v);
				void glTexCoord (const Vector<3, GLfloat> & v);
				void glTexCoord (const Vector<4, GLfloat> & v);
				
			}
		}
	}
}

#endif