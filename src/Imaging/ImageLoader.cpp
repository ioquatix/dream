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

extern "C" {
#include <png.h>
#include <jpeglib.h>
}

#include <exception>
#include <stdexcept>
#include <sstream>

namespace Dream {
	namespace Imaging {
		
		struct DataFile {
			Shared<Buffer> buffer;
			unsigned offset;
			
			DataFile (const REF(IData) data) {
				buffer = data->buffer();
				offset = 0;
			}
			
			const char * readBytes (unsigned length) {
				unsigned prevOffset = offset;
				offset += length;
				return (const char*)(buffer->begin() + prevOffset);
			}
			
			static void pngReadData (png_structp pngReader, png_bytep data, png_size_t length) {
				DataFile *dataFile = (DataFile *) png_get_io_ptr (pngReader);
				memcpy(data, dataFile->readBytes(length), length);
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

		void jpeg_skip_input_data (j_decompress_ptr cinfo, long numBytes) {
			//ensure(numBytes < cinfo->src.bytes_in_buffer && "jpeglib tried to skip further than the end of file!!");
			
			if (numBytes > 0 && numBytes < (long)cinfo->src->bytes_in_buffer) {
				cinfo->src->next_input_byte += (IndexT) numBytes;
				cinfo->src->bytes_in_buffer -= (IndexT) numBytes;
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

		REF(Image) loadJPEGImage (const PTR(IData) data) {
			jpeg_decompress_struct cinfo;
			jpeg_error_mgr jerr;
			
			ImagePixelFormat format = ImagePixelFormat(0);
			ImageDataType dataType = UBYTE;

			REF(Image) resultImage;
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
				
				unsigned rowWidth = 0;
				if (cinfo.jpeg_color_space == JCS_GRAYSCALE) {
					rowWidth = width; 
					format = LUMINANCE; 
				} else {
					rowWidth = 3 * width;
					format = RGB; 
				}
				
				resultImage = new Image(Vector<3, unsigned>(width, height, 1), format, dataType);
				
				ByteT *line = resultImage->pixelData();
				jpeg_start_decompress(&cinfo); 

				// read jpeg image 
				while (cinfo.output_scanline < cinfo.output_height) { 
					jpeg_read_scanlines(&cinfo, &line, 1); 
					line += rowWidth;
				}

				jpeg_finish_decompress(&cinfo); 
				jpeg_destroy_decompress(&cinfo); 
			} catch (std::exception &e) {
				
			}
			
			return resultImage;
		};

	#pragma mark -
	#pragma mark PNG Loader Code
		static void pngError (png_structp pngReader, png_const_charp msg) {
			throw std::runtime_error(msg);
		}
		
		REF(Image) loadPNGImage (const PTR(IData) data) {
			// Image formatting details
			ImagePixelFormat format = ImagePixelFormat(0);
			ImageDataType dataType = ImageDataType(0);
			
			DataFile df(data);
			REF(Image) resultImage;
			Shared<Buffer> buffer = data->buffer();
			
			// internally used by libpng
			png_structp pngReader = NULL;
			// user requested transforms
			png_infop pngInfo = NULL;
			
			png_byte **ppbRowPointers = NULL;
			
			if (!png_check_sig((png_byte*)buffer->begin(), 8)) {
				std::cerr << "Could not verify PNG image!" << std::endl;
				return REF(Image)();
			}
			
			try {
				pngReader = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, pngError, NULL);
				ensure(pngReader != NULL && "png_create_read_struct returned NULL!");
				
				pngInfo = png_create_info_struct(pngReader);
				ensure(pngInfo != NULL && "png_create_info_struct returned NULL!");
				
				// We will use this function to read data from Data class
				png_set_read_fn (pngReader, (void *)&df, DataFile::pngReadData);
				
				// Read PNG header
				png_read_info (pngReader, pngInfo);
				
				// Interpret IMAGE header
				int bitDepth, colorType;
				png_uint_32 width, height;
				
				png_get_IHDR (pngReader, pngInfo, &width, &height, &bitDepth, &colorType, NULL, NULL, NULL);
				
				if (bitDepth < 8) {
					png_set_packing (pngReader);
					//png_set_expand(pngReader);
				}
				
				if (colorType == PNG_COLOR_TYPE_PALETTE) {
					png_set_expand(pngReader);
				}
				
				// after the transformations have been registered update pngInfo data
				png_read_update_info(pngReader, pngInfo);
				png_get_IHDR(pngReader, pngInfo, &width, &height, &bitDepth, &colorType, NULL, NULL, NULL);
				
				// Figure out image format
				if (colorType == PNG_COLOR_TYPE_RGB) {
					format = RGB;
				} else if (colorType == PNG_COLOR_TYPE_RGB_ALPHA) {
					format = RGBA;
				} else if (colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
					format = LUMINANCE_ALPHA;
				} else if (colorType == PNG_COLOR_TYPE_GRAY) {
					format = LUMINANCE;
				}
				
				// Figure out bit depth
				if (bitDepth == 16) {
					// It is possible to convert to 8bpp using: png_set_strip_16 (pngReader);
					dataType = USHORT;
				} else if (bitDepth == 8) {
					dataType = UBYTE;
				} else {
					std::stringstream s; s << "PNG: Bit depth of " << bitDepth << " not supported!" << std::endl;
					throw std::runtime_error(s.str());
				}
				
				// rowBytes is the width x number of channels
				unsigned rowBytes = png_get_rowbytes(pngReader, pngInfo);

				// Allocate the image_data buffer.
				resultImage = new Image(Vector<3, unsigned>(width, height, 1), format, dataType);
				ByteT *imageBytes = resultImage->pixelData();
				ensure(imageBytes != NULL);
				
				//ByteT * imageData = (unsigned char *) malloc(rowbytes * height);
				ppbRowPointers = (png_bytepp)malloc(height * sizeof(png_bytep));
				for (unsigned i = 0; i < height; i++)
					ppbRowPointers[i] = imageBytes + i * rowBytes;
				
				png_read_image(pngReader, ppbRowPointers);
				png_read_end(pngReader, NULL);
				
				free(ppbRowPointers);
				
				if (pngReader) png_destroy_read_struct(&pngReader, &pngInfo, NULL);
			} catch (std::exception &e) {
				std::cerr << "PNG read error: " << e.what() << std::endl;
				
				if (pngReader) png_destroy_read_struct(&pngReader, &pngInfo, NULL);
				pngReader = NULL;
				
				if (ppbRowPointers) free(ppbRowPointers);
				
				throw;
			}
			
			return resultImage;
		}

#pragma mark -
#pragma mark Loader Multiplexer
		
		REF(Image) Image::loadFromData (const PTR(IData) data) {
			static Stopwatch t;
			static unsigned count = 0; ++count;
			
			REF(Image) loadedImage;
			Shared<Buffer> buffer;

			t.start();
			
			buffer = data->buffer();
			
			switch (buffer->mimetype()) {
				case IMAGE_JPEG:
					loadedImage = loadJPEGImage(data);
					break;
				case IMAGE_PNG:
					loadedImage = loadPNGImage(data);
					break;
				//case Data::IMAGE_DDS:
				//	loadedImage = loadDDSImage(data);
				default:
					std::cerr << "Could not load image! Type not understood." << std::endl;
			}
			
			t.pause();
			
			std::cerr << "*** Total time to load " << count << " images: " << t.time() << "s" << std::endl;
			
			return loadedImage;
		}
	}	
}
