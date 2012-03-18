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
			
			void TextureParameters::set_defaults() {
				generate_mip_maps = true;
				min_filter = 0;
				mag_filter = 0;
				anisotropy = 1.0;
				target = 0;
				internal_format = 0;
			}

			TextureParameters::TextureParameters(Quality quality) {
				set_defaults();
				
				switch (quality) {
					case NEAREST:
						min_filter = GL_NEAREST;
						mag_filter = GL_NEAREST;
						break;
					
					case LINEAR:
						min_filter = GL_LINEAR;
						mag_filter = GL_LINEAR;
						break;
					
					case FILTERED:
						anisotropy = 4.0;
						
					case MIPMAP:
						min_filter = GL_LINEAR_MIPMAP_LINEAR;
						mag_filter = GL_LINEAR;
						generate_mip_maps = true;
						break;
						
					default:
						break;
				}
			}
			
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
			
			Texture::Texture(const TextureParameters & parameters, GLuint handle) : _handle(handle), _parameters(parameters) {
			}
			
			Texture::Texture(const TextureParameters & parameters) : _parameters(parameters) {
				glGenTextures(1, &_handle);
			}
			
			Texture::~Texture() {
				glDeleteTextures(1, &_handle);
			}
			
#pragma mark -
			
			void Texture::load_pixel_data(const Vector<3, unsigned> & size, const ByteT * pixels, GLenum format, GLenum data_type) {
				GLenum internal_format = _parameters.get_internal_format(format);
				GLenum target = _parameters.get_target();
				
				// Upload the texture data:
				switch (target) {
#ifdef GL_TEXTURE_1D
					case GL_TEXTURE_1D:
						glTexImage1D(target, 0, internal_format, size[WIDTH], 0, format, data_type, pixels);
						break;
#endif
					case GL_TEXTURE_2D:
						glTexImage2D(target, 0, internal_format, size[WIDTH], size[HEIGHT], 0, format, data_type, pixels);
						break;
#ifdef GL_TEXTURE_3D
					case GL_TEXTURE_3D:
						glTexImage3D(target, 0, internal_format, size[WIDTH], size[HEIGHT], size[DEPTH], 0, format, data_type, pixels);
						break;
#endif
					default:
						throw std::runtime_error("Invalid texture target");
				}
				
				// Update the client-side texture details:
				_size = size;
				_format = format;
				_data_type = data_type;
				
				if (_parameters.generate_mip_maps) {
					glGenerateMipmap(target);
				}
				
				check_graphics_error();
			}
			
			void TextureManager::Binding::resize(Vec3u size, GLenum format, GLenum data_type) {
				if (size != _texture->size()) {
					_texture->load_pixel_data(size, NULL, _texture->format(), _texture->data_type());
				}
			}
			
			void TextureManager::Binding::update(Ptr<IPixelBuffer> pixel_buffer) {
				_texture->load_pixel_data(pixel_buffer->size(), pixel_buffer->pixel_data(), pixel_buffer->pixel_format(), pixel_buffer->pixel_data_type());
			}
			
			void TextureManager::Binding::update(const TextureParameters & parameters, Ptr<IPixelBuffer> pixel_buffer) {
				_texture->set_parameters(parameters);
				
				update(pixel_buffer);
			}
			
#pragma mark -
			
			TextureManager::TextureManager() {
				/* Fetch number of texture units */ {
					GLint image_unit_count = 0;
					glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &image_unit_count);
					_image_unit_count = image_unit_count;
				}
				
				/* Initialize the texture state */ {
					_state.resize(_image_unit_count, NULL);
				}
				
				logger()->log(LOG_INFO, LogBuffer() << "OpenGL Texture Units: " << _image_unit_count);
			}
			
			TextureManager::~TextureManager() {
				logger()->log(LOG_DEBUG, LogBuffer() << "Freeing " << _handles.size() << " unused texture handles");
				
				glDeleteTextures(_handles.size(), _handles.data());
			}

			Ref<Texture> TextureManager::allocate(const TextureParameters & parameters, Ptr<IPixelBuffer> pixel_buffer) {
				const std::size_t HANDLE_POOL_REFILL_SIZE = 64;
				
				if (_handles.size() == 0) {
					logger()->log(LOG_DEBUG, LogBuffer() << "Allocating " << HANDLE_POOL_REFILL_SIZE << " texture handles");
					
					// Generate more texture handles:
					_handles.resize(64);
					glGenTextures(64, _handles.data());
					
					check_graphics_error();
				}
				
				Ref<Texture> texture = new Texture(parameters, _handles.back());
				_handles.pop_back();
				
				check_graphics_error();
				
				if (pixel_buffer) {
					auto & binding = this->bind(texture);
					binding.update(pixel_buffer);
				}
				
				return texture;
			}
			
			void TextureManager::bind(std::size_t unit, Ptr<Texture> texture) {
				DREAM_ASSERT(unit < _image_unit_count);
				
				if (_state[unit] == texture)
					return;
				
				glActiveTexture(GL_TEXTURE0 + unit);
				glBindTexture(texture->target(), texture->handle());
				
				check_graphics_error();
				
				_state[unit] = texture;
				
				GLenum target = texture->target();
				const TextureParameters & parameters = texture->parameters();
				glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(target, GL_TEXTURE_MAG_FILTER, parameters.get_mag_filter());
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, parameters.get_min_filter());
				
				if (parameters.anisotropy != 1.0) {
					glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, parameters.anisotropy);
				}
				
				check_graphics_error();
			}
			
			void TextureManager::bind(const TextureBindingsT & bindings) {
				for (auto binding : bindings) {
					bind(binding.unit, binding.texture);
				}
			}
			
			TextureManager::Binding & TextureManager::bind(Ptr<Texture> texture) {
				bind(0, texture);
				
				_binding.set_texture(texture);
				
				return _binding;
			}
			
			void TextureManager::invalidate() {
				// Clear all existing state.
				for (std::size_t unit = 0; unit < _image_unit_count; unit += 1) {
					_state[unit] = NULL;
				}
			}
			
		}
	}
}
