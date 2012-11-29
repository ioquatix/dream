//
//  Ref.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 3/10/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#include "Reference.h"

#include <set>


namespace Dream {
	SharedObject::SharedObject () : _count(0) {
		#ifdef TRACK_ALLOCATIONS
		s_allocations[this] = ALLOCATED;
		#endif
	}

	SharedObject::SharedObject (const SharedObject & other) : _count(0) {
	}

	SharedObject & SharedObject::operator= (const SharedObject & other) {
		// Don't touch reference count.

		return *this;
	}

	SharedObject::~SharedObject () {
	}

	void SharedObject::retain () const {
		_count.fetch_add(1);
	}

	bool SharedObject::release () const {
		// Returns the value before subtracting 1:
		NumberT count = _count.fetch_sub(1);

		if (count == 1) {
			deallocate();
			return true;
		}

		return false;
	}

	void SharedObject::deallocate () const {
		delete this;
	}

	SharedObject::NumberT SharedObject::reference_count () const {
		return _count.load();
	}

// MARK: -
// MARK: Unit Tests

#ifdef ENABLE_TESTING

	UNIT_TEST(Ref)
	{
		testing("Shared Object");

		Ref<SharedObject> s1, s2, s3;

		check(s1 == s2) << "Null objects are equal";

		s1 = new SharedObject;

		check(s1 != s2) << "Null and non-null objects are not equal";

		s2 = new SharedObject;
		s3 = new SharedObject;

		check(s1 == s1) << "Objects are equal";
		check(s1 != s2) << "Objects are not equal";

		std::set<Ref<SharedObject>> objects;
		objects.insert(s1);
		objects.insert(s2);
		objects.insert(s3);

		check(objects.size() == 3) << "Set contains correct number of objects";
	}

	UNIT_TEST(Shared)
	{
		testing("Shared Integers");

		Shared<int> s1(new int), s2, s3;

		s2 = s1;

		*s1 = 10;

		check(*s2 == 10) << "Value was not the same!";
	}
#endif
}
