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

#include <Euclid/Numerics/Vector.h>

namespace Dream {
	namespace Client {
		namespace Graphics {
			using Dream::Imaging::IPixelBuffer;
			using Euclid::Numerics::Vec3u;

			GLenum texture_pixel_format(Imaging::PixelFormat pixel_format);
			GLenum texture_data_type(Imaging::DataType data_type);

			const char * target_name (GLenum target);
			const char * format_name (GLenum format);

			bool is_valid_texture_target (GLenum target);

			/**
			 A set of parameters which determines the way a texture is loaded and used.

			 */
			struct TextureParameters {
				void set_defaults();

				/// Whether or not mip-maps are used
				bool generate_mip_maps;

				/// The minification filter. This determins how a texture is sampled when a screen-space pixel covers more than one texture-space pixel.
				GLenum min_filter;

				/// The magnification filter. This determins how a texture is sampled when a texture pixel covers more than one screen space pixel.
				GLenum mag_filter;

				/// Anisotropy level
				GLfloat anisotropy;

				/// The texture target.
				GLenum target;

				/// The internal format for the texture. Optional (set to 0)
				GLenum internal_format;

				/// Wrapping behaviour:
				GLenum wrap;

				/// Construct a TextureParameters object to use default values unless specified.
				/// @todo Maybe make constructor for TextureParameters have an argument
				/// i.e. TextureParameters tp(LOW_QUALITY || HIGH_QUALITY || TEXT_QUALITY) etc
				TextureParameters () {
					set_defaults();
				}

				/// Returns the specified minification filter or the default if none was specified.
				GLenum get_min_filter () const;
				/// Returns the specified magnification filter or the default if none was specified.
				GLenum get_mag_filter () const;

				/// Returns the target if specified or default_target if none was given.
				GLenum get_target(GLenum default_target = GL_TEXTURE_2D) const;

				GLenum get_internal_format (GLenum default_internal_format) const;

				enum Quality {
					NEAREST = 0,
					LINEAR = 1,
					MIPMAP = 2,
					FILTERED = 3,
				};

				TextureParameters(Quality quality);
			};

			class Texture;

			struct TextureBinding {
				std::size_t unit;
				Ref<Texture> texture;
			};

			typedef std::vector<TextureBinding> TextureBindingsT;

			// A texture manager is responsible for managing the current state of the texture units.
			class TextureManager : public Object {
			public:
				// A binding represents a bound texture which can then be manipulated:
				class Binding : private NonCopyable {
				protected:
					friend class TextureManager;

					Ptr<Texture> _texture;

					void set_texture(Ptr<Texture> texture) { _texture = texture; }

				public:
					/// Resize the texture, which invalidates any pixel data contained.
					void resize(const Vec3u & size, GLenum format, GLenum data_type);
					void resize(const Vec3u & size);

					/// Update the texture data. Use the pre-existing parameters for the texture data.
					void update(Ptr<IPixelBuffer> pixel_buffer);

					/// Update the texture data and associated parameters.
					void update(const TextureParameters & parameters, Ptr<IPixelBuffer> pixel_buffer);
				};

			protected:
				std::vector<GLuint> _handles;

				std::vector<Ptr<Texture>> _state;
				std::size_t _image_unit_count;
				Binding _binding;

			public:
				TextureManager();
				virtual ~TextureManager();

				/// Create a texture from the pool of available handles.
				Ref<Texture> allocate(const TextureParameters & parameters, Ptr<IPixelBuffer> pixel_buffer = NULL);

				/// Binds the texture index with the given texture unit.
				void bind(std::size_t unit, Ptr<Texture> texture);

				/// Bind a set of textures as specified.
				void bind(const TextureBindingsT & bindings);

				/// Bind the current texture for modification/upload.
				Binding & bind(Ptr<Texture> texture);

				/// If you are using multiple texture managers, you should call this to reset internal state tracking.
				void invalidate();
			};

			/// This class exists to manage the life-cycle of a texture which may be used in many different places.
			class Texture : public Object {
			protected:
				GLuint _handle;

				TextureParameters _parameters;

				Vec3u _size;
				GLenum _format;
				GLenum _data_type;

				friend class TextureManager::Binding;

				void load_pixel_data(const Vec3u & size, const ByteT * pixels, GLenum format, GLenum data_type);
				void set_parameters(const TextureParameters & parameters) { _parameters = parameters; }

			public:
				// The texture will take ownership of the handle:
				Texture(const TextureParameters & parameters, GLuint handle);

				// The texture will be allocated internally:
				Texture(const TextureParameters & parameters);

				// The texture will be released:
				virtual ~Texture();

				GLenum target() const { return _parameters.get_target(); }
				GLuint handle() const { return _handle; }

				const TextureParameters & parameters() const { return _parameters; }

				const Vec3u & size() const { return _size; }
				GLenum format() const { return _format; }
				GLenum data_type() const { return _data_type; }
			};
		}
	}
}

#endif
