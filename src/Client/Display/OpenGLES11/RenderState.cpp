/*
 *  Client/Display/OpenGLES11/RenderState.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 26/04/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#include "RenderState.h"

namespace Dream {
	namespace Client {
		namespace Display {
			namespace OpenGLES11 {
				
				IMPLEMENT_CLASS(RenderState)
				
				RenderState::RenderState () : m_blending(false) {
					
				}
				
				RenderState::~RenderState () {
					
				}
				
				void RenderState::setBlendFunction (GLenum src, GLenum dst) { 
					m_blending = true;
					m_blendFuncSrc = src; 
					m_blendFuncDst = dst;
				}
				
				void RenderState::setTextures (const Textures & textures)
				{
					m_textures = textures;
				}
				
				void RenderState::revokeState (Renderer * renderer)
				{
					renderer->textureController()->reset();					
				}
				
				void RenderState::applyState (Renderer * renderer)
				{					
					if (m_blending) {
						glEnable(GL_BLEND);
						glBlendFunc(m_blendFuncSrc, m_blendFuncDst);
					} else {
						glDisable(GL_BLEND);
					}
					
					renderer->textureController()->updateTextures(m_textures);
				}
				
			}
		}
	}
}
