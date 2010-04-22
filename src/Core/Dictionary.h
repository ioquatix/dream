/*
 *  Core/Dictionary.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 22/12/08.
 *  Copyright 2008 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CORE_DICTIONARY_H
#define _DREAM_CORE_DICTIONARY_H

#include "Class.h"
#include "Value.h"
#include "Data.h"

#include <map>

namespace Dream
{
	namespace Core
	{
		
		class Dictionary : public Object
		{
			EXPOSE_CLASS(Dictionary)
			
			class Class : public Object::Class
			{
				EXPOSE_CLASSTYPE
			};
			
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
			
			REF(IData) serialize () const;
			void deserialize (REF(IData) data);
			
			void debug (std::ostream &) const;
		};
		
	}
}

#endif