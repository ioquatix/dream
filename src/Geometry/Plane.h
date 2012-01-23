//
//  Geometry/Plane.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 23/09/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_GEOMETRY_PLANE_H
#define _DREAM_GEOMETRY_PLANE_H

#include "Geometry.h"

namespace Dream {
	namespace Geometry {
	
		template <unsigned D, typename NumericT>
		class Plane {
	protected:
			typedef Vector<D, NumericT> VectorT;
			
			NumericT m_distance;
			VectorT m_normal;
			
			void convertFromPointNormalForm (const VectorT & point, const VectorT & normal)
			{
				m_normal = normal;
				m_distance = (-normal).dot(point);
			}
	public:
			Plane ()
			{
			}
			
			/// Point is a point on the plain, and direction is the normal
			Plane (const Line<D, NumericT> & line)
			{
				convertFromPointNormalForm(line.point(), line.direction());
			}
			
			Plane (const VectorT & point, const VectorT & normal)
			{
				convertFromPointNormalForm(point, normal);
			}
			
			Plane (const NumericT & d, const VectorT & n) : m_distance(d), m_normal(n)
			{
			}
			
			const RealT & distance() const { return m_distance; }
			const VectorT & normal() const { return m_normal; }
			
			void setDistance(const RealT & r) { m_distance = r; }
			void setNormal(const VectorT & n) { m_normal = n; }

			bool isParallel(const Plane & other) const;
			
			bool intersectsWith (const Plane & plane, Line<3, NumericT> & line) const;
			bool intersectsWith (const Line<3, NumericT> & line, VectorT & at) const;
			
			/// Finds the closed point on a plane to another point
			VectorT closestPoint (const VectorT & point) const
			{
				Vec3 at;
				
				intersectsWith(Line<3,NumericT>(point, m_normal), at);
				
				return at;	
			}
			
			IntersectionResult intersectsWith (const Sphere<D, NumericT> & sphere) const;
			
			/// Can be used to test sphere intersection
			NumericT distanceToPoint (const VectorT &at) const
			{
				/* Because the normal is normalized, it will always be 1.0, however it is possible to generalize
				 this algorithm for planes of the form ax+by+cz+d */
				
				return (m_normal.dot(at) + m_distance) /* / m_normal.length()*/;	
			}
		};
		
		template <unsigned D, typename NumericT>
		std::ostream &operator<< (std::ostream &out, const Plane<D, NumericT> & p);

		typedef Plane<2, RealT> Plane2;
		typedef Plane<3, RealT> Plane3;
	}
}

#include "Plane.impl.h"

#endif
