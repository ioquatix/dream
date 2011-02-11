/*
 *  Client/Graphics/TexturedQuadRenderer.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 22/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_TEXTUREDQUADRENDERER_H
#define _DREAM_CLIENT_GRAPHICS_TEXTUREDQUADRENDERER_H

#include "Graphics.h"
#include "../../Geometry/AlignedBox.h"

namespace Dream
{
	namespace Client
	{
		namespace Graphics
		{
			class TexturedQuadRenderer
			{
			public:
				void render (Display::RendererT *, const Geometry::AlignedBox<2> &, bool flip = true);
			};
			
			class PixelBufferRenderer : public Object
			{
				EXPOSE_CLASS(PixelBufferRenderer)
				
				class Class : public Object::Class
				{
					EXPOSE_CLASSTYPE
				};
			
			protected:
				REF(Texture) m_texture;
				Vec2 m_size;
				
				std::vector<Vec2> m_texCoords;
				std::vector<Vec2> m_vertices;
				
				TextureParameters m_textureParameters;
				
			public:
				PixelBufferRenderer ();
				virtual ~PixelBufferRenderer ();
				
				TextureParameters & textureParameters() { return m_textureParameters; }
				const TextureParameters & textureParameters() const { return m_textureParameters; }
				
				void change (Renderer * renderer, REF(IPixelBuffer) pixels, const Geometry::AlignedBox<2> & box, Vector<2, bool> flip, int rotate = 0);
				
				inline void change (Renderer * renderer, REF(IPixelBuffer) pixels, const Geometry::AlignedBox<2> & box) {
					change(renderer, pixels, box, vec(false, false), 0);
				}
								
				void render (Renderer * renderer);
			};
			
		}
	}
}

#endif
