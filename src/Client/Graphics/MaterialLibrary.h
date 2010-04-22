/*
 *  Client/Graphics/MaterialLibrary.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 13/07/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_MATERIALLIBRARY_H
#define _DREAM_CLIENT_GRAPHICS_MATERIALLIBRARY_H

#include "Graphics.h"
#include "../../Resources/Loader.h"

namespace Dream
{
	namespace Client
	{
		namespace Graphics
		{
			
			/**
			 Load and create RenderState objects as defined.
			 
			 REF(IMaterialLibrary) matLib = loader->load <IMaterialLibrary> ("scene");
			 REF(RenderState) renderState = matLib->resourceNamed("box")->createRenderState();
			 
			 @sa IMaterialLibrary
			 */
			class IMaterialFactory : IMPLEMENTS(Object) 
			{
				EXPOSE_INTERFACE(MaterialFactory)
				
				class Class : IMPLEMENTS(Object::Class)
				{
					
				};
				
			public:				
				virtual const String & resourceName () const abstract;
				virtual const String & shaderPath () const abstract;
				
				virtual REF(RenderState) createRenderState (Renderer *) const abstract;
			};
			
			class IMaterialLibrary : IMPLEMENTS(Object)
			{
				EXPOSE_INTERFACE(MaterialLibrary)
				
				class Class : IMPLEMENTS(Object::Class)
				{
					
				};
				
			public:				
				virtual REF(IMaterialFactory) resourceNamed (String name) const abstract;
			};
			
		}
	}
}

#endif
