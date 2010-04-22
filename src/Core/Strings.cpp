/*
 *  Core/Strings.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 7/08/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "Strings.h"

#include <cmath>

#include <iostream>
#include "Singleton.h"

#include "Strings/utf8.h"

namespace Dream
{
	namespace Core
	{
		/* Deprecated
		std::string convertString (const std::wstring source, locale_t l)
		{
			std::string s;
			int required_chars = wcstombs_l(NULL, source.c_str(), 0, l);
			char *temp_chars = new char[required_chars + 1];

			temp_chars[0] = 0;
			wcstombs_l(temp_chars, source.c_str(), required_chars + 1, l);
			s = temp_chars;

			delete [] temp_chars;
			return s;
		}

		std::wstring convertString (const std::string source, locale_t l)
		{
			std::wstring s;
			int required_chars = mbstowcs_l(NULL, source.c_str(), 0, l);
			wchar_t *temp_chars = new wchar_t[required_chars + 1];

			temp_chars[0] = 0;
			mbstowcs_l(temp_chars, source.c_str(), required_chars + 1, l);
			s = temp_chars;

			delete [] temp_chars;
			return s;
		}
		*/
		
		std::wstring convertStringToUTF16 (const std::string source)
		{
			std::wstring result;
			
			utf8::utf8to16(source.begin(), source.end(), std::back_inserter(result));
			
			return result;
		}
		
		std::string trimmed (std::string const & str, char const * sepSet)
		{
			using namespace std;

			string::size_type const first = str.find_first_not_of(sepSet);

			if (first == string::npos)
				return string();
			else
				return str.substr(first, str.find_last_not_of(sepSet) - first+1);
		}

		std::string center (const std::string & str, unsigned width, char space)
		{
			if (width <= str.size())
				width = 0;
			else
				width -= str.size();

			unsigned lhw = ceilf(0.5 * width);
			unsigned rhw = floorf(0.5 * width);

			std::stringstream s;
			std::string lhs(lhw, space);
			std::string rhs(rhw, space);

			s << lhs << str << rhs;

			return s.str();
		}

#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(Strings)
		{
			testing("Formatting");

			assertEqual(center("Apple", 10, ' '), "   Apple  ", "String is centered");
			assertEqual(center("Apple", 11, ' '), "   Apple   ", "String is centered");
			assertEqual(center("Apple", 11, '-'), "---Apple---", "String is centered");
		}
		
		UNIT_TEST(StringConversions)
		{
			std::string s1("\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e");
			
			std::wstring result = convertStringToUTF16(s1);
			
			assertEqual(result.size(), 4, "Converted string is correct length");
			assertEqual(result[2], 0xd834, "Converted character is correct value");
			assertEqual(result[3], 0xdd1e, "Converted character is correct value");
		}
#endif
	}
}