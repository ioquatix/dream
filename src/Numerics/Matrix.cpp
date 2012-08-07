//
//  Numerics/Matrix.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 13/03/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#include "Matrix.h"

namespace Dream
{
	namespace Numerics
	{
		Mat44 rotation (const RealT & radians, const Vec3 & around_normal, const Vec3 & around_point)
		{
			return Mat44::rotating_matrix(radians, around_normal, around_point);
		}

		Mat44 rotation (const RealT & radians, const Vec3 & around_normal)
		{
			return Mat44::rotating_matrix(radians, around_normal);
		}

		Mat44 rotation (const Vec3 & from_unit_vector, const Vec3 & to_unit_vector, const Vec3 & around_normal)
		{
			return Mat44::rotating_matrix(from_unit_vector, to_unit_vector, around_normal);
		}

// MARK: -
// MARK: Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(Matrix)
		{
			testing("Identity Multiplication");
			Mat44 a, b, c;
			Mat44 I(IDENTITY);

			a.load_test_pattern();

			check(a == a * I) << "Matricies are equivalent after multiplication by identity";
			check(I * a == a) << "Matricies are equivalent after multiplication by identity";

			testing("Vector Multiplication");
			Vec4 r, pt(10.0, 0.0, 0.0, 1.0);
			b = rotation(R90, vec(0.0, 1.0, 0.0));

			r = b * pt;

			check(r.equivalent(vec(0.0, 0.0, 10.0, 1.0))) << "Rotation was successful";

			b = rotation(R180, vec(0.0, 1.0, 0.0), vec(0.0, 10.0, 10.0));

			r = b * pt;

			check(r.equivalent(vec(-10.0, 0.0, -20.0, 1.0))) << "Rotation was successful";
		}

		UNIT_TEST(MatrixVector)
		{
			testing("Vector Assignment");

			Mat44 a(ZERO);
			Mat44 mat_ident(IDENTITY);
			Vec4 vec_ident(IDENTITY);

			a.set(0, 0, vec_ident, 5);

			check(a == mat_ident) << "Vector was copied correctly";

			Vec4 c1(0, 1, 2, 3), c2(4, 5, 6, 7), c3(8, 10, 12, 14), c4(9, 11, 13, 15);
			a.zero();

			a.set(0, 0, c1, 1);
			a.set(1, 0, c2, 1);
			a.set(2, 0, c3, 2);
			a.set(2, 1, c4, 2);

			Mat44 test_pattern;
			test_pattern.load_test_pattern();

			check(a == test_pattern) << "Vector was copied correctly";
		}
#endif
	}
}
