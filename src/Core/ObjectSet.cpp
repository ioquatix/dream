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
				
		ObjectSet::ObjectID::ObjectID (ValueT & value, IdentT ID, ObjectSet * ctx) : m_value (value), m_ID (ID)
		{
			
		}
		
		ObjectSet::ObjectID::~ObjectID ()
		{
			
		}
		
		IdentT ObjectSet::ObjectID::identity ()
		{
			return m_ID;
		}
		
		ObjectSet::ValueT & ObjectSet::ObjectID::value ()
		{
			return m_value;
		}
		
		ObjectSet::ObjectSet ()
		{
		}
		
		ObjectSet::~ObjectSet ()
		{
		}
		
		IndexT ObjectSet::size () const
		{
			return m_objects.size() - m_freeIndices.size();
		}
		
		// O(1)
		ObjectSet::ObjectID ObjectSet::fetch (IdentT identity)
		{
			return ObjectID(m_objects[identity], identity, this);
		}
		
		// O(1)
		ObjectSet::ObjectID ObjectSet::insert (ValueT value)
		{
			IdentT identity;
			
			if (m_freeIndices.size() > 0)
			{
				identity = m_freeIndices.front();
				m_freeIndices.pop_front();
				
				m_objects[identity] = value;
			} 
			else
			{
				identity = m_objects.size();
				m_objects.push_back(value);
			}
			
			return ObjectID(m_objects[identity], identity, this);
		}
		
		// O(1)
		// Assigning objects to arbitrary high indices is not recommended,
		// You should generally only use assign to add objects which have
		// already been added to another ObjectSet.
		ObjectSet::ObjectID ObjectSet::assign (ValueT value, IdentT identity)
		{
			if (identity >= m_objects.size())
			{
				m_objects.resize(identity+1);
			}
			
			m_objects[identity] = value;
			
			return ObjectID(value, identity, this);
		}
		
		// O(1)
		void ObjectSet::erase (IdentT identity)
		{
			m_objects[identity] = NULL;
			m_freeIndices.push_back(identity);
			
			//if (m_freeIndices.size() > (m_objects.size() / 2)) {
			//	shrink();
			//}
		}
		
		void ObjectSet::erase (ObjectID & oid)
		{
			erase(oid.identity());
			oid.m_value = NULL;
		}
		
		ObjectSet::Iterator::Iterator(ObjectSet * ctx, IdentT index, Placement placement) : m_objectContext(ctx), m_index(index), m_placement(placement)
		{
		}
		
		ObjectSet::Iterator::Iterator(ObjectSet * ctx, Placement placement) : m_objectContext(ctx), m_index(-1), m_placement(placement)
		{
		}
		
		void ObjectSet::Iterator::operator++ ()
		{
			if (m_placement == FORWARD)
				moveForward();
			else if (m_placement == REVERSE)
				moveReverse();
		}
		
		void ObjectSet::Iterator::operator-- ()
		{
			if (m_placement == FORWARD)
				moveReverse();
			else if (m_placement == REVERSE)
				moveForward();
		}
		
		ObjectSet::ObjectID ObjectSet::Iterator::operator* ()
		{
			if ((m_placement & END) == 0)
				return m_objectContext->fetch(m_index);
			else
			{
				throw std::out_of_range("Trying to dereference the end iterator!");
			}
		}
		
		bool ObjectSet::Iterator::operator== (const Iterator & other) const
		{
			std::cout << "This: " << m_placement << " Other: " << other.m_placement << std::endl;
			if (m_placement & END)
			{
				return m_placement == other.m_placement;
			} else
				return m_index == other.m_index;
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
		
		void ObjectSet::Iterator::moveForward ()
		{
			IndexT top = m_objectContext->m_objects.size () - 1;
			
			do
			{
				if (m_index == top)
				{
					m_index = -1;
					m_placement |= END;
					
					return;
				}
				
				m_index += 1;
			} while (!m_objectContext->m_objects[m_index]);
		}
		
		void ObjectSet::Iterator::moveReverse ()
		{
			do
			{
				if (m_index == 0)
				{
					m_index = -1;
					m_placement |= END;
					
					return;
				}
				
				m_index -= 1;
			} while (!m_objectContext->m_objects[m_index]);
		}

#pragma mark -
#pragma mark Unit Tests
		
#ifdef ENABLE_TESTING
		namespace {
			class TestResource : public Object {
			public:
			
			};
			
			UNIT_TEST(ObjectSet)
			{
				REF(ObjectSet) objects(new ObjectSet);
				
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
