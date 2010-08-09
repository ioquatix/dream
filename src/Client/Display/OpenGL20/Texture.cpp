/*
 *  Client/Display/OpenGL20/Texture.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 20/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Texture.h"

namespace Dream {
	namespace Client {
		namespace Display {
			namespace OpenGL20 {
				
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
#pragma mark class Texture
				
				IMPLEMENT_CLASS(Texture)
				
				Texture::Texture (TextureController * textureController) : m_target(0), m_textureController(textureController)
				{
					glGenTextures (1, &this->m_id);
				}
				
				Texture::~Texture ()
				{
					
				}
				
				GLuint getCurrentTextureBinding (GLenum target)
				{
					GLint currentTextureBinding = 0;
					
					switch (target) {
						case GL_TEXTURE_1D:
							glGetIntegerv(GL_TEXTURE_BINDING_1D, &currentTextureBinding);
							break;
						case GL_TEXTURE_2D:
							glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentTextureBinding);
							break;
						case GL_TEXTURE_3D:
							glGetIntegerv(GL_TEXTURE_BINDING_3D, &currentTextureBinding);
							break;
					}
					
					return currentTextureBinding;
				}
				
				void Texture::loadPixelData(const Vector<3, unsigned> & size, const ByteT * pixels, GLenum format, GLenum dataType, 
											const TextureParameters & params)
				{
					// Take a copy of the texture parameters
					m_params = params;
					
					// Internal format used to be chosen by the TextureParameters, but for now it isn't.
					m_internalFormat = params.getInternalFormat(format);
					m_target = params.target;
					
					// Figure out who is currently bound
					m_textureController->unit(0)->enable(this);
					
					if (params.generateMipMaps)
						glTexParameteri(m_target, GL_GENERATE_MIPMAP, GL_TRUE);
					else
						glTexParameteri(m_target, GL_GENERATE_MIPMAP, GL_FALSE);
					
					if (m_target == GL_TEXTURE_1D) {
						glTexImage1D(m_target, 0, m_internalFormat, size[WIDTH], 0, format, dataType, pixels);
					} else if (m_target == GL_TEXTURE_2D) {
						glTexImage2D(m_target, 0, m_internalFormat, size[WIDTH], size[HEIGHT], 0, format, dataType, pixels);
					} else if (m_target == GL_TEXTURE_3D) {
						glTexImage3D(m_target, 0, m_internalFormat, size[WIDTH], size[HEIGHT], size[DEPTH], 0, format, dataType, pixels);
					}
					
					m_size = size;
				}
				
				Vec3u Texture::size () const
				{
					return m_size;
				}
				
				void Texture::resize (Vec3u newSize)
				{
					m_textureController->unit(0)->enable(this);
									
					GLenum dataType = GL_UNSIGNED_BYTE;
				
					if (m_target == GL_TEXTURE_1D) {
						glTexImage1D(m_target, 0, m_internalFormat, newSize[WIDTH], 0, m_internalFormat, dataType, NULL);
					} else if (m_target == GL_TEXTURE_2D) {
						glTexImage2D(m_target, 0, m_internalFormat, newSize[WIDTH], newSize[HEIGHT], 0, m_internalFormat, dataType, NULL);
					} else if (m_target == GL_TEXTURE_3D) {
						glTexImage3D(m_target, 0, m_internalFormat, newSize[WIDTH], newSize[HEIGHT], newSize[DEPTH], 0, m_internalFormat, dataType, NULL);
					}
										
					m_size = newSize;
				}
				
				void Texture::updatePixelData (PTR(IPixelBuffer) pixelBuffer, const Vec3u & offset)
				{
					m_textureController->unit(0)->enable(this);
				
					Vec3u size = pixelBuffer->size();
					GLenum format = pixelBuffer->pixelFormat();
					GLenum dataType = pixelBuffer->pixelDataType();
					
					if (m_target == GL_TEXTURE_1D) {
						glTexSubImage1D(m_target, 0, offset[X], size[WIDTH], format, dataType, pixelBuffer->pixelData());
					} else if (m_target == GL_TEXTURE_2D) {
						glTexSubImage2D(m_target, 0, offset[X], offset[Y], size[WIDTH], size[HEIGHT], format, dataType, pixelBuffer->pixelData());
					} else if (m_target == GL_TEXTURE_3D) {
						glTexSubImage3D(m_target, 0, offset[X], offset[Y], offset[Z], size[X], size[Y], size[Z], format, dataType, 
						                pixelBuffer->pixelData());
					}					
				}
								
#pragma mark -
#pragma mark class TextureController
								
				TextureController::TextureController ()
				{
					GLint textureUnitCount = 0;
					glGetIntegerv(GL_MAX_TEXTURE_UNITS, &textureUnitCount);
					
					GLint imageUnitCount = 0;
					glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &imageUnitCount);
					
					IndexT totalUnits = imageUnitCount > textureUnitCount ? imageUnitCount : textureUnitCount;

					m_textureUnits.reserve(totalUnits);
					
					for (unsigned i = 0; i < totalUnits; i += 1) {
						m_textureUnits.push_back(TextureUnit(GL_TEXTURE0 + i, this));
					}
					
					m_basicUnitCount = textureUnitCount;
					m_imageUnitCount = imageUnitCount;
					
					std::cerr << "OpenGL Texture Units: " << m_basicUnitCount << " Image Units: " << m_imageUnitCount << std::endl;
				}
				
				void TextureController::updateTextures (const std::vector<REF(Texture)> & textures)
				{
					IndexT active = textures.size();
					IndexT total = m_textureUnits.size();
					
					unsigned i = 0;
					for (; i < active; i += 1) {
						m_textureUnits[i].enable(textures[i]);
					}
					
					for (; i < total; i += 1) {
						m_textureUnits[i].disable();
					}
				}
				
				void TextureController::reset ()
				{
					IndexT total = count();
					
					for (unsigned i = 0; i < total; i += 1)
						unit(i)->disable();
					
					// Set active texture back to unit 0
					glActiveTexture(GL_TEXTURE0);	
				}

				IndexT TextureController::basicUnits () const
				{
					return m_basicUnitCount;
				}

				IndexT TextureController::imageUnits () const
				{
					return m_imageUnitCount;
				}				
				
				IndexT TextureController::count () const
				{
					return m_textureUnits.size();
				}
				
				TextureUnit * TextureController::unit (IndexT index)
				{
					return &m_textureUnits.at(index);
				}
								
#pragma mark -
#pragma mark class TextureUnit
				
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

				
				TextureUnit::TextureUnit (GLenum textureUnit, TextureController * textureController) 
				: m_textureUnit(textureUnit), m_currentTarget(INVALID_TARGET), m_currentTexture(INVALID_TEXTURE), m_textureController(textureController)
				{
					
				}
				
				void TextureUnit::enable (const PTR(Texture) texture)
				{
					GLenum newTarget = texture->target();
					ensure(isValidTextureTarget(newTarget));
					
					GLuint newTexture = texture->id();
					ensure(newTexture != INVALID_TEXTURE);
					
					// Disable cache
					bool targetActive = false;
					bool textureActive = false;
					
					//bool targetActive = m_currentTarget == newTarget;
					//bool textureActive = m_currentTexture == newTexture;
					
					// If we need to update the target, we need to update the texture too
					if (!targetActive) {
						glActiveTexture(m_textureUnit);
						
						if (m_currentTarget != INVALID_TARGET)
							glDisable(m_currentTarget);
						
						glEnable(newTarget);
						m_currentTarget = newTarget;
						
						if (!textureActive) {
							glBindTexture(newTarget, newTexture);
							m_currentTexture = newTexture;
						}
					}
					// If we need to update only the texture
					else if (!textureActive) {
						glActiveTexture(m_textureUnit);
						
						glBindTexture(m_currentTarget, newTexture);
						m_currentTexture = newTexture;
					}
					
					// If we updated the texture state, we also need to update any texture parameters.
					if (!textureActive) {
						const TextureParameters & params = texture->parameters();
						glTexParameteri(newTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
						glTexParameteri(newTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
						glTexParameteri(newTarget, GL_TEXTURE_MAG_FILTER, params.getMagFilter());
						glTexParameteri(newTarget, GL_TEXTURE_MIN_FILTER, params.getMinFilter());
					}
				}
				
				void TextureUnit::disable ()
				{
					if (m_currentTarget != INVALID_TARGET) {
						glActiveTexture(m_textureUnit);
						
						// This is a small optimization - we don't touch the current texture but disable the target - if we need to use the same texture again
						// we only have to enable the target again rather than the texture.
						
						// Disable texture
						//glBindTexture(m_currentTarget, 0);
						//m_currentTexture = 0;
						
						// Disable target
						glDisable(m_currentTarget);
						m_currentTarget = INVALID_TARGET;
					}
				}
				
				REF(Texture) TextureController::createTexture (PTR(IPixelBuffer) pixelBuffer, const TextureParameters & params)
				{
					REF(Texture) tex = new Texture(this);

					tex->loadPixelData(pixelBuffer->size(), pixelBuffer->pixelData(), pixelBuffer->pixelFormat(), pixelBuffer->pixelDataType(), params);
					
					return tex;
				}
				
			}
		}
	}
}
