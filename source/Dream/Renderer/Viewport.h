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
		struct ViewportEyeSpace {
			Vec3 origin;
			Line<3> forward;
			Vec3 up;

			/// Calculate the object-space coordinates when given the window's viewport and a point in the viewport.
			static ViewportEyeSpace convert_from_viewport_to_object_space (const Mat44 & projection_matrix, const Mat44 & model_view_matrix,
			                                                               const AlignedBox<2> & viewport, const Vec2 & c);

			/// Calculate the object-space coordinates when given a projection-space coordinate on the near plane.
			static ViewportEyeSpace convert_from_projection_space_to_object_space (const Mat44 & projection_matrix, const Mat44 & model_view_matrix, const Vec2 & n);

			/// Calculate the object-space coordinate when given a projection-space coordinate. This function is not fast for many points, as it calculates
			/// inverse matrices per call.
			static Vec4 convert_from_projection_space_to_object_space (const Mat44 & projection_matrix, const Mat44 & model_view_matrix, const Vec3 & n);
		};

		class IViewport : implements IObject {
		public:
			virtual Mat44 view_matrix() const abstract;
			virtual Mat44 projection_matrix() const abstract;
			virtual AlignedBox<2> bounds() const abstract;

			Mat44 display_matrix() const {
				return projection_matrix() * view_matrix();
			}

			ViewportEyeSpace convert_to_object_space(const Vec2 & point);
		};

		class Viewport : public Object, implements IViewport {
		protected:
			bool _bounds_changed;
			AlignedBox<2> _bounds;

			Ref<ICamera> _camera;
			Ref<IProjection> _projection;
			
			Mat44 _projection_matrix_cache;

		public:
			Viewport(Ptr<ICamera> camera, Ptr<IProjection> projection);

			void set_bounds(AlignedBox<2> bounds);

			Ref<ICamera> camera() const;
			void set_camera(Ref<ICamera> camera);

			Ref<IProjection> projection() const;
			void set_projection(Ref<IProjection> projection);

			virtual Mat44 view_matrix() const;
			virtual Mat44 projection_matrix() const;
			virtual AlignedBox<2> bounds() const;
		};
	}
}

#endif
