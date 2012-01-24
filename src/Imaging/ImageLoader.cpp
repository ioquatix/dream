//
//  Imaging/ImageLoader.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 1/05/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "Image.h"
#include "../Core/Data.h"
#include "../Core/Timer.h"
#include "../Events/Logger.h"

extern "C" {
#include <png.h>
#include <jpeglib.h>
}

#include <exception>
#include <stdexcept>
#include <sstream>

namespace Dream {
	namespace Imaging {
		
		using namespace Events::Logging;
		
		struct DataFile {
			Shared<Buffer> buffer;
			unsigned offset;
			
			DataFile (const Ref<IData> data) {
				buffer = data->buffer();
				offset = 0;
			}
			
			const char * read_bytes (unsigned length) {
				unsigned prev_offset = offset;
				offset += length;
				return (const char*)(buffer->begin() + prev_offset);
			}
			
			static void png_read_data (png_structp png_reader, png_bytep data, png_size_t length) {
				DataFile *data_file = (DataFile *) png_get_io_ptr (png_reader);
				memcpy(data, data_file->read_bytes(length), length);
			}
		};

	#pragma mark -
	#pragma mark JPEG Loader Code

		void jpeg_dummy (j_decompress_ptr cinfo) {

		}

		boolean jpeg_fill_input_buffer (j_decompress_ptr cinfo) {
			// Force the jpeg library to stop reading if we run past the end of data
			static JOCTET eoi[2] = {(JOCTET)0xff, (JOCTET)JPEG_EOI};
			cinfo->src->next_input_byte = eoi;
			cinfo->src->bytes_in_buffer = 2;
			
			return false;
		}

		/*
		 * Skip data --- used to skip over a potentially large amount of
		 * uninteresting data (such as an APPn marker).
		 */

		void jpeg_skip_input_data (j_decompress_ptr cinfo, long num_bytes) {
			//ensure(num_bytes < cinfo->src.bytes_in_buffer && "jpeglib tried to skip further than the end of file!!");
			
			if (num_bytes > 0 && num_bytes < (long)cinfo->src->bytes_in_buffer) {
				cinfo->src->next_input_byte += (IndexT) num_bytes;
				cinfo->src->bytes_in_buffer -= (IndexT) num_bytes;
			} else {
				jpeg_fill_input_buffer(cinfo);
			}
		}

		void jpeg_memory_src (j_decompress_ptr cinfo, const JOCTET * buffer, IndexT bufsize) {
			if (cinfo->src == NULL) {
				cinfo->src = (jpeg_source_mgr *) (*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_PERMANENT, sizeof(jpeg_source_mgr));
			}
			
			cinfo->src->init_source = jpeg_dummy;
			
			cinfo->src->fill_input_buffer = jpeg_fill_input_buffer;
			cinfo->src->skip_input_data = jpeg_skip_input_data;
			
			cinfo->src->resync_to_restart = jpeg_resync_to_restart; // default
			cinfo->src->term_source = jpeg_dummy;

			cinfo->src->next_input_byte = buffer;
			cinfo->src->bytes_in_buffer = bufsize;
		}

		Ref<Image> load_jpegimage (const Ptr<IData> data) {
			jpeg_decompress_struct cinfo;
			jpeg_error_mgr jerr;
			
			ImagePixelFormat format = ImagePixelFormat(0);
			ImageDataType data_type = UBYTE;

			Ref<Image> result_image;
			Shared<Buffer> buffer = data->buffer();
			
			unsigned width, height;
			
			try {
				//memset (&cinfo, 0, sizeof (cinfo));
				memset (&jerr, 0, sizeof (jerr));
				
				cinfo.err = jpeg_std_error(&jerr);
			
				jpeg_create_decompress(&cinfo);
				jpeg_memory_src(&cinfo, buffer->begin(), buffer->size());

				jpeg_read_header(&cinfo, TRUE); 

				width = cinfo.image_width;
				height = cinfo.image_height;
				
				unsigned row_width = 0;
				if (cinfo.jpeg_color_space == JCS_GRAYSCALE) {
					row_width = width; 
					format = LUMINANCE; 
				} else {
					row_width = 3 * width;
					format = RGB; 
				}
				
				result_image = new Image(Vector<3, unsigned>(width, height, 1), format, data_type);
				
				ByteT *line = result_image->pixel_data();
				jpeg_start_decompress(&cinfo); 

				// read jpeg image 
				while (cinfo.output_scanline < cinfo.output_height) { 
					jpeg_read_scanlines(&cinfo, &line, 1); 
					line += row_width;
				}

				jpeg_finish_decompress(&cinfo); 
				jpeg_destroy_decompress(&cinfo); 
			} catch (std::exception &e) {
				
			}
			
			return result_image;
		};

