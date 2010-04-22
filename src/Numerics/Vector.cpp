/*
 *  Numerics/Vector.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 19/03/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#include "Vector.h"

#include <iostream>
#include <vector>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>

#include <set>

namespace Dream
{
	namespace Numerics
	{
#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
		template <unsigned D>
		bool checkSimilar (const Vector<D> & a, const Vector<D> & b)
		{
			return (b - a).sum() < 0.00001;
		}

		UNIT_TEST(Vector)
		{
			testing("Construction");

			Vector<1, int> b1(1);
			Vector<2, int> b2(1, 2);
			Vector<3, int> b3(1, 2, 3);
			Vector<4, int> b4(1, 2, 3, 4);

			assertEqual(b1.sum(), 1, "Vector components constructed correctly");
			assertEqual(b2.sum(), 3, "Vector components constructed correctly");
			assertEqual(b3.sum(), 6, "Vector components constructed correctly");
			assertEqual(b4.sum(), 10, "Vector components constructed correctly");

			b1 = vec(5);
			b2 = vec(5, 9);
			b3 = vec(5, 9, 17);
			b4 = vec(5, 9, 17, 29);

			assertEqual(b1.sum(), 5, "Vector components constructed correctly");
			assertEqual(b2.sum(), 5+9, "Vector components constructed correctly");
			assertEqual(b3.sum(), 5+9+17, "Vector components constructed correctly");
			assertEqual(b4.sum(), 5+9+17+29, "Vector components constructed correctly");

			b2.set(b4);

			assertEqual(b2[0], b4[0], "Vector components are equal");
			assertEqual(b2[1], b4[1], "Vector components are equal");

			Vector<4, float> f4;

			f4.set(b4);

			assertEqual(b4.sum(), f4.sum(), "Vector components copied correctly");

			f4.zero();

			assertEqual(f4.sum(), 0.0f, "Vector is zeroed");

			f4.loadIdentity();

			assertEqual(f4.sum(), 4.0f, "Vector is identity");

			f4 = 2.5;

			assertEqual(f4.sum(), 10.0f, "Vector was assigned to correctly.");

			f4 = b4;

			assertEqual(b4.sum(), f4.sum(), "Vector components copied correctly");

			assertEqual(f4, b4, "Vectors are equal");

			testing("Geometric Comparisons");

			assertTrue(vec(4, 5).lessThan(vec(6, 6)), "Vector comparison correct");
			assertFalse(vec(4, 5).lessThan(vec(4, 4)), "Vector comparison correct");

			assertTrue(vec(4, 5).lessThanOrEqual(vec(6, 6)), "Vector comparison correct");
			assertTrue(vec(4, 4).lessThanOrEqual(vec(4, 4)), "Vector comparison correct");
			assertFalse(vec(4, 5).lessThanOrEqual(vec(4, 4)), "Vector comparison correct");

			assertFalse(vec(4, 5).greaterThan(vec(6, 6)), "Vector comparison correct");
			assertFalse(vec(4, 5).greaterThan(vec(4, 4)), "Vector comparison correct");
			assertTrue(vec(5, 5).greaterThan(vec(4, 4)), "Vector comparison correct");

			assertFalse(vec(4, 5).greaterThanOrEqual(vec(6, 6)), "Vector comparison correct");
			assertTrue(vec(4, 5).greaterThanOrEqual(vec(4, 4)), "Vector comparison correct");
			assertTrue(vec(4, 4).greaterThanOrEqual(vec(4, 4)), "Vector comparison correct");
		}

		UNIT_TEST(VectorReflection)
		{
			using namespace std;
			using namespace boost;

			// 0:Wall Normal, 1:I, 2:R
			typedef tuple<Vec2, Vec2, Vec2> ReflectTestRecord;
			vector<ReflectTestRecord> reflectionTests = assign::tuple_list_of
			(Vec2(1.0, 0.0), Vec2(-1.0, -1.0), Vec2(1.0, -1.0))
			(Vec2(1.0, 0.0), Vec2(-1.0, 0.0), Vec2(1.0, 0.0))
			(Vec2(1.0, 1.0).normalize(), Vec2(-1.0, -1.0), Vec2(1.0, 1.0))
			(Vec2(1.0, 1.0).normalize(), Vec2(-1.0, 0.0), Vec2(0.0, 1.0))
			(Vec2(1.0, 0.0), Vec2(-0.417897, 0.908494), Vec2(0.417897, 0.908494))
			(Vec2(0.0, 1.0), Vec2(-0.593251, -0.805017), Vec2(-0.593251, 0.805017))
			(Vec2(0.0, 1.0), Vec2(-1, 0), Vec2(-1, 0))
			(Vec2(0.106533, 0.994309), Vec2(-0.593248, -0.80502), Vec2(-0.409235, 0.912429));

			testing("Vector Reflection");

			unsigned k = 0;
			foreach (ReflectTestRecord r, reflectionTests)
			{
				Vec2 c = get<1> (r) .reflect (get<0> (r));
				std::stringstream s;
				s << k << ": R <" << get<2> (r) << "> is reflected around N <" << get<1> (r) << ">";

				assertTrue (checkSimilar (c, get<2> (r)), s.str ());

				k += 1;
			}
		}
		
		UNIT_TEST(VectorArithmetic)
		{
			using namespace std;
			Vec2 v1(1, 1), v2(1, 2), v3(2, 1), v4(2, 2);
			
			testing ("Equality");
			assertTrue(v1 == v1, "==");
			assertTrue(v1 != v2, "!=");
			
			testing ("Lesser Than");
			assertTrue(v1 < v2, "<");
			assertFalse(v4 < v1, "<");
			assertTrue(v1 <= v2, "<=");
			assertTrue(v1 <= v1, "<=");
			assertFalse(v2 <= v1, "<=");
			
			testing("Greater Than");
			assertTrue(v2 > v1, ">");
			assertTrue(v3 > v2, ">");
			assertFalse(v2 >= v3, ">=");
			assertTrue(v1 >= v1, ">=");
			assertTrue(v3 >= v2, ">=");
			
			testing("Basic Arithmetic");
			assertTrue(v2+v3 == v1+v4, "+ == +");
			assertTrue((v3/v2) * v2 == v3, "/ * ==");
			assertTrue(vec(2, 1) % 2 == vec(0, 1), "% ==");
			
			testing("Advanced Arithemtic");
			assertTrue(v2.sum() == 3, "sum");
			assertTrue(v4.product() == 4, "product");
			assertTrue(vec(2.5, 5.5).floor() == vec(2.0, 5.0), "floor");
			assertTrue(vec(2.5, 5.5).ceil() == vec(3.0, 6.0), "ceil");
			assertTrue(vec(2.5, 5.25).frac() == vec(0.5, 0.25), "frac");
			
			testing("Sets");
			std::set<Vec2> set1, set2, set3;
			set1.insert(v1);
			set1.insert(v2);
			set1.insert(v3);
			
			set2.insert(v1);
			set2.insert(v2);
			
			set3.insert(v1);
			set3.insert(v4);
			
			assertTrue(set1.size() == 3, "Set has correct number of items");
			assertTrue(set2.size() == 2, "Set has correct number of items");
			assertTrue(set3.size() == 2, "Set has correct number of items");
			
			//std::cout << set1 << " " << set2 << " " << set3 << std::endl;
			
			assertTrue(includes(set1.begin(), set1.end(), set2.begin(), set2.end()), "Set functions work correctly");
			assertFalse(includes(set1.begin(), set1.end(), set3.begin(), set3.end()), "Set functions work correctly");
			
			Vec3 a(1, 0, 0), b(0, 1, 0);
			
			//std::cout << "a.dot(b) = " << a.dot(b) << std::endl;
			//std::cout << "b.dot(a) = " << b.dot(a) << std::endl;
			
			testing("Normalization");
			assertTrue(vec(0.0, 0.0, 2.5).normalize() == vec(0.0, 0.0, 1.0), "Normalized correctly");
			assertTrue(vec(0.0, 0.0, 2.5).normalize(2) == vec(0.0, 0.0, 2.0), "Normalized correctly");
			
			/*
			 Vector<4, unsigned> t1(0x13, 0x23, 0xAF, 0x53);
			 unsigned t2 = 0; t1.pack(8, t2);
			 Vector<4, unsigned> t3;
			 t3.unpack(8, t2);
			 
			 std::cout << "Original: " << t1 << " Packed/Unpacked: " << t3 << std::endl;
			 */	
		}
		
		UNIT_TEST(VectorConversion)
		{
			Vec1 v1 = 5.5;
			RealT r1 = v1;
			
			assertEqual(r1, 5.5, "Type conversion successful");
		}
#endif
	}
}