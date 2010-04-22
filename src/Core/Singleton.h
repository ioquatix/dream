/*
 *  Core/Singleton.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 13/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CORE_SINGLETON_H
#define _DREAM_CORE_SINGLETON_H

#include "Core.h"

#include <boost/pool/detail/singleton.hpp>
#include <boost/shared_ptr.hpp>

namespace Dream
{
	namespace Core
	{
		/** Provides a simple singleton template.

		 There are two syntax options, depending on circumstance. Both syntaxes yield the same singleton object.

		 @code
		 class FancyPants;

		 Singleton<FancyPants> fancyPantsA;
		 typedef Singleton<FancyPants> fancyPantsB;

		 fancyPantsA->strut();
		 // The same as above:
		 fancyPantsB::instance().strut();
		 @endcode

		 Storage of singletons is done based on type, therefore you might need to make arbitrary sub-classes to provide more than one singleton, if needed.

		 You should try to avoid the singleton pattern wherever possible.
		 */
		template <typename single_t>
		class Singleton
		{
		protected:
			typedef boost::details::pool::singleton_default<single_t> singleton_t;
		public:
			Singleton ()
			{
			}

			/// Returns an instance of the singleton.
			inline static single_t & instance ()
			{
				return singleton_t::instance();
			}

			/// Returns an instance of the singleton. Syntactic sugar.
			inline single_t * operator-> ()
			{
				return &singleton_t::instance();
			}
		};

	}
}

#endif