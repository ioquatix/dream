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
			
			const char * targetName (GLenum target)
			{
				switch (target) {
					case GL_TEXTURE_1D:
						return "GL_TEXTURE_1D";
					case GL_TEXTURE_2D:
						return "GL_TEXTURE_2D";
					case GL_TEXTURE_3D:
						return "GL_TEXTURE_3D";
					default:
						return "Unknown Target";
				}
			}
			
			bool isValidTextureTarget (GLenum target)
			{
				switch (target) {
					case GL_TEXTURE_1D:
					case GL_TEXTURE_2D:
					case GL_TEXTURE_3D:
						return true;
					default:
						return false;
				}
			}
			
#pragma mark -

			GLenum TextureParameters::getMinFilter () const {
				if (minFilter)
					return minFilter;
				else if (generateMipMaps)
					return GL_LINEAR_MIPMAP_LINEAR;
				else
					return GL_LINEAR;
			}

			GLenum TextureParameters::getMagFilter () const {
				if (magFilter)
					return magFilter;
				else
					return GL_LINEAR;
			}

			GLenum TextureParameters::getTarget (GLenum defaultTarget) const {
				if (target)
					return target;
				else
					return defaultTarget;
			}

			GLenum TextureParameters::getInternalFormat (GLenum defaultInternalFormat) const {
				if (internalFormat)
					return internalFormat;
				else
					return defaultInternalFormat;
			}
			
#pragma mark -
			
			TextureManager::TextureManager()
			{
				GLint imageUnitCount = 0;
				glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &imageUnitCount);
								
				State initial = {INVALID_TEXTURE};
				m_current.resize(imageUnitCount, initial);
								
				m_imageUnitCount = imageUnitCount;
				
				std::cerr << "OpenGL Texture Units: " << imageUnitCount << std::endl;
			}
			
			TextureManager::~TextureManager()
			{
				for (Record & r : m_textures) {
					glDeleteTextures(1, &r.handle);
				}
			}

			IndexT TextureManager::create(const TextureParameters & parameters, PTR(IPixelBuffer) pixelBuffer)
			{
				GLenum handle;
				
				glGenTextures(1, &handle);
				
				Record record = {handle, parameters};
				
				m_textures.push_back(record);
				IndexT index = m_textures.size() - 1;
				
				if (pixelBuffer) {
					update(index, pixelBuffer);
				}
				
				return index;
			}
			
			void TextureManager::loadPixelData (IndexT index, const Vector<3, unsigned> & size, const ByteT * pixels, GLenum format, GLenum dataType)
			{
				const TextureParameters & parameters = m_textures[index].parameters;
				
				// We will use unit 0 to upload the texture data.
				bind(index, 0);
				
				/// Invalidate the first texture unit since we may use it to upload texture data.
				m_current[0].texture = INVALID_TEXTURE;
				
				GLenum internalFormat = parameters.getInternalFormat(format);
				
				switch (parameters.target) {
					case GL_TEXTURE_1D:
						glTexImage1D(parameters.target, 0, internalFormat, size[WIDTH], 0, format, dataType, pixels);
						break;
					case GL_TEXTURE_2D:
						glTexImage2D(parameters.target, 0, internalFormat, size[WIDTH], size[HEIGHT], 0, format, dataType, pixels);
						break;
					case GL_TEXTURE_3D:
						glTexImage3D(parameters.target, 0, internalFormat, size[WIDTH], size[HEIGHT], size[DEPTH], 0, format, dataType, pixels);
						break;
				}
				
				m_textures[index].currentSize = size;
				m_textures[index].currentFormat = format;
				m_textures[index].currentDataType = dataType;
				
				if (parameters.generateMipMaps)
					glGenerateMipmap(parameters.target);
			}
			
			void TextureManager::update(IndexT index, PTR(IPixelBuffer) pixelBuffer, const TextureParameters & parameters)
			{
				m_textures[index].parameters = parameters;
				
				loadPixelData(index, pixelBuffer->size(), pixelBuffer->pixelData(), pixelBuffer->pixelFormat(), pixelBuffer->pixelDataType());
			}
			
			void TextureManager::update(IndexT index, PTR(IPixelBuffer) pixelBuffer)
			{
				loadPixelData(index, pixelBuffer->size(), pixelBuffer->pixelData(), pixelBuffer->pixelFormat(), pixelBuffer->pixelDataType());
			}
			
			void TextureManager::resize(IndexT index, Vec3u size, GLenum format, GLenum dataType)
			{
				if (size != m_textures[index].currentSize)
					loadPixelData(index, size, NULL, m_textures[index].currentFormat, m_textures[index].currentDataType);
			}
			
			void TextureManager::bind(IndexT index, IndexT unit)
			{
				ensure(index < m_textures.size());
				
				const Record & record = m_textures[index];
				State & state = m_current[unit];
				
				if (state.texture != record.handle) {
					glActiveTexture(GL_TEXTURE0 + unit);
					
					GLenum target = record.parameters.target;
					glBindTexture(target, record.handle);
					
					glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
					glTexParameteri(target, GL_TEXTURE_MAG_FILTER, record.parameters.getMagFilter());
					glTexParameteri(target, GL_TEXTURE_MIN_FILTER, record.parameters.getMinFilter());
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
				for (State & s : m_current) {
					s.texture = INVALID_TEXTURE;
				}
			}
			
		}
	}
}
