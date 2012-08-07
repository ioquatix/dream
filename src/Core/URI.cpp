//
//  Core/URI.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 22/10/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "URI.h"

#include <string>

// memset
#include <string.h>

namespace Dream
{
	namespace Core
	{
		namespace URIImpl {
			typedef const StringT::value_type * IteratorT;

			struct Authority {
				IteratorT user_info_begin, user_info_end;
				IteratorT host_begin, host_end;
				IteratorT port_begin, port_end;
			};

			struct Hierarchy {
				IteratorT authority_begin, authority_end;
				IteratorT path_begin, path_end;

				Authority authority;
			};

			struct Components {
				bool complete;

				IteratorT scheme_begin, scheme_end;
				IteratorT hierarchy_begin, hierarchy_end;
				IteratorT query_begin, query_end;
				IteratorT fragment_begin, fragment_end;

				Hierarchy hierarchy;

				Components () {
					memset(this, 0, sizeof(*this));
				}
			};

			namespace Parser {
				static std::size_t count(IteratorT begin, IteratorT end) {
					return end - begin;
				}

				typedef bool (*PredicateT)(IteratorT i);

				static IteratorT parse_predicate(PredicateT predicate, IteratorT begin, IteratorT end) {
					while (begin != end && predicate(begin))
						++begin;

					return begin;
				}

				typedef IteratorT (*ParserT)(IteratorT begin, IteratorT end);

				static IteratorT parse_string(ParserT parser, IteratorT begin, IteratorT end) {
					while (begin != end) {
						IteratorT next = parser(begin, end);

						if (next == begin) {
							// Nothing was parsed.
							return next;
						} else {
							// Some character was parsed.
							begin = next;
						}
					}

					return begin;
				}

				// Constant should be null-terminated.
				static IteratorT parse_constant(const char * constant, IteratorT begin, IteratorT end) {
					IteratorT current = begin;

					while (current != end) {
						// We got to the end.
						if (*constant == '\0')
							return current;

						if (*constant++ != *current++)
							return begin;
					}

					// We didn't finish parsing the constant because we ran out of input.
					return begin;
				}

				static bool is_numeric(IteratorT i) {
					return (*i >= '0' && *i <= '9');
				}

				static bool is_alpha(IteratorT i) {
					return (*i >= 'a' && *i <= 'z') || (*i >= 'A' && *i <= 'Z');
				}

				static bool is_alpha_numeric(IteratorT i) {
					return is_numeric(i) || is_alpha(i);
				}

				static bool is_hex(IteratorT i) {
					return is_numeric(i) || (*i >= 'a' && *i <= 'f') || (*i >= 'A' && *i <= 'F');
				}

				/* ======================================== */
				/* == General URI parsing as per RFC3986 == */
				/* ======================================== */

				//   unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
				static bool is_unreserved(IteratorT i) {
					return is_alpha_numeric(i) || *i == '-' || *i == '.' || *i == '_' || *i == '~';
				}

				//   gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
				//static bool is_gen_delimiter(IteratorT i) {
				//	return *i == ':' || *i == '/' || *i == '?' || *i == '#' || *i == '[' || *i == ']' || *i == '@';
				//}

				//   sub-delims    = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
				static bool is_sub_delimiter(IteratorT i) {
					return *i == '!' || *i == '$' || *i == '&' || *i == '\'' || *i == '(' || *i == ')' || *i == '*' || *i == '+' || *i == ',' || *i == ';' || *i == '=';
				}

				//   reserved      = gen-delims / sub-delims
				//static bool is_reserved(IteratorT i) {
				//	return is_gen_delimiter(i) || is_sub_delimiter(i);
				//}

				//   pct-encoded   = "%" HEXDIG HEXDIG
				static IteratorT parse_percent_encoded(IteratorT begin, IteratorT end) {
					if (count(begin, end) < 3) {
						return begin;
					}

					if (*begin == '%' && is_hex(begin+1) && is_hex(begin+2)) {
						return begin + 3;
					} else {
						return begin;
					}
				}

				//   pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
				static bool is_path_character(IteratorT i) {
					return *i == ':' || *i == '@';
				}

				static IteratorT parse_path_character(IteratorT begin, IteratorT end) {
					// We coalesce the parsing into one step and parse as much as possible each time.
					// This isn't strictly following the standard, but it shouldn't be any different in practice.
					IteratorT next = begin;

					next = parse_predicate(is_unreserved, next, end);
					next = parse_string(parse_percent_encoded, next, end);
					next = parse_predicate(is_sub_delimiter, next, end);
					next = parse_predicate(is_path_character, next, end);

					return next;
				}

				//   userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
				static bool is_user_info_character(IteratorT i) {
					return *i == ':';
				}

				static IteratorT parse_user_info_character(IteratorT begin, IteratorT end) {
					IteratorT next = begin;

					next = parse_predicate(is_unreserved, next, end);
					next = parse_string(parse_percent_encoded, next, end);
					next = parse_predicate(is_sub_delimiter, next, end);
					next = parse_predicate(is_user_info_character, next, end);

					return next;
				}

