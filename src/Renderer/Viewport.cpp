//
//  Viewport.cpp
//  Dream
//
//  Created by Samuel Williams on 5/06/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

#include "Viewport.h"

namespace Dream
{
	namespace Renderer
	{

		ViewportEyeSpace ViewportEyeSpace::convertFromViewportToObjectSpace (const Mat44 & projectionMatrix, const Mat44 & modelViewMatrix, 
																   const AlignedBox<2> & viewport, const Vec2 & c)
		{
			// Reverse the viewport transformation
			Vec2 n;
			
			// Calculate position in normal projection box
			n[X] = ((c[X] - viewport.origin()[X]) / viewport.size()[X]) * 2.0 - 1.0;
			n[Y] = ((c[Y] - viewport.origin()[Y]) / viewport.size()[Y]) * 2.0 - 1.0;
			
			return convertFromProjectionSpaceToObjectSpace (projectionMatrix, modelViewMatrix, n);
		}
		
		ViewportEyeSpace ViewportEyeSpace::convertFromProjectionSpaceToObjectSpace (const Mat44 & projectionMatrix, const Mat44 & modelViewMatrix, const Vec2 & _n)
		{
			// Reverse the viewport transformation from normalized device coordinates back into object space
			Vec3 n = _n << -1;
			
			ViewportEyeSpace result;
			
			Mat44 inverseModelView(modelViewMatrix.inverseMatrix());
			Mat44 inverseProjection(projectionMatrix.inverseMatrix());
			
			result.origin = Vec3(inverseModelView.at(3,0), inverseModelView.at(3,1), inverseModelView.at(3,2));

			Vec4 p1 = (inverseModelView * (inverseProjection * (n << 1.0)));
			
			n[Z] = 1;
			Vec4 p2 = (inverseModelView * (inverseProjection * (n << 1.0)));
							
			p1 /= p1[W];
			p2 /= p2[W];
			
			result.forward = Line<3>(p1.reduce(), (p2 - p1).reduce().normalize());
			
			// Calculate up direction
			Vec3 eyeUp(0.0, 1.0, 0.0);
			// Can't remember why we set these elements to zero...?
			//inverseModelView.at(3,0) = inverseModelView.at(3,1) = inverseModelView.at(3,2) = 0.0;
			//inverseModelView.at(0,3) = inverseModelView.at(1,3) = inverseModelView.at(2,3) = 0.0;
			result.up = inverseModelView * eyeUp;
			
			return result;
		}
		
		Vec4 ViewportEyeSpace::convertFromProjectionSpaceToObjectSpace (const Mat44 & projectionMatrix, const Mat44 & modelViewMatrix, const Vec3 & n)
		{
			Mat44 inverseModelView(modelViewMatrix.inverseMatrix());
			Mat44 inverseProjection(projectionMatrix.inverseMatrix());
			
			return (inverseModelView * (inverseProjection * (n << 1.0)));
		}
		
#pragma mark -

		ViewportEyeSpace IViewport::convertToObjectSpace(const Vec2 & point) {
			return ViewportEyeSpace::convertFromViewportToObjectSpace(projectionMatrix(), viewMatrix(), bounds(), point);
		}
		
#pragma mark -
		
		Viewport::Viewport(PTR(ICamera) camera, PTR(IProjection) projection) : m_bounds(ZERO, ZERO), m_camera(camera), m_projection(projection) {
			
		}
		
		void Viewport::setBounds(AlignedBox<2> bounds) {
			m_bounds = bounds;
		}
		
		REF(ICamera) Viewport::camera() const {
			return m_camera;
		}
		
		void Viewport::setCamera(REF(ICamera) camera) {
			m_camera = camera;
		}
		
		REF(IProjection) Viewport::projection() const {
			return m_projection;
		}
		
		void Viewport::setProjection(REF(IProjection) projection) {
			m_projection = projection;
		}
		
		Mat44 Viewport::viewMatrix() const {
			return m_camera->viewMatrix();
		}
		
		Mat44 Viewport::projectionMatrix() const {
			return m_projection->projectionMatrixForViewport(*this);
		}
		
		AlignedBox<2> Viewport::bounds() const {
			return m_bounds;
		}
		
	}
}
