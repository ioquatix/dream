/*
 *  Client/Graphics/PointCamera.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 16/10/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#include "PointCamera.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			IMPLEMENT_CLASS(PointCamera)
			
			PointCamera::PointCamera () : m_origin(0, 0, 0), m_direction(0, 0, 1), m_up(0, 1, 0) {
				
			}

			Mat44 PointCamera::viewMatrix () const
			{
				// Basically an implementation of gluLookAt, but slightly simpler due to the following constraints:
				// m_direction is already normalized and points from m_origin in the direction we are currently looking in
				// m_up is already normalized
				
				Vec3 s = m_direction.cross(m_up);
				Vec3 u = s.cross(m_direction);
				
				Mat44 m(ZERO);
				m.set(0, 0, s);
				m.set(1, 0, u);
				m.set(2, 0, -m_direction);
				m.at(3, 3) = 1;
				
				Mat44 t = Mat44::translatingMatrix(-m_origin);
				
				return m * t;
			}
			
			void PointCamera::set (const Line<3> &l) {
				m_origin = l.point();
				m_direction = l.direction();
			}
			
		}
	}
}
