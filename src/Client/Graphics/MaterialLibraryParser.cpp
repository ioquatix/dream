/*
 *  Client/Graphics/MaterialLibraryParser.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 4/05/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "MaterialLibraryParser.h"

namespace Dream
{
	namespace Client
	{
		namespace Graphics
		{

			TextureUnitProperties::TextureUnitProperties () : generateMipMap(true)
			{
				
			}
			
#pragma mark -
#pragma mark class MaterialFactory
			
			IMPLEMENT_CLASS(MaterialFactory)
			
			MaterialFactory::MaterialFactory (String name, const ILoader * loader) : m_name(name), m_shaderPathSet(false), m_loader(loader),
			m_blendingEnabled(false)
			{
				
			}
			
			MaterialFactory::~MaterialFactory () {
				
			}
			
			void MaterialFactory::setShaderPath (String shaderPath) {
				m_shaderPath = shaderPath;
				m_shaderPathSet = true;
			}
			
			void MaterialFactory::setBlendFunc (String src, String dst) {
				m_blendFuncSrc = src;
				m_blendFuncDst = dst;
				m_blendingEnabled = true;
			}
			
			void MaterialFactory::setDefine(String name, String value) {
				m_definitions[name] = value;
			}
			
			void MaterialFactory::clearDefine(String name) {
				m_definitions.erase(name);
			}
			
			void MaterialFactory::addTexture (const TextureUnitProperties &p) {
				m_texUnits.push_back(p);
			}

#if defined(DREAM_USE_OPENGLES11) || defined(DREAM_USE_OPENGL20)
			GLenum blendFunctionValue (const String & key, GLenum defaultValue)
			{
				// Common Blend Values
				if (key == "ZERO") return GL_ZERO;
				else if (key == "ONE") return GL_ONE;
				// Source Blend Values
				else if (key == "DST_COLOR") return GL_DST_COLOR;
				else if (key == "ONE_MINUS_DST_COLOR") return GL_ONE_MINUS_DST_COLOR;
				else if (key == "SRC_ALPHA") return GL_SRC_ALPHA;
				else if (key == "ONE_MINUS_SRC_ALPHA") return GL_ONE_MINUS_SRC_ALPHA;
				else if (key == "DST_ALPHA") return GL_DST_ALPHA;
				else if (key == "ONE_MINUS_DST_ALPHA") return GL_ONE_MINUS_DST_ALPHA;
				else if (key == "SRC_ALPHA_SATURATE") return GL_SRC_ALPHA_SATURATE;
				// Destination Blend Values
				else if (key == "SRC_COLOR") return GL_SRC_COLOR;
				else if (key == "ONE_MINUS_SRC_COLOR") return GL_ONE_MINUS_SRC_COLOR;
				else if (key == "DST_ALPHA") return GL_DST_ALPHA;
				else if (key == "ONE_MINUS_DST_ALPHA") return GL_ONE_MINUS_DST_ALPHA;
				else if (key == "SRC_ALPHA") return GL_SRC_ALPHA;
				else if (key == "ONE_MINUS_SRC_ALPHA") return GL_ONE_MINUS_SRC_ALPHA;
				
				return defaultValue;
			}
			
			GLenum filterFunctionValue (const String & key, GLenum defaultValue)
			{
				if (key == "NEAREST") return GL_NEAREST;
				else if (key == "LINEAR") return GL_LINEAR;

				else if (key == "NEAREST_MIPMAP_NEAREST") return GL_NEAREST_MIPMAP_NEAREST;
				else if (key == "LINEAR_MIPMAP_NEAREST") return GL_LINEAR_MIPMAP_NEAREST;
				else if (key == "NEAREST_MIPMAP_LINEAR") return GL_NEAREST_MIPMAP_LINEAR;
				else if (key == "LINEAR_MIPMAP_LINEAR") return GL_LINEAR_MIPMAP_LINEAR;
				
				return defaultValue;
			}
#endif
			
#ifdef DREAM_USE_OPENGL20
			GLenum textureTargetValue (const String & key, GLenum defaultValue)
			{
				if (key == "1D") return GL_TEXTURE_1D;
				else if (key == "2D") return GL_TEXTURE_2D;
				else if (key == "3D") return GL_TEXTURE_3D;
				
				return defaultValue;
			}
#endif
			
			REF(Texture) MaterialFactory::createTexture (Renderer * renderer, const TextureUnitProperties & t) const
			{
				REF(IPixelBuffer) pixbuf = m_loader->load<IPixelBuffer>(t.resourcePath);
				ensure(pixbuf);
				
#if defined(DREAM_USE_OPENGLES11) || defined(DREAM_USE_OPENGL20)
				TextureParameters tp;
				
				tp.generateMipMaps = t.generateMipMap;
				
				tp.minFilter = filterFunctionValue(t.minFilter, 0);
				tp.magFilter = filterFunctionValue(t.magFilter, 0);
#endif
				
#ifdef DREAM_USE_OPENGL20
				tp.target = textureTargetValue(t.target, GL_TEXTURE_2D);
#endif
				
#ifdef DREAM_USE_OPENGLES11
				// OpenGL ES 11 only supports GL_TEXTURE_2D target
				ensure(t.target == "2D" || t.target == "");
				tp.target = GL_TEXTURE_2D;
#endif
				
				REF(Texture) tex = renderer->textureController()->createTexture(pixbuf, tp);
				ensure(tex);
				
				return tex;
			}
			
#ifdef DREAM_USE_OPENGL20
			REF(RenderState) MaterialFactory::createRenderState (Renderer * renderer) const
			{
				// Shader - this may have textures bound into it.
				REF(Shader) shader;
				
				// Create a new render state object.
				REF(RenderState) renderState = new RenderState;
				
				// If a shader is not set in the configuration file, don't try to load it.
				if (m_shaderPathSet) {
					REF(IShaderFactory) shaderFactory = m_loader->load <IShaderFactory> (m_shaderPath);
					ensure (shaderFactory);
					
					shader = shaderFactory->createShader(m_definitions);
					ensure(shader);
					
					renderState->setShader(shader);
				}
				
				if (m_blendFuncSrc != "") {
					renderState->setBlendFunction(blendFunctionValue(m_blendFuncSrc, GL_ONE), blendFunctionValue(m_blendFuncDst, GL_ZERO));
				}
				
				int currentUnit = -1;
				std::vector<REF(Texture)> textures;
				for (iterateEach(m_texUnits, t)) {
					currentUnit += 1;
					
					REF(Texture) tex = createTexture(renderer, *t);
					
					if (shader)
						shader->bindTextureUnit(currentUnit, (*t).name);
					
					textures.push_back(tex);
				}
				
				renderState->setTextures(textures);
				
				// Make sure we didn't run out of texture units
				ensure(currentUnit <= (int)renderer->textureController()->count());
				
				return renderState;
			}
#endif
			
#ifdef DREAM_USE_OPENGLES11
			REF(RenderState) MaterialFactory::createRenderState (Renderer * renderer) const
			{
				// Create a new render state object.
				REF(RenderState) renderState = new RenderState;
				
				if (m_blendFuncSrc != "") {
					renderState->setBlendFunction(blendFunctionValue(m_blendFuncSrc, GL_ONE), blendFunctionValue(m_blendFuncDst, GL_ZERO));
				}
				
				int currentUnit = -1;
				std::vector<REF(Texture)> textures;
				for (iterateEach(m_texUnits, t)) {
					currentUnit += 1;
					
					REF(Texture) tex = createTexture(renderer, *t);
					
					textures.push_back(tex);
				}
				
				renderState->setTextures(textures);
				
				// Make sure we didn't run out of texture units
				ensure(currentUnit <= (int)renderer->textureController()->count());
				
				return renderState;
			}
#endif
			
#pragma mark -
#pragma mark class MaterialLibrary
			
			IMPLEMENT_CLASS(MaterialLibrary)
			
			void MaterialLibrary::Class::registerLoaderTypes (REF(ILoader) loader) {
				loader->setLoaderForExtension(this, "materials");
			}

			REF(Object) MaterialLibrary::Class::initFromData (const PTR(IData) data, const ILoader * loader) {
				return new MaterialLibrary(data, loader);
			}
												
			MaterialLibrary::~MaterialLibrary() {
				
			}
			
			void MaterialLibrary::addMaterialFactory (REF(MaterialFactory) materialFactory) {
				m_factories[materialFactory->resourceName()] = materialFactory;
			}
			
			REF(IMaterialFactory) MaterialLibrary::resourceNamed (String name) const {
				typeof(m_factories.begin()) iter = m_factories.find(name);
				
				if (iter != m_factories.end())
					return (*iter).second;
				else {
					std::cerr << "No material resource named " << name << " available!" << std::endl;
					return NULL;
				}
			}
			
		}
	}
}
