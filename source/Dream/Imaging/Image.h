//
//  Imaging/Image.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 30/04/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_IMAGING_IMAGE_H
#define _DREAM_IMAGING_IMAGE_H

#include "../Framework.h"
#include "PixelBuffer.h"
#include "../Resources/Loader.h"

namespace Dream {
	namespace Imaging {
		using namespace Resources;

		class ImageBase : implements IPixelBuffer {
		protected:
			PixelFormat _format;
			DataType _data_type;
			PixelCoordinateT _size;

		public:
			ImageBase () : _format(PixelFormat(0)), _data_type(DataType(0)), _size(ZERO) {}

			//Accessors
			virtual PixelFormat pixel_format () const
			{
				return _format;
			}

			virtual DataType pixel_data_type () const
			{
				return _data_type;
			}

			virtual PixelCoordinateT size () const
			{
				return _size;
			}
		};

		class UnbufferedImage : public Object, public ImageBase, implements IPixelBuffer {
		protected:
			const ByteT *_data;

		public:
			UnbufferedImage (PixelFormat format, DataType data_type);
			UnbufferedImage (const ByteT *data, const PixelCoordinateT &size, PixelFormat format, DataType data_type);

			void set_format (PixelFormat format, DataType data_type);
			void set_data (const ByteT *data, const PixelCoordinateT &size);

			virtual const ByteT* pixel_data () const
			{
				return _data;
			}
		};

		class Image : public Object, public ImageBase, implements IMutablePixelBuffer {
		public:
			class Loader : public Object, implements ILoadable {
			public:
				virtual void register_loader_types (ILoader * loader);
				virtual Ref<Object> load_from_data (const Ptr<IData> data, const ILoader * loader);
			};

		protected:
			DynamicBuffer _data;

		public:
			Image();

			Image(const PixelCoordinateT &size, PixelFormat format, DataType data_type);

			virtual ~Image ();

			// IMutablePixelBuffer access
			virtual const ByteT * pixel_data () const;
			virtual ByteT * pixel_data();

			/// Offset and Length are in pixels
			void set_data (const PixelCoordinateT &size, PixelFormat format, DataType data_type, const ByteT * buffer);
			void set_data (IndexT offset, IndexT length, const ByteT* buffer);

			/// Sets up the internal buffers for handing data for the specified size, format and data_type.
			void allocate (const Vec3u & size, PixelFormat format, DataType data_type);

		protected:
			static Ref<Image> load_from_data (const Ptr<IData> data);
		};
	}
}

#endif
