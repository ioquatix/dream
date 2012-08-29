//
//  Core/Serialization.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 23/12/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CORE_SERIALIZATION_H
#define _DREAM_CORE_SERIALIZATION_H

#include "Endian.h"
#include "Buffer.h"
#include "Strings.h"

namespace Dream
{
	namespace Core
	{
		typedef uint8_t TypeIdentifierT;

		/// Primative type identifiers
		enum TypeIdentifier {
			TI_UNDEFINED = 0,

			TI_UINT8 = 0x1,
			TI_INT8 = 0x1 | 0x80,

			TI_UINT16 = 0x2,
			TI_INT16 = 0x2 | 0x80,

			TI_UINT32 = 0x4,
			TI_INT32 = 0x4 | 0x80,
			TI_FLOAT32 = 0x4 | 0x40,

			TI_UINT64 = 0x8,
			TI_INT64 = 0x8 | 0x80,
			TI_FLOAT64 = 0x8 | 0x40,

			TI_STRING = 0x20
		};

		template <unsigned>
		class TypeSerialization;

		template <typename t>
		class TypeIdentifierTypeTraits {
		public:
			enum { TypeIdentifierValue = TI_UNDEFINED };
			typedef t TypeT;
		};

		template <unsigned i>
		class TypeIdentifierValueTraits {
		public:
			typedef void TypeT;
		};

		template <unsigned Index>
		class TypeSerialization {
		public:
			typedef typename TypeIdentifierValueTraits<Index>::TypeT TypeT;

			static TypeT read_from_buffer (const Buffer & buf, IndexT & offset)
			{
				TypeT value;
				offset += buf.read(offset, value);
				return order_read(value, library_endian(), host_endian());
			}

			static void append_to_buffer (ResizableBuffer & buf, const TypeT & value)
			{
				TypeT converted_value = order_read(value, host_endian(), library_endian());
				buf.append(sizeof(TypeT), (const ByteT *)&converted_value);
			}

			//static void write_to_buffer (const ResizableBuffer & buf, IndexT & offset, const TypeT & value)
			//{
			//	TypeT value = order_read(value, host_endian(), library_endian());
			//	buf.write(offset, value);
			//}
		};

		template <>
		class TypeSerialization<TI_UNDEFINED>{
		public:
		};

		template <>
		class TypeSerialization<TI_STRING>{
		public:
			typedef StringT TypeT;

			static TypeT read_from_buffer (const Buffer & buf, IndexT & offset)
			{
				uint32_t length;

				offset += buf.read(offset, length);

				StringT value(buf.at(offset), buf.at(offset + length));

				offset += length;

				return value;
			}

			static void append_to_buffer (ResizableBuffer & buf, const TypeT & value)
			{
				buf.append((uint32_t)value.length());
				buf.append(value.length(), (const ByteT *)value.data());
			}
		};

#define TypeIdentifierSpecialization(name, index) \
	template <> \
	class TypeIdentifierTypeTraits<name> \
	{ \
	public: \
		enum { TypeIdentifierValue = index }; \
		typedef name TypeT; \
	}; \
        \
	template <> \
	class TypeIdentifierValueTraits<index> \
	{ \
	public: \
		typedef name TypeT; \
        \
	};

		TypeIdentifierSpecialization(uint8_t, TI_UINT8)
		TypeIdentifierSpecialization(uint16_t, TI_UINT16)
		TypeIdentifierSpecialization(uint32_t, TI_UINT32)
		TypeIdentifierSpecialization(uint64_t, TI_UINT64)
		TypeIdentifierSpecialization(int8_t, TI_INT8)
		TypeIdentifierSpecialization(int16_t, TI_INT16)
		TypeIdentifierSpecialization(int32_t, TI_INT32)
		TypeIdentifierSpecialization(int64_t, TI_INT64)
		TypeIdentifierSpecialization(float, TI_FLOAT32)
		TypeIdentifierSpecialization(double, TI_FLOAT64)
		TypeIdentifierSpecialization(StringT, TI_STRING)
	}
}

#endif
