//
//  Core/Value.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/05/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

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
			StringT _what;
		public:
			ConversionError (const std::type_info & convert_from, const std::type_info & convert_to) throw ();
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
			
			virtual const std::type_info & value_type () const abstract;
			
			/// Compares two values
			/// @returns True if the TypedValue objects are of the same type and contained value.			
			virtual bool equal (const ITypedValue *) const abstract;

			virtual void write_to_stream (std::ostream &) const abstract;
			virtual void read_from_stream (std::istream &) abstract;

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
			virtual TypeIdentifierT type_index () const abstract;
			
			/// Returns a pointer to the value storage.
			virtual const ByteT * value_data () const abstract;
			
			/// Append the value to a buffer.
			virtual void append_to_buffer (ResizableBuffer & buf) const abstract;
		};

		/// Stream helper for printing and converting types to strings
		/// @sa ITypedValue::write_to_stream
		std::ostream & operator<< (std::ostream &, const ITypedValue &);
		std::istream & operator>> (std::istream &, const ITypedValue &);

		template <unsigned TypeIdentifier>
		struct TypedValueSerializer {
			template <typename ValueT>
			void read_from_stream (std::istream & stream, ValueT & value)
			{
				stream >> value;
			}
		
			template <typename ValueT>
			void append_to_buffer (ResizableBuffer & buf, ValueT & value) const
			{
				buf.append((TypeIdentifierT)TypeIdentifier);
				TypeSerialization<TypeIdentifier>::append_to_buffer(buf, value);
			}
		};
		
		template <>
		struct TypedValueSerializer<0> {
			template <typename ValueT>
			void read_from_stream (std::istream & stream, ValueT & value)
			{
				throw SerializationError();
			}

			template <typename ValueT>
			void append_to_buffer (ResizableBuffer & buf, ValueT & value) const
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
			ValueT _value;
			typedef TypedValueSerializer<TypeIdentifierTypeTraits<ValueT>::TypeIdentifierValue> TypedValueSerializerT;

		public:
			TypedValue ()
			{
			}

			/// Construct with supplied value
			TypedValue (const ValueT & v)
			{
				_value = v;
			}

			virtual ~TypedValue ()
			{
			}
			
			virtual TypeIdentifierT type_index () const
			{
				return TypeIdentifierTypeTraits<ValueT>::TypeIdentifierValue;
			}
			
			virtual const std::type_info & value_type () const
			{
				return typeid(ValueT);
			}

			virtual IndexT size () const
			{
				return sizeof(ValueT);
			}
			
			const ByteT * value_data () const
			{
				return (const ByteT *)&_value;
			}

			/// Retrieve non-const value reference.
			ValueT & value ()
			{
				return _value;
			}

			/// Retrieve const value reference.
			const ValueT & value () const
			{
				return _value;
			}

			virtual bool equal (const ITypedValue * other) const
			{
				typedef TypedValue<ValueT> TypedValueT;
				const TypedValueT * other_typed_value = dynamic_cast<const TypedValueT *>(other);

				if (other_typed_value != NULL)
					return this->value() == other_typed_value->value();
				else
					return false;
			}

			virtual void write_to_stream (std::ostream & stream) const
			{
				stream << _value;
			}
			
			virtual void read_from_stream (std::istream & stream)
			{
				TypedValueSerializerT::read_from_stream(stream, _value);
			}
			
			virtual void append_to_buffer (ResizableBuffer & buf) const
			{
				TypedValueSerializerT::append_to_buffer(buf, _value);
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
				throw ConversionError(value_type(), typeid(ValueT));
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
				throw ConversionError(value_type(), typeid(ValueT));
			}
		}

		/** A representation of any particular value.

		 Can be updated or modified with different types.
		 */
		class Value
		{
			REF(ITypedValue) _ptr;
			
		public:
			/// Construct an undefined value.
			Value ();

			/// Construct an object from a particular value.
			template <typename ValueT>
			explicit Value (const ValueT & value) : _ptr (new TypedValue<ValueT> (value))
			{
			}
			
			explicit Value (const char * value) : _ptr (new TypedValue<StringT> (StringT(value)))
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
					return _ptr->extract<ValueT>(value);
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

				return _ptr->extract<ValueT>();
			}

			/// Changes the internal value to something else. Updates the type information.
			template <typename ValueT>
			void set (const ValueT & value)
			{
				_ptr = new TypedValue<ValueT>(value);
			}

			/// Check whether this value is equal to another value
			bool operator== (const Value & other) const;

			/// Check whether this value is equal to another value
			bool operator== (const ITypedValue & other) const;

			/// Prints out the value.
			friend std::ostream & operator<< (std::ostream &, const Value &);
			friend std::istream & operator>> (std::istream &, Value &);
			
			const ITypedValue * typed_value () const;
			
			static Value read_from_buffer (const Buffer & buf, IndexT & offset);
			void append_to_buffer (ResizableBuffer & buf) const;
		};

		/// Stream helper for printing and converting values to strings
		/// @sa ITypedValue::write_to_stream
		std::ostream & operator<< (std::ostream &, const Value &);

		/// Stream helper for reading and converting strings to values
		/// @sa ITypedValue::read_from_stream
		std::istream & operator>> (std::istream &, Value &);
		
	}
}

#endif
