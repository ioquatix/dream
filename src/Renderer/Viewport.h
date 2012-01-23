//
//  Renderer/Viewport.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/06/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#ifndef _DREAM_RENDERER_VIEWPORT_H
#define _DREAM_RENDERER_VIEWPORT_H

#include "Projection.h"
#include "Camera.h"

namespace Dream
{
	namespace Renderer
	{
			
		struct ViewportEyeSpace
		{
			Vec3 origin;
			Line<3> forward;
			Vec3 up;
			
			/// Calculate the object-space coordinates when given the window's viewport and a point in the viewport.
			static ViewportEyeSpace convertFromViewportToObjectSpace (const Mat44 & projectionMatrix, const Mat44 & modelViewMatrix, 
																	  const AlignedBox<2> & viewport, const Vec2 & c);
			
			/// Calculate the object-space coordinates when given a projection-space coordinate on the near plane.
			static ViewportEyeSpace convertFromProjectionSpaceToObjectSpace (const Mat44 & projectionMatrix, const Mat44 & modelViewMatrix, const Vec2 & n);
			
			/// Calculate the object-space coordinate when given a projection-space coordinate. This function is not fast for many points, as it calculates
			/// inverse matrices per call.
			static Vec4 convertFromProjectionSpaceToObjectSpace (const Mat44 & projectionMatrix, const Mat44 & modelViewMatrix, const Vec3 & n);
		};
		
		class IViewport : implements Object {
			public:				
				virtual Mat44 viewMatrix() const abstract;
				virtual Mat44 projectionMatrix() const abstract;
				virtual AlignedBox<2> bounds() const abstract;
				
				Mat44 display_matrix() const {
					return projectionMatrix() * viewMatrix();
				}
			
				ViewportEyeSpace convertToObjectSpace(const Vec2 & point);
		};
		
		class Viewport : public Object, implements IViewport {
		protected:
			bool m_sizeChangedSinceLastUpdate;
			AlignedBox<2> m_bounds;
			
			REF(ICamera) m_camera;
			REF(IProjection) m_projection;
			
		public:
			Viewport(PTR(ICamera) camera, PTR(IProjection) projection);
			
			void setBounds(AlignedBox<2> bounds);
			
			REF(ICamera) camera() const;
			void setCamera(REF(ICamera) camera);
			
			REF(IProjection) projection() const;
			void setProjection(REF(IProjection) projection);
			
			virtual Mat44 viewMatrix() const;
			virtual Mat44 projectionMatrix() const;
			virtual AlignedBox<2> bounds() const;
		};
	}
}

#endif
