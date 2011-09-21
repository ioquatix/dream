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
				StringT resourcePath;
				StringT name;
				StringT minFilter, magFilter;
				StringT blendFuncSrc, blendFuncDst;
				StringT target;
			};
			
#pragma mark -
			
			class MaterialFactory : public Object, implements IMaterialFactory 
			{
			protected:		
				StringT m_name;
				
				bool m_shaderPathSet;
				StringT m_shaderPath;
				
				std::map<StringT, StringT> m_definitions;
				std::list<TextureUnitProperties> m_texUnits;
				
				REF(const ILoader) m_loader;
				
				bool m_blendingEnabled;
				StringT m_blendFuncSrc, m_blendFuncDst;
				
			public:
				MaterialFactory (StringT name, const ILoader * loader);
				virtual ~MaterialFactory ();
				
				virtual const StringT & resourceName () const { return m_name; }
				
				void setShaderPath (StringT shaderPath);
				const StringT & shaderPath () const { return m_shaderPath; }
				
				void setDefine(StringT name, StringT value = "");
				void clearDefine(StringT name);
				
				void setBlendFunc (StringT src, StringT dst);
				
				void addTexture (const TextureUnitProperties &p);
				
				virtual REF(RenderState) createRenderState (Renderer *) const;
				
				REF(Texture) createTexture (Renderer *, const TextureUnitProperties &) const;
			};
			
#pragma mark -
			
			class MaterialLibrary : public Object, implements IMaterialLibrary
			{
			public:
				class Loader : public Object, implements ILoadable
				{
					public:
						void registerLoaderTypes (ILoader * loader);
						REF(Object) loadFromData (const PTR(IData) data, const ILoader * loader);
				};
				
			protected:
				std::map<StringT, REF(IMaterialFactory)> m_factories;
				REF(const ILoader) m_loader;
				
			public:		
				MaterialLibrary(const PTR(IData) data, const ILoader * loader);
				virtual ~MaterialLibrary();
				
				void addMaterialFactory (REF(MaterialFactory) materialFactory);
				
				virtual REF(IMaterialFactory) resourceNamed (StringT name) const;
			};
			
		}
	}
}

#endif
