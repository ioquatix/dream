/*
 *  Client/Display/OpenGLES11/RenderState.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 26/04/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_OPENGLES11_RENDERSTATE_H
#define _DREAM_CLIENT_DISPLAY_OPENGLES11_RENDERSTATE_H

#include "Renderer.h"
#include "Texture.h"

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			namespace OpenGLES11
			{
				
				class RenderState : public Object
				{
				public:
					bool m_blending;
					GLenum m_blendFuncSrc;
					GLenum m_blendFuncDst;
					
				protected:
					friend class Renderer;
					
					typedef std::vector<REF(Texture)> Textures;
					
					Textures m_textures;
					
					void applyState (Renderer * renderer);
					void revokeState (Renderer * renderer);
					
				public:
					RenderState ();
					virtual ~RenderState ();
					
					void setTextures (const Textures &);
					
					void setBlendFunction (GLenum src, GLenum dst);
				};
				
			}
		}
	}
}

#endif
