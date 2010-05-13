/*
 *  Geometry/Sphere.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 18/10/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#ifndef _DREAM_GEOMETRY_SPHERE_H
#define _DREAM_GEOMETRY_SPHERE_H

#include "Geometry.h"

namespace Dream {
	namespace Geometry {
		
		/**
		 A sphere is defined by its center point and its radius.
		 
		 It is fast for using with collision detection, as the comparison between a sphere and other geometric objects is typically very fast due to the
		 simple nature of a sphere.
		 */
		template <unsigned D, typename NumericT>
		class Sphere {
		protected:
			Vector<D, NumericT> m_center;
			NumericT m_radius;

			typedef typename RealType<NumericT>::RealT NumericRealT;

		public:
			/// Undefined constructor
			Sphere ();
			/// Zero constructor. Creates a sphere centered at coordinate zero, with radius zero.
			Sphere (const Zero &);
		    /// Identity constructor. Creates a sphere centered at coordinate zero, with radius n.
			Sphere (const Identity &, const NumericT & n = 1);
			
			/// Full constructor. Supply all parameters.
			Sphere(Vector<D, NumericT> center, NumericT radius);
			
			/// @returns the center of the sphere.
			const Vector<D, NumericT> & center () const;
			/// Sets the center of the sphere.
			void setCenter (const Vector<D> & center);
			
			/// @returns the radius of the sphere.
			const NumericT & radius () const;
			/// Sets the center of the sphere.
			void setRadius (const NumericT & radius);
			
			/// Displacement returns the vector between the two circles.
			NumericRealT distanceBetweenEdges (const Sphere<D, NumericT> & other, Vector<D, NumericT> & displacement) const;

			/// Displacement returns the vector between the two centers.
			NumericRealT distanceFromPoint (const Vector<D, NumericT> & point, Vector<D, NumericT> & displacement) const;
			
			/// Distance is the time along the Line (use pointAtTime).
			IntersectionResult intersectsWith (const Line<D, NumericT> &line, RealT & entryTime, RealT & exitTime) const;
			
			/// Distance is the time along the LineSegment (use pointAtTime).
			IntersectionResult intersectsWith (const LineSegment<D, NumericT> &line, RealT & entryTime, RealT & exitTime) const;
			
			/// Test between two spheres
			IntersectionResult intersectsWith (const Sphere & other, Vector<D, NumericT> & displacement) const;
			
			/// Test between a point and a sphere
			IntersectionResult intersectsWith (const Vector<D, NumericT> & point, Vector<D, NumericT> & displacement) const;
		};
		
		typedef Sphere<2> Sphere2;
		typedef Sphere<3> Sphere3;
	}
}

#include "Sphere.impl.h"

#endif