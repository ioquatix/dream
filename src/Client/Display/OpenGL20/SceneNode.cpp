/*
 *  Client/Display/OpenGL20/SceneNode.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 27/04/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#include "SceneNode.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			IMPLEMENT_INTERFACE(SceneNode)
			
#pragma mark -
#pragma mark class SceneNode
			
			IMPLEMENT_CLASS(SceneNode)
			
			SceneNode::SceneNode() {
				m_translate = false;
				m_rotate = false;
			}
			
			SceneNode::~SceneNode() {
				
			}
			
			REF(RenderState) SceneNode::createRenderState (REF(IShader) shader) {
				REF(RenderState) r = new RenderState;
				r->setShader(shader);
				
				m_renderState = r;
				return r;
			}
			
			void SceneNode::prepareToRender (Renderer * renderer) const {
				if (m_translate)
					glTranslate(m_translation);
				
				if (m_rotate)
					glRotate(m_rotation);
				
				if (m_renderState)
					renderer->updateCurrentRenderState(m_renderState);
			}
			
			void SceneNode::render (Renderer * renderer, TimeT time) const
			{
				prepareToRender(renderer);
			}
			
			
		}
	}
}
