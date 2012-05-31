//
//  Numerics/Average.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 31/10/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#include "Average.h"

namespace Dream
{
	namespace Numerics
	{
// MARK: mark -
// MARK: mark Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(Average)
		{
			testing("Calculating");

			Average<int> s;
			s += 5;
			s += 15;

			check(s.average() == 10) << "Average calculated correctly";

			Average<int> q;
			q += 20;
			q += 20;
			q += s;

			check(q.average() == 15) << "Average calculated correctly";
		}
#endif
	}
}
