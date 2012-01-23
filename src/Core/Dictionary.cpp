//
//  Core/Dictionary.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 22/12/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#include "Dictionary.h"

namespace Dream
{
	namespace Core
	{		
		inline IndexT sizeOfTypeIdentifier (TypeIdentifierT tid)
		{
			return tid & 0x0F;
		}
		
		/// Returns whether the key has a value in the dictionary.
		bool Dictionary::key (const KeyT & key)
		{
			return m_values.find(key) != m_values.end();
		}
		
		void Dictionary::setValue (const KeyT & key, const Value & value)
		{
			m_values[key] = value;
		}
		
		const Value Dictionary::getValue (const KeyT & key) const
		{
			ValuesT::const_iterator i = m_values.find(key);
			
			if (i != m_values.end())
				return i->second;
			else
				return Value();
		}
		
		void Dictionary::update (const PTR(Dictionary) other)
		{
			ValuesT values = other->m_values;
			values.insert(m_values.begin(), m_values.end());
			m_values.swap(values);
		}
		
		void Dictionary::insert (const PTR(Dictionary) other)
		{
			m_values.insert(other->m_values.begin(), other->m_values.end());
		}
		
		REF(IData) Dictionary::serialize () const
		{
			Shared<DynamicBuffer> buffer(new DynamicBuffer);
			
			for (ValuesT::const_iterator i = m_values.begin(); i != m_values.end(); i++)
			{
				const KeyT & key = i->first;
				const Value & value = i->second;
				
				TypeSerialization<TI_STRING>::appendToBuffer(*buffer, key);
				value.appendToBuffer(*buffer);
			}
			
			buffer->hexdump(std::cout);
			
			return new BufferedData(buffer);
		}
		
		void Dictionary::deserialize (REF(IData) data)
		{
			Shared<Buffer> buffer = data->buffer();
			IndexT offset = 0;
			
			while (offset < buffer->size())
			{
				KeyT key = TypeSerialization<TI_STRING>::readFromBuffer(*buffer, offset);
				m_values[key] = Value::readFromBuffer(*buffer, offset);
			}
		}
		
		void Dictionary::debug (std::ostream & out) const
		{
			for (ValuesT::const_iterator i = m_values.begin(); i != m_values.end(); i++)
			{
				const KeyT & key = i->first;
				const Value & value = i->second;
				
				out << key << ": " << value << std::endl;
			}
		}
		
#pragma mark -
#pragma mark Unit Tests
		
#ifdef ENABLE_TESTING
		namespace {
			UNIT_TEST(Dictionary)
			{
				testing("Serialization");
				
				REF(Dictionary) dict1(new Dictionary);
				
				dict1->set("Key1", 5);
				dict1->set("Key2", "Apples");
				
				int key1 = dict1->get<int>("Key1");
				
				check(key1 == 5) << "Value is equal";
				
				REF(IData) data = dict1->serialize();
				
				REF(Dictionary) dict2(new Dictionary);
				
				dict2->deserialize(data);
				
				check(dict1->get<int>("Key1") == dict2->get<int>("Key1")) << "Values are equal";
				
				dict1->set("Dictionary Name", "dict1");
				
				std::cout << "Dict 1" << std::endl;
				dict1->debug(std::cout);
				
				std::cout << "Dict 2" << std::endl;
				dict2->debug(std::cout);
				
				
			}
		}
#endif
		
	}
}
