//
//  Client/Graphics/TextureManager.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 4/12/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#include "TextureManager.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			const GLenum INVALID_TARGET = 0;
			const GLuint INVALID_TEXTURE = (GLuint)-1;
			
			const char * target_name (GLenum target)
			{
				switch (target) {
#ifdef GL_TEXTURE_1D
					case GL_TEXTURE_1D:
						return "GL_TEXTURE_1D";
#endif
					case GL_TEXTURE_2D:
						return "GL_TEXTURE_2D";
#ifdef GL_TEXTURE_3D
					case GL_TEXTURE_3D:
						return "GL_TEXTURE_3D";
#endif
					default:
						return "Unknown Target";
				}
			}
			
			bool is_valid_texture_target (GLenum target)
			{
				switch (target) {
#ifdef GL_TEXTURE_1D
					case GL_TEXTURE_1D:
#endif
					case GL_TEXTURE_2D:
#ifdef GL_TEXTURE_3D
					case GL_TEXTURE_3D:
#endif
						return true;
					default:
						return false;
				}
			}
			
#pragma mark -

			GLenum TextureParameters::get_min_filter () const {
				if (min_filter)
					return min_filter;
				else if (generate_mip_maps)
					return GL_LINEAR_MIPMAP_LINEAR;
				else
					return GL_LINEAR;
			}

			GLenum TextureParameters::get_mag_filter () const {
				if (mag_filter)
					return mag_filter;
				else
					return GL_LINEAR;
			}

			GLenum TextureParameters::get_target (GLenum default_target) const {
				if (target)
					return target;
				else
					return default_target;
			}

			GLenum TextureParameters::get_internal_format (GLenum default_internal_format) const {
				if (internal_format)
					return internal_format;
				else
					return default_internal_format;
			}
			
#pragma mark -
			
			TextureManager::TextureManager()
			{
				GLint image_unit_count = 0;
				glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &image_unit_count);
								
				State initial = {INVALID_TEXTURE};
				_current.resize(image_unit_count, initial);
								
				_imageUnitCount = image_unit_count;
				
				std::cerr << "OpenGL Texture Units: " << image_unit_count << std::endl;
			}
			
			TextureManager::~TextureManager()
			{
				for (Record & r : _textures) {
					glDeleteTextures(1, &r.handle);
				}
			}

			IndexT TextureManager::create(const TextureParameters & parameters, Ptr<IPixelBuffer> pixel_buffer)
			{
				GLenum handle;
				
				glGenTextures(1, &handle);
				
				Record record = {handle, parameters};
				
				_textures.push_back(record);
				IndexT index = _textures.size() - 1;
				
				if (pixel_buffer) {
					update(index, pixel_buffer);
				}
				
				return index;
			}
			
			void TextureManager::load_pixel_data (IndexT index, const Vector<3, unsigned> & size, const ByteT * pixels, GLenum format, GLenum data_type)
			{
				const TextureParameters & parameters = _textures[index].parameters;
				
				// We will use unit 0 to upload the texture data.
				bind(index, 0);
				
				/// Invalidate the first texture unit since we may use it to upload texture data.
				_current[0].texture = INVALID_TEXTURE;
				
				GLenum internal_format = parameters.get_internal_format(format);
				
				switch (parameters.target) {
#ifdef GL_TEXTURE_1D
					case GL_TEXTURE_1D:
						glTexImage1D(parameters.target, 0, internal_format, size[WIDTH], 0, format, data_type, pixels);
						break;
#endif
					case GL_TEXTURE_2D:
						glTexImage2D(parameters.target, 0, internal_format, size[WIDTH], size[HEIGHT], 0, format, data_type, pixels);
						break;
#ifdef GL_TEXTURE_3D
					case GL_TEXTURE_3D:
						glTexImage3D(parameters.target, 0, internal_format, size[WIDTH], size[HEIGHT], size[DEPTH], 0, format, data_type, pixels);
						break;
#endif
				}
				
				_textures[index].current_size = size;
				_textures[index].current_format = format;
				_textures[index].current_data_type = data_type;
				
				if (parameters.generate_mip_maps)
					glGenerateMipmap(parameters.target);
			}
			
			void TextureManager::update(IndexT index, Ptr<IPixelBuffer> pixel_buffer, const TextureParameters & parameters)
			{
				_textures[index].parameters = parameters;
				
				load_pixel_data(index, pixel_buffer->size(), pixel_buffer->pixel_data(), pixel_buffer->pixel_format(), pixel_buffer->pixel_dataType());
			}
			
			void TextureManager::update(IndexT index, Ptr<IPixelBuffer> pixel_buffer)
			{
				load_pixel_data(index, pixel_buffer->size(), pixel_buffer->pixel_data(), pixel_buffer->pixel_format(), pixel_buffer->pixel_dataType());
			}
			
			void TextureManager::resize(IndexT index, Vec3u size, GLenum format, GLenum data_type)
			{
				if (size != _textures[index].current_size)
					load_pixel_data(index, size, NULL, _textures[index].current_format, _textures[index].current_data_type);
			}
			
			void TextureManager::bind(IndexT index, IndexT unit)
			{
				ensure(index < _textures.size());
				
				const Record & record = _textures[index];
				State & state = _current[unit];
				
				if (state.texture != record.handle) {
					glActiveTexture(GL_TEXTURE0 + unit);
					
					GLenum target = record.parameters.target;
					glBindTexture(target, record.handle);
					
					glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
					glTexParameteri(target, GL_TEXTURE_MAG_FILTER, record.parameters.get_mag_filter());
					glTexParameteri(target, GL_TEXTURE_MIN_FILTER, record.parameters.get_min_filter());
				}
			}
			
			void TextureManager::bind(const TextureBindingsT & bindings)
			{
				for (auto i : bindings) {
					bind(i.index, i.unit);
				}
			}
			
			void TextureManager::invalidate()
			{
				for (State & s : _current) {
					s.texture = INVALID_TEXTURE;
				}
			}
			
		}
	}
}
