/*
 *  Client/Display/OpenGLES11/OpenGLES11.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 04/08/07.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#include "OpenGLES11.h"

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			namespace OpenGLES11
			{				
				void glRotate (const Quaternion<float> & v)
				{
					Vec3 axis = v.rotationAxis().normalize();
					
					glRotatef(v.rotationAngle() * R2D, axis[X], axis[Y], axis[Z]);
				}
				
				void glTranslate (const Vector<3, float> & v)
				{
					glTranslatef(v[X], v[Y], v[Z]);
				}
				
				void glColor (const Vector<3, GLfloat> & v)
				{
					glColor4f(v[0], v[1], v[2], 1.0);
				}
				
				void glColor (const Vector<4, GLfloat> & v)
				{
					glColor4f(v[0], v[1], v[2], v[3]);
				}
			
			}
		}
	}
}
