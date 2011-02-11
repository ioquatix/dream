/*
 *  Core/Data.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 5/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "Data.h"

#include <streambuf>
#include <sstream>
#include <stdexcept>

namespace Dream
{
	namespace Core
	{
	
#pragma mark IData

		IMPLEMENT_INTERFACE(Data)
		
#pragma mark -
#pragma mark LocalFileData

		IMPLEMENT_CLASS(LocalFileData)

		LocalFileData::LocalFileData (const Path & path)
			: m_path(path)
		{
			
		}
		
		LocalFileData::~LocalFileData ()
		{
		
		}
		
		Shared<Buffer> LocalFileData::buffer () const
		{
			if (!m_buffer) {
				m_buffer = new FileBuffer(m_path);
			}
			
			return m_buffer;
		}
		
		Shared<std::istream> LocalFileData::inputStream () const
		{
			std::ifstream * fileInputStream = new std::ifstream(m_path.toLocalPath().c_str(), std::ios::binary);
			
			return Shared<std::istream>(fileInputStream);
		}
		
		std::size_t LocalFileData::size () const
		{
			return m_path.fileSize();
		}


#pragma mark -
#pragma mark BufferedData

		IMPLEMENT_CLASS(BufferedData)
		
		BufferedData::BufferedData (Shared<Buffer> buffer)
		{
		
		}
		
		/// Create a buffer from a given input stream
		BufferedData::BufferedData (std::istream & stream)
		{
		
		}
		
		BufferedData::~BufferedData ()
		{
		
		}
		
		Shared<Buffer> BufferedData::buffer () const
		{
			return m_buffer;
		}
		
		Shared<std::istream> BufferedData::inputStream () const
		{
			return new BufferStream(*m_buffer);
		}
		
		std::size_t BufferedData::size () const
		{
			return m_buffer->size();
		}
		
#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(Data)
		{
			const char * data = "First things first -- but not necessarily in that order.";

			testing("Construction");

			Shared<StaticBuffer> sb = new StaticBuffer(StaticBuffer::forCString(data, false));
			REF(IData) a = new BufferedData(sb);

			check(a->buffer()->size() == strlen(data)) << "Data length is correct";
		}
#endif
	}
}