	#pragma mark -
	#pragma mark PNG Loader Code
		static void png_error (png_structp png_reader, png_const_charp msg) {
			throw std::runtime_error(msg);
		}
		
		Ref<Image> load_pngimage (const Ptr<IData> data) {
			// Image formatting details
			ImagePixelFormat format = ImagePixelFormat(0);
			ImageDataType data_type = ImageDataType(0);
			
			DataFile df(data);
			Ref<Image> result_image;
			Shared<Buffer> buffer = data->buffer();
			
			// internally used by libpng
			png_structp png_reader = NULL;
			// user requested transforms
			png_infop png_info = NULL;
			
			png_byte **ppb_row_pointers = NULL;
			
			if (!png_check_sig((png_byte*)buffer->begin(), 8)) {
				logger()->log(LOG_ERROR, "Could not verify PNG image!");
				return Ref<Image>();
			}
			
			try {
				png_reader = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, png_error, NULL);
				ensure(png_reader != NULL && "png_create_read_struct returned NULL!");
				
				png_info = png_create_info_struct(png_reader);
				ensure(png_info != NULL && "png_create_info_struct returned NULL!");
				
				// We will use this function to read data from Data class
				png_set_read_fn (png_reader, (void *)&df, DataFile::png_read_data);
				
				// Read PNG header
				png_read_info (png_reader, png_info);
				
				// Interpret IMAGE header
				int bit_depth, color_type;
				png_uint_32 width, height;
				
				png_get_IHDR (png_reader, png_info, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
				
				if (bit_depth < 8) {
					png_set_packing (png_reader);
					//png_set_expand(png_reader);
				}
				
				if (color_type == PNG_COLOR_TYPE_PALETTE) {
					png_set_expand(png_reader);
				}
				
				// after the transformations have been registered update png_info data
				png_read_update_info(png_reader, png_info);
				png_get_IHDR(png_reader, png_info, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
				
				// Figure out image format
				if (color_type == PNG_COLOR_TYPE_RGB) {
					format = RGB;
				} else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
					format = RGBA;
				} else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
					format = LUMINANCE_ALPHA;
				} else if (color_type == PNG_COLOR_TYPE_GRAY) {
					format = LUMINANCE;
				}
				
				// Figure out bit depth
				if (bit_depth == 16) {
					// It is possible to convert to 8bpp using: png_set_strip_16 (png_reader);
					data_type = USHORT;
				} else if (bit_depth == 8) {
					data_type = UBYTE;
				} else {
					std::stringstream s; s << "PNG: Bit depth of " << bit_depth << " not supported!" << std::endl;
					throw std::runtime_error(s.str());
				}
				
				// row_bytes is the width x number of channels
				unsigned row_bytes = png_get_rowbytes(png_reader, png_info);

				// Allocate the image_data buffer.
				result_image = new Image(Vector<3, unsigned>(width, height, 1), format, data_type);
				ByteT *image_bytes = result_image->pixel_data();
				ensure(image_bytes != NULL);
				
				//ByteT * image_data = (unsigned char *) malloc(rowbytes * height);
				ppb_row_pointers = (png_bytepp)malloc(height * sizeof(png_bytep));
				for (unsigned i = 0; i < height; i++)
					ppb_row_pointers[i] = image_bytes + i * row_bytes;
				
				png_read_image(png_reader, ppb_row_pointers);
				png_read_end(png_reader, NULL);
				
				free(ppb_row_pointers);
				
				if (png_reader) png_destroy_read_struct(&png_reader, &png_info, NULL);
			} catch (std::exception &e) {
				logger()->log(LOG_ERROR, LogBuffer() << "PNG read error: " << e.what());
				
				if (png_reader) png_destroy_read_struct(&png_reader, &png_info, NULL);
				png_reader = NULL;
				
				if (ppb_row_pointers) free(ppb_row_pointers);
				
				throw;
			}
			
			return result_image;
		}

#pragma mark -
#pragma mark Loader Multiplexer
		
		Ref<Image> Image::load_from_data (const Ptr<IData> data) {
			static Stopwatch t;
			static unsigned count = 0; ++count;
			
			Ref<Image> loaded_image;
			Shared<Buffer> buffer;

			t.start();
			
			buffer = data->buffer();
			
			switch (buffer->mimetype()) {
				case IMAGE_JPEG:
					loaded_image = load_jpegimage(data);
					break;
				case IMAGE_PNG:
					loaded_image = load_pngimage(data);
					break;
				//case Data::IMAGE_DDS:
				//	loaded_image = load_ddsimage(data);
				default:
					logger()->log(LOG_ERROR, "Could not load image: Unsupported image format.");
			}
			
			t.pause();
			
			logger()->log(LOG_INFO, LogBuffer() << "*** Total time to load " << count << " images: " << t.time() << "s");
			
			return loaded_image;
		}
	}	
}
