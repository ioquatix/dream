/*
 *  Imaging/PixelBuffer.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 30/04/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_IMAGING_PIXELBUFFER_H
#define _DREAM_IMAGING_PIXELBUFFER_H

#include "../Framework.h"
#include "../Numerics/Vector.h"

namespace Dream {
	namespace Imaging {
		using namespace Dream::Numerics;
		
		/// Pixel data type.
		/// These should be compatible with the equiv. OpenGL types
		enum ImageDataType {
			BYTE = 0x1400,
			UBYTE = 0x1401,
			SHORT = 0x1402,
			USHORT = 0x1403,
			INT = 0x1404,
			UINT = 0x1405,
			FLOAT = 0x1406,
			DOUBLE = 0x140A,
		};
		
		/// Pixel packing type.
		enum ImagePixelPacking {
			UBYTE_3_3_2 = 0x8032,
			USHORT_4_4_4_4 = 0x8033,
			USHORT_5_5_5_1 = 0x8034,
			UINT_8_8_8_8 = 0x8035,
			UINT_10_10_10_2 = 0x8036,
			UBYTE_2_3_3_REV = 0x8362,
			USHORT_5_6_5 = 0x8363,
			USHORT_5_6_5_REV = 0x8364,
			USHORT_4_4_4_4_REV = 0x8365,
			USHORT_1_5_5_5_REV = 0x8366,
			UINT_8_8_8_8_REV = 0x8367,
			UINT_2_10_10_10_REV = 0x8368
		};
		
		/// Image pixel formats.
		enum ImagePixelFormat {
			RED = 0x1903,
			GREEN = 0x1904,
			BLUE = 0x1905,
			ALPHA = 0x1906,
			RGB = 0x1907,
			RGBA = 0x1908, 
			LUMINANCE = 0x1909,
			LUMINANCE_ALPHA = 0x190A
		};
			
		unsigned dataTypeByteSize(ImageDataType type);
		unsigned packedPixelChannelCount (ImagePixelPacking type);
		unsigned pixelFormatChannelCount (ImagePixelFormat type);
		
		// This type is guaranteed to be big enough to hold even RGBA16.
		// This is useful when you want a generic representation of a pixel
		typedef unsigned long long PixelT;
		
		class IPixelBuffer : implements IObject
		{
		public:					
			enum Component {
				RED = 0,
				GREEN = 1,
				BLUE = 2,
				ALPHA = 3,
				INTENSITY = 4,
				LUMINANCE = 5
			};
						
			virtual ImagePixelFormat pixelFormat () const abstract; // eg GL_RGBA
			virtual ImageDataType pixelDataType () const; // eg GL_UNSIGNED_BYTE
			
			unsigned pixelDataLength () const { return size().product() * bytesPerPixel(); }
			
			// Returns the equivalent pixel type, ie GL_UNSIGNED_INT_8_8_8_8 -> GL_UNSIGNED_INT
			ImageDataType packedType () const;
			unsigned channelCount () const;
			
			bool isPackedFormat () const;
			bool isByteOrderReversed () const;
			
			unsigned bytesPerPixel () const;
			
			// Helper: Returns the maximum value of an individual pixel
			PixelT maxPixelSize () const {
				return ((PixelT)1 << (bytesPerPixel() * 8)) - 1;
			}
					
			virtual Vector<3, unsigned> size () const abstract;
			
			// Data accessors
			virtual const ByteT * pixelData () const abstract;
			
			// Get the number of bytes to index into pixelData
			unsigned pixelOffset (const Vector<3, unsigned> &at) const {
				unsigned offset = at[X] + (at[Y] * size()[X]) + (at[Z] * size()[X] * size()[Y]);
				return offset * bytesPerPixel();
			}
			
			// pixbuf->at(vec<unsigned>(43, 12));
			const ByteT* pixelDataAt (const Vector<3, unsigned> &at) const {
				return pixelData() + pixelOffset(at);
			}
			
			// Helper to read pixel data
			// This may convert between the pixbuf format and the output pixel
			// void readPixel (const Vector<3, unsigned> &at, Vector<4, float> &output) const;
			template <typename data_t>
			void readDataAt (const unsigned &idx, data_t &val) {
				const ByteT *data = pixelData();
				
				val = *(data_t*)(&data[idx]);
			}
			
			// This does not do _any_ sanity checking what-so-ever.
			template <unsigned D, typename NumericT>
			void readPixel (const Vector<3, unsigned> &at, Vector<D, NumericT> &output) const {
				ensure(!isPackedFormat() && "Packed pixel formats not supported for reading!");
				
				unsigned from = pixelOffset(at);
				
				for (unsigned i = 0; i < D; i += 1) {
					readDataAt(from + (i * bytesPerPixel()), output[i]);
				}
			}
			
			PixelT readPixel (const Vector<3, unsigned> &at);
		};
		
		enum CopyFlags {
			CopyNormal = 0,
			CopyFlip = 1
		};
		
		// Some pixel buffers (for example the screen buffer), can never have mutable operations without
		// high cost, so there is a sub-interface for 'typically' mutable pixbufs.
		class IMutablePixelBuffer : implements IPixelBuffer
		{
		public:			
			virtual ByteT * pixelData () abstract;
			
			void zero (PixelT px = 0);
			
			ByteT* pixelDataAt (const Vector<3, unsigned> &at)
			{
				return pixelData() + pixelOffset(at);
			}
			
			template <typename data_t>
			void writeDataAt (const unsigned &idx, const data_t &val)
			{
				ByteT *data = pixelData();
				
				*(data_t*)(&data[idx]) = val;
			}
			
			/// Set all bytes to zero.
			void clear ();

			// void writePixel (const Vector<3, unsigned> &at, const Vector<4, float> &input);
			template <unsigned D, typename NumericT>
			void writePixel (const Vector<3, unsigned> &at, const Vector<D, NumericT> &input);
			
			void writePixel (const Vector<3, unsigned> &at, const PixelT &px);
			
			// Copy from buf to this
			void copyPixelsFrom(const IPixelBuffer& buf, const Vector<3, unsigned> &from, const Vector<3, unsigned> &to, const Vector<3, unsigned> &size, CopyFlags copyFlags = CopyNormal);
			
			void copyPixelsFrom(const IPixelBuffer& buf, const Vector<3, unsigned> &to, CopyFlags copyFlags = CopyNormal)
			{
				copyPixelsFrom(buf, vec<unsigned>(0, 0, 0), to, buf.size(), copyFlags);
			}
		};
		

	}
}


#endif