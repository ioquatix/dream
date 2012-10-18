//
//  Imaging/PixelBuffer.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 1/05/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "PixelBuffer.h"

#include <iostream>
#include <unistd.h>

namespace Dream {
	namespace Imaging {
		std::size_t data_type_byte_size(DataType data_type) {
			return 0xFF & (unsigned)data_type;
		}

		unsigned pixel_format_channel_count (PixelFormat pixel_format) {
			return 0xFF & (unsigned)pixel_format;
		}

// MARK: -
// MARK: class IPixelBuffer

		PixelT IPixelBuffer::read_pixel (const PixelCoordinateT &at) {
			const ByteT * src = this->pixel_data_at(at);
			PixelT px = 0;
			ByteT * dst = (ByteT*)&px;
			memcpy(dst, src, this->bytes_per_pixel());

			return px;
		}

		DataType IPixelBuffer::pixel_data_type () const
		{
			return DataType::BYTE;
		}

		std::size_t IPixelBuffer::bytes_per_pixel () const {
			return data_type_byte_size(pixel_data_type()) * pixel_format_channel_count(pixel_format());
		}

		unsigned IPixelBuffer::channel_count () const {
			return pixel_format_channel_count (pixel_format());
		}

// MARK: -
// MARK: class IMutablePixelBuffer

		void IMutablePixelBuffer::clear ()
		{
			ByteT * buffer = pixel_data();
			bzero(buffer, pixel_data_length());
		}

		void IMutablePixelBuffer::write_pixel (const PixelCoordinateT &at, const PixelT &px)
		{
			ByteT * dst = this->pixel_data_at(at);
			const ByteT * src = (const ByteT *)&px;
			memcpy(dst, src, this->bytes_per_pixel());
		}

		void IMutablePixelBuffer::zero (PixelT px)
		{
			std::size_t bps = bytes_per_pixel();

			DREAM_ASSERT(pixel_data() != NULL && "Cannot zero null buffer!");

			if (px == 0) {
				bzero(pixel_data(), pixel_data_length());
			} else {
				for (IndexT s = 0; s < pixel_data_length(); s += bps) {
					write (px, pixel_data() + s, bps);
				}
			}
		}

		// Copy from buf to this
		void IMutablePixelBuffer::copy_pixels_from (const IPixelBuffer & buf, const PixelCoordinateT &from, const PixelCoordinateT &to, const PixelCoordinateT &size, CopyFlags copy_flags)
		{
			DREAM_ASSERT(this->channel_count() == buf.channel_count());
			DREAM_ASSERT(this->pixel_data_type() == buf.pixel_data_type());

			//std::cout << from << " -> " << to << " : " << size << std::endl;
			//std::cout << buf.size() << " -> " << this->size() << std::endl;

			DREAM_ASSERT(from.less_than(buf.size()));
			DREAM_ASSERT(to.less_than(this->size()));
			DREAM_ASSERT((from+size).less_than_or_equal(buf.size()));

			//std::cout <<  (to+size) << ": " << this->size() << std::endl;
			DREAM_ASSERT((to+size).less_than_or_equal(this->size()));

			const std::size_t pixel_size = this->bytes_per_pixel();
			PixelCoordinateT s, d;

			const ByteT * src = buf.pixel_data();
			ByteT * dst = this->pixel_data();

			for (std::size_t z = 0; z < size[Z]; z += 1)
				for (std::size_t y = 0; y < size[Y]; y += 1) {
					s[X] = 0; s[Y] = y; s[Z] = z;
					d = s;

					if (copy_flags & CopyFlip) {
						s[Y] = (size[Y] - 1) - s[Y];
						//std::cout << "Flipping row: " << y << " => " << c[Y] << std::endl;
					}

					memcpy(dst + this->pixel_offset(to + d), src + buf.pixel_offset(from + s), pixel_size * size[X]);
					//memcpy(this->pixel_data_at(to + d), buf.pixel_data_at(from + s), pixel_size * size[X]);
				}
		}
	}
}
