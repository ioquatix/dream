/*
 *  Client/Graphics/MaterialLibraryParser.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 4/05/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_MATERIALLIBRARYPARSER_H
#define _DREAM_CLIENT_GRAPHICS_MATERIALLIBRARYPARSER_H

// This is a private header, and should not be used as public API.

#include "Graphics.h"
#include "MaterialLibrary.h"

#include <map>
#include <list>
#include <set>
#include <boost/spirit/include/classic.hpp>
#include <boost/bind.hpp>
#include <iterator>

namespace Dream
{
	namespace Client
	{
		namespace Graphics
		{
			using namespace Resources;
			
			struct TextureUnitProperties
			{
				TextureUnitProperties ();
				
				bool generateMipMap;
				String resourcePath;
				String name;
				String minFilter, magFilter;
				String blendFuncSrc, blendFuncDst;
				String target;
			};
			
#pragma mark -
			
			class MaterialFactory : public Object, IMPLEMENTS(MaterialFactory) {
				EXPOSE_CLASS(MaterialFactory)
				
				class Class : public Object::Class, IMPLEMENTS(MaterialFactory::Class) {
					EXPOSE_CLASSTYPE
				};
				
			protected:		
				String m_name;
				
				bool m_shaderPathSet;
				String m_shaderPath;
				
				std::map<String, String> m_definitions;
				std::list<TextureUnitProperties> m_texUnits;
				
				REF(const ILoader) m_loader;
				
				bool m_blendingEnabled;
				String m_blendFuncSrc, m_blendFuncDst;
				
			public:
				MaterialFactory (String name, const ILoader * loader);
				virtual ~MaterialFactory ();
				
				virtual const String & resourceName () const { return m_name; }
				
				void setShaderPath (String shaderPath);
				const String & shaderPath () const { return m_shaderPath; }
				
				void setDefine(String name, String value = "");
				void clearDefine(String name);
				
				void setBlendFunc (String src, String dst);
				
				void addTexture (const TextureUnitProperties &p);
				
				virtual REF(RenderState) createRenderState (Renderer *) const;
				
				REF(Texture) createTexture (Renderer *, const TextureUnitProperties &) const;
			};
			
#pragma mark -
			
			class MaterialLibrary : public Object, IMPLEMENTS(MaterialLibrary) {
				EXPOSE_CLASS(MaterialLibrary)
				
				class Class : public Object::Class, IMPLEMENTS(Loadable::Class), IMPLEMENTS(MaterialLibrary::Class) {
					EXPOSE_CLASSTYPE
					
					void registerLoaderTypes (REF(ILoader) loader);
					REF(Object) initFromData (const PTR(IData) data, const ILoader * loader);
				};
				
			protected:
				std::map<String, REF(IMaterialFactory)> m_factories;
				REF(const ILoader) m_loader;
				
			public:		
				MaterialLibrary(const PTR(IData) data, const ILoader * loader);
				virtual ~MaterialLibrary();
				
				void addMaterialFactory (REF(MaterialFactory) materialFactory);
				
				virtual REF(IMaterialFactory) resourceNamed (String name) const;
			};
			
		}
	}
}

#endif
