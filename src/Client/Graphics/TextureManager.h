//
//  Client/Graphics/TextureManager.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 20/04/09.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#ifndef _DREAM_CLIENT_GRAPHICS_TEXTUREMANAGER_H
#define _DREAM_CLIENT_GRAPHICS_TEXTUREMANAGER_H

#include "../../Imaging/PixelBuffer.h"
#include "Graphics.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			using Dream::Imaging::IPixelBuffer;
			
			const char * targetName (GLenum target);
			bool isValidTextureTarget (GLenum target);
			
			struct TextureBinding {
				GLenum index;
				GLenum unit;
			};
			
			typedef std::vector<TextureBinding> TextureBindingsT;
			
			/**
			 A set of parameters which determines the way a texture is loaded and used.
			 
			 */
			struct TextureParameters
			{					
				/// Whether or not mip-maps are used
				bool generateMipMaps;
				
				/// The minification filter. This determins how a texture is sampled when a screen-space pixel covers more than one texture-space pixel.
				GLenum minFilter;
				
				/// The magnification filter. This determins how a texture is sampled when a texture pixel covers more than one screen space pixel.
				GLenum magFilter;
				
				/// The texture target.
				GLenum target;
				
				/// The internal format for the texture. Optional (set to 0)
				GLenum internalFormat;
				
				/// Construct a TextureParameters object to use default values unless specified.
				/// @todo Maybe make constructor for TextureParameters have an argument
				/// i.e. TextureParameters tp(LOW_QUALITY || HIGH_QUALITY || TEXT_QUALITY) etc
				TextureParameters ()
				: generateMipMaps(true), minFilter(0), magFilter(0), target(0), internalFormat(0)
				{
				}
				
				/// Returns the specified minification filter or the default if none was specified.
				GLenum getMinFilter () const;
				/// Returns the specified magnification filter or the default if none was specified.
				GLenum getMagFilter () const;
				
				/// Returns the target if specified or defaultTarget if none was given.
				GLenum getTarget(GLenum defaultTarget) const;
				
				GLenum getInternalFormat (GLenum defaultInternalFormat) const;
			};
			
			// A texture manager retains ownership over a set of indexed textures.
			// If the texture manager is freed, the textures are freed with it.
			class TextureManager : public Object {
			protected:
				struct Record {
					GLuint handle;
					
					TextureParameters parameters;
					
					Vec3u currentSize;
					GLenum currentFormat;
					GLenum currentDataType;
				};
				
				std::vector<Record> m_textures;
				
				IndexT m_imageUnitCount;
				
				struct State {
					GLuint texture;	
				};
				
				std::vector<State> m_current;
				
				void loadPixelData(IndexT index, const Vector<3, unsigned> & size, const ByteT * pixels, GLenum format, GLenum dataType);
				
			public:
				TextureManager();
				~TextureManager();
				
				/// Creates a texture with the given pixelBuffer.
				IndexT create(const TextureParameters &, PTR(IPixelBuffer) pixelBuffer);
				
				/// Also update the texture parameters.
				void update(IndexT index, PTR(IPixelBuffer) pixelBuffer, const TextureParameters &);
				
				/// Use the pre-existing parameters for the texture data.
				void update(IndexT index, PTR(IPixelBuffer) pixelBuffer);
				
				/// Resize the texture, which invalidates any pixel data contained.
				void resize(IndexT index, Vec3u size, GLenum format, GLenum dataType);
				
				/// Binds the texture index with the given texture unit.
				void bind(IndexT index, IndexT unit);
				
				/// Bind a set of textures as specified.
				void bind(const TextureBindingsT & bindings);
				
				/// If you are using multiple texture managers, you should call this to reset internal state tracking.
				void invalidate ();
			};
			
		}
	}
}

#endif
