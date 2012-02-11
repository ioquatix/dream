//
//  Renderer/Viewport.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/06/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#include "Viewport.h"

namespace Dream
{
	namespace Renderer
	{

		ViewportEyeSpace ViewportEyeSpace::convert_from_viewport_to_object_space (const Mat44 & projection_matrix, const Mat44 & model_view_matrix, 
																   const AlignedBox<2> & viewport, const Vec2 & c)
		{
			// Reverse the viewport transformation
			Vec2 n;
			
			// Calculate position in normal projection box
			n[X] = ((c[X] - viewport.origin()[X]) / viewport.size()[X]) * 2.0 - 1.0;
			n[Y] = ((c[Y] - viewport.origin()[Y]) / viewport.size()[Y]) * 2.0 - 1.0;
			
			return convert_from_projection_space_to_object_space (projection_matrix, model_view_matrix, n);
		}
		
		ViewportEyeSpace ViewportEyeSpace::convert_from_projection_space_to_object_space (const Mat44 & projection_matrix, const Mat44 & model_view_matrix, const Vec2 & _n)
		{
			// Reverse the viewport transformation from normalized device coordinates back into object space
			Vec3 n = _n << -1;
			
			ViewportEyeSpace result;
			
			Mat44 inverse_model_view(model_view_matrix.inverse_matrix());
			Mat44 inverse_projection(projection_matrix.inverse_matrix());
			
			result.origin = Vec3(inverse_model_view.at(3,0), inverse_model_view.at(3,1), inverse_model_view.at(3,2));

			Vec4 p1 = (inverse_model_view * (inverse_projection * (n << 1.0)));
			
			n[Z] = 1;
			Vec4 p2 = (inverse_model_view * (inverse_projection * (n << 1.0)));
							
			p1 /= p1[W];
			p2 /= p2[W];
			
			result.forward = Line<3>(p1.reduce(), (p2 - p1).reduce().normalize());
			
			// Calculate up direction
			Vec3 eye_up(0.0, 1.0, 0.0);
			// Can't remember why we set these elements to zero...?
			//inverse_model_view.at(3,0) = inverse_model_view.at(3,1) = inverse_model_view.at(3,2) = 0.0;
			//inverse_model_view.at(0,3) = inverse_model_view.at(1,3) = inverse_model_view.at(2,3) = 0.0;
			result.up = inverse_model_view * eye_up;
			
			return result;
		}
		
		Vec4 ViewportEyeSpace::convert_from_projection_space_to_object_space (const Mat44 & projection_matrix, const Mat44 & model_view_matrix, const Vec3 & n)
		{
			Mat44 inverse_model_view(model_view_matrix.inverse_matrix());
			Mat44 inverse_projection(projection_matrix.inverse_matrix());
			
			return (inverse_model_view * (inverse_projection * (n << 1.0)));
		}
		
#pragma mark -

		ViewportEyeSpace IViewport::convert_to_object_space(const Vec2 & point) {
			return ViewportEyeSpace::convert_from_viewport_to_object_space(projection_matrix(), view_matrix(), bounds(), point);
		}
		
#pragma mark -
		
		Viewport::Viewport(Ptr<ICamera> camera, Ptr<IProjection> projection) : _bounds(ZERO, ZERO), _camera(camera), _projection(projection) {
			
		}
		
		void Viewport::set_bounds(AlignedBox<2> bounds) {
			_bounds = bounds;
		}
		
		Ref<ICamera> Viewport::camera() const {
			return _camera;
		}
		
		void Viewport::set_camera(Ref<ICamera> camera) {
			_camera = camera;
		}
		
		Ref<IProjection> Viewport::projection() const {
			return _projection;
		}
		
		void Viewport::set_projection(Ref<IProjection> projection) {
			_projection = projection;
		}
		
		Mat44 Viewport::view_matrix() const {
			return _camera->view_matrix();
		}
		
		Mat44 Viewport::projection_matrix() const {
			return _projection->projection_matrix_for_viewport(*this);
		}
		
		AlignedBox<2> Viewport::bounds() const {
			return _bounds;
		}
		
	}
}
