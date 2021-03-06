//
//  Core/Path.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/09/10.
//  Copyright (c) 2010 Samuel Williams. All rights reserved.
//
//

#include "Path.h"
#include <iterator>

#include <cstddef>

namespace Dream
{
	namespace Core
	{
		Path::Path(const ComponentsT & components) : _components(components) {
		}

		Path::Path(const char * string_rep) {
			split(string_rep, Path::SEPARATOR, std::inserter(_components, _components.begin()));
		}

		Path::Path(const StringT & string_rep) {
			split(string_rep, Path::SEPARATOR, std::inserter(_components, _components.begin()));
		}

		Path::Path(const StringT & string_rep, const char separator) {
			split(string_rep, separator, std::inserter(_components, _components.begin()));
		}

		StringT Path::to_local_path() const {
			StringStreamT buffer;

			for (std::size_t i = 0; i < _components.size(); i++) {
				if (i > 0) buffer << Path::SEPARATOR;

				buffer << _components[i];
			}

			return buffer.str();
		}

		bool Path::is_absolute() const {
			return _components.front() == "";
		}

		Path Path::to_absolute() {
			ComponentsT result;
			result.push_back("");
			result.insert(result.end(), _components.begin(), _components.end());

			return Path(result);
		}

		bool Path::is_directory() const {
			return _components.back() == "";
		}

		Path Path::to_directory() {
			ComponentsT result(_components);
			result.push_back("");

			return Path(result);
		}

		Path Path::simplify() const {
			ComponentsT result;

			if (is_absolute())
				result.push_back("");

			for (std::size_t i = 0; i < _components.size(); i++) {
				const StringT & bit = _components[i];

				if (bit == "..") {
					result.pop_back();
				} else if (bit != "." && bit != "") {
					result.push_back(bit);
				}
			}

			return Path(result);
		}

		Path::NameComponents Path::last_name_components () const {
			NameComponents name;
			const StringT & last_component = _components.back();

			if (!last_component.empty()) {
				std::size_t pos = last_component.find('.');

				if (pos != StringT::npos) {
					name.basename = StringT(&last_component[0], pos);

					pos++; // Skip over the dot
					name.extension = StringT(&last_component[pos], last_component.size() - pos);
				} else {
					name.basename = last_component;
				}
			}

			return name;
		}

		Path Path::parent_path (std::size_t n) const {
			ComponentsT result;

			if (n < _components.size()) {
				result.insert(result.end(), _components.begin(), _components.end() - n);
			} else if (is_absolute()) {
				result.push_back("");
			}

			return Path(result);
		}

		Path Path::with_extension(const StringT & extension) const {
			Path copy = this->parent_path();

			NameComponents name = this->last_name_components();
			name.extension = extension;

			return copy + name;
		}

		Path Path::operator+(const Path & other) const {
			ComponentsT result(_components);
			result.insert(result.end(), other._components.begin(), other._components.end());

			return Path(result);
		}

		Path Path::operator+(const NameComponents & other) const {
			ComponentsT result(_components);
			result.push_back(other.basename + '.' + other.extension);

			return Path(result);
		}

		bool Path::operator<(const Path & other) const {
			std::size_t i = 0;

			for (; i < _components.size() && i < other._components.size(); i++) {
				if (_components[i] < other._components[i])
					return true;

				if (_components[i] > other._components[i])
					return false;
			}

			std::ptrdiff_t diff = _components.size() - other._components.size();

			// diff is less than zero if |this| < |other|
			return diff < 0;
		}

		bool Path::operator==(const Path & other) const {
			return _components == other._components;
		}

		std::ostream & operator<<(std::ostream & output, const Path & path) {
			return (output << path.to_local_path());
		}

// MARK: -
// MARK: Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(Path)
		{
			testing("Absolute & Directory");

			Path p1("/apples/oranges"), p2("grapefruit/"), p3("bananna/peach");

			check(p1.is_absolute()) << p1 << " is absolute";
			check(!p2.is_absolute()) << p2 << " is relative";
			check(p2.is_directory()) << p2 << " is a directory";
			check(!p3.is_directory()) << p3 << " is not a directory";

			testing("Local Paths");

			check(p1.to_local_path() == "/apples/oranges") << p1 << " was correctly converted";
			check(p2.to_local_path() == "grapefruit/") << p2 << " was correctly converted";
			check(p3.to_local_path() == "bananna/peach") << p3 << " was correctly converted";

			testing("Path Manipulation");

			Path p4("files/image.jpeg"), p5("bob"), p6("/dole");

			Path::NameComponents name_components = p4.last_name_components();

			check(name_components.basename == "image") << p4 << " has correct basename; got " << name_components.basename;
			check(name_components.extension == "jpeg") << p4 << " has correct extension; got " << name_components.extension;
			check((p5 + p4) == "bob/files/image.jpeg") << p5 << " + " << p4 << " gave correct result";

			Path p7 = p1 + "../john.png";

			check(p7.simplify() == "/apples/john.png") << p7 << " was simplified correctly";
		}
#endif
	}
}
