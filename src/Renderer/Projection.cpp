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
		: m_box(box) {
			
		}
		
		FixedProjection::FixedProjection(const Mat44 projectionMatrix)
		: m_projectionMatrix(projectionMatrix) {
			
		}
		
		Mat44 FixedProjection::projectionMatrixForViewport(const IViewport & viewport) {
			return m_projectionMatrix;
		}
		
		Mat44 OrthographicProjection::projectionMatrixForViewport(const IViewport & viewport) {
			return orthographicMatrix<RealT>(m_box.center(), m_box.size());
		}
		
		void OrthographicProjection::setBox(const AlignedBox<3> & box) {
			m_box = box;
		}
		
		const AlignedBox<3> & OrthographicProjection::box() const {
			return m_box;
		}
		
		ScaledOrthographicProjection::ScaledOrthographicProjection(const AlignedBox<3> & box, IndexT scaleAxis) 
		: OrthographicProjection(box), m_scaleAxis(scaleAxis){
			
		}
		
		Mat44 ScaledOrthographicProjection::projectionMatrixForViewport(const IViewport & viewport) {
			Vec3 scaledSize = m_box.size();
			RealT aspectRatio = viewport.bounds().size().aspectRatio();
			
			if (m_scaleAxis == X) {
				scaledSize[X] *= aspectRatio;
			} else if (m_scaleAxis == Y) {
				scaledSize[Y] *= 1.0 / aspectRatio;
			}
			
			return orthographicMatrix<RealT>(m_box.center(), scaledSize);
		}
		
		PerspectiveProjection::PerspectiveProjection(RealT fieldOfView, RealT near, RealT far)
		: m_fieldOfView(fieldOfView), m_near(near), m_far(far) {
			
		}
		
		Mat44 PerspectiveProjection::projectionMatrixForViewport(const IViewport & viewport) {
			const Vec2 & size = viewport.bounds().size();
			RealT aspectRatio = size[X] / size[Y];
			
			return perspectiveMatrix<RealT>(m_fieldOfView, aspectRatio, m_near, m_far);
		}
		
		void PerspectiveProjection::setFieldOfView(RealT fieldOfView) {
			m_fieldOfView = fieldOfView;
		}
		
		RealT PerspectiveProjection::fieldOfView() const {
			return m_fieldOfView;
		}
	}
}
