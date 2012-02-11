//
//  Core/Data.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/05/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "Data.h"

#include <streambuf>
#include <sstream>
#include <stdexcept>

namespace Dream
{
	namespace Core
	{
		
#pragma mark -
#pragma mark LocalFileData

		LocalFileData::LocalFileData (const Path & path)
			: _path(path)
		{
			
		}
		
		LocalFileData::~LocalFileData ()
		{
		
		}
		
		Shared<Buffer> LocalFileData::buffer () const
		{
			if (!_buffer) {
				_buffer = new FileBuffer(_path);
			}
			
			return _buffer;
		}
		
		Shared<std::istream> LocalFileData::input_stream () const
		{
			std::ifstream * file_input_stream = new std::ifstream(_path.to_local_path().c_str(), std::ios::binary);
			
			return Shared<std::istream>(file_input_stream);
		}
		
		std::size_t LocalFileData::size () const
		{
			return _path.file_size();
		}


#pragma mark -
#pragma mark BufferedData
		
		BufferedData::BufferedData (Shared<Buffer> buffer)
			: _buffer(buffer)
		{
		
		}
		
		/// Create a buffer from a given input stream
		BufferedData::BufferedData (std::istream & stream)
		{
			Shared<DynamicBuffer> buffer = new DynamicBuffer;
			
			buffer->append(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
			
			_buffer = buffer;
		}
		
		BufferedData::~BufferedData ()
		{
		
		}
		
		Shared<Buffer> BufferedData::buffer () const
		{
			return _buffer;
		}
		
		Shared<std::istream> BufferedData::input_stream () const
		{
			return new BufferStream(*_buffer);
		}
		
		std::size_t BufferedData::size () const
		{
			return _buffer->size();
		}
		
#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(Data)
		{
			const char * data = "First things first -- but not necessarily in that order.";

			testing("Construction");

			Shared<StaticBuffer> sb = new StaticBuffer(StaticBuffer::for_cstring(data, false));
			Ref<IData> a = new BufferedData(sb);

			check(a->buffer()->size() == strlen(data)) << "Data length is correct";
		}
#endif
	}
}
