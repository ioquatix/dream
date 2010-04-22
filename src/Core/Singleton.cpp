/*
 *  Core/Singleton.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 30/10/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#include "Singleton.h"

namespace Dream
{
	namespace Core
	{
#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING

		class FancyPants
		{
		public:
			int value;

			int strut ()
			{
				return value;
			}
		};

		int myFunction1 ()
		{
			Singleton<FancyPants> g_fancyPants;

			return g_fancyPants->strut();
		}

		void myFunction2 (int value)
		{
			Singleton<FancyPants> g_fancyPants;

			g_fancyPants->value = value;
		}

		int myFunction3 ()
		{
			typedef Singleton<FancyPants> fancyPants;

			return fancyPants::instance().strut();
		}

		UNIT_TEST(Singleton)
		{
			testing("Construction and Access");

			myFunction2(10);
			assertEqual(myFunction1(), 10, "Value was set correctly");
			assertEqual(myFunction3(), 10, "Value was set correctly");
		}
#endif
	}
}