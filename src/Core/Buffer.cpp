//
//  Core/Buffer.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 26/10/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#include "Buffer.h"

// File and memory manipulation
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <unistd.h>

// For testing
#include <string>
#include <iomanip>
#include <algorithm>

namespace Dream
{
	namespace Core
	{
// MARK: mark -
// MARK: mark class Buffer

		Buffer::~Buffer ()
		{
		}

		bool Buffer::empty () const
		{
			return size() == 0;
		}

		const ByteT * Buffer::at (IndexT loc) const
		{
			DREAM_ASSERT(loc <= size());
			return begin() + loc;
		}
		
		void Buffer::read (IndexT offset, IndexT size, ByteT * value) const
		{
			memcpy(value, at(offset), size);
		}

		const ByteT * Buffer::end () const
		{
			return begin() + size();
		}

		const ByteT & Buffer::operator[] (IndexT idx) const
		{
			return begin()[idx];
		}

		bool Buffer::operator== (const Buffer & other) const
		{
			if (size() != other.size())
				return false;

			// If size is the same, check data is the same
			return bcmp(begin(), other.begin(), size()) == 0;
		}

		bool Buffer::operator!= (const Buffer & other) const
		{
			return !((*this) == other);
		}

		/* From MacOS X mime.magic:
		 # JPEG images
		 0	beshort		0xffd8		image/jpeg

		 #PNG Image Format
		 0	string		\x89PNG			image/png

		 # PC bitmaps (OS/2, Windoze BMP files)  (Greg Roelofs, newt@uchicago.edu)
		 0	string		BM		image/bmp

		 # TIFF and friends
		 #					TIFF file, big-endian
		 0	string		MM		image/tiff
		 #					TIFF file, little-endian
		 0	string		II		image/tiff
		 */

		Mimetype Buffer::mimetype ()
		{
			if (size() < 4) return UNKNOWN;

			const ByteT * buffer = begin();
			DREAM_ASSERT(buffer != NULL);

			if (buffer[0] == 0xFF && buffer[1] == 0xD8)
				return IMAGE_JPEG;

			if (strncmp("PNG", (const char *)&buffer[1], 3) == 0)
				return IMAGE_PNG;

			if (strncmp("DDS ", (const char *)&buffer[0], 4) == 0)
				return IMAGE_DDS;
			
			if (strncmp("RIFF", (const char *)&buffer[0], 4) == 0)
				return AUDIO_XWAV;
			
			if (strncmp(".snd", (const char *)&buffer[0], 4) == 0)
				return AUDIO_BASIC;
			
			if (strncmp("OggS", (const char *)&buffer[0], 4) == 0)
				return APPLICATION_OGG;

			return UNKNOWN;
		}
		
		
		// >04000000 4B657931 84050000 00040000< ....Key1........ 00000000
		// >004B6579 32200600 00004170 706C6573< .Key2 ....Apples 00000010
		void Buffer::hexdump (std::ostream & out)
		{
			// http://stahlforce.com/dev/index.php?tool=csc01
			const ByteT * current = begin();
			std::size_t remaining = size();
			
			while (true)
			{
				StringStreamT buffer;
				
				buffer << "0x";
				
				buffer.fill('0');
				buffer.width(sizeof(long) * 2);
				buffer.setf(std::ios::hex, std::ios::basefield);
				
				buffer << (current - begin()) << " >";
				
				std::size_t count = std::min(remaining, (std::size_t)4*4);
								
				for (std::size_t i = 0; i < (4*4); i += 1) {
					if (i > 0 && i % 4 == 0)
						buffer << ' ';
					
					if (i < count) {
						buffer.width(2);
						buffer << (int)(*(current + i));
					} else
						buffer << "  ";
				}
				
				buffer << "< ";
				
				out << buffer.str();
				
				for (std::size_t i = 0; i < count; i += 1) {
					ByteT character = *(current + i);
					if (character >= 32 && character <= 128)
						out << character;
					else
						out << ".";
				}
				
				out << std::endl;
				
				remaining -= count;
				if (remaining == 0)
					break;
				
				current += count;
			}	
		}
		
