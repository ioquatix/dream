/*
 *  Core/URI.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Administrator on 22/10/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CORE_URI_H
#define _DREAM_CORE_URI_H

#include "Strings.h"

#include <vector>
#include <boost/filesystem/path.hpp>

namespace Dream
{
	namespace Core
	{
		typedef boost::filesystem::path Path;

		/**
		 A simple URI parser. Provides access to the components of RFC2396 formatted URIs.

		 Here are the results of parsing an example URI:
		 @verbatim
		 URI: http://user12:abc@www.google.com:80/blah?bob=2
		 Scheme: http
		 Location: user12:abc@www.google.com:80
		 Username: user12
		 Password: abc
		 Hostname: www.google.com
		 Port: 80
		 Path: /blah
		 Query: ?bob=2
		 @endverbatim
		 */
		class URI
		{
		private:
			String m_url;

			class Parser
			{
			private:
				void parse ();
				void clear ();
			public:
				void debug ();

				typedef const char * ScannerT;
				ScannerT url, stop;

				bool full, hit;
				unsigned length;

				ScannerT schemeStart, schemeEnd;
				ScannerT usernameStart, usernameEnd;
				ScannerT passwordStart, passwordEnd;
				ScannerT hostnameStart, hostnameEnd;
				ScannerT portStart, portEnd;
				ScannerT relStart, relEnd;
				ScannerT absStart, absEnd;
				ScannerT locationStart, locationEnd;
				ScannerT pathStart, pathEnd;
				ScannerT paramsStart, paramsEnd;
				ScannerT queryStart, queryEnd;
				ScannerT fragmentStart, fragmentEnd;

				Parser ();
				Parser (ScannerT s);
			};

			Parser m_parser;

		public:
			/// Construct a URI from a RFC2396 formatted string.
			URI (const String & s);

			/// Construct a URI from a file path.
			URI (const String & scheme, const Path & path);

			/// The scheme component of the URI.
			String scheme () const;

			/// The location portion of the URI.
			String location () const;

			/// The hostname portion of the URI.
			String hostname () const;

			/// The username portion of the URI.
			String username () const;

			/// The password portion of the URI.
			String password () const;

			/// The port number of the URI (if specified) or 0.
			unsigned port () const;

			/// The port number as a string, or the scheme.
			String service () const;

			String resource () const;

			/// Whether or not the URI was relative or absolute.
			bool isAbsolute () const;

			/// The path portion of the URI.
			String path () const;

			/// The query portion of the URI.
			String query () const;

			/// The parameters portion of the URI.
			String params () const;

			/// The fragment portion of the URI.
			String fragment () const;

			/// True if the URI represents a file path. The path is local and accessible via path().
			bool isFilePath ();
		};
	}
}

#endif