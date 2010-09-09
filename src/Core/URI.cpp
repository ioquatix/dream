/*
 *  Core/URI.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Administrator on 22/10/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "URI.h"

#include <boost/spirit/include/classic.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

namespace Dream
{
	namespace Core
	{
		typedef const char * ScannerT;

		void scan (ScannerT * start, ScannerT * end, ScannerT a, ScannerT b)
		{
			*start = a;
			*end = b;
		}

		URI::Parser::Parser () : url (NULL)
		{
		}

		URI::Parser::Parser (ScannerT s) : url (s)
		{
			clear();
			parse();
		}

		void URI::Parser::debug ()
		{
			using namespace std;

			cout << "URI: " << url << endl;

			if (schemeStart)
				cout << "   Scheme: " << string(schemeStart, schemeEnd) << endl;

			if (locationStart)
				cout << " Location: " << string(locationStart, locationEnd) << endl;

			if (usernameStart)
				cout << " Username: " << string(usernameStart, usernameEnd) << endl;

			if (passwordStart)
				cout << " Password: " << string(passwordStart, passwordEnd) << endl;

			if (hostnameStart)
				cout << " Hostname: " << string(hostnameStart, hostnameEnd) << endl;

			if (portStart)
				cout << "     Port: " << string(portStart, portEnd) << endl;

			if (relStart)
				cout << "      Rel: " << string(relStart, relEnd) << endl;

			if (absStart)
				cout << "      Abs: " << string(absStart, absEnd) << endl;

			if (pathStart)
				cout << "     Path: " << string(pathStart, pathEnd) << endl;

			if (paramsStart)
				cout << "   Params: " << string(paramsStart, paramsEnd) << endl;

			if (queryStart)
				cout << "    Query: " << string(queryStart, queryEnd) << endl;

			if (fragmentStart)
				cout << " Fragment: " << string(fragmentStart, fragmentEnd) << endl;
		}

		void URI::Parser::clear ()
		{
			schemeStart = usernameStart = passwordStart = hostnameStart = portStart = NULL;
			pathStart = paramsStart = queryStart = relStart = absStart = fragmentStart = NULL;
			locationStart = paramsStart = NULL;
		}

		void URI::Parser::parse ()
		{
			using namespace boost;
			using namespace boost::spirit::classic;

			#define SCAN(s) bind(scan, &s##Start, &s##End, _1, _2)
			rule<ScannerT> escape, safe, extra, reserved, unreserved, uchar, pchar,
			fragment, query, net_loc, scheme, param, params, segment, fsegment, path,
			rel_path, abs_path, net_path, rel_url, scheme_rl, abs_res, abs_url, URI;

			rule<ScannerT> hostname, username, password, credentials;

			escape = ch_p('%') >> hex_p >> hex_p;
			safe = ch_p('$') | '-' | '_' | '.' | '+';
			extra = ch_p('!') | '*' | '\'' | '(' | ')' | ',';

			reserved = ch_p(';') | '/' | '?' | ':' | '@' | '&' | '=';
			unreserved = alnum_p | safe | extra;

			uchar = unreserved | escape;
			pchar = uchar | ':' | '@' | '&' | '=';

			hostname = +( uchar );
			username = *( unreserved | escape );
			password = *( unreserved | escape );

			credentials = username[SCAN(username)] >> !(':' >> password[SCAN(password)]);
			fragment = *( uchar | reserved );
			query = *( uchar | reserved );
			net_loc = !(credentials >> '@') >> (hostname[SCAN(hostname)] >> !(':' >> (*range_p('0', '9'))[SCAN(port)])) | *( pchar | ';' | '?' );
			scheme = +( alnum_p | '+' | '-' | '.' );

			param = *( pchar | '/' );
			params = param >> *( ';' >> param );
			segment = *pchar;
			fsegment = +pchar;
			path = fsegment >> *( '/' >> segment );

			rel_path = !path[SCAN(path)] >> !( ';' >> params )[SCAN(params)] >> !( '?' >> query )[SCAN(query)];
			abs_path = (ch_p('/') >> rel_path)[SCAN(abs)];
			net_path = str_p("//") >> net_loc[SCAN(location)] >> !abs_path;
			abs_res = *( uchar | reserved );

			rel_url = net_path | abs_path | rel_path[SCAN(rel)];
			abs_url = scheme[SCAN(scheme)] >> ':' >> (rel_url | abs_res);

			URI = (abs_url | rel_url) >> !('#' >> fragment[SCAN(fragment)]);

			parse_info<> info = boost::spirit::classic::parse(url, URI);

			// Fix absolute paths to have initial '/'.
			if (absStart != NULL)
				--pathStart;

			hit = info.hit;
			stop = info.stop;
			full = info.full;
			length = info.length;
			#undef SCAN
		}

		URI::URI (const String & s) : m_url (s), m_parser (m_url.c_str ())
		{
		}

		URI::URI (const String & scheme, const Path & path)
		{
			std::stringstream s;
			s << scheme << ":" << path;

			m_url = s.str();
			m_parser = Parser(m_url.c_str());
		}

		std::string URI::scheme () const
		{
			if (m_parser.schemeStart)
				return std::string(m_parser.schemeStart, m_parser.schemeEnd);
			else
				return "";
		}

		std::string URI::location () const
		{
			if (m_parser.locationStart)
				return std::string(m_parser.locationStart, m_parser.locationEnd);
			else
				return "";
		}

		std::string URI::hostname () const
		{
			if (m_parser.hostnameStart)
				return std::string(m_parser.hostnameStart, m_parser.hostnameEnd);
			else
				return "";
		}

		unsigned URI::port () const
		{
			using namespace boost;

			if (m_parser.portStart)
			{
				try
				{
					std::string portString(m_parser.portStart, m_parser.portEnd);
					return lexical_cast<unsigned>(portString);
				} catch(bad_lexical_cast &)
				{
					return 0;
				}
			} else
				return 0;
		}

		String URI::service () const
		{
			if (m_parser.portStart)
				return std::string(m_parser.portStart, m_parser.portEnd);
			else if (m_parser.schemeStart)
				return scheme();
			else
				return "";
		}

		std::string URI::username () const
		{
			if (m_parser.usernameStart)
				return std::string(m_parser.usernameStart, m_parser.usernameEnd);
			else
				return "";
		}

		std::string URI::password () const
		{
			if (m_parser.passwordStart)
				return std::string(m_parser.passwordStart, m_parser.passwordEnd);
			else
				return "";
		}

		bool URI::isAbsolute () const
		{
			return m_parser.absStart != NULL;
		}

		std::string URI::resource () const
		{
			if (m_parser.absStart)
				return std::string(m_parser.absStart, m_parser.absEnd);
			else if (m_parser.relStart)
				return std::string(m_parser.relStart, m_parser.relEnd);
			else
				return "";
		}

		std::string URI::path () const
		{
			if (m_parser.pathStart)
				return std::string(m_parser.pathStart, m_parser.pathEnd);
			else
				return "";
		}

		std::string URI::query () const
		{
			if (m_parser.queryStart)
				return std::string(m_parser.queryStart, m_parser.queryEnd);
			else
				return "";
		}

		std::string URI::params () const
		{
			if (m_parser.paramsStart)
				return std::string(m_parser.paramsStart, m_parser.paramsEnd);
			else
				return "";
		}

		std::string URI::fragment () const
		{
			if (m_parser.fragmentStart)
				return std::string(m_parser.fragmentStart, m_parser.fragmentEnd);
			else
				return "";
		}

		bool URI::isFilePath ()
		{
			return scheme() == "file";
		}

#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(URI)
		{
			testing("HTTP URI");
			URI a("http://user12:abc@www.google.com:80/blah?bob=2");
			check(a.scheme() == "http") << "Scheme is correct";
			check(a.location() == "user12:abc@www.google.com:80") << "Location is correct";
			check(a.username() == "user12") << "User is correct";
			check(a.password() == "abc") << "Password is correct";
			check(a.hostname() == "www.google.com") << "Hostname is correct";
			check(a.port() == 80) << "Port is correct";
			check(a.path() == "/blah") << "Path is correct";
			check(a.query() == "?bob=2") << "Query is correct";

			testing("Mailto URI");
			URI b("mailto:blah@blah.com");
			check(b.scheme() == "mailto") << "Scheme is correct";
			check(b.path() == "blah@blah.com") << "Path is correct";

			testing("File URI");
			URI c("file:/etc/fstab");
			check(c.scheme() == "file") << "Scheme is correct";
			check(c.path() == "/etc/fstab") << "Path is correct";

			testing("Simple File URI");
			URI d("/etc/fstab");
			check(d.scheme() == "") << "Scheme is correct";
			check(d.path() == "/etc/fstab") << "Path is correct";

			testing("Path Constructed URI");
			URI e("file", "/Apples/and/oranges");
			check(e.scheme() == "file") << "Scheme is correct";
			check(e.path() == "/Apples/and/oranges") << "Path is correct";
		}
#endif
	}
}