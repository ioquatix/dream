//
//  Core/Value.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/05/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#include "Value.h"

namespace Dream
{
	namespace Core
	{
// MARK: -
// MARK: class ConversionError

		ConversionError::ConversionError (const std::type_info & convert_from, const std::type_info & convert_to) throw ()
		{
			std::stringstream buf;
			buf << "Could not convert from type " << convert_from.name () << " to " << convert_to.name ();

			_what = buf.str ();
		}

		ConversionError::~ConversionError () throw ()
		{
		}

		/// Returns a C-style character string describing the general cause of the current error.
		const char * ConversionError::what () const throw ()
		{
			return _what.c_str();
		}

// MARK: -
// MARK: class ValueUndefinedError

		ValueUndefinedError::ValueUndefinedError () throw ()
		{
		}

		ValueUndefinedError::~ValueUndefinedError () throw ()
		{
		}

		/// Returns a C-style character string describing the general cause of the current error.
		const char * ValueUndefinedError::what () const throw ()
		{
			return "The value is undefined";
		}

// MARK: -
// MARK: class ValueUndefinedError

		SerializationError::SerializationError () throw ()
		{
		}

		SerializationError::~SerializationError () throw ()
		{
		}

		/// Returns a C-style character string describing the general cause of the current error.
		const char * SerializationError::what () const throw ()
		{
			return "The value could not be serialized.";
		}

// MARK: -
// MARK: Stream Functions

		std::ostream & operator<< (std::ostream & outs, const ITypedValue & value)
		{
			value.write_to_stream(outs);

			return outs;
		}

		static std::istream & operator>> (std::istream & ins, ITypedValue & value)
		{
			value.read_from_stream(ins);

			return ins;
		}

// MARK: -
// MARK: class Value

		Value::Value ()
		{
		}

		void Value::clear ()
		{
			_ptr.clear();
		}

		bool Value::defined () const
		{
			return _ptr;
		}

		bool Value::undefined () const
		{
			return !_ptr;
		}

		bool Value::operator== (const Value & other) const
		{
			// Are both null, or do both point to the same location?
			if (_ptr == other._ptr)
				return true;

			// Are either values undefined?
			if (undefined() || other.undefined())
				return false;

			// Compare the actual ITypedValues
			return _ptr.get()->equal(other._ptr.get());
		}

		bool Value::operator== (const ITypedValue & other) const
		{
			if (undefined()) return false;

			return _ptr.get()->equal(&other);
		}

		const ITypedValue * Value::typed_value () const
		{
			return _ptr.get();
		}

		std::ostream & operator<< (std::ostream & outs, const Value & value)
		{
			if (value.undefined()) {
				outs << "undefined";
			} else {
				outs << *(value._ptr);
			}

			return outs;
		}

		std::istream & operator>> (std::istream & ins, Value & value)
		{
			if (value.undefined()) throw ValueUndefinedError();

			ins >> *(value._ptr);

			return ins;
		}

		Value Value::read_from_buffer (const Buffer & buf, std::size_t & offset)
		{
			TypeIdentifierT type_identifier;

			offset += buf.read(offset, type_identifier);

#define TI_CASE(index) case index: return Value(TypeSerialization<index>::read_from_buffer(buf, offset));
			switch (type_identifier) {
				TI_CASE(TI_UINT8)
				TI_CASE(TI_INT8)
				TI_CASE(TI_UINT16)
				TI_CASE(TI_INT16)
				TI_CASE(TI_UINT32)
				TI_CASE(TI_INT32)
				TI_CASE(TI_FLOAT32)
				TI_CASE(TI_UINT64)
				TI_CASE(TI_INT64)
				TI_CASE(TI_FLOAT64)
				TI_CASE(TI_STRING)
			default:
				return Value();
			}

#undef TI_CASE
		}

		void Value::append_to_buffer (ResizableBuffer & buf) const
		{
			if (undefined()) throw ValueUndefinedError();

			return _ptr->append_to_buffer(buf);
		}

// MARK: -
// MARK: Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(Value)
		{
			testing("Typed Value");
			TypedValue<int> int_value (10);

			check(int_value.value_type() == typeid(5)) << "Typeinfo is same";
			check(int_value.size() == sizeof(5)) << "Size is same";

			TypedValue<unsigned> unsigned_value (10);
			ITypedValue * q1 = &int_value;
			ITypedValue * q2 = &unsigned_value;

			check(q1->extract<int>() == q2->extract<unsigned>()) << "Values are equal";

			q1->set<int>(5);
			q2->set<unsigned>(5);

			check(q1->extract<int>() == q2->extract<unsigned>()) << "Values are equal";

			bool exception_thrown;

			try {
				q1->set<bool>(true);
			} catch (ConversionError & err)   {
				exception_thrown = true;
			}

			check(exception_thrown) << "Exception thrown when type is not correct";

			testing("Generic Values");

			Value v1(5);
			Value v2(10);
			Value v3;

			check(v1.extract<int>() == 5) << "Value is set correctly";
			check(v2.extract<int>() == 10) << "Value is set correctly";

			check(v1.defined()) << "Value is defined";
			check(v2.defined()) << "Value is defined";

			check(!v3.defined()) << "Value is undefined";
			check(v3.undefined()) << "Value is undefined";

			v1.set(5);
			v2.set(5);

			check(v1 == v2) << "Values are equal";
			check(!(v1 == v3)) << "Values are not equal";

			testing("Input and Output");

			std::stringstream b1, b2;

			b1 << 15;
			b1 >> v1;

			check(v1.extract<int>() == 15) << "Value was parsed correctly";

			b2 << v1;
			b2 >> v2;

			check(v1 == v2) << "Values are equal";

			testing("Pointers");

			int i = 1;
			float f = 1.0;
			v1.set(&i);
			v2.set(&i);
			v3.set(&f);

			check(v1 == v2) << "Values are equal";
			check(!(v2 == v3)) << "Values are not equal";
		}
#endif
	}
}
