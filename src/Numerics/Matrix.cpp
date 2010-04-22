/*
 *  Numerics/Matrix.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 13/03/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#include "Matrix.h"

namespace Dream
{
	namespace Numerics
	{
		Mat44 rotation (const RealT & radians, const Vec3 & aroundNormal, const Vec3 & aroundPoint)
		{
			return Mat44::rotatingMatrix(radians, aroundNormal, aroundPoint);
		}

		Mat44 rotation (const RealT & radians, const Vec3 & aroundNormal)
		{
			return Mat44::rotatingMatrix(radians, aroundNormal);
		}

		Mat44 rotation (const Vec3 & fromUnitVector, const Vec3 & toUnitVector, const Vec3 & aroundNormal)
		{
			return Mat44::rotatingMatrix(fromUnitVector, toUnitVector, aroundNormal);
		}

#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(Matrix)
		{
			testing("Identity Multiplication");
			Mat44 a, b, c;
			Mat44 I(IDENTITY);

			a.loadTestPattern();

			assertEqual(a, a * I, "Matricies are equivalent after multiplication by identity");
			assertEqual(I * a, a, "Matricies are equivalent after multiplication by identity");

			testing("Vector Multiplication");
			Vec4 r, pt(10.0, 0.0, 0.0, 1.0);
			b = rotation(R90, vec(0.0, 1.0, 0.0));

			r = b * pt;

			assertTrue(r.equivalent(vec(0.0, 0.0, 10.0, 1.0)), "Rotation was successful");

			b = rotation(R180, vec(0.0, 1.0, 0.0), vec(0.0, 10.0, 10.0));

			r = b * pt;

			assertTrue(r.equivalent(vec(-10.0, 0.0, -20.0, 1.0)), "Rotation was successful");
		}
		
		UNIT_TEST(MatrixVector)
		{
			testing("Vector Assignment");
					
			Mat44 a(ZERO);
			Mat44 matIdent(IDENTITY);
			Vec4 vecIdent(IDENTITY);
			
			a.set(0, 0, vecIdent, 5);
			
			assertEqual(a, matIdent, "Vector was copied correctly");
			
			Vec4 c1(0, 1, 2, 3), c2(4, 5, 6, 7), c3(8, 10, 12, 14), c4(9, 11, 13, 15);
			a.zero();
			
			a.set(0, 0, c1, 1);
			a.set(1, 0, c2, 1);
			a.set(2, 0, c3, 2);
			a.set(2, 1, c4, 2);
			
			Mat44 testPattern;
			testPattern.loadTestPattern();
			
			assertEqual(a, testPattern, "Vector was copied correctly");
		}
#endif
	}
}