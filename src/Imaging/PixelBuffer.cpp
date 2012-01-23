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
		
		unsigned dataTypeByteSize(ImageDataType type) {
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
		
		unsigned packedPixelChannelCount (ImagePixelPacking type) {
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
		
		unsigned pixelFormatChannelCount (ImagePixelFormat type) {
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
		
		PixelT IPixelBuffer::readPixel (const Vector<3, unsigned> &at) {
			const ByteT * src = this->pixelDataAt(at);
			PixelT px = 0;
			ByteT * dst = (ByteT*)&px;
			memcpy(dst, src, this->bytesPerPixel());
			
			return px;
		}
		
		ImageDataType IPixelBuffer::pixelDataType () const
		{
			return UBYTE; /* Suits most pixel formats */
		}
		
		unsigned IPixelBuffer::bytesPerPixel () const {
			unsigned n = 1;
			ImageDataType pixfmt;
			
			if (isPackedFormat()) {
				pixfmt = packedType();
				// Packed formats only use one element (n = 1)
			} else {
				pixfmt = pixelDataType();
				n = pixelFormatChannelCount(pixelFormat());
			}
			
			ensure((dataTypeByteSize(pixfmt) * n) > 0 && "bytesPerPixel is obviously incorrect!");
			return dataTypeByteSize(pixfmt) * n;
		}
		
		unsigned IPixelBuffer::channelCount () const {
			return pixelFormatChannelCount (pixelFormat());
		}
		
		// Components read from MSB to LSB
		// (ie, host-ordered data formats rather than byte ordered formats).
		bool IPixelBuffer::isByteOrderReversed () const {		
			switch(pixelDataType()) {			
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
		
		bool IPixelBuffer::isPackedFormat () const {
			switch(pixelDataType()) {
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
		
		ImageDataType IPixelBuffer::packedType () const {
			switch(pixelDataType()) {
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
			ByteT * buffer = pixelData();
			bzero(buffer, pixelDataLength());
		}
		
		void IMutablePixelBuffer::writePixel (const Vector<3, unsigned> &at, const PixelT &px)
		{
			ByteT * dst = this->pixelDataAt(at);
			const ByteT * src = (const ByteT *)&px;
			memcpy(dst, src, this->bytesPerPixel());
		}
		
		void IMutablePixelBuffer::zero (PixelT px)
		{
			unsigned bps = bytesPerPixel();
			
			ensure(pixelData() != NULL && "Cannot zero null buffer!");
			
			if (px == 0) {
				bzero(pixelData(), pixelDataLength());
			} else {
				for (IndexT s = 0; s < pixelDataLength(); s += bps) {
					write (px, pixelData() + s, bps);
				}
			}
		}
		
		// void writePixel (const Vector<3, unsigned> &at, const Vector<4, float> &input);
		template <unsigned D, typename NumericT>
		void IMutablePixelBuffer::writePixel (const Vector<3, unsigned> &at, const Vector<D, NumericT> &input)
		{
			ensure(!isPackedFormat() && "Packed pixel formats not supported for reading!");
			ensure(D == this->channelCount());
			
			unsigned from = pixelOffset(at);
			unsigned subPixelSize = bytesPerPixel() / channelCount();
			ensure(sizeof(NumericT) == subPixelSize);
			
			for (unsigned i = 0; i < D; i += 1) {
				writeDataAt(from + (i * bytesPerPixel()), input[i]);
			}
		}
		
		// Copy from buf to this
		void IMutablePixelBuffer::copyPixelsFrom (const IPixelBuffer & buf, const Vector<3, unsigned> &from, const Vector<3, unsigned> &to, 
												  const Vector<3, unsigned> &size, CopyFlags copyFlags) 
		{
			ensure(!isPackedFormat() && "Packed pixel formats not supported for reading!");
			ensure(this->channelCount() == buf.channelCount());
			ensure(this->pixelDataType() == buf.pixelDataType());
			
			//std::cout << from << " -> " << to << " : " << size << std::endl;
			//std::cout << buf.size() << " -> " << this->size() << std::endl;
			
			ensure(from.lessThan(buf.size()));
			ensure(to.lessThan(this->size()));
			ensure((from+size).lessThanOrEqual(buf.size()));
			
			//std::cout <<  (to+size) << ": " << this->size() << std::endl;
			ensure((to+size).lessThanOrEqual(this->size()));
					
			const unsigned pixelSize = this->bytesPerPixel();
			Vector<3, unsigned> s, d;
			
			const ByteT * src = buf.pixelData();
			ByteT * dst = this->pixelData();
			
			for (unsigned z = 0; z < size[Z]; z += 1)
				for (unsigned y = 0; y < size[Y]; y += 1) {
					s[X] = 0; s[Y] = y; s[Z] = z;
					d = s;
					
					if (copyFlags & CopyFlip) {
						s[Y] = (size[Y] - 1) - s[Y];
						//std::cout << "Flipping row: " << y << " => " << c[Y] << std::endl;
					}
					
					memcpy(dst + this->pixelOffset(to + d), src + buf.pixelOffset(from + s), pixelSize * size[X]);
					//memcpy(this->pixelDataAt(to + d), buf.pixelDataAt(from + s), pixelSize * size[X]);
				}
		}
		
	}
}
