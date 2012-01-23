//
//  Numerics/Numerics.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 13/05/06.
//  Copyright 2006 Samuel WilliamsWilliams. All rights reserved.
//
//

#include "Numerics.h"

#include "Number.h"

#include <iostream>
#include <cstdlib>

namespace Dream
{
	namespace Numerics
	{
		template <typename t>
		class FloatingPointIntegerTraits
		{
		};

		template <>
		class FloatingPointIntegerTraits<float>
		{
		public:
			typedef int32_t FIntT;
			typedef uint32_t FUIntT;
			
			/// Manually calculated - approximate accuracy of zero point for libm when dealing with trigonometric functions.
			static const uint32_t ACCURACY = 897988541;
		};

		template <>
		class FloatingPointIntegerTraits<double>
		{
		public:
			typedef int64_t FIntT;
			typedef uint64_t FUIntT;

			/// Manually calculated - approximate accuracy of zero point for libm when dealing with trigonometric functions.
			static const uint64_t ACCURACY = 4427486594234968593ULL;
		};

		template <typename t>
		class FloatingPointTraits
		{
		public:
			typedef t FloatT;
			typedef typename FloatingPointIntegerTraits<t>::FIntT FIntT;

			union Conversion
			{ FloatT f; FIntT i; };

			static const FIntT S = FIntT (0x80ULL << ((sizeof (FloatT) - 1) * 8));

			static FIntT convertToInteger (const FloatT & f)
			{
				Conversion c;
				c.f = f;

				if (c.i < 0)
					return S - c.i;
				else
					return c.i;
			}

			static FloatT convertToFloat (const FIntT & i)
			{
				Conversion c;
				c.i = i;

				if (c.i < 0)
					c.i = S - c.i;

				return c.f;
			}

			static bool isZero (const FIntT & value, const FIntT & threshold)
			{
				if (value < 0)
					return (value + threshold) > 0;
				else
					return (value - threshold) < 0;
			}

			static bool isZero (const FloatT & value, const FIntT & threshold)
			{
				FIntT p = convertToInteger(value);

				return isZero (p, threshold);
			}

			static bool isZero (const FloatT & value)
			{
				return isZero(value, FloatingPointIntegerTraits<t>::ACCURACY);
			}

			static bool isZero (const FIntT & value)
			{
				return isZero(value, FloatingPointIntegerTraits<t>::ACCURACY);
			}

			static bool equalWithinTolerance (const FloatT & a, const FloatT & b, const unsigned & maxUlps)
			{
				// Make sure maxUlps is non-negative and small enough that the
				// default NAN won't compare as equal to anything.
				ensure(maxUlps < 4 * 1024 * 1024);

				// Make lexicographically ordered as a twos-complement int
				FIntT aInt = convertToInteger(a);
				FIntT bInt = convertToInteger(b);

				FIntT intDiff = Number<FIntT>::abs(aInt - bInt);

				if (intDiff <= maxUlps)
					return true;

				return false;
			}

			static bool equivalent (const FloatT & a, const FloatT & b)
			{
				// Make lexicographically ordered as a twos-complement int
				FIntT aInt = convertToInteger(a);
				FIntT bInt = convertToInteger(b);

				if (aInt == bInt) return true;

				if (isZero(aInt) && isZero(bInt)) return true;

				FIntT intDiff = Number<FIntT>::abs(aInt - bInt);

				if (intDiff <= 100)
					return true;

				return false;
			}
		};

		// http://acius2.blogspot.com/2007/11/calculating-next-power-of-2.html
		uint32_t nextHighestPowerOf2 (uint32_t n)
		{
			if (n == 0) return 0;
			
			n--;
			n |= n >> 1;
			n |= n >> 2;
			n |= n >> 4;
			n |= n >> 8;
			n |= n >> 16;
			n++;
			return n;
		}

		bool isPowerOf2 (uint32_t k)
		{
			return (k & k-1) == 0;
		}

		// Usable AlmostEqual function
		bool equalWithinTolerance (const float & a, const float & b, const unsigned & maxUlps)
		{
			return FloatingPointTraits<float>::equalWithinTolerance(a, b, maxUlps);
		}

