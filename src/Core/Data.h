/*
 *  Core/Data.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 5/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CORE_DATA_H
#define _DREAM_CORE_DATA_H

#include "Buffer.h"
#include "URI.h"

namespace Dream
{
	namespace Core
	{
		/**
		 A abstract class representing a sequence of read-only bytes.

		 This class provides a very simple interface to a block of data. This is because this class is not designed for manipulating data, but rather loading,
		 saving and passing it around. As it is a fully fledged object, it is reference counted.
		 */
		class IData : IMPLEMENTS (Object)
		{
			EXPOSE_INTERFACE (Data)

			class Class : IMPLEMENTS (Object::Class)
			{
			};

		public:			
			/// Returns the start of the data area.
			virtual const ByteT * start () const abstract;
			
			/// Returns the length of the data area.
			virtual IndexT size () const abstract;
			
			/// Returns a buffer interface for reading the internal data
			virtual Buffer * buffer () abstract;
		};

		/**
		 A class that provides a set of methods for loading data from local file paths and in-memory data buffers.

		 Fetching from URIs is not handled here, because URIs may take some time to retrieve, or may fail, depending on the resource. Therefore, additional
		 structure must be available to handle these cases.

		 @sa Data::Class::initWithPath
		 @sa Data::Class::initWithBuffer
		 */
		class Data : public Object, IMPLEMENTS (Data)
		{
			Path m_path;
			Buffer * m_buffer;

			Data (const Path & p);
			Data (const Buffer & buf);

			EXPOSE_CLASS (Data)

			class Class : public Object::Class, IMPLEMENTS (Data::Class)
			{
			public:
				EXPOSE_CLASSTYPE

				/// Load data from a local file path.
				virtual REF(Data) initWithPath (const Path & p);

				/// Load data from an in-memory buffer.
				virtual REF(Data) initWithBuffer (const Buffer & buf);
			};

		public:
			virtual ~Data ();
			
			virtual const ByteT* start () const;
			virtual IndexT size () const;
			
			virtual Buffer * buffer ();
			
			/// If the data was loaded from a file system path, this path is non-empty.
			/// This is a hack for APIs which don't provide memory based loaders - it
			/// should be avoided if at all possible.
			const Path & path () const;
		};

	}
}

#endif