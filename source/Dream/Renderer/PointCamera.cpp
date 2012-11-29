//
//  Renderer/PointCamera.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 16/10/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#include "PointCamera.h"

namespace Dream
{
	namespace Renderer
	{
		Mat44 PointCamera::look_at(const Vec3 & origin, const Vec3 & direction, const Vec3 & up) {
			// Basically an implementation of gluLookAt, but slightly simpler due to the following constraints:
			// _direction is already normalized and points from _origin in the direction we are currently looking in
			// _up is already normalized

			Vec3 s = direction.cross(up);
			Vec3 u = s.cross(direction);

			Mat44 m(ZERO);
			m.set(0, 0, s, 4);
			m.set(1, 0, u, 4);
			m.set(2, 0, -direction, 4);
			m.at(3, 3) = 1;

			Mat44 t = Mat44::translating_matrix(-origin);

			return m.transposed_matrix() * t;
		}

		PointCamera::PointCamera () : _origin(0, 0, 0), _direction(0, 0, 1), _up(0, 1, 0) {
		}

		Mat44 PointCamera::view_matrix () const
		{
			Vec3 s = _direction.cross(_up);
			Vec3 u = s.cross(_direction);

			Mat44 m(ZERO);
			m.set(0, 0, s, 4);
			m.set(1, 0, u, 4);
			m.set(2, 0, -_direction, 4);
			m.at(3, 3) = 1;

			Mat44 t = Mat44::translating_matrix(-_origin);

			return m * t;
		}

		void PointCamera::set (const Line<3> &l) {
			_origin = l.point();
			_direction = l.direction();
		}
	}
}
