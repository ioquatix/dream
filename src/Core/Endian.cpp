//
//  Core/Endian.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 26/10/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#include "Endian.h"

namespace Dream
{
	namespace Core
	{
#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
		template <Endian store_t>
		struct OrderedTestStruct
		{
			Ordered<uint32_t, store_t> value;
		};

		UNIT_TEST(Endian)
		{
			uint32_t a, b, c;
			uint64_t d, e;

			Endian other_endian = LITTLE;

			if (host_endian() == LITTLE)
			{
				other_endian = BIG;
			}

			testing("Endian Conversions");

			// 32-bit value
			a = 0x10203040;

			order_copy(a, b, host_endian(), other_endian);
			check(b == 0x40302010) << "32-bit endian conversion is correct";

			a = 100;
			order_copy(a, b, host_endian(), other_endian);
			order_copy(b, c, other_endian, host_endian());
			check(a == c) << "Converted values are the same";

			endian_decode(b, other_endian, host_endian());
			check(a == b) << "Correctly converted in-place";

			order_copy(a, b, LITTLE, LITTLE);
			check(a == b) << "Values are not converted when endian is the same";

			order_copy(a, b, BIG, BIG);
			check(a == b) << "Values are not converted when endian is the same";

			order_copy(a, b, LITTLE, BIG);
			check(a != b) << "Values are not converted when endian is the same";

			d = 0x1020304050607080ull;
			order_copy(d, e, host_endian(), other_endian);
			check(e == 0x8070605040302010ull) << "64-bit endian conversion is correct";

			testing("Ordered Structs");

			OrderedTestStruct<LITTLE> ls;
			OrderedTestStruct<BIG> bs;

			ls.value = 0x10203040;
			bs.value = 0x10203040;

			if (host_endian() == LITTLE)
			{
				check(ls.value.base == 0x10203040) << "Value is stored in native order";
				check(bs.value.base == 0x40302010) << "Value is stored in converted order";
			} else
			{
				check(bs.value.base == 0x10203040) << "Value is stored in native order";
				check(ls.value.base == 0x40302010) << "Value is stored in converted order";
			}

			a = ls.value;
			b = bs.value;

			check(a == b) << "Converted values are same";
			check(ls.value == b) << "Equality operator works";
			check(bs.value == a) << "Equality operator works";
		}
#endif
	}
}
