/*
 *  Client/Display/OpenGL20/OpenGL20.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 04/08/07.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#include "OpenGL20.h"

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			namespace OpenGL20
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
				
				void glVertex (const Vector<2, GLfloat> & v)
				{
					glVertex2fv(v.value());
				}
				
				void glVertex (const Vector<3, GLfloat> & v)
				{
					glVertex3fv(v.value());
				}
				
				void glColor (const Vector<3, GLfloat> & v)
				{
					glColor3fv(v.value());
				}
				
				void glColor (const Vector<4, GLfloat> & v)
				{
					glColor4fv(v.value());
				}
				
				void glTexCoord (const Vector<2, GLfloat> & v)
				{
					glTexCoord2fv(v.value());
				}

				void glTexCoord (const Vector<3, GLfloat> & v)
				{
					glTexCoord3fv(v.value());
				}
				
				void glTexCoord (const Vector<4, GLfloat> & v)
				{
					glTexCoord4fv(v.value());
				}
			}			
		}
	}
}
