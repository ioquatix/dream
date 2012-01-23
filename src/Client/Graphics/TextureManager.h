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
			
			const char * target_name (GLenum target);
			bool is_valid_texture_target (GLenum target);
			
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
				bool generate_mip_maps;
				
				/// The minification filter. This determins how a texture is sampled when a screen-space pixel covers more than one texture-space pixel.
				GLenum min_filter;
				
				/// The magnification filter. This determins how a texture is sampled when a texture pixel covers more than one screen space pixel.
				GLenum mag_filter;
				
				/// The texture target.
				GLenum target;
				
				/// The internal format for the texture. Optional (set to 0)
				GLenum internal_format;
				
				/// Construct a TextureParameters object to use default values unless specified.
				/// @todo Maybe make constructor for TextureParameters have an argument
				/// i.e. TextureParameters tp(LOW_QUALITY || HIGH_QUALITY || TEXT_QUALITY) etc
				TextureParameters ()
				: generate_mip_maps(true), min_filter(0), mag_filter(0), target(0), internal_format(0)
				{
				}
				
				/// Returns the specified minification filter or the default if none was specified.
				GLenum get_min_filter () const;
				/// Returns the specified magnification filter or the default if none was specified.
				GLenum get_mag_filter () const;
				
				/// Returns the target if specified or default_target if none was given.
				GLenum get_target(GLenum default_target) const;
				
				GLenum get_internal_format (GLenum default_internal_format) const;
			};
			
			// A texture manager retains ownership over a set of indexed textures.
			// If the texture manager is freed, the textures are freed with it.
			class TextureManager : public Object {
			protected:
				struct Record {
					GLuint handle;
					
					TextureParameters parameters;
					
					Vec3u current_size;
					GLenum current_format;
					GLenum current_data_type;
				};
				
				std::vector<Record> _textures;
				
				IndexT _imageUnitCount;
				
				struct State {
					GLuint texture;	
				};
				
				std::vector<State> _current;
				
				void load_pixel_data(IndexT index, const Vector<3, unsigned> & size, const ByteT * pixels, GLenum format, GLenum data_type);
				
			public:
				TextureManager();
				~TextureManager();
				
				/// Creates a texture with the given pixel_buffer.
				IndexT create(const TextureParameters &, Ptr<IPixelBuffer> pixel_buffer);
				
				/// Also update the texture parameters.
				void update(IndexT index, Ptr<IPixelBuffer> pixel_buffer, const TextureParameters &);
				
				/// Use the pre-existing parameters for the texture data.
				void update(IndexT index, Ptr<IPixelBuffer> pixel_buffer);
				
				/// Resize the texture, which invalidates any pixel data contained.
				void resize(IndexT index, Vec3u size, GLenum format, GLenum data_type);
				
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
