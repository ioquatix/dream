//
//  Client/Graphics/Renderer.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 31/01/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _DREAM_CLIENT_GRAPHICS_RENDERER_H
#define _DREAM_CLIENT_GRAPHICS_RENDERER_H

#include "TextureManager.h"
#include "ShaderManager.h"
#include "../Display/Scene.h"
#include "../../Renderer/Viewport.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			/*
				How do we manage state?
			 
				All non-graphics state is encapsulated in class INode - e.g. position, shape, mesh, colour, animation.
				
				Graphics related state should be encapsulated in the IRenderer - e.g. particle position, camera position, lighting, etc.
				
				To be efficient, some state (e.g. pixel buffer) may be uploaded to the graphics card. Uploading the same pixel buffer multiple times is not efficient. So, the rendering layer should cache these types of state changes internally.
			 */
			
			class IRenderer;
			
			class INode {
			public:
				virtual ~INode();
				
				/// Tell the renderer to render this node.
				virtual void accept(IRenderer * renderer);
				
				/// Iterate over all logical children and pass them to the renderer via accept.
				virtual void traverse(IRenderer * renderer);				
			};
			
			class IRenderer {
			public:
				virtual ~IRenderer();
				
				// Overload this function with specific object callbacks. You may want to implement a hard coded version of this to avoid dynamic dispatch.
				virtual void render(INode * node);
				
				// Call this function to begin the graph traversal.
				virtual void traverse(INode * node);
			};
			
			/// This case encapsulates basic state common to many renderers - it may not be suitable for all types of renderers.
			class BasicRenderer : public Object, implements IRenderer {
			protected:
				Ref<Resources::Loader> _resource_loader;
				Ref<TextureManager> _texture_manager;
				Ref<ShaderManager> _shader_manager;
				
				Ref<Renderer::Viewport> _viewport;
				
				// These are essentially helper methods to load shader programs:
				GLuint compile_shader_of_type (GLenum type, StringT name);
				Ref<Program> load_program(StringT name);
				
			public:
				BasicRenderer(Ptr<Resources::Loader> resource_loader, Ptr<TextureManager> texture_manager, Ptr<ShaderManager> shader_manager, Ptr<Renderer::Viewport> viewport);
				virtual ~BasicRenderer();
				
				// By default, we can render nothing in particular.
			};
			
		}
	}
}

#endif
