//
//  Geometry/Plane.impl.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/12/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_GEOMETRY_PLANE_H
#error This header should not be included manually. Include Plane.h instead.
#endif

namespace Dream
{
	namespace Geometry
	{
		template <unsigned D, typename NumericT>
		bool Plane<D, NumericT>::isParallel (const Plane<D, NumericT> & other) const
		{
			return m_normal == other.m_normal || m_normal == (- other.m_normal);
		}

		template <unsigned D, typename NumericT>
		bool Plane<D, NumericT>::intersectsWith (const Plane<D, NumericT> & other, Line<3, NumericT> & line) const
		{
			/* Planes are parallel? */
			if (other.normal() == m_normal)
				return false;
			
			VectorT u = m_normal.cross(other.m_normal).normalize();
			
			line.setDirection(u);
			line.setPoint(- ((m_normal*other.m_distance) - (other.m_normal*m_distance)).cross(u) / u.length2());
			
			return true;	
		}
		
		template <unsigned D, typename NumericT>
		bool Plane<D, NumericT>::intersectsWith (const Line<3, NumericT> & line, VectorT & at) const
		{
			NumericT d = m_normal.dot(line.direction());
			
			/* Line and Plane are parallel? */
			if (d == 0.0) return false;
			
            // This minus sign may need to be inside the (-m_normal)
			NumericT r = -(m_normal.dot(line.point()) - m_distance);
			NumericT t = r / d;
			
			at = line.point() + line.direction() * t;
			
			return true;
		}
		
		template <unsigned D, typename NumericT>
		IntersectionResult Plane<D, NumericT>::intersectsWith (const Sphere<D, NumericT> & sphere) const
		{
			NumericT d = distanceToPoint(sphere.center());
			
			if (d > sphere.radius())
				return NO_INTERSECTION;
			else if (Number<NumericT>::equivalent(d, sphere.radius()))
				return EDGES_INTERSECT;
			else
				return SHAPES_INTERSECT;
		}

		template <unsigned D, typename NumericT>
		std::ostream &operator<<(std::ostream &out, const Plane<D, NumericT> & p) {
			return out << "norm: " << p.normal() << " d: " << p.distance();
		}		
		
	}
}
