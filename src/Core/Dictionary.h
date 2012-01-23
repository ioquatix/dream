//
//  Core/Dictionary.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 22/12/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CORE_DICTIONARY_H
#define _DREAM_CORE_DICTIONARY_H

#include "../Class.h"
#include "Value.h"
#include "Data.h"

#include <map>

namespace Dream
{
	namespace Core
	{
		
		class Dictionary : public Object
		{
		public:
			typedef std::string KeyT;
			
		protected:
			typedef std::map<KeyT, Value> ValuesT;
			ValuesT m_values;
			
		public:			
			/// Returns whether the key has a value in the dictionary.
			bool key (const KeyT & key);
			
			void setValue (const KeyT & key, const Value & value);
			const Value getValue (const KeyT & key) const;
			
			template <typename t>
			void set (const KeyT & key, const t & value)
			{
				setValue(key, Value(value));
			}
			
			template <typename ValueT>
			const ValueT get (const KeyT & key)
			{
				Value v = getValue(key);
				
				return v.extract<ValueT>();
			}
			
			template <typename ValueT>
			bool get (const KeyT & key, ValueT & value)
			{
				Value v = getValue(key);
				
				if (v.defined()) {
					value = v.extract<ValueT>();
					
					return true;
				} else {
					return false;
				}
			}
			
			/// Updates a value if the key exists in the dictionary.
			template <typename t>
			bool update (const KeyT & key, t & value) const
			{
				ValuesT::const_iterator i = m_values.find(key);
				
				if (i != m_values.end())
				{
					value = i->second.extract<t>();
					return true;
				}
				
				return false;
			}
			
			// Overwrites values present in the other dictionary.
			void update (const PTR(Dictionary) other);
			
			// Only inserts key-values that don't already exist.
			void insert (const PTR(Dictionary) other);
			
			REF(IData) serialize () const;
			void deserialize (REF(IData) data);
			
			void debug (std::ostream &) const;
		};
		
	}
}

#endif
