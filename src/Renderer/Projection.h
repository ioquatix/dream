//
//  Projection.h
//  Dream
//
//  Created by Samuel Williams on 5/06/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

#ifndef _DREAM_RENDERER_PROJECTION_H
#define _DREAM_RENDERER_PROJECTION_H

#include "Renderer.h"
#include "../Numerics/Matrix.h"
#include "../Geometry/AlignedBox.h"

namespace Dream
{
	namespace Renderer
	{
		using namespace Dream::Numerics;
		using namespace Dream::Geometry;
		
		class IViewport;
		
		class IProjection : implements Object {
		public:
			virtual ~IProjection();
			// This function must calculate an appropriate projection matrix for the given viewport.
			// The viewport provides information such as size, etc.
			virtual Mat44 projectionMatrixForViewport(const IViewport & viewport) abstract;
		};
		
		class FixedProjection : public Object, implements IProjection {
		protected:
			Mat44 m_projectionMatrix;
			
		public:
			FixedProjection(const Mat44 projectionMatrix);
			
			virtual Mat44 projectionMatrixForViewport(const IViewport & viewport);
		};
		
		class OrthographicProjection : public Object, implements IProjection {
		protected:
			AlignedBox<3> m_box;
			
		public:
			/// The box provides the center and size for the orthographic projection.
			/// The size controls the bounds from -1 to +1 from left to right and top to bottom,
			/// along with the near and far planes.
			OrthographicProjection(const AlignedBox<3> & box);
			
			virtual void setBox(const AlignedBox<3> & box);
			const AlignedBox<3> & box() const;
			
			virtual Mat44 projectionMatrixForViewport(const IViewport & viewport);
		};
		
		class ScaledOrthographicProjection : public OrthographicProjection {
		protected:
			IndexT m_scaleAxis;
			
		public:
			/// Provide a scaleAxis to control how the orthographic projection should be resized
			/// when the window is non-square.
			ScaledOrthographicProjection(const AlignedBox<3> & box, IndexT scaleAxis);		
			
			virtual Mat44 projectionMatrixForViewport(const IViewport & viewport);
		};
		
		class PerspectiveProjection : public Object, implements IProjection {
		protected:
			RealT m_fieldOfView;
			RealT m_near, m_far;
			
		public:
			PerspectiveProjection(RealT fieldOfView, RealT near, RealT far);
			
			virtual void setFieldOfView(RealT fieldOfView);
			RealT fieldOfView() const;
			
			virtual Mat44 projectionMatrixForViewport(const IViewport & viewport);
		};
		
	}
}

#endif
