//
//  Imaging/Image.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 1/05/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "Image.h"
#include <cassert>

namespace Dream {
	namespace Imaging {

		UnbufferedImage::UnbufferedImage(ImagePixelFormat format, ImageDataType data_type) : _data(NULL) {
			set_format(format, data_type);
		}
		
		UnbufferedImage::UnbufferedImage(const ByteT *data, const Vector<3, unsigned> &size, ImagePixelFormat format, ImageDataType data_type) : _data(NULL) { // black image of size
			set_format(format, data_type);
			set_data(data, size);
		}

		void UnbufferedImage::set_format(ImagePixelFormat format, ImageDataType data_type) {
			_format = format;
			_data_type = data_type;
		}
		
		void UnbufferedImage::set_data(const ByteT *data, const Vector<3, unsigned> &size) {
			_data = data;
			_size = size;		
		}

		void Image::Loader::register_loader_types (ILoader * loader)
		{
			loader->set_loader_for_extension(this, "jpg");
			loader->set_loader_for_extension(this, "png");
		}
		
		Ref<Object> Image::Loader::load_from_data(const Ptr<IData> data, const ILoader * loader)
		{
			return Image::load_from_data(data);
		}
		 
		Image::Image ()
		{
		
		}
		
		Image::Image (const Vector<3, unsigned> &size, ImagePixelFormat format, ImageDataType data_type)
		{
			allocate(size, format, data_type);
		}

		Image::~Image()
		{
			
		}
				
		void Image::allocate (const Vec3u & size, ImagePixelFormat format, ImageDataType data_type)
		{
			_size = size;
			_format = format;
			_data_type = data_type;

			_data.resize(pixel_data_length());
		}
		
		const ByteT * Image::pixel_data () const
		{
			return _data.begin();
		}
		
		ByteT * Image::pixel_data()
		{
			return _data.begin();
		}
		
		void Image::set_data (const Vector<3, unsigned> &size, ImagePixelFormat format, ImageDataType data_type, const ByteT * buffer)
		{
			allocate(size, format, data_type);
			
			if (buffer) {
				set_data(0, _size.product(), buffer);
			}
		}
		
		void Image::set_data (IndexT offset, IndexT length, const ByteT* buffer)
		{
			_data.assign(buffer, buffer + length, offset);
		}
		
	}		
}
