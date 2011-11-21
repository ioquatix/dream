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

namespace Dream
{
	namespace Core
	{
		typedef std::string StringT;
		typedef std::stringstream StringStreamT;
		
		/// Converts UTF8 to UTF16
		std::wstring convertStringToUTF16 (const std::string source);

		std::string trimmed (std::string const & str, char const * sepSet);

		std::string center (const std::string & str, unsigned width, char space);
		
		/// This function is typically used for parsing OpenGL extension strings.		
		template <typename OutT>
		void split(const StringT & input, const char divider, OutT result) {
			std::size_t pos = 0, next = 0;
			
			do {
				next = input.find(divider, pos);
				
				StringT bit(&input[pos], (next == StringT::npos) ? (input.size() - pos) : (next - pos));
				*result++ = bit;					
				
				pos = next + 1;
			} while (next != StringT::npos);
		}
		
		StringT unescapeString (const StringT &);
		StringT escapeString (const StringT &);
		
	}
}

#endif