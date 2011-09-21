/*
 *  Core/Value.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 5/05/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CORE_VALUE_H
#define _DREAM_CORE_VALUE_H

#include "Strings.h"
#include "Data.h"
#include "Serialization.h"

namespace Dream
{
	namespace Core
	{		
		/** Thrown when a value cannot be converted to another of a different type.
		 */
		class ConversionError : public std::exception
		{
			StringT m_what;
		public:
			ConversionError (const std::type_info & convertFrom, const std::type_info & convertTo) throw ();
			virtual ~ConversionError () throw ();

			/// Returns a C-style character string describing the general cause of the current error.
			virtual const char * what () const throw ();
		};

		/** Thrown when a value is dereferenced and is undefined.
		 */
		class ValueUndefinedError : public std::exception
		{
		public:
			ValueUndefinedError () throw ();
			virtual ~ValueUndefinedError () throw ();

			/// Returns a C-style character string describing the general cause of the current error.
			virtual const char * what () const throw ();
		};
		
		/** Thrown when a value unable to be serialized.
		 */
		class SerializationError : public std::exception
		{
		public:
			SerializationError () throw ();
			virtual ~SerializationError () throw ();

			/// Returns a C-style character string describing the general cause of the current error.
			virtual const char * what () const throw ();
		};

		/** Abstract typed value base. Provides a basic set of functionality for dealing with values of unknown type.
		 */
		class ITypedValue : public SharedObject
		{
		public:
			/// @sa equal
			bool operator== (const ITypedValue & other) const
			{
				return equal(&other);
			}
			
			virtual const std::type_info & valueType () const abstract;
			
			/// Compares two values
			/// @returns True if the TypedValue objects are of the same type and contained value.			
			virtual bool equal (const ITypedValue *) const abstract;

			virtual void writeToStream (std::ostream &) const abstract;
			virtual void readFromStream (std::istream &) abstract;

			/// Attempts to extract the contained value as the given type.
			/// @returns true if the value was extracted
			template <typename ValueT>
			bool extract (ValueT &) const;

			/// Attempts to extract the contained value as the given type.
			/// @throws ConversionError if the type given is incorrect.
			template <typename ValueT>
			const ValueT & extract () const;

			/// Attempts to set the value contained within.
			/// @throws ConversionError if the type given is incorrect.
			template <typename ValueT>
			void set (const ValueT &);
			
			/// A bit like typeinfo, but only works for specific primitive types and returns well-defined values.
			virtual TypeIdentifierT typeIndex () const abstract;
			
			/// Returns a pointer to the value storage.
			virtual const ByteT * valueData () const abstract;
			
			/// Append the value to a buffer.
			virtual void appendToBuffer (ResizableBuffer & buf) const abstract;
		};

		/// Stream helper for printing and converting types to strings
		/// @sa ITypedValue::writeToStream
		std::ostream & operator<< (std::ostream &, const ITypedValue &);
		std::istream & operator>> (std::istream &, const ITypedValue &);

		template <unsigned TypeIdentifier>
		struct TypedValueSerializer {
			template <typename ValueT>
			void readFromStream (std::istream & stream, ValueT & value)
			{
				stream >> value;
			}
		
			template <typename ValueT>
			void appendToBuffer (ResizableBuffer & buf, ValueT & value) const
			{
				buf.append((TypeIdentifierT)TypeIdentifier);
				TypeSerialization<TypeIdentifier>::appendToBuffer(buf, value);
			}
		};
		
		template <>
		struct TypedValueSerializer<0> {
			template <typename ValueT>
			void readFromStream (std::istream & stream, ValueT & value)
			{
				throw SerializationError();
			}

			template <typename ValueT>
			void appendToBuffer (ResizableBuffer & buf, ValueT & value) const
			{
				throw SerializationError();
			}
		};

		/** A simple typed value wrapper class.

		 @code
		 TypedValue<int> container(5);
		 container.value = 5;
		 @endcode
		 */
		template <typename ValueT>
		class TypedValue : implements ITypedValue, protected TypedValueSerializer<TypeIdentifierTypeTraits<ValueT>::TypeIdentifierValue>
		{
		protected:
			ValueT m_value;
			typedef TypedValueSerializer<TypeIdentifierTypeTraits<ValueT>::TypeIdentifierValue> TypedValueSerializerT;

		public:
			TypedValue ()
			{
			}

			/// Construct with supplied value
			TypedValue (const ValueT & v)
			{
				m_value = v;
			}

			virtual ~TypedValue ()
			{
			}
			
			virtual TypeIdentifierT typeIndex () const
			{
				return TypeIdentifierTypeTraits<ValueT>::TypeIdentifierValue;
			}
			
			virtual const std::type_info & valueType () const
			{
				return typeid(ValueT);
			}

			virtual IndexT size () const
			{
				return sizeof(ValueT);
			}
			
			const ByteT * valueData () const
			{
				return (const ByteT *)&m_value;
			}

			/// Retrieve non-const value reference.
			ValueT & value ()
			{
				return m_value;
			}

			/// Retrieve const value reference.
			const ValueT & value () const
			{
				return m_value;
			}

			virtual bool equal (const ITypedValue * other) const
			{
				typedef TypedValue<ValueT> TypedValueT;
				const TypedValueT * otherTypedValue = dynamic_cast<const TypedValueT *>(other);

				if (otherTypedValue != NULL)
					return this->value() == otherTypedValue->value();
				else
					return false;
			}

			virtual void writeToStream (std::ostream & stream) const
			{
				stream << m_value;
			}
			
			virtual void readFromStream (std::istream & stream)
			{
				TypedValueSerializerT::readFromStream(stream, m_value);
			}
			
			virtual void appendToBuffer (ResizableBuffer & buf) const
			{
				TypedValueSerializerT::appendToBuffer(buf, m_value);
			}
		};

		template <typename ValueT>
		bool ITypedValue::extract (ValueT & value) const
		{
			typedef TypedValue<ValueT> TypedValueT;
			const TypedValueT * v = dynamic_cast<const TypedValueT *>(this);

			if (v == NULL) return false;

			value = v->value();
			return true;
		}

		template <typename ValueT>
		const ValueT & ITypedValue::extract () const
		{
			typedef TypedValue<ValueT> TypedValueT;
			const TypedValueT * v = dynamic_cast<const TypedValueT *>(this);

			if (v != NULL)
			{
				return v->value();
			}
			else 
			{
				throw ConversionError(valueType(), typeid(ValueT));
			}
		}

		template <typename ValueT>
		void ITypedValue::set (const ValueT & value)
		{
			typedef TypedValue<ValueT> TypedValueT;
			TypedValueT * v = dynamic_cast<TypedValueT *>(this);

			if (v != NULL)
			{
				v->value() = value;
			} else
			{
				throw ConversionError(valueType(), typeid(ValueT));
			}
		}

		/** A representation of any particular value.

		 Can be updated or modified with different types.
		 */
		class Value
		{
			REF(ITypedValue) m_ptr;
			
		public:
			/// Construct an undefined value.
			Value ();

			/// Construct an object from a particular value.
			template <typename ValueT>
			explicit Value (const ValueT & value) : m_ptr (new TypedValue<ValueT> (value))
			{
			}
			
			explicit Value (const char * value) : m_ptr (new TypedValue<StringT> (StringT(value)))
			{
			}

			/// Clears any stored value. The value becomes undefined.
			void clear ();

			/// Checks if there is a value defined.
			/// @returns true if there is a value.
			bool defined () const;
			/// Checks if there is a value defined.
			/// @returns false if there is a value.
			bool undefined () const;

			/// Attempts to extract the contained value as the given type.
			/// @returns true if the value was extracted.
			template <typename ValueT>
			bool extract (ValueT & value) const
			{
				if (defined())
				{
					return m_ptr->extract<ValueT>(value);
				}

				return false;
			}

			/// Attempts to extract the contained value as the given type.
			/// @throws ConversionError if the type given is incorrect.
			/// @throws ValueUndefinedError if the value is undefined.
			template <typename ValueT>
			const ValueT & extract () const
			{
				if (undefined())
					throw ValueUndefinedError();

				return m_ptr->extract<ValueT>();
			}

			/// Changes the internal value to something else. Updates the type information.
			template <typename ValueT>
			void set (const ValueT & value)
			{
				m_ptr = new TypedValue<ValueT>(value);
			}

			/// Check whether this value is equal to another value
			bool operator== (const Value & other) const;

			/// Check whether this value is equal to another value
			bool operator== (const ITypedValue & other) const;

			/// Prints out the value.
			friend std::ostream & operator<< (std::ostream &, const Value &);
			friend std::istream & operator>> (std::istream &, Value &);
			
			const ITypedValue * typedValue () const;
			
			static Value readFromBuffer (const Buffer & buf, IndexT & offset);
			void appendToBuffer (ResizableBuffer & buf) const;
		};

		/// Stream helper for printing and converting values to strings
		/// @sa ITypedValue::writeToStream
		std::ostream & operator<< (std::ostream &, const Value &);

		/// Stream helper for reading and converting strings to values
		/// @sa ITypedValue::readFromStream
		std::istream & operator>> (std::istream &, Value &);
		
	}
}

#endif