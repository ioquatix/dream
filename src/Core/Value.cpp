/*
 *  Core/Value.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 5/05/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#include "Value.h"

namespace Dream
{
	namespace Core
	{
#pragma mark -
#pragma mark class ConversionError

		ConversionError::ConversionError (const std::type_info & convertFrom, const std::type_info & convertTo) throw ()
		{
			std::stringstream buf;
			buf << "Could not convert from type " << convertFrom.name () << " to " << convertTo.name ();

			m_what = buf.str ();
		}

		ConversionError::~ConversionError () throw ()
		{
		}

		/// Returns a C-style character string describing the general cause of the current error.
		const char * ConversionError::what () const throw ()
		{
			return m_what.c_str();
		}

#pragma mark -
#pragma mark class ValueUndefinedError

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

#pragma mark -
#pragma mark Stream Functions

		std::ostream & operator<< (std::ostream & outs, const ITypedValue & value)
		{
			value.writeToStream(outs);

			return outs;
		}

		std::istream & operator>> (std::istream & ins, ITypedValue & value)
		{
			value.readFromStream(ins);

			return ins;
		}

#pragma mark -
#pragma mark class Value

		Value::Value ()
		{
		}

		void Value::clear ()
		{
			m_ptr.clear();
		}

		bool Value::defined () const
		{
			return m_ptr;
		}

		bool Value::undefined () const
		{
			return !m_ptr;
		}

		bool Value::operator== (const Value & other) const
		{
			// Are both null, or do both point to the same location?
			if (m_ptr == other.m_ptr)
				return true;

			// Are either values undefined?
			if (undefined() || other.undefined())
				return false;

			// Compare the actual ITypedValues
			return *(m_ptr.get()) == *(other.m_ptr.get());
		}

		bool Value::operator== (const ITypedValue & other) const
		{
			if (undefined()) return false;

			return *(m_ptr.get()) == other;
		}
		
		const ITypedValue * Value::typedValue () const
		{
			return m_ptr.get();
		}

		std::ostream & operator<< (std::ostream & outs, const Value & value)
		{
			if (value.undefined()) throw ValueUndefinedError();

			outs << *(value.m_ptr);

			return outs;
		}

		std::istream & operator>> (std::istream & ins, Value & value)
		{
			if (value.undefined()) throw ValueUndefinedError();

			ins >> *(value.m_ptr);

			return ins;
		}
		
		Value Value::readFromBuffer (const Buffer & buf, IndexT & offset)
		{
			TypeIdentifierT typeIdentifier;
			
			offset += buf.read(offset, typeIdentifier);
			
#define TI_CASE(index) case index: return Value(TypeSerialization<index>::readFromBuffer(buf, offset));
			switch (typeIdentifier)
			{
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
		
		void Value::appendToBuffer (ResizableBuffer & buf) const
		{
			if (undefined()) throw ValueUndefinedError();
			
			return m_ptr->appendToBuffer(buf);
		}

#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(Value)
		{
			BEGIN_TEST
			
			testing("Typed Value");
			TypedValue<int> intValue (10);

			assertEqual(intValue.typeinfo(), typeid(5), "Typeinfo is same");
			assertEqual(intValue.size(), sizeof(5), "Size is same");

			TypedValue<unsigned> unsignedValue (10);
			ITypedValue * q1 = &intValue;
			ITypedValue * q2 = &unsignedValue;

			assertEqual(q1->extract<int>(), q2->extract<unsigned>(), "Values are equal");

			q1->set<int>(5);
			q2->set<unsigned>(5);

			assertEqual(q1->extract<int>(), q2->extract<unsigned>(), "Values are equal");

			bool exceptionThrown;

			try
			{
				q1->set<bool>(true);
			}
			catch (ConversionError & err)
			{
				exceptionThrown = true;
			}

			assertTrue(exceptionThrown, "Exception thrown when type is not correct");

			testing("Generic Values");

			Value v1(5);
			Value v2(10);
			Value v3;

			assertEqual(v1.extract<int>(), 5, "Value is set correctly");
			assertEqual(v2.extract<int>(), 10, "Value is set correctly");

			assertTrue(v1.defined(), "Value is defined");
			assertTrue(v2.defined(), "Value is defined");

			assertFalse(v3.defined(), "Value is undefined");
			assertTrue(v3.undefined(), "Value is undefined");

			v1.set(5);
			v2.set(5);

			assertEqual(v1, v2, "Values are equal");
			assertFalse(equal(v1, v3, "Values are not equal"));

			testing("Input and Output");

			std::stringstream buf;

			buf << 15;
			buf >> v1;

			assertEqual(v1.extract<int>(), 15, "Value was parsed correctly");

			buf << v1;
			buf >> v2;

			assertEqual(v1, v2, "Values are equal");
		}
#endif
	}
}