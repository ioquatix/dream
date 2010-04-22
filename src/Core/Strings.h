/*
 *  Core/Strings.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 7/08/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CORE_STRINGS_H
#define _DREAM_CORE_STRINGS_H

#include "Core.h"

#include <string>
#include <iostream>
#include <sstream>

#include <boost/format.hpp>
#include <set>

namespace Dream
{
	namespace Core
	{
		typedef std::string String;

		using boost::format;

		// It is important to consider file encoding when using these functions
		// and wchar_t/wstring. Deprecated
		// std::string convertString (const std::wstring source, locale_t l);
		// std::wstring convertString (const std::string source, locale_t l);
		
		/// Converts UTF8 to UTF16
		std::wstring convertStringToUTF16 (const std::string source);

		std::string trimmed (std::string const & str, char const * sepSet);

		std::string center (const std::string & str, unsigned width, char space);
		
		/// This function is typically used for parsing OpenGL extension strings.
		template <typename Out>
		Out split(const char * input, const char divider, Out res)
		{
			const char * start = input;
			const char * next;
			
			while ((next = strchr(start, divider)) && next != NULL) {
				*res++ = String(start, next);
				start = next + 1;
			}
			
			return res;
		}
		
	}
}

#endif