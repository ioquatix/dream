/*
 *  Imaging/Image.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 1/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "Image.h"
#include <cassert>

namespace Dream {
	namespace Imaging {
		
		IMPLEMENT_CLASS(UnbufferedImage)
		
		UnbufferedImage::UnbufferedImage(ImagePixelFormat format, ImageDataType dataType) : m_data(NULL) {
			setFormat(format, dataType);
		}
		
		UnbufferedImage::UnbufferedImage(const ByteT *data, const Vector<3, unsigned> &size, ImagePixelFormat format, ImageDataType dataType) : m_data(NULL) { // black image of size
			setFormat(format, dataType);
			setData(data, size);
		}

		void UnbufferedImage::setFormat(ImagePixelFormat format, ImageDataType dataType) {
			m_format = format;
			m_dataType = dataType;
		}
		
		void UnbufferedImage::setData(const ByteT *data, const Vector<3, unsigned> &size) {
			m_data = data;
			m_size = size;		
		}
		
		IMPLEMENT_CLASS(Image)
		
		void Image::Class::registerLoaderTypes (REF(ILoader) loader)
		{
			loader->setLoaderForExtension(this, "jpg");
			loader->setLoaderForExtension(this, "png");
		}
		
		REF(Object) Image::Class::initWithSize(const Vector<3, unsigned> &size, ImagePixelFormat format, ImageDataType dataType)
		{
			return ptr(new Image(size, format, dataType));
		}
		
		REF(Object) Image::Class::initFromData(const REF(Data) data, const ILoader * loader)
		{
			return Image::loadFromData (data);
		}
		
		Image::Image ()
		{
		
		}
		
		Image::Image (const Vector<3, unsigned> &size, ImagePixelFormat format, ImageDataType dataType)
		{
			allocate(size, format, dataType);
		}

		Image::~Image()
		{
			
		}
				
		void Image::allocate (const Vec3u & size, ImagePixelFormat format, ImageDataType dataType)
		{
			m_size = size;
			m_format = format;
			m_dataType = dataType;

			m_data.resize(pixelDataLength());
		}
		
		const ByteT * Image::pixelData () const
		{
			return m_data.begin();
		}
		
		ByteT * Image::pixelData()
		{
			return m_data.begin();
		}
		
		void Image::setData (const Vector<3, unsigned> &size, ImagePixelFormat format, ImageDataType dataType, const ByteT * buffer)
		{
			allocate(size, format, dataType);
			
			if (buffer) {
				setData(0, m_size.product(), buffer);
			}
		}
		
		void Image::setData (IndexT offset, IndexT length, const ByteT* buffer)
		{
			m_data.assign(buffer, buffer + length, offset);
		}
		
	}		
}