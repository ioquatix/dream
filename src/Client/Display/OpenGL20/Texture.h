/*
 *  Client/Display/OpenGL20/Texture.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 20/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_OPENGL20_TEXTURE_H
#define _DREAM_CLIENT_DISPLAY_OPENGL20_TEXTURE_H

// This is a private header, and should not be used as public API.

#include "OpenGL20.h"
#include "../../../Imaging/PixelBuffer.h"

#include <vector>

namespace Dream {
	namespace Client {
		namespace Display {
			namespace OpenGL20 {				
				using Dream::Imaging::IPixelBuffer;
				
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
				
				class TextureController;
								
				class Texture : public Object
				{
				protected:
					TextureParameters m_params;
					GLuint m_id;
					GLenum m_target;
					GLenum m_internalFormat;
					
					Vec3u m_size;
					
					friend class TextureController;

					TextureController * m_textureController;
					
					Texture (TextureController *);
					
					void loadPixelData (const Vector<3, unsigned> & size, const ByteT * pixels, GLenum format, GLenum dataType, 
										const TextureParameters & params);
					
				public:
					virtual ~Texture ();
					
					/// Return the size of the internal texture buffer.
					Vec3u size () const;
					
					/// Resize the texture. This will invalidate any image data, which will need to be resupplied by calling updatePixelData.
					/// Please note that you cannot change the size to something that is unsupported by the current target - i.e. any excess components wlll be
					/// ignored.
					void resize (Vec3u);
					
					/// Composite the supplied pixelBuffer into the texture at the given offset
					/// The supplied pixelBuffer must be of the same format as that of the texture.
					void updatePixelData (PTR(IPixelBuffer) pixelBuffer, const Vec3u & offset);
					
					GLenum target () const { return m_target; }
					GLuint id () const { return m_id; }
					const TextureParameters & parameters () const { return m_params; }
				};
				
				class TextureUnit;
				
				class TextureController
				{					
				protected:
					std::vector<TextureUnit> m_textureUnits;
					IndexT m_basicUnitCount;
					IndexT m_imageUnitCount;
										
				public:
					TextureController ();
					
					void updateTextures (const std::vector<REF(Texture)> & textures);
					
					/// Disables all texture units
					void reset ();
					
					IndexT basicUnits () const;
					IndexT imageUnits () const;
					
					IndexT count () const;
					TextureUnit * unit (IndexT index);
					
					REF(Texture) createTexture (PTR(IPixelBuffer) pixelBuffer, const TextureParameters & params);
				};
				
				class TextureUnit
				{
				protected:
					GLenum m_textureUnit;
					GLenum m_currentTarget;
					GLuint m_currentTexture;
					
					TextureController * m_textureController;
					
				public:
					TextureUnit (GLenum textureUnit, TextureController * textureController);
					
					void enable (const PTR(Texture) texture);
					void disable ();
				};
			}
		}
	}
}

#endif
