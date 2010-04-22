/*
 *  Geometry/Triangle.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 12/07/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#include "Triangle.h"

#include "AlignedBox.h"
#include "Plane.h"
#include "Line.h"

namespace Dream {
	namespace Geometry {
		
#pragma mark -
#pragma mark Unit Tests
		
#ifdef ENABLE_TESTING

		UNIT_TEST(Triangle) {
			testing("Construction");
			
			Triangle3 t(Vec3(ZERO), Vec3(5.0, 5.0, 5.0), Vec3(5.0, 5.0, 0.0));
			
			assertTrue(t.normal().dot(Vec3(-1, 1, 1)), "Normal is correct");
		}
		
#endif

	}
}