		// Usable AlmostEqual function
		bool equalWithinTolerance (const double & a, const double & b, const unsigned & maxUlps)
		{
			return FloatingPointTraits<double>::equalWithinTolerance(a, b, maxUlps);
		}

		// Usable AlmostEqual function
		bool equivalent (const float & a, const float & b)
		{
			return FloatingPointTraits<float>::equivalent(a, b);
		}

		// Usable AlmostEqual function
		bool equivalent (const double & a, const double & b)
		{
			return FloatingPointTraits<double>::equivalent(a, b);
		}

#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(Numerics)
		{
			const double v = 5193823413.0;

			testing("Equality");
			float a, b, c;
			double d, e, f;

			a = v / 7.5;
			b = v / 7.500001;
			c = v / 7.5001;

			check(equalWithinTolerance(a, b)) << "Float values are equal";
			check(!equalWithinTolerance(a, c)) << "Float values are not equal";

			d = v / 7.5;
			e = v / 7.50000000001;
			f = v / 7.50000000000001;

			check(!equalWithinTolerance(d, e)) << "Double values are not equal";
			check(equalWithinTolerance(d, f)) << "Double values are equal";
		}

		UNIT_TEST(FloatingPointTraits)
		{
			typedef FloatingPointTraits<float> F;
			F::FIntT i = F::convertToInteger(1.0056f);
			F::FloatT f = F::convertToFloat(i);
			check(1.0056f == f) << "Integer - float conversion correct for 1.0056";

			i = F::convertToInteger(-0.52f);
			f = F::convertToFloat(i);
			check(-0.52f == f) << "Integer - float conversion correct for -0.52";

			double t = 0.0000001;
			std::cout << "acosf: " << cos(R90) << std::endl;
			check(FloatingPointTraits<double>::isZero(cos(R90)) << FloatingPointTraits<double>::convertToInteger(t)) << "cosf is zero";

			std::cout << "Accuracy of float: " << FloatingPointTraits<double>::convertToInteger(t) << std::endl;
		}
		
		UNIT_TEST(PowerOfTwo)
		{
			int k = nextHighestPowerOf2(16);
			check(k == 16) << "Next power of two calculated correctly";
			
			k = nextHighestPowerOf2(17);
			check(k == 32) << "Next power of two calculated correctly";
		}

		/// Calculate the accuracy of cos function. Interesting results..
		template <typename t>
		void calculateFloatAccuracy ()
		{
			using namespace std;
			typedef FloatingPointTraits<t> F;
			typedef typename F::FIntT FIntT;
			typedef typename F::FloatT FloatT;

			FloatT f = R90;
			FloatT v1 = Number<FloatT>::cos(f);

			if (v1 < (FloatT) 0.0)
			{
				// Increment floating point number to next discrete step
				f = F::convertToFloat (F::convertToInteger (f) + 1);
			} else
			{
				// Decrement floating point number to previous discrete step
				f = F::convertToFloat (F::convertToInteger (f) - 1);
			}

			FloatT v2 = Number<FloatT>::cos(f);

			FIntT d1 = Number<FIntT>::abs(F::convertToInteger(v1));
			FIntT d2 = Number<FIntT>::abs(F::convertToInteger(v2));
			FIntT d3 = Number<FIntT>::max(d1, d2);

			std::cout << "d1 : " << d1 << " d2 : " << d2 << " = d3 : " << d3 << std::endl;
			std::cout << "Trigonometric precision of " << typeid(t).name() << " around zero is ";
			std::cout << F::convertToFloat(d3) << std::endl;
		}

		UNIT_TEST(CalculateFloatAccuracy)
		{
			calculateFloatAccuracy<float>();
			calculateFloatAccuracy<double>();

			std::cout << "   Float ACCURACY: " << FloatingPointTraits<float>::convertToInteger(0.000001) << std::endl;
			std::cout << "  Double ACCURACY: " << FloatingPointTraits<double>::convertToInteger(0.000000000001) << std::endl;
		}
		
		UNIT_TEST(CheckRotationAccuracy)
		{
			testing("Rotations");
			
			check(equivalent(R45 * 2, R90)) << "R45 is correct";
			check(equivalent(R90 * 2, R180)) << "R90 is correct";
			check(equivalent(R180 * 2, R360)) << "R360 is correct";
		}
#endif
	}
}
