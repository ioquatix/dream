//
//  Geometry/Sphere.impl.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 16/11/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_GEOMETRY_SPHERE_H
#error This header should not be included manually. Include Sphere.h instead.
#endif

#include "Line.h"

namespace Dream
{
	namespace Geometry
	{
		template <unsigned D, typename NumericT>
		Sphere<D, NumericT>::Sphere ()
		{
		}
		
		template <unsigned D, typename NumericT>
		Sphere<D, NumericT>::Sphere (const Zero &) : m_center(ZERO), m_radius(0)
		{
		}
		
		template <unsigned D, typename NumericT>
		Sphere<D, NumericT>::Sphere (const Identity &, const NumericT & n) : m_center(ZERO), m_radius(n)
		{
		}
		
		template <unsigned D, typename NumericT>
		Sphere<D, NumericT>::Sphere(Vector<D, NumericT> center, NumericT radius) : m_center(center), m_radius(radius)
		{
		}

		template <unsigned D, typename NumericT>
		const Vector<D, NumericT> & Sphere<D, NumericT>::center () const
		{
			return m_center;
		}
		
		template <unsigned D, typename NumericT>
		void Sphere<D, NumericT>::setCenter (const Vector<D> & center)
		{
			m_center = center;
		}

		template <unsigned D, typename NumericT>
		const NumericT & Sphere<D, NumericT>::radius () const
		{
			return m_radius;
		}
		
		template <unsigned D, typename NumericT>
		void Sphere<D, NumericT>::setRadius (const NumericT & radius)
		{
			m_radius = radius;
		}
		
		template <unsigned D, typename NumericT>
		typename RealType<NumericT>::RealT Sphere<D, NumericT>::distanceBetweenEdges (const Sphere<D, NumericT> & other, Vector<D, NumericT> & displacement) const
		{
			displacement = m_center - other.m_center;
			typename RealType<NumericT>::RealT total_radius = m_radius + other.m_radius;
			
			return displacement.length() - total_radius;
		}
		
		template <unsigned D, typename NumericT>
		typename RealType<NumericT>::RealT Sphere<D, NumericT>::distanceFromPoint (const Vector<D, NumericT> & point, Vector<D, NumericT> & displacement) const
		{
			displacement = point - m_center;
			
			return displacement.length() - m_radius;
		}
		
		template <unsigned D, typename NumericT>
		IntersectionResult Sphere<D, NumericT>::intersectsWith (const Sphere<D, NumericT> & other, Vector<D, NumericT> & displacement) const
		{
			typename RealType<NumericT>::RealT d = distanceBetweenEdges(other, displacement);
			
			if (d < 0.0) {
				return SHAPES_INTERSECT;
			} else if (d == 0.0) {
				return EDGES_INTERSECT;
			} else {
				return NO_INTERSECTION;
			}
		}

		template <unsigned D, typename NumericT>
		IntersectionResult Sphere<D, NumericT>::intersectsWith (const Vector<D, NumericT> & point, Vector<D, NumericT> & displacement) const
		{
			typename RealType<NumericT>::RealT d = distanceFromPoint(point, displacement);
			
			if (d < 0.0) {
				return SHAPES_INTERSECT;
			} else if (d == 0.0) {
				return EDGES_INTERSECT;
			} else {
				return NO_INTERSECTION;
			}
		}

		template <unsigned D, typename NumericT>
		IntersectionResult Sphere<D, NumericT>::intersectsWith (const Line<D, NumericT> &line, RealT & entryTime, RealT & exitTime) const
		{
			//Optimized method sphere/ray intersection
			Vector<D> dst(line.point() - m_center);
			
			RealT b = dst.dot(line.direction());
			RealT c = dst.dot(dst) - (m_radius * m_radius);
			
			// If d is negative there are no real roots, so return 
			// false as ray misses sphere
			RealT d = b * b - c;
			
			if (d == 0.0) {
				entryTime = (-b) - Number<NumericT>::sqrt(d);
				exitTime = entryTime;
				return EDGES_INTERSECT;				
			} if (d > 0) {
				entryTime = (-b) - Number<NumericT>::sqrt(d);
				exitTime = (-b) + Number<NumericT>::sqrt(d);
				return SHAPES_INTERSECT;
			} else {
				return NO_INTERSECTION;
			}
		}
		
		template <unsigned D, typename NumericT>
		IntersectionResult Sphere<D, NumericT>::intersectsWith (const LineSegment<D, NumericT> &segment, RealT & entryTime, RealT & exitTime) const
		{
			Line<D> line = segment.toLine();
			
			IntersectionResult result = intersectsWith(line, entryTime, exitTime);
			
			// timeAtIntersection is the time in unit length from segment.start() in direction segment.offset()
			// We will normalize the time in units of segment.offset()s, so that segment.pointAtTime(timeAtIntersection)
			// works as expected
			
			RealT n = segment.offset().length();
			entryTime = entryTime / n;
			exitTime = exitTime / n;
			
			// The line segment intersects the sphere at one or two points:
			if ((entryTime >= 0.0 && entryTime <= 1.0) || (exitTime >= 0.0 && exitTime <= 1.0)) {
				return result;
			}
			
			// Line segment is completely within sphere:
			if (entryTime < 0.0 && exitTime > 1.0) {
				return SHAPE_EMBEDDED;
			}
			
			// The line segment did not intersect.
			return NO_INTERSECTION;
		}
	}
}
