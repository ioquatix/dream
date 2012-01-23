//
//  Core/URI.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 22/10/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CORE_URI_H
#define _DREAM_CORE_URI_H

#include "Strings.h"
#include "Path.h"

#include <vector>

namespace Dream
{
	namespace Core
	{
		class URI
		{
		private:
			StringT _url;
			
			/// According to RFC3986.
			StringT _scheme, _authority, _path, _query, _fragment;
			
			/// Standard parts of the authority
			StringT _hostname;
			unsigned _port;
			
			/// Other parts that may or may not be defined:
			std::vector<StringT> _user_info;
			
		public:
			
			class InvalidFormatError : std::exception {
			protected:
				StringT _url;
				std::size_t _offset;
				
				StringT _message;
				
			public:
				InvalidFormatError(const StringT & url, std::size_t offset);
				
				virtual const char* what() const noexcept;
			};
			
			/// Construct a URI from a RFC2396 formatted string.
			URI (const StringT & s);

			/// Construct a URI from a file path.
			URI (const StringT & scheme, const Path & path);

			/// The scheme component of the URI.
			const StringT & scheme () const;

			/// The location portion of the URI.
			const StringT & authority () const;
			
			/// The path portion of the URI.
			const StringT & path () const;
			
			/// The query portion of the URI.
			const StringT & query () const;
			
			/// The fragment portion of the URI.
			const StringT & fragment () const;

			/// The hostname portion of the URI.
			const StringT & hostname () const;

			/// A list of user info items, normally username, password.
			const std::vector<StringT> & user_info () const;
			StringT username () const;
			StringT password () const;

			/// The port number of the URI (if specified) or 0.
			unsigned port () const;

			/// The port number as a string, or the scheme.
			StringT service () const;
			
			/// Whether or not the URI was relative or absolute.
			bool is_absolute () const;
			
			/// True if the URI represents a file path. The path is local and accessible via path().
			bool is_file_path ();
		};
	}
}

#endif
