//
//  Core/Buffer.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 26/10/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CORE_BUFFER_H
#define _DREAM_CORE_BUFFER_H

#include "URI.h"
#include "Endian.h"

#include <sstream>

namespace Dream
{
	namespace Core
	{
		/**
		 A list of useful mime-type constants.

		 We are only interested in a subset of possible types. Therefore, we only check for these particular types.
		 */
		enum Mimetype {
			UNKNOWN = 0,
			IMAGE_JPEG = 10,
			IMAGE_PNG = 11,
			IMAGE_DDS = 12,
			AUDIO_XWAV = 40,
			AUDIO_BASIC = 41,
			APPLICATION_OGG = 80
		};

		/**
		 Abstract buffer providing read-only access to data.
		 */
		class Buffer {
		public:
			virtual ~Buffer ();

			/// Access data at a particular location. Range checked.
			const ByteT * at (IndexT loc) const;

			/// Read a variable out of the buffer
			void read (IndexT offset, IndexT size, ByteT * value) const;

			/// Helper to read values of specific type
			template <typename t>
			IndexT read (IndexT offset, t & value) const
			{
				read(offset, sizeof(t), (ByteT *)&value);

				return sizeof(t);
			}

			template <typename t>
			IndexT read (IndexT offset, t & value, Endian src_type, Endian dst_type) const
			{
				IndexT cnt = read(offset, value);
				endian_decode(value, src_type, dst_type);
				return cnt;
			}

			template <typename t>
			IndexT read (IndexT offset, t & value, Endian src_type) const
			{
				return read(offset, value, src_type, host_endian());
			}

			/// Access data at a particular location. Not range checked.
			const ByteT & operator[] (IndexT idx) const;

			/// Returns true if size() == 0, otherwise false.
			bool empty () const;

			/// Returns the length of data stored in the buffer.
			virtual IndexT size () const abstract;

			/// Returns the address of the first byte in the buffer.
			virtual const ByteT * begin () const abstract;

			/// Returns the address of the last byte + 1 in the buffer.
			const ByteT * end () const;

			/// Tests whether the data in the buffers is equivalent.
			bool operator== (const Buffer & other) const;

			/// Tests whether the data in the buffers is different.
			bool operator!= (const Buffer & other) const;

			/// Check the mime-type of the data contained within the buffer.
			Mimetype mimetype ();

			/// Dump the buffer as hex to the given stream.
			void hexdump (std::ostream &);

			/// Basic data checksum
			uint32_t checksum () const;

			/// Write the contents of the buffer to the specified file path.
			void write_to_file (const Path &);
		};

		/**
		 Abstract buffer providing read/write access to data.
		 */
		class MutableBuffer : public Buffer {
		public:
			virtual ~MutableBuffer ();

			/// Provide the const version
			using Buffer::begin;

			/// Returns the address of the first byte in the buffer.
			virtual ByteT * begin () abstract;
			/// Returns the address of the last byte + 1 in the buffer.
			ByteT * end ();

			/// Access data at a particular location. Range checked.
			ByteT * at (IndexT loc);
			/// Access data at a particular location. Not range checked.
			ByteT & operator[] (IndexT idx);

			/// Copy count copies of value into the buffer at the specified offset.
			void assign (IndexT count, const ByteT & value, IndexT offset = 0);
			/// Copy data from another range of bytes at the specified offset.
			void assign (const ByteT * other_begin, const ByteT * other_end, IndexT offset = 0);
			/// Assign data from another buffer
			void assign (const Buffer & other, IndexT offset = 0);
			/// Copy a slice of data from another buffer
			void assign (const Buffer & other, IndexT other_offset, IndexT other_size, IndexT offset = 0);
			/// Copy a c-style string into the buffer
			void assign (const char * string, IndexT offset = 0);

			/// Write a specific value into the buffer at the specified offset.
			template <typename AnyT>
			IndexT write (const AnyT & value, IndexT offset)
			{
				assign((const ByteT *)value, (const ByteT *)value + sizeof(AnyT), offset);

				return offset + sizeof(AnyT);
			}
		};

		/**
		 A buffer that can be resized. A resizable buffer may have a capacity >= size.
		 */
		class ResizableBuffer : public MutableBuffer {
		public:
			virtual ~ResizableBuffer ();

			/// The currently allocated capacity of the buffer. Can be changed by calling reserve().
			virtual IndexT capacity () const abstract;

			/// Reserve/allocate more capacity if required. Will release capacity if size is smaller than current capacity.
			virtual void reserve (IndexT size) abstract;

			/// Change the size of the buffer. Will allocate more capacity if required.
			virtual void resize (IndexT size) abstract;

			/// Increase the size of the buffer by the given size.
			void expand (IndexT amount);

			/// Appends a set number of bytes to the end of the buffer
			void append (IndexT size, const ByteT * data);

			// Helper for appending primitive types.
			template <typename t>
			void append (const t & value)
			{
				append(sizeof(t), (const ByteT *)&value);
			}

