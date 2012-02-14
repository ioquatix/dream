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
		
		unsigned data_typeByteSize(ImageDataType type) {
			switch(type) {
				case UBYTE:
				case BYTE:
					return sizeof(char); // GLbyte
				case USHORT:
				case SHORT:
					return sizeof(short); // GLshort
				case UINT:
				case INT:
					return sizeof(long); // GLint
				case FLOAT:
					return sizeof(float); // GLfloat
				case DOUBLE:
					return sizeof(double); //GLdouble
				default:
					return 0;
			}
		}
		
		unsigned packed_pixel_channel_count (ImagePixelPacking type) {
			switch(type) {
				case UBYTE_3_3_2:
				case UBYTE_2_3_3_REV:
				case USHORT_5_6_5:
				case USHORT_5_6_5_REV:
					return 3;
					
				case USHORT_4_4_4_4:
				case USHORT_5_5_5_1:
				case USHORT_4_4_4_4_REV:
				case USHORT_1_5_5_5_REV:
				case UINT_8_8_8_8:
				case UINT_10_10_10_2:
				case UINT_8_8_8_8_REV:
				case UINT_2_10_10_10_REV:
					return 4;
					
				default:
					return 0;
			}
		}
		
		unsigned pixel_format_channel_count (ImagePixelFormat type) {
			switch(type) {
				case 1:
				case RED:
				case GREEN:
				case BLUE:
				case ALPHA: // Internal Format
				case LUMINANCE:
					return 1;
					
				case 2:
				case LUMINANCE_ALPHA: // Internal Format
					return 2;
				
				case 3:
				case RGB: // Internal Format
					return 3;
				
				case 4:
				case RGBA: // Internal Format
					return 4;
					
				default:
					return 0;
			}
		}
		
#pragma mark -
#pragma mark class IPixelBuffer
		
		PixelT IPixelBuffer::read_pixel (const Vector<3, unsigned> &at) {
			const ByteT * src = this->pixel_dataAt(at);
			PixelT px = 0;
			ByteT * dst = (ByteT*)&px;
			memcpy(dst, src, this->bytes_per_pixel());
			
			return px;
		}
		
		ImageDataType IPixelBuffer::pixel_dataType () const
		{
			return UBYTE; /* Suits most pixel formats */
		}
		
		unsigned IPixelBuffer::bytes_per_pixel () const {
			unsigned n = 1;
			ImageDataType pixfmt;
			
			if (is_packed_format()) {
				pixfmt = packed_type();
				// Packed formats only use one element (n = 1)
			} else {
				pixfmt = pixel_dataType();
				n = pixel_format_channel_count(pixel_format());
			}
			
			DREAM_ASSERT((data_typeByteSize(pixfmt) * n) > 0 && "bytes_per_pixel is obviously incorrect!");
			return data_typeByteSize(pixfmt) * n;
		}
		
		unsigned IPixelBuffer::channel_count () const {
			return pixel_format_channel_count (pixel_format());
		}
		
		// Components read from MSB to LSB
		// (ie, host-ordered data formats rather than byte ordered formats).
		bool IPixelBuffer::is_byte_order_reversed () const {		
			switch(pixel_dataType()) {			
				case UBYTE_2_3_3_REV:
				case USHORT_5_6_5_REV:
				case USHORT_4_4_4_4_REV:
				case USHORT_1_5_5_5_REV:
				case UINT_8_8_8_8_REV:
				case UINT_2_10_10_10_REV:
					return true;
				default:
					return false;
			}		
		}
		
		bool IPixelBuffer::is_packed_format () const {
			switch(pixel_dataType()) {
				case UBYTE_3_3_2:
				case USHORT_4_4_4_4:
				case USHORT_5_5_5_1:
				case UINT_8_8_8_8:
				case UINT_10_10_10_2:
				case USHORT_5_6_5:
				
				case UBYTE_2_3_3_REV:
				case USHORT_5_6_5_REV:
				case USHORT_4_4_4_4_REV:
				case USHORT_1_5_5_5_REV:
				case UINT_8_8_8_8_REV:
				case UINT_2_10_10_10_REV:
					return true;
					
				default:
					return false;
			}
		}
		
		ImageDataType IPixelBuffer::packed_type () const {
			switch(pixel_dataType()) {
				case UBYTE_3_3_2:
				case UBYTE_2_3_3_REV:
					return UBYTE;
					
				case USHORT_4_4_4_4:
				case USHORT_5_5_5_1:
				case USHORT_5_6_5:
				case USHORT_5_6_5_REV:
				case USHORT_4_4_4_4_REV:
				case USHORT_1_5_5_5_REV:
					return USHORT;
				
				case UINT_8_8_8_8:
				case UINT_10_10_10_2:
				case UINT_8_8_8_8_REV:
				case UINT_2_10_10_10_REV:
					return UINT;
					
				default:
					return ImageDataType(0);
			}
		}
		
