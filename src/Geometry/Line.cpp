/*
 *  Geometry/Line.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 15/03/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#include "Line.h"

namespace Dream {
	namespace Geometry {
		
#pragma mark -
#pragma mark Unit Tests
		
#ifdef ENABLE_TESTING
		UNIT_TEST(Line)
		{
			testing("Intersections");
			
			Line3 a(Vec3(0, 0, 0), Vec3(10, 5, 3).normalize());
			Line3 b(Vec3(10, 5, 3), Vec3(5, 2, -8).normalize());
			
			RealT ta, tb;
			assertTrue(a.intersectsWith(b, ta, tb), "Intersection exists");
			
			Line2 c(Vec2(0, 0), Vec2(1, 1).normalize());
			Line2 d(Vec2(1, 0), Vec2(-1, 1).normalize());
			
			assertTrue(c.intersectsWith(d, ta, tb), "Intersection exists");
			assertTrue(c.pointAtTime(ta).equivalent(Vec2(0.5, 0.5)), "Intersection at correct point");
			
			testing("Transformations");
			
			Vec3 norm(0, 0, 1);
			Vec3 v1(10, 0, 0);
			a.setPoint(Vec3(10, 0, 0));
			a.setDirection(Vec3(1, 0, 0));
			
			b.setPoint(Vec3(ZERO));
			b.setDirection(Vec3(-1, 0, 0));
			
			Mat44 mat = a.transformationToMateWithLine(b, norm);
			Vec3 r = mat * v1;
			
			assertTrue(r.equivalent(Vec3(ZERO)), "Transformed vertex is correct");
			
			v1 *= 2;
			r = mat * v1;
			
			assertTrue(r.equivalent(Vec3(-10, 0, 0)), "Transformed vertex is correct");
			
			//void rotate (const VectorT & rotationNormal, const NumericRealT & angle);
			//LineT rotatedLine (const VectorT & rotationNormal, const NumericRealT & angle) const;
			
			Line3 q = a.rotatedLine(norm, R90);
			
			assertTrue(q.direction().equivalent(Vec3(0, -1, 0)), "Rotated line is correct");
		}
		
		UNIT_TEST(LineSegment)
		{
			testing("Constructors");

			LineSegment3 a(Vec3(ZERO), Vec3(IDENTITY, 10));
			assertEqual(a.offset().length2(), (10 * 10) * 3, "Line is correct length");
			
			LineSegment3 d(ZERO);
			assertTrue(d.isDegenerate(), "Line segment is degenerate");
			
			Line3 la(IDENTITY, 1);
			LineSegment3 e(la, 10, 30);
			assertTrue(e.offset().equivalent(Vec3(IDENTITY, 20)), "LineSegment is correct size");
			assertTrue(e.start().equivalent(Vec3(IDENTITY, 10)), "LineSegment starts at correct point");
			
			testing("Intersections");

			LineSegment3 b(Vec3(0, 10, 0), Vec3(10, 0, 10));
						
			RealT ta, tb;
			assertTrue(a.intersectsWith(b, ta, tb), "Line segments intersect");
			assertTrue(a.pointAtTime(ta).equivalent(Vec3(5, 5, 5)), "Intersection point is correct");
			
			LineSegment3 c(Vec3(ZERO), Vec3(IDENTITY, 10.1));
			assertFalse(a.intersectsWith(c, ta, tb), "Line segments don't intersect");
		}
#endif		
	}
}