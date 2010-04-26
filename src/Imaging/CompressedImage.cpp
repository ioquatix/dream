/*
 *  Imaging/CompressedImage.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 28/04/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#include "CompressedImage.h"

namespace Dream {
	namespace Imaging {
	
		CompressedImage::loadTexture(const Texture & tex) {
			int nHeight     = pDDSImageData->height;
			int nWidth      = pDDSImageData->width;
			int nNumMipMaps = pDDSImageData->numMipMaps;

			int nBlockSize;

			if( pDDSImageData->format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT )
				nBlockSize = 8;
			else
				nBlockSize = 16;

			glGenTextures( 1, &g_compressedTextureID );
			glBindTexture( GL_TEXTURE_2D, g_compressedTextureID );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			int nSize;
			int nOffset = 0;

			// Load the mip-map levels

			for( int i = 0; i < nNumMipMaps; ++i )
			{
				if( nWidth  == 0 ) nWidth  = 1;
				if( nHeight == 0 ) nHeight = 1;

				nSize = ((nWidth+3)/4) * ((nHeight+3)/4) * nBlockSize;

				glCompressedTexImage2DARB( GL_TEXTURE_2D,
										   i,
										   pDDSImageData->format,
										   nWidth,
										   nHeight,
										   0,
										   nSize,
										   pDDSImageData->pixels + nOffset );
				
				glTexImage2D          (<#GLenum target#>, <#GLint level#>, <#GLenum internalformat#>, <#GLsizei width#>, <#GLsizei height#>, <#GLint border#>, <#GLenum format#>, <#GLenum type#>, <#const GLvoid * pixels#>)
				glCompressedTexImage2D(<#GLenum target#>, <#GLint level#>, <#GLenum internalformat#>, <#GLsizei width#>, <#GLsizei height#>, <#GLint border#>, <#GLsizei imageSize#>, <#const GLvoid * data#>)

				nOffset += nSize;

				// Half the image size for the next mip-map level...
				nWidth  = (nWidth  / 2);
				nHeight = (nHeight / 2);
			}
		}

		if( pDDSImageData != NULL )
		{
			if( pDDSImageData->pixels != NULL )
				free( pDDSImageData->pixels );

			free( pDDSImageData );
		}
	}

	#pragma mark -
	#pragma mark DDS Image Loader
		
		struct DDColorKey {
			uint32_t dw1;
			uint32_t dw2;
		} PACKED;
		
		struct DDPixelFormat {
			uint32_t dwSize;
			uint32_t dwFlags;
			uint32_t dwFourCC;
			union {
				uint32_t dwRGBBitCount;
				uint32_t dwYUVBitCount;
				uint32_t dwAlphaBitDepth;
			};
			union {
				uint32_t dwRBitMask;
				uint32_t dwYBitMask;
			};
			union {
				uint32_t dwGBitMask;
				uint32_t dwUBitMask;
			};
			union {
				uint32_t dwBBitMask;
				uint32_t dwVBitMask;
			};
			union {
				uint32_t dwRGBAlphaBitMask;
			};
		} PACKED;
		
		struct DDSCaps {
			uint32_t dwCaps1;
			uint32_t dwCaps2;
			uint32_t reserved[2];
		} PACKED;
		
		struct DDSSurfaceDesc {
			ByteT magic[4];
			uint32_t dwSize;
			uint32_t dwFlags;
			uint32_t dwHeight;
			uint32_t dwWidth;
			uint32_t dwPitchOrLinearSize;
			uint32_t dwDepth;
			uint32_t dwMipMapCount;
			uint32_t dwReserved1[11];
			DDPixelFormat ddpfPixelFormat;
			DDSCaps ddsCaps;
			uint32_t dwReserved2;
		} PACKED;

		REF(CompressedImage) loadDDSImage (const REF(Data) data) {
			const ByteT * start = data->start();
			DDSSurfaceDesc * desc = (DDSSurfaceDesc*)start;
			
			// Image formatting details
			ImagePixelFormat format = ImagePixelFormat(0);
			ImageDataType dataType = ImageDataType(0);
			unsigned factor = 1;
			
			REF(Image) resultImage;		
			
			switch (desc->ddpfPixelFormat.dwFourCC) {
			case 'DXT1':
				format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				factor = 2;
				break;
			case 'DXT3':
				format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				factor = 4;
				break;
			case 'DXT5':
				format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				factor = 4;
				break;
			default:
				std::stringstream s; << "DDS: Format type not supported: " << desc->ddpfPixelFormat.dwFourCC << std::endl;
				throw std::runtime_error(s.str());
			}
			
			if (desc->dwPitchOrLinearSize == 0) {
				throw std::runtime_error("DDS: Linear size is zero.. unsupported DDS format");
			}
			
			unsigned bufSize;
			if (desc->dwMipMapCount > 1) {
				bufSize = desc->dwPitchOrLinearSize * factor;
			} else {
				bufSize = desc->dwPitchOrLinearSize;
			}
			
			// Allocate the image_data buffer.
			resultImage = new Image(Vector<3, unsigned>(desc->dwWidth, desc->dwHeight, 1, format, dataType));
			ByteT *imageBytes = resultImage->pixelData();
			ensure(imageBytes != NULL);
			
			memcpy(imageBytes, start + sizeof(DDSSurfaceDesc), bufSize);
		}
	
	}
}