//
//  Core/Endian.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 29/09/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

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
		inline Endian host_endian ()
		{
			return LITTLE;
		}
#elif defined(__BIG_ENDIAN__) || defined(ppc)
		inline Endian host_endian ()
		{
			return BIG;
		}
#else
		inline bool _is_big_endian ()
		{
			unsigned x = 1;
			return !(*(char * )( & x));
		}

		inline Endian host_endian ()
		{
			if (_is_big_endian())
				return BIG;
			else
				return LITTLE;
		}
#endif

		/// Network order is big endian
		/// Use this if you need to write something that deals specifically with network order.
		inline Endian network_endian ()
		{
			return BIG;
		}

		/// The standard endian of any application linked with this binary.
		/// X86 (Intel/AMD) is little endian. Platforms using this library will generally be little endian (X86). This will need little or no encoding/decoding
		/// for the majority of platforms.
		inline Endian library_endian ()
		{
			return LITTLE;
		}

		/// Decode a value in-place.
		/// @code
		/// uint32_t val = read_int(...);
		/// endian_decode(val, library_endian(), host_endian());
		/// @endcode
		template <typename BaseT>
		inline void endian_decode (BaseT & value, Endian src_type, Endian dst_type)
		{
			if (src_type != dst_type)
			{
				BaseT copy;
				order_copy(value, copy, src_type, dst_type);
				value = copy;
			}
		}

		// This interface can be used when reading data directly from memory.
		inline void order_copy (const unsigned char * src, unsigned char * dst, std::size_t len, Endian src_type, Endian dst_type)
		{
			if (src_type != dst_type)
			{
				dst += len - 1;
				for (std::size_t i = 0; i < len; i++)
					*(dst--) = *(src++);
			} else
			{
				for (std::size_t i = 0; i < len; i++)
					*(dst++) = *(src++);
			}
		}

		template <typename BaseT>
		void order_copy (const BaseT & _src, BaseT & _dst, Endian src_type, Endian dst_type)
		{
			const unsigned char * src = (const unsigned char *) &_src;
			unsigned char * dst = (unsigned char *) &_dst;

			order_copy(src, dst, sizeof(BaseT), src_type, dst_type);
		}

		template <typename BaseT>
		inline BaseT order_read (const BaseT & _src, Endian src_type, Endian dst_type)
		{
			BaseT r;
			order_copy(_src, r, src_type, dst_type);
			return r;
		}

		template <typename BaseT>
		inline void order_write (const BaseT & val, BaseT & dst, Endian src_type, Endian dst_type)
		{
			// for the time being this is the same as OrderRead
			order_copy(val, dst, src_type, dst_type);
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
				return order_read(base, store_t, host_endian());
			}

			/// Implicit cast into the specified type.
			/// Will convert the external value to the specified order
			inline const BaseT & operator= (const BaseT & val)
			{
				order_write(val, base, host_endian(), store_t);
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
