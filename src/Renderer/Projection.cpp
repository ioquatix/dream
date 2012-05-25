//
//  Renderer/Projection.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/06/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#include "Projection.h"
#include "Viewport.h"

namespace Dream
{
	namespace Renderer
	{
		
		IProjection::~IProjection () {
			
		}
		
		OrthographicProjection::OrthographicProjection(const AlignedBox<3> & box)
		: _box(box) {
			
		}
		
		FixedProjection::FixedProjection(const Mat44 projection_matrix)
		: _projection_matrix(projection_matrix) {
			
		}
		
		Mat44 FixedProjection::projection_matrix_for_viewport(const IViewport & viewport) {
			return _projection_matrix;
		}
		
		Mat44 OrthographicProjection::projection_matrix_for_viewport(const IViewport & viewport) {
			return orthographic_matrix<RealT>(_box.center(), _box.size());
		}
		
		void OrthographicProjection::set_box(const AlignedBox<3> & box) {
			_box = box;
		}
		
		const AlignedBox<3> & OrthographicProjection::box() const {
			return _box;
		}
		
		ScaledOrthographicProjection::ScaledOrthographicProjection(const AlignedBox<3> & box) : OrthographicProjection(box) {
			
		}
		
		Mat44 ScaledOrthographicProjection::projection_matrix_for_viewport(const IViewport & viewport) {
			Vec3 scaled_size = _box.size();
			RealT desired_aspect_ratio = scaled_size.reduce().aspect_ratio();
			RealT aspect_ratio = viewport.bounds().size().aspect_ratio();
			
			if (desired_aspect_ratio < aspect_ratio) {
				scaled_size[X] = scaled_size[Y] * aspect_ratio;
			} else {
				scaled_size[Y] = scaled_size[X] * (1.0 / aspect_ratio);
			}
			
			return orthographic_matrix<RealT>(_box.center(), scaled_size);
		}
		
		PerspectiveProjection::PerspectiveProjection(RealT field_of_view, RealT near, RealT far) : _field_of_view(field_of_view), _near(near), _far(far) {
			
		}
		
		Mat44 PerspectiveProjection::projection_matrix_for_viewport(const IViewport & viewport) {
			const Vec2 & size = viewport.bounds().size();
			RealT aspect_ratio = size[X] / size[Y];
			
			return perspective_matrix<RealT>(_field_of_view, aspect_ratio, _near, _far);
		}
		
		void PerspectiveProjection::set_field_of_view(RealT field_of_view) {
			_field_of_view = field_of_view;
		}
		
		RealT PerspectiveProjection::field_of_view() const {
			return _field_of_view;
		}
	}
}
