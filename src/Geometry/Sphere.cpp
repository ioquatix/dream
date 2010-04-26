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
		
		UNIT_TEST(Sphere) {
			testing("Sphere-Sphere Intersection");
			
			Sphere<2> a(Vec2(-10, 0), 10);
			Sphere<2> b(Vec2(  0, 0), 10);
			Sphere<2> c(Vec2( 20, 0), 10);
			
			Vec2 d;
			assertTrue(a.intersectsWith(b, d) == SHAPES_INTERSECT, "Spheres overlap");
			assertEqual(d.length(), 10.0, "Spheres displaced by the distance between their centers");
			
			assertTrue(b.intersectsWith(c, d) == EDGES_INTERSECT, "Sphere's edges overlap");
			assertEqual(d.length(), 20.0, "Spheres displaced by the distance between their centers");
			
			testing("Sphere-Point Intersection");
			
			Vec2 p(5.0, 0.0);
			assertTrue(b.intersectsWith(p, d) == SHAPES_INTERSECT, "Point lies within sphere");
			assertTrue(a.intersectsWith(p, d) == NO_INTERSECTION, "Point lies outside of sphere");
			
			testing("Sphere-Line Intersection");
			
			RealT t1, t2;
			LineSegment<2> segmentA(Vec2(-50, 0), Vec2(50, 0));
			LineSegment<2> segmentB(Vec2(-50, -10), Vec2(50, -10));
			
			assertTrue(b.intersectsWith(segmentA, t1, t2) == SHAPES_INTERSECT, "Line and sphere overlap at two points");
			assertEquivalent(segmentA.pointAtTime(t1), Vec2(-10.0, 0), "Line intersects surface of sphere");
			assertEquivalent(segmentA.pointAtTime(t2), Vec2(10.0, 0), "Line intersects surface of sphere");
			
			assertTrue(b.intersectsWith(segmentB, t1, t2) == EDGES_INTERSECT, "Line and sphere touch at one point");
			assertEquivalent(segmentB.pointAtTime(t1), Vec2(0, -10.0), "Line intersects surface of sphere");
		}
	}
}