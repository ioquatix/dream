/*
 *  Core/Buffer.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 26/10/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#include "Buffer.h"

#include <sys/mman.h>
#include <sys/fcntl.h>
#include <boost/filesystem.hpp>

// For testing
#include <string>

namespace Dream
{
	namespace Core
	{
#pragma mark -
#pragma mark class Buffer

		Buffer::~Buffer ()
		{
		}

		bool Buffer::empty () const
		{
			return size() == 0;
		}

		const ByteT * Buffer::at (IndexT loc) const
		{
			ensure(loc <= size());
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
			ensure(buffer != NULL);

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
		
		void Buffer::hexdump (std::ostream & out)
		{
			// http://stahlforce.com/dev/index.php?tool=csc01
			const void * pAddressIn = (void*)begin();
			long lSize = size();
			
			char szBuf[256];
			long lIndent = 1;
			long lOutLen, lIndex, lIndex2, lOutLen2;
			long lRelPos;
			struct { char *pData; unsigned long lSize; } buf;
			unsigned char *pTmp,ucTmp;
			unsigned char *pAddress = (unsigned char *)pAddressIn;
			
			buf.pData   = (char *)pAddress;
			buf.lSize   = lSize;
			
			while (buf.lSize > 0)
			{
				pTmp     = (unsigned char *)buf.pData;
				lOutLen  = (int)buf.lSize;
				if (lOutLen > 16)
					lOutLen = 16;
				
				// create a 64-character formatted output line:
				sprintf(szBuf, " >                            "
						"                      "
						"    %08lX", pTmp-pAddress);
				lOutLen2 = lOutLen;
				
				for(lIndex = 1+lIndent, lIndex2 = 53-15+lIndent, lRelPos = 0;
					lOutLen2;
					lOutLen2--, lIndex += 2, lIndex2++
					)
				{
					ucTmp = *pTmp++;
					
					sprintf(szBuf + lIndex, "%02X ", (unsigned short)ucTmp);
					if(!isprint(ucTmp))  ucTmp = '.'; // nonprintable char
					szBuf[lIndex2] = ucTmp;
					
					if (!(++lRelPos & 3))     // extra blank after 4 bytes
					{  lIndex++; szBuf[lIndex+2] = ' '; }
				}
				
				if (!(lRelPos & 3)) lIndex--;
				
				szBuf[lIndex  ]   = '<';
				szBuf[lIndex+1]   = ' ';
				
				//printf("%s\n", szBuf);
				out << szBuf << std::endl;
				
				buf.pData   += lOutLen;
				buf.lSize   -= lOutLen;
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
		
		void Buffer::writeToFile (const Path & p)
		{
			FileDescriptorT fd;
			int result;
			
			std::cout << __func__ << " : " << p.file_string().c_str() << std::endl;
			
			// Open and create the output file
			mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
			
			fd = open(p.file_string().c_str(), O_RDWR | O_CREAT | O_TRUNC, mode);
			ensure(fd >= 0);
			
			// Seek to the end
			result = lseek(fd, size() - 1, SEEK_SET);
			ensure(result != -1);
			
			// Write a byte to give the file appropriate size
			result = write(fd, "", 1);
			ensure(result != -1);
			
			// mmap the file
			ByteT * dst = (ByteT *)mmap(0, size(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
			ensure(dst != (ByteT *)-1);
						
			madvise(dst, size(), MADV_SEQUENTIAL);
			
			// Copy the data
			memcpy(dst, begin(), size());
			
			// Clean up
			munmap(dst, size());
			close(fd);
		}

#pragma mark -
#pragma mark class MutableBuffer

		MutableBuffer::~MutableBuffer ()
		{
		}

		ByteT * MutableBuffer::at (IndexT loc)
		{
			ensure(loc < size());
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
			ensure((count + offset) <= size());

			memset(begin() + offset, value, count);
		}

		void MutableBuffer::assign (const ByteT * otherBegin, const ByteT * otherEnd, IndexT offset)
		{
			ensure((otherEnd - otherBegin) + offset <= size());

			memcpy(begin() + offset, otherBegin, otherEnd - otherBegin);
		}

		void MutableBuffer::assign (const Buffer & other, IndexT offset)
		{
			assign(other.begin(), other.end(), offset);
		}

		void MutableBuffer::assign (const Buffer & other, IndexT otherOffset, IndexT otherSize, IndexT offset)
		{
			assign(other.begin() + otherOffset, other.begin() + otherOffset + otherSize, offset);
		}

		void MutableBuffer::assign (const char * string, IndexT offset)
		{
			IndexT len = strlen(string);
			assign((const ByteT *)string, (const ByteT *)string + len, offset);
		}
		
#pragma mark -
#pragma mark class Resizable
		
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

#pragma mark -
#pragma mark class StaticBuffer

		StaticBuffer StaticBuffer::forCString (const char * str, bool includeNullByte)
		{
			return StaticBuffer((const ByteT*)str, strlen(str) + (int)includeNullByte);
		}
		
		StaticBuffer::StaticBuffer (const ByteT * buf, const IndexT & size) : m_buf (buf), m_size (size)
		{
		}

		StaticBuffer::~StaticBuffer ()
		{
		}

		IndexT StaticBuffer::size () const
		{
			return m_size;
		}

		const ByteT * StaticBuffer::begin () const
		{
			return m_buf;
		}

#pragma mark -
#pragma mark class FileBuffer
		
		FileBuffer::FileBuffer (const Path & filePath)
		{
			FileDescriptorT input = open(filePath.file_string().c_str(), O_RDONLY);

			if (input == -1)
				perror(__PRETTY_FUNCTION__);
			
			ensure(input != -1);
			
			m_size = lseek(input, 0, SEEK_END);
			
			m_buf = mmap(0, m_size, PROT_READ, MAP_SHARED, input, 0);
			ensure(m_buf != (ByteT *)-1);
		}
		
		FileBuffer::~FileBuffer ()
		{
			munmap(m_buf, m_size);
		}
		
		IndexT FileBuffer::size () const
		{
			return m_size;
		}
		
		const ByteT * FileBuffer::begin () const
		{
			return (const ByteT *)m_buf;
		}
		
#pragma mark -
#pragma mark class DynamicBuffer

		void DynamicBuffer::allocate (IndexT size)
		{
			if (size != m_capacity)
			{
				m_buf = (ByteT*)realloc(m_buf, size);
				ensure(m_buf != NULL);

				m_capacity = size;
			}
		}

		void DynamicBuffer::deallocate ()
		{
			if (m_buf)
			{
				free(m_buf);
				m_buf = NULL;
				m_size = 0;
				m_capacity = 0;
			}
		}

		DynamicBuffer::DynamicBuffer () : m_capacity (0), m_size (0), m_buf (NULL)
		{
		}

		DynamicBuffer::DynamicBuffer (IndexT size, bool reserved) : m_buf (NULL)
		{
			allocate(size);
			
			if (reserved == false)
				m_size = size;
			else
				m_size = 0;
		}

		DynamicBuffer::~DynamicBuffer ()
		{
			deallocate();
		}

		IndexT DynamicBuffer::capacity () const
		{
			return m_capacity;
		}

		IndexT DynamicBuffer::size () const
		{
			return m_size;
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
			if (size > m_capacity)
			{
				allocate(size);
			}

			m_size = size;
		}

		ByteT * DynamicBuffer::begin ()
		{
			return m_buf;
		}

		const ByteT * DynamicBuffer::begin () const
		{
			return m_buf;
		}

#pragma mark -
#pragma mark class PackedData

		PackedBuffer::PackedBuffer (IndexT size) : m_size (size)
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

		PackedBuffer * PackedBuffer::newBuffer (IndexT size)
		{
			IndexT total = sizeof(PackedBuffer) + size;
			void * data = malloc(total);
			PackedBuffer * buffer = new(data) PackedBuffer(size);

			return buffer;
		}

		PackedBuffer::Ptr PackedBuffer::sharedBuffer (IndexT size)
		{
			return PackedBuffer::Ptr(newBuffer(size));
		}

		IndexT PackedBuffer::size () const
		{
			return m_size;
		}

		ByteT * PackedBuffer::begin ()
		{
			return data();
		}

		const ByteT * PackedBuffer::begin () const
		{
			return data();
		}

#pragma mark -
#pragma mark class BufferStream

		BufferStream::BufferStream (const Buffer & buf) : std::streambuf (), std::istream (this)
		{
			std::streambuf::setg ((char *) buf.begin (), (char *) buf.begin (), (char *) buf.end ());
		}

#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(BufferRead)
		{
			StaticBuffer buf = StaticBuffer::forCString("Bobby");
			
			uint8_t v;
			buf.read(2, v);
			
			assertEqual(v, 'b', "Read value is equal");
		}
		
		UNIT_TEST(DynamicBuffer)
		{
			const char * data = "Human resources are human first, and resources second.";
			unsigned dataLength = strlen(data);

			testing("Construction");

			DynamicBuffer a(100), b;

			assertTrue(a.size() == 100, "Size set by constructor");
			assertTrue(b.size() == 0, "Size set by constructor");

			assertFalse(a.empty(), "Sized construtor is not empty");
			assertTrue(b.empty(), "Default construtor is empty");

			assertTrue(a.capacity() >= a.size(), "Capacity >= size");
			assertTrue(b.capacity() >= b.size(), "Capacity >= size");

			testing("Assigning Data");

			IndexT prevCapacity = a.capacity();
			a.resize(dataLength);
			a.assign((const ByteT*)data, (const ByteT*)data + dataLength);

			// Performance check
			assertTrue(a.capacity() == prevCapacity, "Don't realloc if size is within capacity");

			b.resize(dataLength);
			b.assign((const ByteT*)data, (const ByteT*)data + dataLength);

			assertTrue(a == b, "Data and size is the same");
			assertTrue(a[5] == data[5], "Data indexing is correct");

			testing("Clearing buffers");

			a.clear();
			assertTrue(a.empty(), "Cleared buffer is empty");

			assertFalse(a == b, "Buffers are different after being cleared");

			b.clear();
			assertTrue(a == b, "Cleared buffers are equivalent");

			testing("Capacity");

			a.reserve(200);
			assertTrue(a.capacity() == 200, "Reserved capacity for 200 bytes");

			b.reserve(400);
			assertTrue(b.capacity() == 400, "Reserved capacity for 400 bytes");

			a.resize(600);
			assertTrue(a.size() == 600, "Size increase was successful");
			assertTrue(a.capacity() >= 600, "Capacity increased after size increase");
			
			a.expand(100);
			
			assertTrue(a.size() == 700, "Size expansion was successful");
			
			testing("Appending");
			
			a.resize(0);
			a.append(5, (const ByteT *)"abcde");
			a.append(5, (const ByteT *)"abcde");
			
			assertEqual(a.size(), 10, "Size is correct after appending 10 characters");
			assertEqual(a[1], 'b', "Character is correct");
		}

		UNIT_TEST(PackedBuffer)
		{
			const char * data = "Packed Buffer.";
			unsigned dataLength = strlen(data);

			PackedBuffer * buffer;

			testing("Construction");

			buffer = PackedBuffer::newBuffer(dataLength);
			assertTrue(buffer != NULL, "Buffer was created successfully");

			testing("Assigning Data");

			buffer->assign((const ByteT*)data, (const ByteT*)data + dataLength, 0);
			assertEqual(buffer->size(), dataLength, "Data size is consistent");

			for (unsigned i = 0; i < buffer->size(); i += 1)
				assertEqual((*buffer)[i], data[i], "Data is correct");
		}
		
		UNIT_TEST(ReadingAndWritingBuffers)
		{
			using namespace boost::filesystem;
			
			const char * tmpPath = "dream-buffer-test";
			const char * data = "When the only tool you have is a hammer, you tend to treat everything as if it were a nail.";
			unsigned dataLength = strlen(data);
			
			testing("Writing");
			
			ByteT cwd[256];
			getcwd((char *)cwd, 255);
			cwd[255] = '\0';
			
			std::cout << "Writing files in directory: " << cwd << std::endl;
			
			if (exists(tmpPath)) {
				std::cout << "Removing " << tmpPath << std::endl;
				remove(tmpPath);
			}
			
			PackedBuffer * writeBuffer;
			writeBuffer = PackedBuffer::newBuffer(dataLength);
			
			writeBuffer->assign((const ByteT*)data, (const ByteT*)data + dataLength, 0);
			
			writeBuffer->writeToFile(tmpPath);
			
			testing("Reading");
			FileBuffer readBuffer(tmpPath);
			
			std::string writeString(writeBuffer->begin(), writeBuffer->end());
			std::string readString(readBuffer.begin(), readBuffer.end());
			
			assertEqual(writeBuffer->size(), readBuffer.size(), "Data size is consistent");
			assertEqual(writeBuffer->checksum(), readBuffer.checksum(), "Data is correct");
			
			assertEqual(writeString, readString, "Data string is equal");
			
			writeBuffer->hexdump(std::cout);
			readBuffer.hexdump(std::cout);
		}
#endif
	}
}