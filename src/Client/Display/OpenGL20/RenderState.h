/*
 *  Client/Display/OpenGL20/RenderState.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 26/04/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_OPENGL20_RENDERSTATE_H
#define _DREAM_CLIENT_DISPLAY_OPENGL20_RENDERSTATE_H

#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			namespace OpenGL20
			{
				class RenderState : public Object
				{
					bool m_blending;
					GLenum m_blendFuncSrc;
					GLenum m_blendFuncDst;
					
				protected:
					friend class Renderer;
					
					typedef std::vector<REF(Texture)> Textures;
					
					REF(Shader) m_shader;
					Textures m_textures;
					
					void applyState (Renderer * renderer);
					void revokeState (Renderer * renderer);
					
				public:
					RenderState ();
					
					void setShader(REF(Shader) shader);
					REF(Shader) shader () { return m_shader; }
					
					void setTextures (const Textures &);
					
					void setBlendFunction (GLenum src, GLenum dst);
				};
				
			}
		}
	}
}

#endif
