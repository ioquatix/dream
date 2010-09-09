/*
 *  Path.cpp
 *  Dream
 *
 *  Created by Samuel Williams on 2/09/10.
 *  Copyright 2010 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Path.h"
#include <iterator>

#include <cstddef>

namespace Dream
{
	namespace Core
	{
		Path::Path(const ComponentsT & components) : m_components(components) {

		}
		
		Path::Path(const char * stringRep) {
			split(stringRep, Path::SEPARATOR, std::inserter(m_components, m_components.begin()));
		}
		
		Path::Path(const StringT & stringRep) {
			split(stringRep, Path::SEPARATOR, std::inserter(m_components, m_components.begin()));
		}
		
		Path::Path(const StringT & stringRep, const char separator) {
			split(stringRep, separator, std::inserter(m_components, m_components.begin()));
		}
		
		StringT Path::toLocalPath() const {
			StringStreamT buffer;
			
			for (std::size_t i = 0; i < m_components.size(); i++) {
				if (i > 0) buffer << Path::SEPARATOR;
				
				buffer << m_components[i];
			}
			
			return buffer.str();
		}

		bool Path::isAbsolute() const {
			return m_components.front() == "";
		}

		Path Path::toAbsolute() {
			ComponentsT result;
			result.push_back("");
			result.insert(result.end(), m_components.begin(), m_components.end());
			
			return Path(result);
		}

		bool Path::isDirectory() const {
			return m_components.back() == "";
		}

		Path Path::toDirectory() {
			ComponentsT result(m_components);
			result.push_back("");
			
			return Path(result);
		}
					
		Path Path::simplify() const {
			ComponentsT result;

			if (isAbsolute())
				result.push_back("");
			
			for (std::size_t i = 0; i < m_components.size(); i++) {
				const StringT & bit = m_components[i];
				
				if (bit == "..") {
					result.pop_back();
				} else if (bit != "." && bit != "") {
					result.push_back(bit);
				}
			}
			
			return Path(result);
		}
		
		Path::NameInfo Path::splitFileName () const {
			NameInfo nameInfo;
			const StringT & name = m_components.back();
			
			if (!name.empty()) {
				std::size_t pos = name.find('.');
				
				if (pos != StringT::npos) {
					nameInfo.basename = StringT(&name[0], pos);

					pos++; // Skip over the dot
					nameInfo.extension = StringT(&name[pos], name.size() - pos);
				} else {
					nameInfo.basename = name;
				}
			}
			
			return nameInfo;
		}
		
		Path Path::dirname (std::size_t n) const {
			ComponentsT result;
			
			if (n < m_components.size()) {
				result.insert(result.end(), m_components.begin(), m_components.end() - n);
			} else if (isAbsolute()) {
				result.push_back("");
			}
			
			return Path(result);
		}
		
		Path Path::operator+(const Path & other) const {
			ComponentsT result(m_components);
			result.insert(result.end(), other.m_components.begin(), other.m_components.end());
			
			return Path(result);
		}
		
		Path Path::operator+(const NameInfo & other) const {
			ComponentsT result(m_components);
			result.push_back(other.basename + other.extension);
			
			return Path(result);
		}
		
		bool Path::operator<(const Path & other) const {
			std::size_t i = 0;
			
			for (; i < m_components.size() && i < other.m_components.size(); i++) {
				if (m_components[i] < other.m_components[i])
					return true;
				
				if (m_components[i] > other.m_components[i])
					return false;
			}
			
			std::ptrdiff_t diff = m_components.size() - other.m_components.size();
			
			// diff is less than zero if |this| < |other|
			return diff < 0;			
		}
		
		bool Path::operator==(const Path & other) const {
			return m_components == other.m_components;
		}
		
		std::ostream & operator<<(std::ostream & output, const Path & path) {
			return (output << path.toLocalPath());
		}
		
#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(Path)
		{
			testing("Absolute & Directory");
			
			Path p1("/apples/oranges"), p2("grapefruit/"), p3("bananna/peach");

			check(p1.isAbsolute()) << p1 << " is absolute";
			check(!p2.isAbsolute()) << p2 << " is relative";
			check(p2.isDirectory()) << p2 << " is a directory";
			check(!p3.isDirectory()) << p3 << " is not a directory";
			
			testing("Local Paths");
			
			check(p1.toLocalPath() == "/apples/oranges") << p1 << " was correctly converted";
			check(p2.toLocalPath() == "grapefruit/") << p2 << " was correctly converted";
			check(p3.toLocalPath() == "bananna/peach") << p3 << " was correctly converted";
			
			testing("Path Manipulation");
			
			Path p4("files/image.jpeg"), p5("bob"), p6("/dole");
			
			Path::NameInfo nameInfo = p4.splitFileName();
			
			check(nameInfo.basename == "image") << p4 << " has correct basename; got " << nameInfo.basename;
			check(nameInfo.extension == "jpeg") << p4 << " has correct extension; got " << nameInfo.extension;
			check((p5 + p4) == "bob/files/image.jpeg") << p5 << " + " << p4 << " gave correct result";
			
			Path p7 = p1 + "../john.png";
			
			check(p7.simplify() == "/apples/john.png") << p7 << " was simplified correctly";
		}
#endif
		
	}
}

