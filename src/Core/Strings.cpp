//
//  Core/Strings.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 7/08/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "Strings.h"

#include <cmath>

#include <iostream>

#include "Strings/utf8.h"

namespace Dream
{
	namespace Core
	{
		std::wstring convert_string_to_utf16 (const std::string source)
		{
			std::wstring result;
			
			utf8::utf8to16(source.begin(), source.end(), std::back_inserter(result));
			
			return result;
		}
		
		std::string trimmed (std::string const & str, char const * sep_set)
		{
			using namespace std;

			string::size_type const first = str.find_first_not_of(sep_set);

			if (first == string::npos)
				return string();
			else
				return str.substr(first, str.find_last_not_of(sep_set) - first+1);
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

		StringT::value_type convert_toDigit(char c) {
			StringT::value_type d = c - '0';
			if (d < 10) {
				return d;
			} else {
				d = c - 'A';
				
				if (d < 26) {
					return d + 10;
				}
			}
			
			throw std::range_error("Could not convert character to digit - out of range!");
		}
		
		char convert_toChar(StringT::value_type d) {
			if (d < 10) {
				return '0' + d;
			} else if (d < 36) {
				return 'A' + (d - 10);
			}
			
			throw std::range_error("Could not convert digit to character - out of range!"); 
		}
	
		StringT unescape_string (const StringT & value) {
			StringStreamT buffer;
			
			StringT::const_iterator i = value.begin(), end = value.end();
			
			// Skip enclosing quotes
			++i;
			--end;
			
			for (; i != end; ++i) {
				if (*i == '\\') {
					++i;
					
					switch (*i) {
						case 't':
							buffer << '\t';
							continue;
						case 'r':
							buffer << '\r';
							continue;
						case 'n':
							buffer << '\n';
							continue;
						case '\\':
							buffer << '\\';
							continue;
						case '"':
							buffer << '"';
							continue;
						case '\'':
							buffer << '\'';
							continue;
						case 'x':
							if ((end - i) >= 2) {
								StringT::value_type value = convert_toDigit(*(++i)) << 4;
								value |= convert_toDigit(*(++i));
								buffer << (StringT::value_type)value;
								continue;
							} else {
								break;
							}
						case '.':
							continue;
					}
					
					throw std::runtime_error("Could not parse string escape!");
				} else {
					buffer << *i;
				}
			}
		
			return buffer.str();
		}
		
		StringT escape_string (const StringT & value) {
			StringStreamT buffer;
			
			StringT::const_iterator i = value.begin(), end = value.end();
			buffer << '"';
			
			for (; i != end; ++i) {
				if (*i == '"') {
					buffer << "\\\"";
				} else {
					buffer << *i;
				}
			}
			
			buffer << '"';
			return buffer.str();
		}

#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(Strings)
		{
			testing("Formatting");

			check(center("Apple", 10, ' ') == "   Apple  ") << "String is centered";
			check(center("Apple", 11, ' ') == "   Apple   ") << "String is centered";
			check(center("Apple", 11, '-') == "---Apple---") << "String is centered";
		}
		
		UNIT_TEST(StringConversions)
		{
			std::string s1("\xe6\x97\xa5\xd1\x88\xf0\x9d\x84\x9e");
			
			std::wstring result = convert_string_to_utf16(s1);
			
			check(result.size() == 4) << "Converted string is correct length";
			check(result[2] == 0xd834) << "Converted character is correct value";
			check(result[3] == 0xdd1e) << "Converted character is correct value";
		}
#endif
	}
}
