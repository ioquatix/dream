/*
 *  Client/Display/OpenGL20/RenderState.cpp
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
			namespace OpenGL20 {
				
				
				
				RenderState::RenderState () : m_blending(false) {
					
				}
				
				void RenderState::setShader(REF(Shader) shader) {
					m_shader = shader;
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
					// Disable any shader
					glUseProgram(0);
					
					renderer->textureController()->reset();
					
					glPopClientAttrib();
					glPopAttrib();
				}
				
				void RenderState::applyState (Renderer * renderer)
				{
					glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_LIGHTING_BIT | GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT);
					glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
					
					if (m_blending) {
						glEnable(GL_BLEND);
						glBlendFunc(m_blendFuncSrc, m_blendFuncDst);
					}
					
					renderer->textureController()->updateTextures(m_textures);

					if (m_shader)
						m_shader->enable();
				}
				
			}
		}
	}
}
