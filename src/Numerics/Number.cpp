/*
 *  Numerics/Number.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 31/10/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#include "Number.h"

namespace Dream
{
	namespace Numerics
	{
#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(Number)
		{
			testing("Clamping");

			assertEqual(Number<int>::clamp(10, 0, 5), 5, "Number clamped at top");
			assertEqual(Number<int>::clamp(-10, -5, 5), -5, "Number clamped at bottom");
			assertEqual(Number<int>::clamp(2, -5, 5), 2, "Number not clamped");

			assertEqual(Number<float>::clampTop(25, 20), 20, "Number clamped at top");
			assertEqual(Number<float>::clampTop(15, 20), 15, "Number not clamped");

			assertEqual(Number<unsigned>::clampBottom(15, 20), 20, "Number clamped at top");
			assertEqual(Number<unsigned>::clampBottom(25, 20), 25, "Number not clamped");

			testing("Min and Max");

			assertEqual(Number<float>::min(5.5, 4.5), 4.5, "Minimum selected correctly");
			assertEqual(Number<float>::max(5.5, 4.5), 5.5, "Minimum selected correctly");

			testing("Reciprocal");

			assertEqual(Number<float>::recip(10), 0.1f, "Reciprocal calculated correctly");

			testing("Wrap");

			assertEqual(Number<int>::wrap(12, 5), 2, "Number wrapped correctly");
			assertEqual(Number<int>::wrap(-12, 5), 3, "Number wrapped correctly");

			testing("Floor Value");

			assertEqual(Number<float>::floor(5.4), 5.0, "Number rounded down");
			assertEqual(Number<float>::floor(-5.4), -6.0, "Number rounded down");

			testing("Ceiling Value");

			assertEqual(Number<float>::ceil(-5.4), -5.0, "Number rounded up");
			assertEqual(Number<float>::ceil(5.4), 6.0, "Number rounded up");

			testing("Absolute Value");

			assertEqual(Number<double>::abs(-1), 1, "Number inverted");
			assertEqual(Number<double>::abs(4), 4, "Number not inverted");

			testing("Trigonometry");

			assertTrue(equivalent(Number<float>::sin(0.0), 0.0f), "Correct result from sin");
			assertTrue(equivalent(Number<float>::cos(R90), 0.0f), "Correct result from cos");
			assertTrue(equivalent(Number<float>::acos(0.0), (float)R90), "Correct result from acos");
		}
#endif
	}
}

#ifdef ENABLE_TESTING
namespace Dream
{
	namespace Core
	{
		bool testEquivalency (const float & lhs, const float & rhs)
		{
			return Numerics::Number<float>::equivalent(lhs, rhs);
		}
		
		bool testEquivalency (const double & lhs, const double & rhs)
		{
			return Numerics::Number<double>::equivalent(lhs, rhs);
		}
	}
}
#endif
