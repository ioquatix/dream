/*
 *  Client/Display/OpenGL20/GLUQuadric.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 22/05/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#include "GLUQuadric.h"

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			namespace OpenGL20
			{
					
				
				
				
				
				
				GLUQuadric::GLUQuadric () {
					m_quadric = gluNewQuadric();
				}
				
				GLUQuadric::~GLUQuadric () {
					if (m_quadric) gluDeleteQuadric(m_quadric);
					
					m_quadric = NULL;
				}
				
				GLUSphere::GLUSphere (const RealT &radius, const IndexT &slices, const IndexT &stacks) 
				: m_radius(radius), m_slices(slices), m_stacks(stacks) {
					
				}
				
				void GLUSphere::render (Renderer * renderer) const {
					gluSphere(m_quadric, m_radius, m_slices, m_stacks);
				}
				
				GLUCylinder::GLUCylinder (const RealT &baseRadius, const RealT &topRadius, const RealT &height, const IndexT &slices, const IndexT &stacks)
				: m_baseRadius(baseRadius), m_topRadius(topRadius), m_height(height), m_slices(slices), m_stacks(stacks) {
					
				}
				
				void GLUCylinder::render (Renderer * renderer) const {
					gluCylinder(m_quadric, m_baseRadius, m_topRadius, m_height, m_slices, m_stacks);
				}
				
				GLUDisk::GLUDisk (const RealT &innerRadius, const RealT &outerRadius, const IndexT &slices, const IndexT &loops, const RealT &startAngle, const RealT &sweepAngle) 
				: m_innerRadius(innerRadius), m_outerRadius(outerRadius), m_startAngle(startAngle), m_sweepAngle(sweepAngle), m_slices(slices), m_loops(loops) {
					
				}
				
				void GLUDisk::render (Renderer * renderer) const {
					if (Math::equivalent(m_sweepAngle, R360))
						gluDisk(m_quadric, m_innerRadius, m_outerRadius, m_slices, m_loops);
					else
						gluPartialDisk(m_quadric, m_innerRadius, m_outerRadius, m_slices, m_loops, m_startAngle * R2D, m_sweepAngle * R2D);
				}
				
			}
		}
	}
}