			/// Append data from an incremental iterator
			template <typename AnyT>
			void append (AnyT begin, AnyT end)
			{
				while (begin != end) {
					append(*begin);
					++begin;
				}
			}
		};

		/**
		 A read-only buffer that provides access to const data, such as a c-style string.

		 The buffer does not copy the data that is passed to it. Therefore, it would be unwise to pass in a dynamic pointer or other data that may be
		 deallocated before the buffer is deallocated.

		 A useful case might be if you have some data, and want to manipulate it or pass it to another function as a buffer:

		 @code
		 ByteT * data = read_data();

		 // Data is not copied, therefore performance is not lost:
		 StaticBuffer buf(data);
		 process_buffer(buf);
		 @endcode

		 In this case, we could wrap the data up in a buffer, but we didn't have to copy the data needlessly. Using a different kind of buffer, such as
		 DynamicBuffer, would copy the data.
		 */
		class StaticBuffer : public Buffer {
			IndexT _size;
			const ByteT * _buf;

		public:
			/// Allocate the data with a c-style string. Uses strlen to determine the length of the buffer. Includes the
			/// null character by default.
			static StaticBuffer for_cstring (const char * str, bool include_null_byte = true);

			/// Allocate the data with a sequence of bytes, buf, of specified size.
			StaticBuffer (const ByteT * buf, const IndexT & size);

			// Standard copy constructer is fine.

			virtual ~StaticBuffer ();

			virtual IndexT size () const;
			virtual const ByteT * begin () const;
		};

		/**
		 A read-only buffer that provides fast access to files on the file-system.

		 The buffer uses mmap (or equivalent) internally to load data from the disk. This buffer is designed to be as fast as possible to load data from the
		 disk, and will reduce the number of copies required.
		 */
		class FileBuffer : public Buffer, private NonCopyable {
		protected:
			IndexT _size;
			void * _buf;

		public:
			/// Maps the data from the file specified by file_path into memory.
			FileBuffer (const Path & file_path);

			virtual ~FileBuffer ();

			virtual IndexT size () const;
			virtual const ByteT * begin () const;
		};

		/**
		 A flexible read/write buffer class that can be resized.

		 This buffer provides maximum flexibility when dealing with data which may change its size, and is almost API compatible with <tt>std::vector<unsigned char></tt>, but has optimizations for data buffering. This can provide up to 30% increase in performance when dealing with a lot of data.
		 */
		class DynamicBuffer : public ResizableBuffer, private NonCopyable {
			IndexT _capacity, _size;
			ByteT * _buf;

			void allocate (IndexT size);
			void deallocate ();
		public:
			/// Construct an empty buffer.
			DynamicBuffer ();

			/// Construct a pre-sized buffer.
			/// If reserved is true, the size refers to capacity i.e. equivalent of calling reserve(size).
			DynamicBuffer (IndexT size, bool reserved = false);

			virtual ~DynamicBuffer ();

			virtual IndexT capacity () const;
			/// The current size of the data stored in the buffer. This will always be <= capacity().
			virtual IndexT size () const;

			/// Set the size of the buffer to 0. Capacity is not changed.
			void clear ();

			virtual void reserve (IndexT size);
			virtual void resize (IndexT size);

			virtual ByteT * begin ();
			virtual const ByteT * begin () const;
		};

		/**
		 A fast read/write buffer that can't be resized.

		 This class is similar to DynamicBuffer in the sense that it is mutable, however it is slightly more efficient for buffers that won't change size -
		 because a packed buffer will allocate its class data in line with the buffer - so the most expensive part (malloc/new) only happens at most once
		 instead of twice or more.

		 This buffer is similar to the traditional c-style buffer:

		 @code
		 struct CStyleBuffer {
		 unsigned size;
		 unsigned char data[0];
		 };

		 CStyleBuffer * buf = (CStyleBuffer)malloc(sizeof(CStyleBuffer) + 100);
		 buf.size = 100;

		 buf.data[50] = 'a';
		 @endcode

		 This class will copy its data. Therefore, you may want to consider StaticBuffer if you don't want to copy the data.
		 */
		class PackedBuffer : public MutableBuffer, private NonCopyable {
			IndexT _size;

			PackedBuffer (IndexT size);

			ByteT * data ();
			const ByteT * data () const;

		public:
			virtual ~PackedBuffer ();

			/// Create a new buffer.
			static PackedBuffer * new_buffer (IndexT size);

			virtual IndexT size () const;

			virtual ByteT * begin ();
			virtual const ByteT * begin () const;
		};

		/// This class is about 30% faster than using std::vector<ByteT>
		typedef DynamicBuffer BufferT;

		/** Provides an istream interface to reading data from a buffer.

		 This can be used to provide a buffer to a stream processing function.
		 */
		class BufferStream : public std::streambuf, public std::istream {
		public:
			BufferStream (const Buffer & buf);
		};
	}
}

#endif
