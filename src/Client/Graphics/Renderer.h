//
//  Client/Graphics/Renderer.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 31/01/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef Dream_Renderer_h
#define Dream_Renderer_h

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
			
			class ISceneManager;
			class IScene;
			
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
				
				// Prepare the renderer with any required resources.
				virtual void did_become_current(ISceneManager * manager, IScene * scene);
				
				// Release any resources that are no longer needed.
				virtual void will_revoke_current(ISceneManager * manager, IScene * scene);
			};
			
		}
	}
}

#endif
