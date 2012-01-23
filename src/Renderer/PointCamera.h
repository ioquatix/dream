//
//  Renderer/PointCamera.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 16/10/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_RENDERER_POINTCAMERA_H
#define _DREAM_RENDERER_POINTCAMERA_H

#include "Camera.h"
#include "../Geometry/Line.h"

namespace Dream
{
	namespace Renderer
	{
		using namespace Dream::Geometry;
		
		class PointCamera : public Object, implements ICamera {
		protected:
			Vec3 _origin;
			Vec3 _direction;
			Vec3 _up;
			
		public:
			PointCamera ();
			
			static Mat44 look_at(const Vec3 & origin, const Vec3 & direction, const Vec3 & up);
			
			void set_direction (const Vec3 &dir) { _direction = dir; }
			void set_origin (const Vec3 &origin) { _origin = origin; }
			void set_up (const Vec3 &up) { _up = up; }
			
			void set (const Line<3> &l);
			
			virtual Mat44 view_matrix () const;				
		};
		
	}
}

#endif
