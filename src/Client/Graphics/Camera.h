/*
 *  Client/Graphics/Camera.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 9/06/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_CAMERA_H
#define _DREAM_CLIENT_GRAPHICS_CAMERA_H

#include "Graphics.h"
#include "../../Numerics/Vector.h"
#include "../../Numerics/Matrix.h"
#include "../../Geometry/Line.h"
#include "../../Geometry/AlignedBox.h"
#include "../Display/Input.h"

namespace Dream {
	namespace Client {
		namespace Graphics {			
			using namespace Dream::Client::Display;
			
			using Geometry::Line;
			using Geometry::AlignedBox;
						
			struct ViewportEyeSpace
			{
				Vec3 origin;
				Line<3> forward;
				Vec3 up;
			};
			
			/**
			 
			 */
			class Camera : public Object, IMPLEMENTS(InputHandler) {
				EXPOSE_CLASS(Camera)
				
				class Class : public Object::Class, IMPLEMENTS(InputHandler::Class) {
					EXPOSE_CLASSTYPE
				};
				
			public:				
				virtual ~Camera ();
								
				virtual bool button(const ButtonInput &);
				virtual bool motion(const MotionInput &);
				
				virtual Mat44 viewMatrix () const abstract;
				
				/// Calculate the object-space coordinates when given the window's viewport and a point in the viewport.
				static ViewportEyeSpace convertFromViewportToObjectSpace (const Mat44 & projectionMatrix, const Mat44 & modelViewMatrix, 
																		  const AlignedBox<2,int> & viewport, const Vec2 & c);
				
				/// Calculate the object-space coordinates when given a projection-space coordinate on the near plane.
				static ViewportEyeSpace convertFromProjectionSpaceToObjectSpace (const Mat44 & projectionMatrix, const Mat44 & modelViewMatrix, const Vec2 & n);
				
				/// Calculate the object-space coordinate when given a projection-space coordinate. This function is not fast for many points, as it calculates
				/// inverse matrices per call.
				static Vec4 convertFromProjectionSpaceToObjectSpace (const Mat44 & projectionMatrix, const Mat44 & modelViewMatrix, const Vec3 & n);
			};
		}
	}
}

#endif
