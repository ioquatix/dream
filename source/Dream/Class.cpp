//
//  Class.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 10/05/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "Class.h"

#include "Core/Strings.h"
#include "Core/Timer.h"

#include <map>

namespace Dream
{
// MARK: -
// MARK: class Object

	IObject::~IObject ()
	{
	}

	IClassType::~IClassType()
	{
	}

	Object::Object() : _finalizers(nullptr) {
	}

	Object::~Object () {
		FinalizerReference * current = _finalizers;

		while (current) {
			if (current->finalizer)
				current->finalizer->finalize(this);

			FinalizerReference * next = current->next;

			delete current;
			current = next;
		}
	}

	void Object::insert_finalizer(IFinalizer * finalizer) {
		FinalizerReference * next = new FinalizerReference;

		next->finalizer = finalizer;
		next->next = _finalizers;

		_finalizers = next;
	}

	bool Object::erase_finalizer(IFinalizer * finalizer) {
		FinalizerReference ** previous = &_finalizers;
		FinalizerReference * current = _finalizers;

		// It may be possible to improve upon this approach since it is a linear scan.
		while (current) {
			if (current->finalizer == finalizer) {
				*previous = current->next;

				delete current;

				return true;
			}

			previous = &(current->next);
			current = current->next;
		}

		return false;
	}

// MARK: -
// MARK: Unit Tests

#ifdef ENABLE_TESTING
	UNIT_TEST(Object)
	{
		Ref<ObjectCache<Object>> object_cache = new ObjectCache<Object>;

		{
			Ref<Object> key = new Object;
			Ref<Object> value = new Object;

			object_cache->set(key, value);

			check(object_cache->lookup(key) == value) << "Object was found in cache";
			check(object_cache->objects().size() == 1) << "Cache size is correct";

			check(key->reference_count() == 1) << "Key is referenced locally only";
			check(value->reference_count() == 2) << "Value is referenced locally and in cache";

			key = nullptr;

			check(value->reference_count() == 1) << "Value has been purged from cache";

			value = nullptr;
		}

		// Now that we are out of the scope where key exists, the object has been removed from the cache, since key was automatically deleted.
		check(object_cache->objects().size() == 0) << "Object has been removed";
	}
#endif
}