				//   host          = IP-literal / IPv4address / reg-name
				//   IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
				//   IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
				//   IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
				//   reg-name      = *( unreserved / pct-encoded / sub-delims )
				static IteratorT parse_host_character(IteratorT begin, IteratorT end) {
					IteratorT next = begin;

					next = parse_predicate(is_unreserved, next, end);
					next = parse_string(parse_percent_encoded, next, end);
					next = parse_predicate(is_sub_delimiter, next, end);

					return next;
				}

				static IteratorT parse_host(IteratorT begin, IteratorT end) {
					// For convenience we shortcut this parsing for now.
					return parse_string(parse_host_character, begin, end);
				}

				//   authority     = [ userinfo "@" ] host [ ":" port ]
				//   port          = *DIGIT
				static IteratorT parse_authority(IteratorT begin, IteratorT end, Authority & authority) {
					IteratorT user_info_begin = begin;

					IteratorT user_info_end = parse_string(parse_user_info_character, begin, end);
					IteratorT host_begin = parse_constant("@", user_info_end, end);

					// Can we parse the "@" symbol?
					if (host_begin != user_info_end) {
						// Valid user info was found:
						authority.user_info_begin = user_info_begin;
						authority.user_info_end = user_info_end;
					} else {
						host_begin = begin;
					}

					IteratorT host_end = parse_host(host_begin, end);

					if (host_end == host_begin) {
						// We can't continue as host is required.
						return host_end;
					}

					authority.host_begin = host_begin;
					authority.host_end = host_end;

					IteratorT port_begin = parse_constant(":", host_end, end);

					if (port_begin != host_end) {
						IteratorT port_end = parse_predicate(is_numeric, port_begin, end);

						authority.port_begin = port_begin;
						authority.port_end = port_end;

						return port_end;
					} else {
						return host_end;
					}
				}

				static IteratorT parse_path(IteratorT begin, IteratorT end) {
					// This is optional, and signifies an absolute path.
					IteratorT absolute_begin = parse_constant("/", begin, end);

					IteratorT current = parse_string(parse_path_character, absolute_begin, end);

					if (current == begin)
						return begin;

					while (current != end) {
						IteratorT next = parse_constant("/", current, end);

						if (next == current)
							return current;

						current = parse_string(parse_path_character, next, end);
					}

					return current;
				}

				//   path-abempty  = *( "/" segment )
				//   path-absolute = "/" [ segment-nz *( "/" segment ) ]
				//   path-noscheme = segment-nz-nc *( "/" segment )
				//   path-rootless = segment-nz *( "/" segment )
				//   path-empty    = 0<pchar>

				//   segment       = *pchar
				//   segment-nz    = 1*pchar
				//   segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
				//                   ; non-zero-length segment without any colon ":"

				static IteratorT parse_hierarchy(IteratorT begin, IteratorT end, Hierarchy & hierarchy) {
					// (//([^/?#]*))?
					IteratorT authority_begin = parse_constant("//", begin, end);
					IteratorT authority_end = authority_begin;

					if (authority_begin != begin) {
						authority_end = parse_authority(authority_begin, end, hierarchy.authority);

						hierarchy.authority_begin = authority_begin;
						hierarchy.authority_end = authority_end;
					}

					// ([^?#]*)
					hierarchy.path_begin = authority_end;
					hierarchy.path_end = parse_path(hierarchy.path_begin, end);

					return hierarchy.path_end;
				}

				//   scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
				static bool is_scheme_character(IteratorT i) {
					return is_alpha_numeric(i) || *i == '+' || *i == '-' || *i == '.';
				}

				static IteratorT parse_scheme(IteratorT begin, IteratorT end) {
					IteratorT next = parse_predicate(is_alpha, begin, end);

					if (next == begin) {
						return begin;
					}

					next = parse_predicate(is_scheme_character, next, end);
					IteratorT scheme_end = parse_constant(":", next, end);

					if (scheme_end == next) {
						return begin;
					} else {
						return scheme_end;
					}
				}

				//   fragment      = *( pchar / "/" / "?" )
				//   query         = *( pchar / "/" / "?" )
				static bool is_meta_character(IteratorT i) {
					return *i == '/' || *i == '?';
				}

				static IteratorT parse_meta_character(IteratorT begin, IteratorT end) {
					IteratorT next = begin;

					next = parse_path_character(next, end);
					next = parse_predicate(is_meta_character, next, end);

					return next;
				}

				static IteratorT parse_meta(const char * prefix, IteratorT begin, IteratorT end) {
					IteratorT next = parse_constant(prefix, begin, end);

					if (next != begin) {
						return parse_string(parse_meta_character, next, end);
					} else {
						return begin;
					}
				}

