/*
 *  Geometry/Sphere.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 18/10/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#include "Sphere.h"
#include "../Numerics/Number.h"
#include "../Numerics/Vector.h"

namespace Dream {
	namespace Geometry {
		
#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
		
		UNIT_TEST(Sphere) {
			testing("Sphere-Sphere Intersection");
			
			Sphere<2> a(Vec2(-10, 0), 10);
			Sphere<2> b(Vec2(  0, 0), 10);
			Sphere<2> c(Vec2( 20, 0), 10);
			
			Vec2 d;
			check(a.intersectsWith(b, d) == SHAPES_INTERSECT) << "Spheres overlap";
			check(d.length() == 10.0) << "Spheres displaced by the distance between their centers"; 
			
			check(b.intersectsWith(c, d) == EDGES_INTERSECT) << "Sphere's edges overlap";
			check(d.length() == 20.0) << "Spheres displaced by the distance between their centers";
			
			testing("Sphere-Point Intersection");
			
			Vec2 p(5.0, 0.0);
			check(b.intersectsWith(p, d) == SHAPES_INTERSECT) << "Point lies within sphere";
			check(a.intersectsWith(p, d) == NO_INTERSECTION) << "Point lies outside of sphere";
			
			testing("Sphere-Line Intersection");
			
			RealT t1, t2;
			LineSegment<2> segmentA(Vec2(-50, 0), Vec2(50, 0));
			LineSegment<2> segmentB(Vec2(-50, -10), Vec2(50, -10));
			
			check(b.intersectsWith(segmentA, t1, t2) == SHAPES_INTERSECT) << "Line and sphere overlap at two points";
			check(segmentA.pointAtTime(t1).equivalent(Vec2(-10.0, 0))) << "Line intersects surface of sphere";
			check(segmentA.pointAtTime(t2).equivalent(Vec2(10.0, 0))) << "Line intersects surface of sphere";
			
			check(b.intersectsWith(segmentB, t1, t2) == EDGES_INTERSECT) << "Line and sphere touch at one point";
			check(segmentB.pointAtTime(t1).equivalent(Vec2(0, -10.0))) << "Line intersects surface of sphere";
		}

#endif

	}
}