#pragma mark -
#pragma mark class IMutablePixelBuffer
		
		void IMutablePixelBuffer::clear ()
		{
			ByteT * buffer = pixel_data();
			bzero(buffer, pixel_dataLength());
		}
		
		void IMutablePixelBuffer::write_pixel (const Vector<3, unsigned> &at, const PixelT &px)
		{
			ByteT * dst = this->pixel_dataAt(at);
			const ByteT * src = (const ByteT *)&px;
			memcpy(dst, src, this->bytes_per_pixel());
		}
		
		void IMutablePixelBuffer::zero (PixelT px)
		{
			unsigned bps = bytes_per_pixel();
			
			DREAM_ASSERT(pixel_data() != NULL && "Cannot zero null buffer!");
			
			if (px == 0) {
				bzero(pixel_data(), pixel_dataLength());
			} else {
				for (IndexT s = 0; s < pixel_dataLength(); s += bps) {
					write (px, pixel_data() + s, bps);
				}
			}
		}
		
		// void write_pixel (const Vector<3, unsigned> &at, const Vector<4, float> &input);
		template <unsigned D, typename NumericT>
		void IMutablePixelBuffer::write_pixel (const Vector<3, unsigned> &at, const Vector<D, NumericT> &input)
		{
			DREAM_ASSERT(!is_packed_format() && "Packed pixel formats not supported for reading!");
			DREAM_ASSERT(D == this->channel_count());
			
			unsigned from = pixel_offset(at);
			unsigned sub_pixel_size = bytes_per_pixel() / channel_count();
			DREAM_ASSERT(sizeof(NumericT) == sub_pixel_size);
			
			for (unsigned i = 0; i < D; i += 1) {
				write_data_at(from + (i * bytes_per_pixel()), input[i]);
			}
		}
		
		// Copy from buf to this
		void IMutablePixelBuffer::copy_pixels_from (const IPixelBuffer & buf, const Vector<3, unsigned> &from, const Vector<3, unsigned> &to, 
												  const Vector<3, unsigned> &size, CopyFlags copy_flags) 
		{
			DREAM_ASSERT(!is_packed_format() && "Packed pixel formats not supported for reading!");
			DREAM_ASSERT(this->channel_count() == buf.channel_count());
			DREAM_ASSERT(this->pixel_dataType() == buf.pixel_dataType());
			
			//std::cout << from << " -> " << to << " : " << size << std::endl;
			//std::cout << buf.size() << " -> " << this->size() << std::endl;
			
			DREAM_ASSERT(from.less_than(buf.size()));
			DREAM_ASSERT(to.less_than(this->size()));
			DREAM_ASSERT((from+size).less_than_or_equal(buf.size()));
			
			//std::cout <<  (to+size) << ": " << this->size() << std::endl;
			DREAM_ASSERT((to+size).less_than_or_equal(this->size()));
					
			const unsigned pixel_size = this->bytes_per_pixel();
			Vector<3, unsigned> s, d;
			
			const ByteT * src = buf.pixel_data();
			ByteT * dst = this->pixel_data();
			
			for (unsigned z = 0; z < size[Z]; z += 1)
				for (unsigned y = 0; y < size[Y]; y += 1) {
					s[X] = 0; s[Y] = y; s[Z] = z;
					d = s;
					
					if (copy_flags & CopyFlip) {
						s[Y] = (size[Y] - 1) - s[Y];
						//std::cout << "Flipping row: " << y << " => " << c[Y] << std::endl;
					}
					
					memcpy(dst + this->pixel_offset(to + d), src + buf.pixel_offset(from + s), pixel_size * size[X]);
					//memcpy(this->pixel_dataAt(to + d), buf.pixel_dataAt(from + s), pixel_size * size[X]);
				}
		}
		
	}
}
