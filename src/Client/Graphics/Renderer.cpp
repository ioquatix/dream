//
//  Client/Graphics/Renderer.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 31/01/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "Renderer.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			INode::~INode() {
				
			}
			
			void INode::accept(IRenderer * renderer) {
				renderer->render(this);
			}
			
			void INode::traverse(IRenderer * renderer) {
				// An implementation would look something like this:
				//for (auto child : this->children()) {
				//	child->accept(renderer);
				//}
			}
			
#pragma mark -
			
			IRenderer::~IRenderer() {
				
			}
				
			void IRenderer::render(INode * node) {
				node->traverse(this);
			}
			
			void IRenderer::traverse(INode * node) {
				node->accept(this);
			}
			
		}
	}
}
