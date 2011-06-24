//
//  Viewport.h
//  Dream
//
//  Created by Samuel Williams on 5/06/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

#include "../Display/Layer.h"
#include "Projection.h"
#include "Camera.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
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
			
			class IViewport : implements Object, implements ILayer {
				public:
					virtual Mat44 viewMatrix() const abstract;
					virtual Mat44 projectionMatrix() const abstract;
					virtual AlignedBox<2> bounds() const abstract;
					
					ViewportEyeSpace convertToObjectSpace(const Vec2 & point);
			};
			
			class Viewport : public Group, implements IViewport {
				protected:
					bool m_sizeChangedSinceLastUpdate;
					AlignedBox<2> m_bounds;
					
					REF(ICamera) m_camera;
					REF(IProjection) m_projection;
				
				public:
					Viewport(PTR(ICamera) camera, PTR(IProjection) projection);
					
					REF(ICamera) camera() const;
					void setCamera(REF(ICamera) camera);
					
					REF(IProjection) projection() const;
					void setProjection(REF(IProjection) projection);
					
					// Viewport doesn't pass resize events to children.
					bool resize(const ResizeInput & resizeInput);
					
					bool process(const Input & input);
					void renderFrameForTime (IScene * scene, TimeT time);
					
					virtual Mat44 viewMatrix() const;
					virtual Mat44 projectionMatrix() const;
					virtual AlignedBox<2> bounds() const;
			};
			
		}
	}
}
