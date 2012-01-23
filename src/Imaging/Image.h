//
//  Imaging/Image.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 30/04/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_IMAGING_IMAGE_H
#define _DREAM_IMAGING_IMAGE_H

#include "../Framework.h"
#include "PixelBuffer.h"
#include "../Resources/Loader.h"

namespace Dream {
	namespace Imaging {
		using namespace Resources;
		
		class ImageBase : implements IPixelBuffer {
		protected:
			ImagePixelFormat m_format;
			ImageDataType m_dataType;
			Vector<3, unsigned> m_size;
			
		public:			
			ImageBase () : m_format(ImagePixelFormat(0)), m_dataType(ImageDataType(0)), m_size(ZERO) {}
			
			//Accessors
			virtual ImagePixelFormat pixelFormat () const
			{
				return m_format;
			}
			
			virtual ImageDataType pixelDataType () const
			{
				return m_dataType;
			}
			
			virtual Vector<3, unsigned> size () const
			{ 
				return m_size;
			}
		};
		
		class UnbufferedImage : public Object, public ImageBase, implements IPixelBuffer {
		protected:
			const ByteT *m_data;
		
		public:			
			UnbufferedImage (ImagePixelFormat format, ImageDataType dataType);
			UnbufferedImage (const ByteT *data, const Vector<3, unsigned> &size, ImagePixelFormat format, ImageDataType dataType);
						
			void setFormat (ImagePixelFormat format, ImageDataType dataType);
			void setData (const ByteT *data, const Vector<3, unsigned> &size);
			
			virtual const ByteT* pixelData () const
			{
				return m_data;
			}
		};
		
		class Image : public Object, public ImageBase, implements IMutablePixelBuffer {
		public:
			class Loader : public Object, implements ILoadable {
			public:
				virtual void registerLoaderTypes (ILoader * loader);
				virtual REF(Object) loadFromData (const PTR(IData) data, const ILoader * loader);
			};
			
		protected:
			DynamicBuffer m_data;
					
		public:
			Image();
			
			Image(const Vector<3, unsigned> &size, ImagePixelFormat format, ImageDataType dataType);
			
			virtual ~Image ();
					
			// IMutablePixelBuffer access
			virtual const ByteT * pixelData () const;
			virtual ByteT * pixelData();
			
			/// Offset and Length are in pixels
			void setData (const Vector<3, unsigned> &size, ImagePixelFormat format, ImageDataType dataType, const ByteT * buffer);
			void setData (IndexT offset, IndexT length, const ByteT* buffer);
			
			/// Sets up the internal buffers for handing data for the specified size, format and dataType.
			void allocate (const Vec3u & size, ImagePixelFormat format, ImageDataType dataType);

		protected:
			static REF(Image) loadFromData (const PTR(IData) data);

		};
	}
}

#endif
