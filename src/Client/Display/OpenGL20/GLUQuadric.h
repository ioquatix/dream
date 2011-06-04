/*
 *  Client/Display/OpenGL20/GLUQuadric.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 22/05/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_OPENGL20_GLUQUADRIC_H
#define _DREAM_CLIENT_DISPLAY_OPENGL20_GLUQUADRIC_H

#include "OpenGL20.h"
#include "Renderer.h"

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			namespace OpenGL20
			{
				
				/* Renderable interface for GLU Quadrics */	
				class GLUQuadric : public Object {
				protected:
					GLUquadricObj *m_quadric;
					
					GLUQuadric ();
				public:
					void setDrawStyle (GLenum draw) {
						gluQuadricDrawStyle(m_quadric, draw);
					}
					
					void setNormalsStyle (GLenum norms) {
						gluQuadricNormals(m_quadric, norms);
					}
					
					void setOrientation (GLenum orient) {
						gluQuadricOrientation(m_quadric, orient);
					}
					
					void setTextured (bool textured)
					{
						gluQuadricTexture(m_quadric, textured);
					}
					
					virtual ~GLUQuadric ();
				};
				
				class GLUSphere : public GLUQuadric {
				protected:
					RealT m_radius;
					IndexT m_slices, m_stacks;
					
				public:
					GLUSphere (const RealT &radius, const IndexT &slices, const IndexT &stacks);
					
					
					virtual void render (Renderer * renderer) const;
				};
				
				class GLUCylinder : public GLUQuadric {
				protected:
					RealT m_baseRadius, m_topRadius, m_height;
					IndexT m_slices, m_stacks;
					
				public:
					GLUCylinder (const RealT &baseRadius, const RealT &topRadius, const RealT &height, const IndexT &slices, const IndexT &stacks);
					
					virtual void render (Renderer * renderer) const;
				};
				
				class GLUDisk : public GLUQuadric {
				protected:
					RealT m_innerRadius, m_outerRadius, m_startAngle, m_sweepAngle;
					IndexT m_slices, m_loops;
					
				public:
					GLUDisk (const RealT &innerRadius, const RealT &outerRadius, const IndexT &slices, const IndexT &loops, const RealT &startAngle = 0.0, const RealT &sweepAngle = R360);
					
					virtual void render (Renderer * renderer) const;
				};
			}			
		}
	}
}

#endif
