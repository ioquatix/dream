//
//  Geometry/Frustrum.impl.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/12/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_GEOMETRY_FRUSTUM_H
#error This header should not be included manually. Include Frustum.h instead.
#endif

namespace Dream
{
	namespace Geometry
	{
		template <typename NumericT>
		inline Plane<3, NumericT> convertPlaneFromMatrixEqn (const NumericT & a, const NumericT & b, const NumericT & c, NumericT distance)
		{
			Vector<3, NumericT> normal(a, b, c);
			
			// Pseudo-normalize all components
			NumericT m = normal.length();
			
			normal /= m;
			distance /= m;
			
			return Plane<3, NumericT>(distance, normal);
		}
		
		template <typename NumericT>
		void Frustum<NumericT>::buildFrustrumFromMatrix (const Matrix<4, 4, NumericT> & m)
		{
			// The constructor used for Plane will normalize its elements automatically
			
			// Left   (m3 + m0)
			m_planes[LEFT] = convertPlaneFromMatrixEqn(m.at(3,0)+m.at(0,0), m.at(3,1)+m.at(0,1), m.at(3,2)+m.at(0,2), (m.at(3,3)+m.at(0,3)));
			
			// Right  (m3 - m0)
			m_planes[RIGHT] = convertPlaneFromMatrixEqn(m.at(3,0)-m.at(0,0), m.at(3,1)-m.at(0,1), m.at(3,2)-m.at(0,2), (m.at(3,3)-m.at(0,3)));
			
			// Top    (m3 - m1)
			m_planes[TOP] = convertPlaneFromMatrixEqn(m.at(3,0)-m.at(1,0), m.at(3,1)-m.at(1,1), m.at(3,2)-m.at(1,2), (m.at(3,3)-m.at(1,3)));
			
			// Bottom (m3 + m1)
			m_planes[BOTTOM] = convertPlaneFromMatrixEqn(m.at(3,0)+m.at(1,0), m.at(3,1)+m.at(1,1), m.at(3,2)+m.at(1,2), (m.at(3,3)+m.at(1,3)));
			
			// Near
			m_planes[NEAR] = convertPlaneFromMatrixEqn(m.at(2,0), m.at(2,1), m.at(2,2), m.at(2,3));
			
			// Far    (m3 - m2)
			m_planes[FAR] = convertPlaneFromMatrixEqn(m.at(3,0)-m.at(2,0), m.at(3,1)-m.at(2,1), m.at(3,2)-m.at(2,2), (m.at(3,3)-m.at(2,3)));
		}
		
		template <typename NumericT>		
		bool Frustum<NumericT>::intersectsWith (const Sphere<3, NumericT> & s) const
		{
			for (unsigned i = 0; i < 6; ++i) {
				NumericT d = m_planes[i].distanceToPoint(s.center());
				
				if (d <= - s.radius())
					return false;
			}
			
			return true;
		}
		
		template <typename NumericT>
		bool Frustum<NumericT>::intersectsWith (const AlignedBox<3, NumericT> &b) const
		{
			return intersectsWith(b.boundingSphere());
		}
		
		/*	
		 bool Frustum::containsPoint (const Vec3 &p) const {
			for (unsigned i = 0; i < 6; ++i) {
				RealT d = m_planes[i].distanceToPoint(p);
		 
				if (d <= 0)
					return false;
			}
		 
			return true;
		 }
		 */
	}
}
