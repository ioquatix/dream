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

#include "../Class.h"
#include "Buffer.h"
#include "URI.h"
#include <fstream>

namespace Dream
{
	namespace Core
	{		
		/**
		 An abstract class representing a data store, such as an on-disk local file.
		 */
		class IData : IMPLEMENTS(Object)
		{
			EXPOSE_INTERFACE(Data)

			class Class : IMPLEMENTS(Object::Class)
			{
			};

		public:
			/// Access the data as a buffer. This buffer is shared (same buffer returned every time).
			virtual Shared<Buffer> buffer () const abstract;
			
			/// Access the data as an input stream. The stream is unique (new stream returned each time).
			virtual Shared<std::istream> inputStream () const abstract;

			/// Return the size of the input data if it is known.
			virtual std::size_t size () const abstract;
		};
		
		class LocalFileData : public Object, IMPLEMENTS(Data) {
			EXPOSE_CLASS(LocalFileData)

			class Class : public Object::Class, IMPLEMENTS (Data::Class)
			{
				EXPOSE_CLASSTYPE
			};
			
			protected:
				Path m_path;
				mutable Shared<Buffer> m_buffer;
			
			public:
				LocalFileData (const Path & path);
				virtual ~LocalFileData ();
				
				virtual Shared<Buffer> buffer () const;
				virtual Shared<std::istream> inputStream () const;
				
				virtual std::size_t size () const;
		};
		
		class BufferedData : public Object, IMPLEMENTS(Data) {
			EXPOSE_CLASS(BufferedData)

			class Class : public Object::Class, IMPLEMENTS (Data::Class)
			{
				EXPOSE_CLASSTYPE
			};
			
			protected:
				Shared<Buffer> m_buffer;
			
			public:
				BufferedData (Shared<Buffer> buffer);
				
				/// Create a buffer from a given input stream
				BufferedData (std::istream & stream);
				
				virtual ~BufferedData ();
				
				virtual Shared<Buffer> buffer () const;
				virtual Shared<std::istream> inputStream () const;	
				
				virtual std::size_t size () const;
		};
	}
}

#endif