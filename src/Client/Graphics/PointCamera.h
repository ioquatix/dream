/*
 *  Client/Graphics/PointCamera.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 16/10/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_POINTCAMERA_H
#define _DREAM_CLIENT_GRAPHICS_POINTCAMERA_H

#include "Camera.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			class PointCamera : public Camera {
			protected:
				Vec3 m_origin;
				Vec3 m_direction;
				Vec3 m_up;
				
			public:
				class Class : public Camera::Class {
				public:
					EXPOSE_CLASSTYPE
					
					virtual REF(Object) init () {
						return new PointCamera();
					}
				};
				
				EXPOSE_CLASS(PointCamera)
				
				PointCamera ();
				
				void setDirection (const Vec3 &dir) { m_direction = dir; }
				void setOrigin (const Vec3 &origin) { m_origin = origin; }
				void setUp (const Vec3 &up) { m_up = up; }
				
				void set (const Line<3> &l);
				
				virtual Mat44 viewMatrix () const;				
			};
			
		}
	}
}

#endif
