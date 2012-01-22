/*
 *  Core/URI.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Administrator on 22/10/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "URI.h"

#include <string>

namespace Dream
{
	namespace Core
	{
		namespace URIImpl {	
			typedef const StringT::value_type * IteratorT;
			
			struct Authority {
				IteratorT userInfoBegin, userInfoEnd;
				IteratorT hostBegin, hostEnd;
				IteratorT portBegin, portEnd;
			};
			
			struct Hierarchy {
				IteratorT authorityBegin, authorityEnd;
				IteratorT pathBegin, pathEnd;
				
				Authority authority;
			};
			
			struct Components {
				bool complete;
								
				IteratorT schemeBegin, schemeEnd;
				IteratorT hierarchyBegin, hierarchyEnd;
				IteratorT queryBegin, queryEnd;
				IteratorT fragmentBegin, fragmentEnd;
				
				Hierarchy hierarchy;
				
				Components () {
					memset(this, 0, sizeof(*this));
				}
			};
			
			namespace Parser {
				static std::size_t count(IteratorT begin, IteratorT end) {
					return end - begin;
				}
				
				typedef bool(*PredicateT)(IteratorT i);
				
				static IteratorT parsePredicate(PredicateT predicate, IteratorT begin, IteratorT end) {
					while (begin != end && predicate(begin))
						++begin;
					
					return begin;
				}
								
				typedef IteratorT (*ParserT)(IteratorT begin, IteratorT end);
				
				static IteratorT parseString(ParserT parser, IteratorT begin, IteratorT end) {
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
				static IteratorT parseConstant(const char * constant, IteratorT begin, IteratorT end) {
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
				
				static bool isNumeric(IteratorT i) {
					return (*i >= '0' && *i <= '9');
				}
				
				static bool isAlpha(IteratorT i) {
					return (*i >= 'a' && *i <= 'z') || (*i >= 'A' && *i <= 'Z');
				}
				
				static bool isAlphaNumeric(IteratorT i) {
					return isNumeric(i) || isAlpha(i);
				}
				
				static bool isHex(IteratorT i) {
					return isNumeric(i) || (*i >= 'a' && *i <= 'f') || (*i >= 'A' && *i <= 'F');
				}
				
				/* ======================================== */
				/* == General URI parsing as per RFC3986 == */
				/* ======================================== */
				
				//   unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
				static bool isUnreserved(IteratorT i) {
					return isAlphaNumeric(i) || *i == '-' || *i == '.' || *i == '_' || *i == '~';
				}
				
				//   gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
				//static bool isGenDelimiter(IteratorT i) {
				//	return *i == ':' || *i == '/' || *i == '?' || *i == '#' || *i == '[' || *i == ']' || *i == '@';
				//}
				
				//   sub-delims    = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
				static bool isSubDelimiter(IteratorT i) {
					return *i == '!' || *i == '$' || *i == '&' || *i == '\'' || *i == '(' || *i == ')' || *i == '*' || *i == '+' || *i == ',' || *i == ';' || *i == '=';
				}
				
				//   reserved      = gen-delims / sub-delims
				//static bool isReserved(IteratorT i) {
				//	return isGenDelimiter(i) || isSubDelimiter(i);
				//}
				
				//   pct-encoded   = "%" HEXDIG HEXDIG
				static IteratorT parsePercentEncoded(IteratorT begin, IteratorT end) {
					if (count(begin, end) < 3) {
						return begin;
					}
					
					if (*begin == '%' && isHex(begin+1) && isHex(begin+2)) {
						return begin + 3;
					} else {
						return begin;
					}
				}
				
				//   pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
				static bool isPathCharacter(IteratorT i) {
					return *i == ':' || *i == '@';
				}
				
				static IteratorT parsePathCharacter(IteratorT begin, IteratorT end) {
					// We coalesce the parsing into one step and parse as much as possible each time.
					// This isn't strictly following the standard, but it shouldn't be any different in practice.
					IteratorT next = begin;
					
					next = parsePredicate(isUnreserved, next, end);
					next = parseString(parsePercentEncoded, next, end);
					next = parsePredicate(isSubDelimiter, next, end);
					next = parsePredicate(isPathCharacter, next, end);
					
					return next;
				}
				
				//   userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
				static bool isUserInfoCharacter(IteratorT i) {
					return *i == ':';
				}
				
				static IteratorT parseUserInfoCharacter(IteratorT begin, IteratorT end) {
					IteratorT next = begin;
					
					next = parsePredicate(isUnreserved, next, end);
					next = parseString(parsePercentEncoded, next, end);
					next = parsePredicate(isSubDelimiter, next, end);
					next = parsePredicate(isUserInfoCharacter, next, end);
					
					return next;
				}
				
				//   host          = IP-literal / IPv4address / reg-name		
				//   IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
				//   IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
				//   IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
				//   reg-name      = *( unreserved / pct-encoded / sub-delims )
				static IteratorT parseHostCharacter(IteratorT begin, IteratorT end) {
					IteratorT next = begin;
					
					next = parsePredicate(isUnreserved, next, end);
					next = parseString(parsePercentEncoded, next, end);
					next = parsePredicate(isSubDelimiter, next, end);
					
					return next;
				}
				
				static IteratorT parseHost(IteratorT begin, IteratorT end) {
					// For convenience we shortcut this parsing for now.
					return parseString(parseHostCharacter, begin, end);
				}
				
				//   authority     = [ userinfo "@" ] host [ ":" port ]
				//   port          = *DIGIT
				static IteratorT parseAuthority(IteratorT begin, IteratorT end, Authority & authority) {
					IteratorT userInfoBegin = begin;
					IteratorT userInfoEnd = parseString(parseUserInfoCharacter, begin, end);
					
					IteratorT hostBegin = parseConstant("@", userInfoEnd, end);
					if (hostBegin != userInfoEnd) {
						// Valid user info was found:
						authority.userInfoBegin = userInfoBegin;
						authority.userInfoEnd = userInfoEnd;
					}
					
					IteratorT hostEnd = parseHost(hostBegin, end);
					
					if (hostEnd == hostBegin) {
						// We can't continue as host is required.
						return hostEnd;
					}
					
					authority.hostBegin = hostBegin;
					authority.hostEnd = hostEnd;
					
					IteratorT portBegin = parseConstant(":", hostEnd, end);
					
					if (portBegin != hostEnd) {
						IteratorT portEnd = parsePredicate(isNumeric, portBegin, end);
						
						authority.portBegin = portBegin;
						authority.portEnd = portEnd;
						
						return portEnd;
					} else {
						return hostEnd;
					}
				}
				
				static IteratorT parsePath(IteratorT begin, IteratorT end) {
					// This is optional, and signifies an absolute path.
					IteratorT absoluteBegin = parseConstant("/", begin, end);
					
					IteratorT current = parseString(parsePathCharacter, absoluteBegin, end);
					
					if (current == begin)
						return begin;
					
					while (current != end) {
						IteratorT next = parseConstant("/", current, end);
						
						if (next == current)
							return current;
						
						current = parseString(parsePathCharacter, next, end);
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
				
				static IteratorT parseHierarchy(IteratorT begin, IteratorT end, Hierarchy & hierarchy) {
					// (//([^/?#]*))?
					IteratorT authorityBegin = parseConstant("//", begin, end);
					IteratorT authorityEnd = authorityBegin;
					
					if (authorityBegin != begin) {
						authorityEnd = parseAuthority(authorityBegin, end, hierarchy.authority);
						
						hierarchy.authorityBegin = authorityBegin;
						hierarchy.authorityEnd = authorityEnd;
					}
					
					// ([^?#]*)
					hierarchy.pathBegin = authorityEnd;
					hierarchy.pathEnd = parsePath(hierarchy.pathBegin, end);
					
					return hierarchy.pathEnd;
				}
				
				//   scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
				static bool isSchemeCharacter(IteratorT i) {
					return isAlphaNumeric(i) || *i == '+' || *i == '-' || *i == '.';
				}
				
				static IteratorT parseScheme(IteratorT begin, IteratorT end) {
					IteratorT next = parsePredicate(isAlpha, begin, end);
					
					if (next == begin) {
						return begin;
					}
					
					next = parsePredicate(isSchemeCharacter, next, end);
					IteratorT schemeEnd = parseConstant(":", next, end);
					
					if (schemeEnd == next) {
						return begin;
					} else {
						return schemeEnd;
					}
				}
				
				//   fragment      = *( pchar / "/" / "?" )
				//   query         = *( pchar / "/" / "?" )
				static bool isMetaCharacter(IteratorT i) {
					return *i == '/' || *i == '?';
				}
				
				static IteratorT parseMetaCharacter(IteratorT begin, IteratorT end) {
					IteratorT next = begin;
					
					next = parsePathCharacter(next, end);
					next = parsePredicate(isMetaCharacter, next, end);
					
					return next;
				}
				
				static IteratorT parseMeta(const char * prefix, IteratorT begin, IteratorT end) {
					IteratorT next = parseConstant(prefix, begin, end);
					
					if (next != begin) {
						return parseString(parseMetaCharacter, next, end);
					} else {
						return begin;
					}
				}
				
				//   URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
				//   ^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?
				//    12            3  4          5       6  7        8 9
				static IteratorT parse(IteratorT begin, IteratorT end, Components & components) {
					// (([^:/?#]+):)?
					IteratorT schemeBegin = begin;
					IteratorT schemeEnd = parseScheme(schemeBegin, end);
					
					if (schemeEnd != schemeBegin) {
						components.schemeBegin = schemeBegin;
						components.schemeEnd = schemeEnd - 1;	
					}
					
					IteratorT hierarchyBegin = parseConstant(":", schemeEnd, end);
					IteratorT hierarchyEnd = parseHierarchy(hierarchyBegin, end, components.hierarchy);
					
					if (hierarchyEnd == hierarchyBegin) {
						return hierarchyBegin;
					}
					
					components.hierarchyBegin = hierarchyBegin;
					components.hierarchyEnd = hierarchyEnd;
					
					IteratorT queryBegin = hierarchyEnd;
					IteratorT queryEnd = parseMeta("?", queryBegin, end);
					
					if (queryEnd != queryBegin) {
						components.queryBegin = queryBegin;
						components.queryEnd = queryEnd;
					}
					
					IteratorT fragmentBegin = queryEnd;
					IteratorT fragmentEnd = parseMeta("#", fragmentBegin, end);
					
					if (fragmentEnd != fragmentBegin) {
						components.fragmentBegin = fragmentBegin;
						components.fragmentEnd = fragmentEnd;
					}
					
					components.complete = true;
					
					return fragmentEnd;
				}
			}
		};
		
		URI::InvalidFormatError::InvalidFormatError(const StringT & url, std::size_t offset)
		: m_url(url), m_offset(offset)
		{
			StringStreamT buffer;
			
			buffer << "Invalid URI(offset=" << m_offset << "): " << m_url;
			
			m_message = buffer.str();
		}
		
		const char * URI::InvalidFormatError::what() const noexcept
		{
			return m_message.c_str();
		}
		
		URI::URI(const StringT & s) : m_url(s)
		{
			URIImpl::Components components;
			
			URIImpl::IteratorT end = URIImpl::Parser::parse(m_url.data(), m_url.data() + m_url.size(), components);
			
			if (!components.complete) {
				throw InvalidFormatError(m_url, end - m_url.data());
			}
			
			if (components.schemeBegin != NULL) {
				m_scheme = StringT(components.schemeBegin, components.schemeEnd);				
			}

			if (components.hierarchyBegin != NULL) {
				if (components.hierarchy.authorityBegin != NULL) {
					m_authority = StringT(components.hierarchy.authorityBegin, components.hierarchy.authorityEnd);
					
					if (components.hierarchy.authority.userInfoBegin != NULL) {
						StringT userInfo(components.hierarchy.authority.userInfoBegin, components.hierarchy.authority.userInfoEnd);
						split(userInfo, ':', std::back_inserter(m_userInfo));
					}
					
					if (components.hierarchy.authority.hostBegin != NULL) {
						m_hostname = StringT(components.hierarchy.authority.hostBegin, components.hierarchy.authority.hostEnd);
					}
					
					if (components.hierarchy.authority.portBegin != NULL) {
						m_port = std::stoul(StringT(components.hierarchy.authority.portBegin, components.hierarchy.authority.portEnd));
					}
				}
				
				if (components.hierarchy.pathBegin != NULL) {
					m_path = StringT(components.hierarchy.pathBegin, components.hierarchy.pathEnd);
				}
			}
			
			if (components.queryBegin != NULL) {
				m_query = StringT(components.queryBegin, components.queryEnd);
			}
			
			if (components.fragmentBegin != NULL) {
				m_fragment = StringT(components.fragmentBegin, components.fragmentEnd);
			}
		}

		URI::URI (const StringT & scheme, const Path & path)
		: m_scheme(scheme), m_path(path.toLocalPath())
		{
			
		}

		const StringT & URI::scheme () const
		{
			return m_scheme;
		}
		
		const StringT & URI::authority () const
		{
			return m_authority;
		}
		
		const StringT & URI::path () const
		{
			return m_path;
		}
		
		const StringT & URI::query () const
		{
			return m_query;
		}
		
		const StringT & URI::fragment () const
		{
			return m_fragment;
		}

		const StringT & URI::hostname () const
		{
			return m_hostname;
		}
		
		const std::vector<StringT> & URI::userInfo () const
		{
			return m_userInfo;
		}
		
		StringT URI::username () const
		{
			if (m_userInfo.size() >= 1) {
				return m_userInfo[0];
			} else {
				return "";
			}
		}
		
		StringT URI::password () const
		{
			if (m_userInfo.size() >= 2) {
				return m_userInfo[1];
			} else {
				return "";
			}
		}
		
		unsigned URI::port () const
		{
			return m_port;
		}
		
		StringT URI::service () const
		{
			if (m_port) {
				StringStreamT buffer;
				buffer << m_port;
				
				return buffer.str();
			} else {
				return m_scheme;
			}
		}
		
		bool URI::isAbsolute () const
		{
			return m_path[0] == '/';
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