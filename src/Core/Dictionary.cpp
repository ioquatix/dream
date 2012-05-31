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
		inline IndexT size_of_type_identifier (TypeIdentifierT tid)
		{
			return tid & 0x0F;
		}
		
		/// Returns whether the key has a value in the dictionary.
		bool Dictionary::key (const KeyT & key)
		{
			return _values.find(key) != _values.end();
		}
		
		void Dictionary::set_value (const KeyT & key, const Value & value)
		{
			_values[key] = value;
		}
		
		const Value Dictionary::get_value (const KeyT & key) const
		{
			ValuesT::const_iterator i = _values.find(key);
			
			if (i != _values.end())
				return i->second;
			else
				return Value();
		}
		
		void Dictionary::update (const Ptr<Dictionary> other)
		{
			ValuesT values = other->_values;
			values.insert(_values.begin(), _values.end());
			_values.swap(values);
		}
		
		void Dictionary::insert (const Ptr<Dictionary> other)
		{
			_values.insert(other->_values.begin(), other->_values.end());
		}
		
		Ref<IData> Dictionary::serialize () const
		{
			Shared<DynamicBuffer> buffer(new DynamicBuffer);
			
			for (ValuesT::const_iterator i = _values.begin(); i != _values.end(); i++)
			{
				const KeyT & key = i->first;
				const Value & value = i->second;
				
				TypeSerialization<TI_STRING>::append_to_buffer(*buffer, key);
				value.append_to_buffer(*buffer);
			}
			
			buffer->hexdump(std::cout);
			
			return new BufferedData(buffer);
		}
		
		void Dictionary::deserialize (Ref<IData> data)
		{
			Shared<Buffer> buffer = data->buffer();
			IndexT offset = 0;
			
			while (offset < buffer->size())
			{
				KeyT key = TypeSerialization<TI_STRING>::read_from_buffer(*buffer, offset);
				_values[key] = Value::read_from_buffer(*buffer, offset);
			}
		}
		
		void Dictionary::debug (std::ostream & out) const
		{
			for (ValuesT::const_iterator i = _values.begin(); i != _values.end(); i++)
			{
				const KeyT & key = i->first;
				const Value & value = i->second;
				
				out << key << ": " << value << std::endl;
			}
		}
		
// MARK: mark -
// MARK: mark Unit Tests
		
#ifdef ENABLE_TESTING
		namespace {
			UNIT_TEST(Dictionary)
			{
				testing("Serialization");
				
				Ref<Dictionary> dict1(new Dictionary);
				
				dict1->set("Key1", 5);
				dict1->set("Key2", "Apples");
				
				int key1 = dict1->get<int>("Key1");
				
				check(key1 == 5) << "Value is equal";
				
				Ref<IData> data = dict1->serialize();
				
				Ref<Dictionary> dict2(new Dictionary);
				
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
