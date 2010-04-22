/*
 *  Core/Endian.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 29/09/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CORE_ENDIAN_H
#define _DREAM_CORE_ENDIAN_H

#include "Core.h"

namespace Dream
{
	namespace Core
	{
		enum Endian
		{
			BIG,
			LITTLE
		};

#if defined(__LITTLE_ENDIAN__) || defined(i386)
		inline Endian hostEndian ()
		{
			return LITTLE;
		}
#elif defined(__BIG_ENDIAN__) || defined(ppc)
		inline Endian hostEndian ()
		{
			return BIG;
		}
#else
		inline bool _isBigEndian ()
		{
			unsigned x = 1;
			return !(*(char * )( & x));
		}

		inline Endian hostEndian ()
		{
			if (_isBigEndian())
				return BIG;
			else
				return LITTLE;
		}
#endif

		/// Network order is big endian
		/// Use this if you need to write something that deals specifically with network order.
		inline Endian networkEndian ()
		{
			return BIG;
		}

		/// The standard endian of any application linked with this binary.
		/// X86 (Intel/AMD) is little endian. Platforms using this library will generally be little endian (X86). This will need little or no encoding/decoding
		/// for the majority of platforms.
		inline Endian libraryEndian ()
		{
			return LITTLE;
		}

		/// Decode a value in-place.
		/// @code
		/// uint32_t val = readInt(...);
		/// endianDecode(val, libraryEndian(), hostEndian());
		/// @endcode
		template <typename BaseT>
		inline void endianDecode (BaseT & value, Endian srcType, Endian dstType)
		{
			if (srcType != dstType)
			{
				BaseT copy;
				orderCopy(value, copy, srcType, dstType);
				value = copy;
			}
		}

		// This interface can be used when reading data directly from memory.
		inline void orderCopy (const unsigned char * src, unsigned char * dst, IndexT len, Endian srcType, Endian dstType)
		{
			if (srcType != dstType)
			{
				dst += len - 1;
				for (int i = 0; i < len; i++)
					*(dst--) = *(src++);
			} else
			{
				for (int i = 0; i < len; i++)
					*(dst++) = *(src++);
			}
		}

		template <typename BaseT>
		void orderCopy (const BaseT & _src, BaseT & _dst, Endian srcType, Endian dstType)
		{
			const unsigned char * src = (const unsigned char *) &_src;
			unsigned char * dst = (unsigned char *) &_dst;

			orderCopy(src, dst, sizeof(BaseT), srcType, dstType);
		}

		template <typename BaseT>
		inline BaseT orderRead (const BaseT & _src, Endian srcType, Endian dstType)
		{
			BaseT r;
			orderCopy(_src, r, srcType, dstType);
			return r;
		}

		template <typename BaseT>
		inline void orderWrite (const BaseT & val, BaseT & dst, Endian srcType, Endian dstType)
		{
			// for the time being this is the same as OrderRead
			orderCopy(val, dst, srcType, dstType);
		}
		/**
		 A simple primative type wrapper that allows simple access to endian-correct value.

		 Writing to the value using operator= will convert the value into the specified endian. Reading is done using the implicit cast operator, and will also
		 cast the variable into the specified endian.

		 @code
		 template <Endian store_t>
		 struct OrderedTestStruct {
			Ordered<uint32_t, store_t> value;
		 };
		 @endcode
		 */
		template <typename _BaseT, Endian store_t = LITTLE>
		class Ordered
		{
		public:
			typedef _BaseT BaseT;
			BaseT base;

			/// Implicit cast into the specified type.
			/// Will convert the internal value to host order.
			inline operator BaseT () const
			{
				return orderRead(base, store_t, hostEndian());
			}

			/// Implicit cast into the specified type.
			/// Will convert the external value to the specified order
			inline const BaseT & operator= (const BaseT & val)
			{
				orderWrite(val, base, hostEndian(), store_t);
				return val;
			}
		};

		template <typename BaseT, Endian store_t, typename OtherT>
		bool operator== (const Ordered<BaseT, store_t> & t, const OtherT & other)
		{
			return BaseT(t) == other;
		}

		template <typename BaseT, Endian store_t, typename OtherT>
		bool operator!= (const Ordered<BaseT, store_t> & t, const OtherT & other)
		{
			return BaseT(t) != other;
		}
	}
}

#endif