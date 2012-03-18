//
//  Imaging/PixelBuffer.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 30/04/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

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
			
		unsigned data_type_byte_size(ImageDataType type);
		unsigned packed_pixel_channel_count (ImagePixelPacking type);
		unsigned pixel_format_channel_count (ImagePixelFormat type);
		
		// This type is guaranteed to be big enough to hold even RGBA16.
		// This is useful when you want a generic representation of a pixel
		typedef uint64_t PixelT;
		
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
						
			virtual ImagePixelFormat pixel_format () const abstract; // eg GL_RGBA
			virtual ImageDataType pixel_data_type () const; // eg GL_UNSIGNED_BYTE
			
			unsigned pixel_data_length () const { return size().product() * bytes_per_pixel(); }
			
			// Returns the equivalent pixel type, ie GL_UNSIGNED_INT_8_8_8_8 -> GL_UNSIGNED_INT
			ImageDataType packed_type () const;
			unsigned channel_count () const;
			
			bool is_packed_format () const;
			bool is_byte_order_reversed () const;
			
			unsigned bytes_per_pixel () const;
			
			// Helper: Returns the maximum value of an individual pixel
			PixelT max_pixel_size () const {
				return ((PixelT)1 << (bytes_per_pixel() * 8)) - 1;
			}
					
			virtual Vector<3, unsigned> size () const abstract;
			
			// Data accessors
			virtual const ByteT * pixel_data () const abstract;
			
			// Get the number of bytes to index into pixel_data
			unsigned pixel_offset (const Vector<3, unsigned> &at) const {
				unsigned offset = at[X] + (at[Y] * size()[X]) + (at[Z] * size()[X] * size()[Y]);
				return offset * bytes_per_pixel();
			}
			
			// pixbuf->at(vec<unsigned>(43, 12));
			const ByteT* pixel_data_at (const Vector<3, unsigned> &at) const {
				return pixel_data() + pixel_offset(at);
			}
			
			// Helper to read pixel data
			// This may convert between the pixbuf format and the output pixel
			// void read_pixel (const Vector<3, unsigned> &at, Vector<4, float> &output) const;
			template <typename DataT>
			void read_data_at (const unsigned &idx, DataT &val) const {
				const ByteT *data = pixel_data();
				
				val = *(DataT*)(&data[idx]);
			}
			
			// This does not do _any_ sanity checking what-so-ever.
			template <unsigned D, typename NumericT>
			void read_pixel (const Vector<3, unsigned> &at, Vector<D, NumericT> &output) const {
				DREAM_ASSERT(!is_packed_format() && "Packed pixel formats not supported for reading!");
				
				unsigned from = pixel_offset(at);
				
				for (unsigned i = 0; i < D; i += 1) {
					read_data_at(from + (i * bytes_per_pixel()), output[i]);
				}
			}
			
			PixelT read_pixel (const Vector<3, unsigned> &at);
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
			virtual ByteT * pixel_data () abstract;
			using IPixelBuffer::pixel_data;
			
			void zero (PixelT px = 0);
			
			ByteT* pixel_data_at (const Vector<3, unsigned> &at)
			{
				return pixel_data() + pixel_offset(at);
			}
			
			template <typename data_t>
			void write_data_at (const unsigned &idx, const data_t &val)
			{
				ByteT *data = pixel_data();
				
				*(data_t*)(&data[idx]) = val;
			}
			
			/// Set all bytes to zero.
			void clear ();

			// void write_pixel (const Vector<3, unsigned> &at, const Vector<4, float> &input);
			template <unsigned D, typename NumericT>
			void write_pixel (const Vector<3, unsigned> &at, const Vector<D, NumericT> &input);
			
			void write_pixel (const Vector<3, unsigned> &at, const PixelT &px);
			
			// Copy from buf to this
			void copy_pixels_from(const IPixelBuffer& buf, const Vector<3, unsigned> &from, const Vector<3, unsigned> &to, const Vector<3, unsigned> &size, CopyFlags copy_flags = CopyNormal);
			
			void copy_pixels_from(const IPixelBuffer& buf, const Vector<3, unsigned> &to, CopyFlags copy_flags = CopyNormal)
			{
				copy_pixels_from(buf, Vec3u(0, 0, 0), to, buf.size(), copy_flags);
			}
		};
		

	}
}


#endif
