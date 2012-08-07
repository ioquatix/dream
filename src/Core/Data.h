//
//  Core/Data.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/05/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

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
		class IData : implements IObject {
		public:
			/// Access the data as a buffer. This buffer is shared (same buffer returned every time).
			virtual Shared<Buffer> buffer () const abstract;

			/// Access the data as an input stream. The stream is unique (new stream returned each time).
			virtual Shared<std::istream> input_stream () const abstract;

			/// Return the size of the input data if it is known.
			virtual std::size_t size () const abstract;
		};

		class LocalFileData : public Object, implements IData {
		protected:
			Path _path;
			mutable Shared<Buffer> _buffer;

		public:
			LocalFileData (const Path & path);
			virtual ~LocalFileData ();

			virtual Shared<Buffer> buffer () const;
			virtual Shared<std::istream> input_stream () const;

			virtual std::size_t size () const;
		};

		class BufferedData : public Object, implements IData {
		protected:
			Shared<Buffer> _buffer;

		public:
			BufferedData (Shared<Buffer> buffer);

			/// Create a buffer from a given input stream
			BufferedData (std::istream & stream);

			virtual ~BufferedData ();

			virtual Shared<Buffer> buffer () const;
			virtual Shared<std::istream> input_stream () const;

			virtual std::size_t size () const;
		};
	}
}

#endif
