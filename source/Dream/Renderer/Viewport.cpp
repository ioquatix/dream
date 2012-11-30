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
		IViewport::~IViewport()
		{

		}

		Eye<> IViewport::convert_to_object_space(const Vec2 & point)
		{
			auto transformation = Euclid::Geometry::eye_transformation(projection_matrix(), view_matrix());

			return transformation.convert_from_viewport_space_to_object_space(bounds(), point);
		}

		Viewport::Viewport(Ptr<ICamera> camera, Ptr<IProjection> projection) : _bounds_changed(true), _bounds(ZERO, ZERO), _camera(camera), _projection(projection) {
		}

		void Viewport::set_bounds(AlignedBox<2> bounds) {
			_bounds = bounds;
			
			_projection_matrix_cache = _projection->projection_matrix_for_viewport(*this);
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
			return _projection_matrix_cache;
		}

		AlignedBox<2> Viewport::bounds() const {
			return _bounds;
		}

// MARK: -
// MARK: Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(Viewport)
		{
			testing("Projections");

			Ref<Camera> camera = new Camera(IDENTITY);
			Ref<IProjection> projection = new OrthographicProjection(AlignedBox3::from_center_and_size(ZERO, 20.0));
			Ref<Viewport> viewport = new Viewport(camera, projection);

			{
				viewport->set_bounds(AlignedBox2::from_center_and_size(ZERO, 20.0));
				Vec3 r = viewport->display_matrix() * Vec3(-10, -10, 0);
				check(r.equivalent(Vec3(-1, -1, 0)));
			}

			{
				// Changing the size of the viewport doesn't change the size of the NDC box.
				viewport->set_bounds(AlignedBox2::from_center_and_size(ZERO, 40.0));
				Vec3 r = viewport->display_matrix() * Vec3(-10, -10, 0);
				check(r.equivalent(Vec3(-1, -1, 0)));
			}

			viewport->set_camera(new Camera(Mat44::translating_matrix(Vec3(10, 10, 0))));

			{
				Vec3 r = viewport->display_matrix() * Vec3(-10, -10, 0);
				check(r.equivalent(Vec3(0, 0, 0)));
			}
		}
#endif
	}
}
