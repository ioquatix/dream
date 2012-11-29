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

		/// Pixel component data type. All data types are assumed to be unsigned.
		enum class DataType : unsigned {
			BYTE = 0x0101,
			SHORT = 0x0202,
			INTEGER = 0x0304,
			FLOAT = 0x0404,
		};
		
		/// Image pixel formats.
		enum class PixelFormat : unsigned {
			// Single channel formats:
			R = 0x0101,
			G = 0x0201,
			B = 0x0301,
			A = 0x0401,

			// Full colour formats:
			RGB = 0x0503,
			RGBA = 0x0604,
			BGRA = 0x0704,

			// Luminance formats:
			L = 0x0801,
			LA = 0x0902,
		};

		std::size_t data_type_byte_size(DataType type);
		unsigned data_type_channel_count(DataType type);
		unsigned pixel_format_channel_count(PixelFormat type);

		// This type is guaranteed to be big enough to hold even RGBA16.
		// This is useful when you want a generic representation of a pixel
		typedef uint64_t PixelT;
		typedef std::size_t DimentionT;
		typedef Vector<3, std::size_t> PixelCoordinateT;

		class IPixelBuffer : implements IObject {
		public:
			virtual PixelFormat pixel_format () const abstract;
			virtual DataType pixel_data_type () const;

			std::size_t pixel_data_length () const { return size().product() * bytes_per_pixel(); }

			unsigned channel_count () const;

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
				copy_pixels_from(buf, ZERO, to, buf.size(), copy_flags);
			}
		};
	}
}

#endif
