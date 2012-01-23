//
//  Geometry/Geometry.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 11/11/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#include "Geometry.h"

namespace Dream
{
	namespace Geometry
	{
		
#pragma mark -
#pragma mark Unit Tests
		
#ifdef ENABLE_TESTING
		UNIT_TEST(Shapes)
		{
			testing("Construction");
			
			typedef Shape<2, 4, int> shape_t;
			
			shape_t s;
			s[0] = vec<int>(-5, -5);
			s[1] = vec<int>(5, -5);
			s[2] = vec<int>(5, 5);
			s[3] = vec<int>(-5, 5);			
			
			check(s.center() == vec<int>(0) << 0) << "Center is located correctly";
		}
#endif
	}
}
