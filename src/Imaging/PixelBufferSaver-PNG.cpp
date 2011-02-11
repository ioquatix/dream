/*
 *  Imaging/PixelBufferSaver-PNG.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 15/05/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "PixelBufferSaver.h"
#include "Image.h"

#include <stdexcept>

extern "C" {
#include <png.h>
#include <jpeglib.h>
}

namespace Dream
{
	namespace Imaging
	{
		static void pngWriteToBuffer (png_structp pngWriter, png_bytep data, png_size_t length)
		{
			DynamicBuffer * buffer = (DynamicBuffer*)png_get_io_ptr(pngWriter);
			
			buffer->append(length, data);
		}
		
		static void pngFlushBuffer (png_structp read_ptr)
		{
			
		}
		
		static void pngError(png_structp png_ptr, png_const_charp msg) {
			throw std::runtime_error(msg);
		}

		int pngColorType (ImagePixelFormat pixelFormat)
		{
			switch (pixelFormat) {
			case RED:
			case GREEN:
			case BLUE:
			case LUMINANCE:
			case ALPHA:
				return PNG_COLOR_TYPE_GRAY;
			case LUMINANCE_ALPHA:
				return PNG_COLOR_TYPE_GRAY_ALPHA;
			case RGB:
				return PNG_COLOR_TYPE_RGB;
			case RGBA:
				return PNG_COLOR_TYPE_RGBA;
			}
			
			return -1;
		}
		
		REF(Core::IData) savePixelBufferAsPNG (IPixelBuffer * pixelBuffer)
		{
			Vec3u size = pixelBuffer->size();
		
			ensure(!pixelBuffer->isPackedFormat());
			ensure(size[Z] == 1);
			
			Shared<DynamicBuffer> resultData(new DynamicBuffer);
			
			png_structp pngWriter = NULL;
			png_infop pngInfo = NULL;
			std::vector<png_bytep> rows(size[HEIGHT]);
			
			for (IndexT y = 0; y < size[HEIGHT]; y += 1) {
				rows[y] = (png_bytep)pixelBuffer->pixelDataAt(Vec3u(0, y, 0));
			}
			
			try {
				pngWriter = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, pngError, NULL);
				ensure(pngWriter != NULL && "png_create_write_struct returned NULL!");
				
				pngInfo = png_create_info_struct(pngWriter);
				ensure(pngInfo != NULL && "png_create_info_struct returned NULL!");
				
				png_set_write_fn(pngWriter, (void *)(resultData.get()), pngWriteToBuffer, pngFlushBuffer);
				
				int bitDepth = (pixelBuffer->bytesPerPixel() * 8) / pixelBuffer->channelCount();
				int colorType = pngColorType(pixelBuffer->pixelFormat());

				png_set_IHDR(pngWriter, pngInfo, size[WIDTH], size[HEIGHT], bitDepth, colorType, NULL, NULL, NULL);
				
				png_write_info(pngWriter, pngInfo);
				
				png_write_image(pngWriter, (png_bytepp)&rows[0]);
				
				// After you are finished writing the image, you should finish writing the file.
				png_write_end(pngWriter, NULL);
			} catch (std::exception & e) {
				std::cerr << "PNG write error: " << e.what() << std::endl;
			
				if (pngWriter)
					png_destroy_write_struct(&pngWriter, &pngInfo);
				
				throw;
			}
			
			png_destroy_write_struct(&pngWriter, &pngInfo);
			
			return new BufferedData(resultData);
		}
		
		
#pragma mark -
#pragma mark Unit Tests
		
#ifdef ENABLE_TESTING

		UNIT_TEST(PixelBufferSaver) {
			REF(IPixelBuffer) pixelBuffer = new Image(vec(100.0, 100.0, 1.0), RGB, UBYTE);
			testing("PNG");
			
			REF(IData) pngData = savePixelBufferAsPNG(pixelBuffer.get());
			
			pngData->buffer()->writeToFile("UnitTest.png");
		}
		
#endif
		
	}
}