				//   URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
				//   ^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?
				//    12            3  4          5       6  7        8 9
				static IteratorT parse(IteratorT begin, IteratorT end, Components & components) {
					// (([^:/?#]+):)?
					IteratorT scheme_begin = begin;
					IteratorT scheme_end = parse_scheme(scheme_begin, end);

					if (scheme_end != scheme_begin) {
						components.scheme_begin = scheme_begin;
						components.scheme_end = scheme_end - 1;
					}

					IteratorT hierarchy_begin = parse_constant(":", scheme_end, end);
					IteratorT hierarchy_end = parse_hierarchy(hierarchy_begin, end, components.hierarchy);

					if (hierarchy_end == hierarchy_begin) {
						return hierarchy_begin;
					}

					components.hierarchy_begin = hierarchy_begin;
					components.hierarchy_end = hierarchy_end;

					IteratorT query_begin = hierarchy_end;
					IteratorT query_end = parse_meta("?", query_begin, end);

					if (query_end != query_begin) {
						components.query_begin = query_begin;
						components.query_end = query_end;
					}

					IteratorT fragment_begin = query_end;
					IteratorT fragment_end = parse_meta("#", fragment_begin, end);

					if (fragment_end != fragment_begin) {
						components.fragment_begin = fragment_begin;
						components.fragment_end = fragment_end;
					}

					components.complete = true;

					return fragment_end;
				}
			}
		};

		URI::InvalidFormatError::InvalidFormatError(const StringT & url, std::size_t offset) : _url(url), _offset(offset)
		{
			StringStreamT buffer;

			buffer << "Invalid URI(offset=" << _offset << "): " << _url;

			_message = buffer.str();
		}

		const char * URI::InvalidFormatError::what() const noexcept
		{
			return _message.c_str();
		}

		URI::URI(const StringT & s) : _url(s), _port(0)
		{
			URIImpl::Components components;

			URIImpl::IteratorT end = URIImpl::Parser::parse(_url.data(), _url.data() + _url.size(), components);

			if (!components.complete) {
				throw InvalidFormatError(_url, end - _url.data());
			}

			if (components.scheme_begin != NULL) {
				_scheme = StringT(components.scheme_begin, components.scheme_end);
			}

			if (components.hierarchy_begin != NULL) {
				if (components.hierarchy.authority_begin != NULL) {
					_authority = StringT(components.hierarchy.authority_begin, components.hierarchy.authority_end);

					if (components.hierarchy.authority.user_info_begin != NULL) {
						StringT user_info(components.hierarchy.authority.user_info_begin, components.hierarchy.authority.user_info_end);
						split(user_info, ':', std::back_inserter(_user_info));
					}

					if (components.hierarchy.authority.host_begin != NULL) {
						_hostname = StringT(components.hierarchy.authority.host_begin, components.hierarchy.authority.host_end);
					}

					if (components.hierarchy.authority.port_begin != NULL) {
						_port = std::stoul(StringT(components.hierarchy.authority.port_begin, components.hierarchy.authority.port_end));
					}
				}

				if (components.hierarchy.path_begin != NULL) {
					_path = StringT(components.hierarchy.path_begin, components.hierarchy.path_end);
				}
			}

			if (components.query_begin != NULL) {
				_query = StringT(components.query_begin, components.query_end);
			}

			if (components.fragment_begin != NULL) {
				_fragment = StringT(components.fragment_begin, components.fragment_end);
			}
		}

		URI::URI (const StringT & scheme, const Path & path) : _scheme(scheme), _path(path.to_local_path()), _port(0)
		{
		}

		const StringT & URI::scheme () const
		{
			return _scheme;
		}

		const StringT & URI::authority () const
		{
			return _authority;
		}

		const StringT & URI::path () const
		{
			return _path;
		}

		const StringT & URI::query () const
		{
			return _query;
		}

		const StringT & URI::fragment () const
		{
			return _fragment;
		}

		const StringT & URI::hostname () const
		{
			return _hostname;
		}

		const std::vector<StringT> & URI::user_info () const
		{
			return _user_info;
		}

		StringT URI::username () const
		{
			if (_user_info.size() >= 1) {
				return _user_info[0];
			} else {
				return "";
			}
		}

		StringT URI::password () const
		{
			if (_user_info.size() >= 2) {
				return _user_info[1];
			} else {
				return "";
			}
		}

		unsigned URI::port () const
		{
			return _port;
		}

		StringT URI::service () const
		{
			if (_port) {
				StringStreamT buffer;
				buffer << _port;

				return buffer.str();
			} else {
				return _scheme;
			}
		}

		bool URI::is_absolute () const
		{
			return _path[0] == '/';
		}

		bool URI::is_file_path ()
		{
			return scheme() == "file";
		}

// MARK: -
// MARK: Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(URI)
		{
			testing("HTTP URI");
			URI a("http://user12:abc@www.google.com:80/blah?bob=2");
			check(a.scheme() == "http") << "Scheme is correct";

			check(a.username() == "user12") << "User is correct";
			check(a.password() == "abc") << "Password is correct";

			check(a.hostname() == "www.google.com") << "Hostname is correct";
			check(a.port() == 80) << "Port is correct";
			check(a.path() == "/blah") << "Path is correct";
			check(a.query() == "?bob=2") << "Query is correct";

			URI a2("http://localhost");
			check(a2.scheme() == "http") << "scheme " << a2.scheme() << " should be 'http'";
			check(a2.hostname() == "localhost") << "hostname " << a2.hostname() << " should be 'localhost'";

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
