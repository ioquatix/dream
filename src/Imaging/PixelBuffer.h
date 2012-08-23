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

			// Packed data types:
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
			// Generic pixel formats:
			GENERIC_1_CHANNEL = 1,
			GENERIC_2_CHANNEL = 2,
			GENERIC_3_CHANNEL = 3,
			GENERIC_4_CHANNEL = 4,

			// These types map to GL_ enumerations:
			RED = 0x1903,
			GREEN = 0x1904,
			BLUE = 0x1905,
			ALPHA = 0x1906,
			RGB = 0x1907,
			RGBA = 0x1908,
			LUMINANCE = 0x1909,
			LUMINANCE_ALPHA = 0x190A
		};

		std::size_t data_type_byte_size(ImageDataType type);
		unsigned data_type_channel_count(ImageDataType type);
		unsigned pixel_format_channel_count(ImagePixelFormat type);

		// This type is guaranteed to be big enough to hold even RGBA16.
		// This is useful when you want a generic representation of a pixel
		typedef uint64_t PixelT;
		typedef std::size_t DimentionT;
		typedef Vector<3, std::size_t> PixelCoordinateT;

		class IPixelBuffer : implements IObject {
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

			std::size_t pixel_data_length () const { return size().product() * bytes_per_pixel(); }

			// Returns the equivalent pixel type, ie GL_UNSIGNED_INT_8_8_8_8 -> GL_UNSIGNED_INT
			ImageDataType packed_type () const;
			unsigned channel_count () const;

			bool is_packed_format () const;
			bool is_byte_order_reversed () const;

			std::size_t bytes_per_pixel () const;

			// Helper: Returns the maximum value of an individual pixel
			PixelT max_pixel_size () const {
				return ((PixelT)1 << (bytes_per_pixel() * 8)) - 1;
			}

			virtual PixelCoordinateT size () const abstract;

			// Data accessors
			virtual const ByteT * pixel_data () const abstract;

			// Get the number of bytes to index into pixel_data
			std::size_t pixel_offset (const PixelCoordinateT & at) const {
				std::size_t offset = at[X] + (at[Y] * size()[X]) + (at[Z] * size()[X] * size()[Y]);
				return offset * bytes_per_pixel();
			}

			// pixbuf->at(vec<unsigned>(43, 12));
			const ByteT* pixel_data_at (const PixelCoordinateT &at) const {
				return pixel_data() + pixel_offset(at);
			}

			// Helper to read pixel data
			// This may convert between the pixbuf format and the output pixel
			// void read_pixel (const PixelCoordinateT &at, Vector<4, float> &output) const;
			template <typename DataT>
			void read_data_at (const std::size_t &idx, DataT &val) const {
				const ByteT *data = pixel_data();

				val = *(DataT*)(&data[idx]);
			}

			// This does not do _any_ sanity checking what-so-ever.
			template <unsigned D, typename NumericT>
			void read_pixel (const PixelCoordinateT &at, Vector<D, NumericT> &output) const {
				DREAM_ASSERT(!is_packed_format() && "Packed pixel formats not supported for reading!");

				std::size_t from = pixel_offset(at);
				std::size_t bytes_per_component = bytes_per_pixel() / channel_count();

				for (std::size_t i = 0; i < D; i += 1) {
					read_data_at(from + (i * bytes_per_component), output[i]);
				}
			}

			PixelT read_pixel (const PixelCoordinateT &at);
		};

		enum CopyFlags {
			CopyNormal = 0,
			CopyFlip = 1
		};

		// Some pixel buffers (for example the screen buffer), can never have mutable operations without
		// high cost, so there is a sub-interface for 'typically' mutable pixbufs.
		class IMutablePixelBuffer : implements IPixelBuffer {
		public:
			virtual ByteT * pixel_data () abstract;
			using IPixelBuffer::pixel_data;

			void zero (PixelT px = 0);

			ByteT* pixel_data_at (const PixelCoordinateT &at) {
				return pixel_data() + pixel_offset(at);
			}

			template <typename DataT>
			void write_data_at (const std::size_t &idx, const DataT &val)
			{
				ByteT *data = pixel_data();

				*(DataT*)(&data[idx]) = val;
			}

			/// Set all bytes to zero.
			void clear ();

			// void write_pixel (const PixelCoordinateT &at, const Vector<4, float> &input);
			template <unsigned D, typename NumericT>
			void write_pixel (const PixelCoordinateT &at, const Vector<D, NumericT> &input) {
				DREAM_ASSERT(!is_packed_format() && "Packed pixel formats not supported for reading!");

				std::size_t from = pixel_offset(at);
				std::size_t bytes_per_component = bytes_per_pixel() / channel_count();

				DREAM_ASSERT(sizeof(NumericT) == bytes_per_component);

				for (std::size_t i = 0; i < D; i += 1) {
					write_data_at(from + (i * bytes_per_component), input[i]);
				}
			}

			void write_pixel (const PixelCoordinateT &at, const PixelT &px);

			// Copy from buf to this
			void copy_pixels_from(const IPixelBuffer& buf, const PixelCoordinateT &from, const PixelCoordinateT &to, const PixelCoordinateT &size, CopyFlags copy_flags = CopyNormal);

			void copy_pixels_from(const IPixelBuffer& buf, const PixelCoordinateT &to, CopyFlags copy_flags = CopyNormal) {
				copy_pixels_from(buf, Vec3u(0, 0, 0), to, buf.size(), copy_flags);
			}
		};
	}
}

#endif
