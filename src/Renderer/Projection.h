//
//  Renderer/Projection.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/06/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
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
			virtual Mat44 projection_matrix_for_viewport(const IViewport & viewport) abstract;
		};
		
		class FixedProjection : public Object, implements IProjection {
		protected:
			Mat44 _projection_matrix;
			
		public:
			FixedProjection(const Mat44 projection_matrix);
			
			virtual Mat44 projection_matrix_for_viewport(const IViewport & viewport);
		};
		
		class OrthographicProjection : public Object, implements IProjection {
		protected:
			AlignedBox<3> _box;
			
		public:
			/// The box provides the center and size for the orthographic projection.
			/// The size controls the bounds from -1 to +1 from left to right and top to bottom,
			/// along with the near and far planes.
			OrthographicProjection(const AlignedBox<3> & box);
			
			virtual void set_box(const AlignedBox<3> & box);
			const AlignedBox<3> & box() const;
			
			virtual Mat44 projection_matrix_for_viewport(const IViewport & viewport);
		};
		
		class ScaledOrthographicProjection : public OrthographicProjection {
		public:
			/// The scaled orthographic projection ensures that the box will always be the correct aspect ratio and visible.
			ScaledOrthographicProjection(const AlignedBox<3> & box);		
			
			virtual Mat44 projection_matrix_for_viewport(const IViewport & viewport);
		};
		
		class PerspectiveProjection : public Object, implements IProjection {
		protected:
			RealT _field_of_view;
			RealT _near, _far;
			
		public:
			PerspectiveProjection(RealT field_of_view, RealT near, RealT far);
			
			virtual void set_field_of_view(RealT field_of_view);
			RealT field_of_view() const;
			
			virtual Mat44 projection_matrix_for_viewport(const IViewport & viewport);
		};
		
	}
}

#endif
