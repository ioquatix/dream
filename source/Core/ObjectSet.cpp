//
//  Core/ObjectSet.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 23/05/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#include "ObjectSet.h"

namespace Dream
{
	namespace Core
	{
		ObjectSet::ObjectID::ObjectID (ValueT & value, IdentT ID, ObjectSet * ctx) : _value (value), _id (ID)
		{
		}

		ObjectSet::ObjectID::~ObjectID ()
		{
		}

		IdentT ObjectSet::ObjectID::identity ()
		{
			return _id;
		}

		ObjectSet::ValueT & ObjectSet::ObjectID::value ()
		{
			return _value;
		}

		ObjectSet::ObjectSet ()
		{
		}

		ObjectSet::~ObjectSet ()
		{
		}

		IndexT ObjectSet::size () const
		{
			return _objects.size() - _free_indices.size();
		}

		// O(1)
		ObjectSet::ObjectID ObjectSet::fetch (IdentT identity)
		{
			return ObjectID(_objects[identity], identity, this);
		}

		// O(1)
		ObjectSet::ObjectID ObjectSet::insert (ValueT value)
		{
			IdentT identity;

			if (_free_indices.size() > 0) {
				identity = _free_indices.front();
				_free_indices.pop_front();

				_objects[identity] = value;
			} else   {
				identity = _objects.size();
				_objects.push_back(value);
			}

			return ObjectID(_objects[identity], identity, this);
		}

		// O(1)
		// Assigning objects to arbitrary high indices is not recommended,
		// You should generally only use assign to add objects which have
		// already been added to another ObjectSet.
		ObjectSet::ObjectID ObjectSet::assign (ValueT value, IdentT identity)
		{
			if (identity >= _objects.size()) {
				_objects.resize(identity+1);
			}

			_objects[identity] = value;

			return ObjectID(value, identity, this);
		}

		// O(1)
		void ObjectSet::erase (IdentT identity)
		{
			_objects[identity] = NULL;
			_free_indices.push_back(identity);

			//if (_free_indices.size() > (_objects.size() / 2)) {
			//	shrink();
			//}
		}

		void ObjectSet::erase (ObjectID & oid)
		{
			erase(oid.identity());
			oid._value = NULL;
		}

		ObjectSet::Iterator::Iterator(ObjectSet * ctx, IdentT index, Placement placement) : _object_context(ctx), _index(index), _placement(placement)
		{
		}

		ObjectSet::Iterator::Iterator(ObjectSet * ctx, Placement placement) : _object_context(ctx), _index(-1), _placement(placement)
		{
		}

		void ObjectSet::Iterator::operator++ ()
		{
			if (_placement == FORWARD)
				move_forward();
			else if (_placement == REVERSE)
				move_reverse();
		}

		void ObjectSet::Iterator::operator-- ()
		{
			if (_placement == FORWARD)
				move_reverse();
			else if (_placement == REVERSE)
				move_forward();
		}

		ObjectSet::ObjectID ObjectSet::Iterator::operator* ()
		{
			if ((_placement & END) == 0)
				return _object_context->fetch(_index);
			else {
				throw std::out_of_range("Trying to deRef the end iterator!");
			}
		}

		bool ObjectSet::Iterator::operator== (const Iterator & other) const
		{
			std::cout << "This: " << _placement << " Other: " << other._placement << std::endl;
			if (_placement & END) {
				return _placement == other._placement;
			} else
				return _index == other._index;
		}

		bool ObjectSet::Iterator::operator!= (const Iterator & other) const
		{
			return !(*this == other);
		}

		ObjectSet::Iterator ObjectSet::begin ()
		{
			return Iterator(this, 0, Iterator::FORWARD);
		}

		ObjectSet::Iterator ObjectSet::end ()
		{
			return Iterator(this, Iterator::FORWARD_END);
		}

		ObjectSet::Iterator ObjectSet::rbegin ()
		{
			return Iterator(this, 0, Iterator::REVERSE);
		}

		ObjectSet::Iterator ObjectSet::rend ()
		{
			return Iterator(this, Iterator::REVERSE_END);
		}

		void ObjectSet::Iterator::move_forward ()
		{
			IndexT top = _object_context->_objects.size () - 1;

			do {
				if (_index == top) {
					_index = -1;
					_placement |= END;

					return;
				}

				_index += 1;
			} while (!_object_context->_objects[_index]);
		}

		void ObjectSet::Iterator::move_reverse ()
		{
			do {
				if (_index == 0) {
					_index = -1;
					_placement |= END;

					return;
				}

				_index -= 1;
			} while (!_object_context->_objects[_index]);
		}

// MARK: -
// MARK: Unit Tests

#ifdef ENABLE_TESTING
		namespace {
			class TestResource : public Object {
			public:
			};

			UNIT_TEST(ObjectSet)
			{
				Ref<ObjectSet> objects(new ObjectSet);

				ObjectSet::ObjectID oid1 = objects->insert(new TestResource);
				ObjectSet::ObjectID oid2 = objects->insert(new TestResource);
				ObjectSet::ObjectID oid3 = objects->insert(new TestResource);
				ObjectSet::ObjectID oid4 = objects->insert(new TestResource);

				check(objects->size() == 4) << "Object set contains 4 objects";

				objects->erase(oid3);

				check(objects->size() == 3) << "Object set contains 3 objects";

				ObjectSet::ObjectID oid5 = objects->insert(new TestResource);

				check(objects->size() == 4) << "Object set contains 4 objects";
				check(oid3.identity() == oid5.identity()) << "Object was placed into previous slot";
			}
		}
#endif
	}
}
