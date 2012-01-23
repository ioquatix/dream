//
//  Geometry/Intersection.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 17/11/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#include "Intersection.h"

namespace Dream
{
	namespace Geometry
	{
#pragma mark -
#pragma mark Unit Tests
		
#ifdef ENABLE_TESTING
		UNIT_TEST(Intersection)
		{
			testing("Segment Intersection");
			
			Vector<2, int> s1(0, 5), s2(2, 4), s3(3, 10), s4(-5, 10), s5(-10, -5), s6(5, 6), o;
			
			check(segments_intersect(s1, s2, o)) << "Segments overlap";
			check(o == s2) << "Segment overlap is correct";
			
			check(!segments_intersect(s1, s5, o)) << "Segments don't overlap";
			
			check(segments_intersect(s1, s3, o)) << "Segments overlap";
			check(o == vec(3, 5)) << "Segment overlap is correct";
			
			check(segments_intersect(s1, s4, o)) << "Segments overlap";
			check(o == s1) << "Segment overlap is correct";
			
			check(segments_intersect(s1, s6, o)) << "Segments overlap";
			check(o == vec(5, 5)) << "Segment overlap is correct";
		}
#endif
	}
}
