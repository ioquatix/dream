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

namespace Dream
{
	namespace Core
	{
		
#pragma mark -
#pragma mark class IData

		IMPLEMENT_INTERFACE(Data)
		
#pragma mark -
#pragma mark class Data

		IMPLEMENT_CLASS (Data)

		REF(Data) Data::Class::initWithPath (const Path & path)
		{
			return ptr(new Data(path));
		}
		
		REF(Data) Data::Class::initWithBuffer (const Buffer & buf)
		{
			return ptr(new Data(buf));
		}
		
		Data::Data (const Path & p)
		{
			m_buffer = new FileBuffer(p);
		}

		Data::Data (const Buffer & buf)
		{
			PackedBuffer * packedBuffer = PackedBuffer::newBuffer(buf.size());
			
			packedBuffer->assign(buf);
			
			m_buffer = packedBuffer;
		}
		
		Data::~Data ()
		{
			delete m_buffer;
		}
		
		const ByteT* Data::start () const
		{
			return m_buffer->begin();
		}
		
		IndexT Data::size () const
		{
			return m_buffer->size();
		}
		
		Buffer * Data::buffer ()
		{
			return m_buffer;
		}
		
#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
		UNIT_TEST(Data)
		{
			const char * data = "First things first -- but not necessarily in that order.";

			testing("Construction");

			StaticBuffer sb = StaticBuffer::forCString(data, false);

			REF(IData) a = Data::klass.initWithBuffer(sb);

			assertTrue(a->size() == strlen(data), "Data length is correct");
		}
#endif
	}
}