		// http://www.flounder.com/checksum.htm
		uint32_t Buffer::checksum () const
		{
			uint32_t sum = 0;
			uint32_t r = 55665;
			const uint32_t C1 = 52845;
			const uint32_t C2 = 22719;
			
			IndexT s = size();
			const ByteT * b = begin();
			
			for (unsigned i = 0; i < s; i += 1) {
				ByteT cipher = (b[i] ^ (r >> 8));
				r = (cipher + r) * C1 + C2;
				sum += cipher;
			}
			
			return sum;
		}
		
		void Buffer::write_to_file (const Path & p)
		{
			FileDescriptorT fd;
			int result;
						
			// Open and create the output file
			mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
			
			fd = open(p.to_local_path().c_str(), O_RDWR | O_CREAT | O_TRUNC, mode);
			DREAM_ASSERT(fd >= 0);
			
			// Seek to the end
			result = lseek(fd, size() - 1, SEEK_SET);
			DREAM_ASSERT(result != -1);
			
			// Write a byte to give the file appropriate size
			result = write(fd, "", 1);
			DREAM_ASSERT(result != -1);
			
			// mmap the file
			ByteT * dst = (ByteT *)mmap(0, size(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
			DREAM_ASSERT(dst != (ByteT *)-1);
						
			madvise(dst, size(), MADV_SEQUENTIAL);
			
			// Copy the data
			memcpy(dst, begin(), size());
			
			// Clean up
			munmap(dst, size());
			close(fd);
		}

// MARK: mark -
// MARK: mark class MutableBuffer

		MutableBuffer::~MutableBuffer ()
		{
		}

		ByteT * MutableBuffer::at (IndexT loc)
		{
			DREAM_ASSERT(loc < size());
			return begin() + loc;
		}

		ByteT * MutableBuffer::end ()
		{
			return begin() + size();
		}

		ByteT & MutableBuffer::operator[] (IndexT idx)
		{
			return begin()[idx];
		}

		void MutableBuffer::assign (IndexT count, const ByteT & value, IndexT offset)
		{
			DREAM_ASSERT((count + offset) <= size());

			memset(begin() + offset, value, count);
		}

		void MutableBuffer::assign (const ByteT * other_begin, const ByteT * other_end, IndexT offset)
		{
			DREAM_ASSERT((other_end - other_begin) + offset <= size());

			memcpy(begin() + offset, other_begin, other_end - other_begin);
		}

		void MutableBuffer::assign (const Buffer & other, IndexT offset)
		{
			assign(other.begin(), other.end(), offset);
		}

		void MutableBuffer::assign (const Buffer & other, IndexT other_offset, IndexT other_size, IndexT offset)
		{
			assign(other.begin() + other_offset, other.begin() + other_offset + other_size, offset);
		}

		void MutableBuffer::assign (const char * string, IndexT offset)
		{
			IndexT len = strlen(string);
			assign((const ByteT *)string, (const ByteT *)string + len, offset);
		}
		
// MARK: mark -
// MARK: mark class Resizable
		
		ResizableBuffer::~ResizableBuffer ()
		{
			
		}
		
		void ResizableBuffer::expand (IndexT amount)
		{
			resize(size() + amount);
		}
		
		void ResizableBuffer::append (IndexT size, const ByteT * data)
		{
			expand(size);
			
			memcpy(this->end() - size, data, size);
		}

// MARK: mark -
// MARK: mark class StaticBuffer

		StaticBuffer StaticBuffer::for_cstring (const char * str, bool include_null_byte)
		{
			return StaticBuffer((const ByteT*)str, strlen(str) + (int)include_null_byte);
		}
		
		StaticBuffer::StaticBuffer (const ByteT * buf, const IndexT & size) : _size(size), _buf(buf)
		{
		}

		StaticBuffer::~StaticBuffer ()
		{
		}

		IndexT StaticBuffer::size () const
		{
			return _size;
		}

		const ByteT * StaticBuffer::begin () const
		{
			return _buf;
		}

// MARK: mark -
// MARK: mark class FileBuffer
		
		FileBuffer::FileBuffer (const Path & file_path)
		{
			FileDescriptorT input = open(file_path.to_local_path().c_str(), O_RDONLY);

			if (input == -1)
				perror(__PRETTY_FUNCTION__);
			
			DREAM_ASSERT(input != -1);
			
			_size = lseek(input, 0, SEEK_END);
			
			_buf = mmap(0, _size, PROT_READ, MAP_SHARED, input, 0);
			DREAM_ASSERT(_buf != (ByteT *)-1);
		}
		
		FileBuffer::~FileBuffer ()
		{
			munmap(_buf, _size);
		}
		
		IndexT FileBuffer::size () const
		{
			return _size;
		}
		
		const ByteT * FileBuffer::begin () const
		{
			return (const ByteT *)_buf;
		}
		
// MARK: mark -
// MARK: mark class DynamicBuffer

		void DynamicBuffer::allocate (IndexT size)
		{
			if (size != _capacity)
			{
				_buf = (ByteT*)realloc(_buf, size);
				DREAM_ASSERT(_buf != NULL);

				_capacity = size;
			}
		}

		void DynamicBuffer::deallocate ()
		{
			if (_buf)
			{
				free(_buf);
				_buf = NULL;
				_size = 0;
				_capacity = 0;
			}
		}

		DynamicBuffer::DynamicBuffer () : _capacity (0), _size (0), _buf (NULL)
		{
		}

		DynamicBuffer::DynamicBuffer (IndexT size, bool reserved) : _buf (NULL)
		{
			allocate(size);
			
			if (reserved == false)
				_size = size;
			else
				_size = 0;
		}

		DynamicBuffer::~DynamicBuffer ()
		{
			deallocate();
		}

		IndexT DynamicBuffer::capacity () const
		{
			return _capacity;
		}

		IndexT DynamicBuffer::size () const
		{
			return _size;
		}

		void DynamicBuffer::clear ()
		{
			deallocate();
		}

		void DynamicBuffer::reserve (IndexT size)
		{
			allocate(size);
		}

		void DynamicBuffer::resize (IndexT size)
		{
			if (size > _capacity)
			{
				allocate(size);
			}

			_size = size;
		}

		ByteT * DynamicBuffer::begin ()
		{
			return _buf;
		}

		const ByteT * DynamicBuffer::begin () const
		{
			return _buf;
		}

// MARK: mark -
// MARK: mark class PackedData

		PackedBuffer::PackedBuffer (IndexT size) : _size (size)
		{
		}

		PackedBuffer::~PackedBuffer ()
		{
		}

		// Data is packed at the end of the class.
		ByteT * PackedBuffer::data ()
		{
			return (ByteT*)this + sizeof(*this);
		}

		const ByteT * PackedBuffer::data () const
		{
			return (const ByteT*)this + sizeof(*this);
		}

		PackedBuffer * PackedBuffer::new_buffer (IndexT size)
		{
			IndexT total = sizeof(PackedBuffer) + size;
			void * data = malloc(total);
			PackedBuffer * buffer = new(data) PackedBuffer(size);

			return buffer;
		}

		IndexT PackedBuffer::size () const
		{
			return _size;
		}

		ByteT * PackedBuffer::begin ()
		{
			return data();
		}

		const ByteT * PackedBuffer::begin () const
		{
			return data();
		}

// MARK: mark -
// MARK: mark class BufferStream

		BufferStream::BufferStream (const Buffer & buf) : std::streambuf (), std::istream (this)
		{
			std::streambuf::setg ((char *) buf.begin (), (char *) buf.begin (), (char *) buf.end ());
		}

// MARK: mark -
// MARK: mark Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(BufferRead)
		{
			StaticBuffer buf = StaticBuffer::for_cstring("Bobby");
			
			uint8_t v;
			buf.read(2, v);
			
			check(v == 'b') << "Read value is equal";
		}
		
		UNIT_TEST(DynamicBuffer)
		{
			const char * data = "Human resources are human first, and resources second.";
			unsigned data_length = strlen(data);

			testing("Construction");

			DynamicBuffer a(100), b;

			check(a.size() == 100) << "Size set by constructor";
			check(b.size() == 0) << "Size set by constructor";

			check(!a.empty()) << "Sized construtor is not empty";
			check(b.empty()) << "Default construtor is empty";

			check(a.capacity() >= a.size()) << "Capacity >= size";
			check(b.capacity() >= b.size()) << "Capacity >= size";

			testing("Assigning Data");

			IndexT prev_capacity = a.capacity();
			a.resize(data_length);
			a.assign((const ByteT*)data, (const ByteT*)data + data_length);

			// Performance check
			check(a.capacity() == prev_capacity) << "Don't realloc if size is within capacity";

			b.resize(data_length);
			b.assign((const ByteT*)data, (const ByteT*)data + data_length);

			check(a == b) << "Data and size is the same";
			check(a[5] == data[5]) << "Data indexing is correct";

			testing("Clearing buffers");

			a.clear();
			check(a.empty()) << "Cleared buffer is empty";

			check(a != b) << "Buffers are different after being cleared";

			b.clear();
			check(a == b) << "Cleared buffers are equivalent";

			testing("Capacity");

			a.reserve(200);
			check(a.capacity() == 200) << "Reserved capacity for 200 bytes";

			b.reserve(400);
			check(b.capacity() == 400) << "Reserved capacity for 400 bytes";

			a.resize(600);
			check(a.size() == 600) << "Size increase was successful";
			check(a.capacity() >= 600) << "Capacity increased after size increase";
			
			a.expand(100);
			
			check(a.size() == 700) << "Size expansion was successful";
			
			testing("Appending");
			
			a.resize(0);
			a.append(5, (const ByteT *)"abcde");
			a.append(5, (const ByteT *)"abcde");
			
			check(a.size() == 10) << "Size is correct after appending 10 characters";
			check(a[1] == 'b') << "Character is correct";
		}

		UNIT_TEST(PackedBuffer)
		{
			const char * data = "Packed Buffer.";
			unsigned data_length = strlen(data);

			PackedBuffer * buffer;

			testing("Construction");

			buffer = PackedBuffer::new_buffer(data_length);
			check(buffer != NULL) << "Buffer was created successfully";

			testing("Assigning Data");

			buffer->assign((const ByteT*)data, (const ByteT*)data + data_length, 0);
			check(buffer->size() == data_length) << "Data size is consistent";

			for (unsigned i = 0; i < buffer->size(); i += 1)
				check((*buffer)[i] == data[i]) << "Data is correct";
		}
		
		UNIT_TEST(ReadingAndWritingBuffers)
		{
			Path tmp_path = Path::temporary_file_path();
			const char * data = "When the only tool you have is a hammer, you tend to treat everything as if it were a nail.";
			unsigned data_length = strlen(data);
			
			testing("Writing");
			
			PackedBuffer * write_buffer;
			write_buffer = PackedBuffer::new_buffer(data_length);
			
			write_buffer->assign((const ByteT*)data, (const ByteT*)data + data_length, 0);
			
			write_buffer->write_to_file(tmp_path);
			
			testing("Reading");
			FileBuffer read_buffer(tmp_path);
			
			std::string write_string(write_buffer->begin(), write_buffer->end());
			std::string read_string(read_buffer.begin(), read_buffer.end());
			
			check(write_buffer->size() == read_buffer.size()) << "Data size is consistent";
			check(write_buffer->checksum() == read_buffer.checksum()) << "Data is correct";
			
			check(write_string == read_string) << "Data string is equal";
			
			write_buffer->hexdump(std::cout);
			read_buffer.hexdump(std::cout);
			
			// Remove the temporary file
			tmp_path.remove();
		}
#endif
	}
}
