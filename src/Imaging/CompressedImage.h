/*
 *  Imaging/CompressedImage.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 28/04/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_IMAGING_COMPRESSEDIMAGE_H
#define _DREAM_IMAGING_COMPRESSEDIMAGE_H

#include "../Framework.h"

namespace Dream {
	namespace Imaging {
			
		enum CompressedPixelFormat {
			// These are used for CompressedImage
			COMPRESSED_RGBA_S3TC_DXT1 = 0x83F1,
			COMPRESSED_RGBA_S3TC_DXT3 = 0x83F2,
			COMPRESSED_RGBA_S3TC_DXT5 = 0x83F3
		};
		
		class CompressedImage : public Object, IMPLEMENTS(TextureFactory) {
			EXPOSE_CLASS(CompressedImage)
			
			class Class : public Object::Class, IMPLEMENTS(Loadable::Class) {
				EXPOSE_CLASSTYPE
				
				virtual REF(Object) initFromData(const REF(Data) data, const ILoader * loader) {
					return CompressedImage::loadFromData (data);
				}
			};
		
		protected:
			ByteT * m_data;
			
		public:
			CompressedImage (CompressedImagePixelFormat fmt);
			
			virtual ~CompressedImage ();
			
		};
	}
}

#endif