/*
 *  Core/ObjectSet.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 23/05/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CORE_OBJECTSET_H
#define _DREAM_CORE_OBJECTSET_H

#include "../Class.h"

#include <vector>
#include <list>
#include <stdexcept>

#include <iostream>

namespace Dream
{
	namespace Core
	{
		typedef uint32_t IdentT;
/*		
		class IStateful : IMPLEMENTS(Object)
		{
		public:
			// Returns an identical shallow copy of this stateful object.
			virtual REF(IStateful) duplicateState () abstract;
			virtual REF(IData) serializeState () abstract;
			
		};
*/
		
		// Most operations on this set are O(1) at the expense of memory
		class ObjectSet : public Object
		{
			EXPOSE_CLASS (ObjectSet)
			
			class Class : public Object::Class
			{
				EXPOSE_CLASSTYPE
			};
			
		public:
			typedef REF(Object) ValueT;
			
			class ObjectID
			{
			protected:
				ValueT & m_value;
				IdentT m_ID;
				
				friend class ObjectSet;
				
				ObjectID (ValueT & value, IdentT ID, ObjectSet * ctx);
				
			public:
				virtual ~ObjectID ();
				
				IdentT identity ();
				ValueT & value ();
			};
			
		protected:
			std::vector<ValueT> m_objects;
			std::list<IdentT> m_freeIndices;
			
		public:
			ObjectSet ();
			virtual ~ObjectSet ();
			
			IndexT size () const;
			
			// O(1)
			ObjectID fetch (IdentT identity);
			
			// O(1)
			ObjectID insert (ValueT value);
			
			// O(1)
			// Assigning objects to arbitrary high indices is not recommended,
			// You should generally only use assign to add objects which have
			// already been added to another ObjectSet.
			ObjectID assign (ValueT value, IdentT identity);
			
			// O(1)
			void erase (IdentT identity);
			void erase (ObjectID & oid);
			
			class Iterator
			{
			protected:
				friend class ObjectSet;
				
				enum Placement
				{
					FORWARD = 1,
					REVERSE = 2,
					END = 4,
					FORWARD_END = FORWARD | END,
					REVERSE_END = REVERSE | END
				};
				
				ObjectSet * m_objectContext;
				IdentT m_index;
				unsigned m_placement;
				
				Iterator(ObjectSet * ctx, IdentT index, Placement placement);
				Iterator(ObjectSet * ctx, Placement placement);
				
				void moveForward ();
				void moveReverse ();
				
			public:
				void operator++ ();
				void operator-- ();
				
				ObjectID operator* ();
				
				bool operator== (const Iterator & other) const;
				bool operator!= (const Iterator & other) const;
			};
			
			Iterator begin ();
			Iterator end ();
			Iterator rbegin ();
			Iterator rend ();
		};		
	}
}

#endif