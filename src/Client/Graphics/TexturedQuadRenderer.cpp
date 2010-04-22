/*
 *  Client/Graphics/TexturedQuadRenderer.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 22/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "TexturedQuadRenderer.h"
#include "../Display/Context.h"

namespace Dream
{
	namespace Client
	{
		namespace Graphics
		{

#if defined(DREAM_USE_OPENGLES11) || defined(DREAM_USE_OPENGL20)
			void TexturedQuadRenderer::render (Renderer * r, const Geometry::AlignedBox<2> & b, bool flip)
			{				
				/*
				 const GLubyte squareColors[] = {
				 255, 255,   0, 255,
				 0,   255, 255, 255,
				 0,     0,   0,   0,
				 255,   0, 255, 255,
				 };
				 */
				
				const GLshort texCoords[] = {
					0, 0,
					1, 0,
					0, 1,
					1, 1,
				};
				
				GLfloat squareVertices[8];
				
				Vec2 p1(b.min());
				Vec2 p2(b.max());
				
				if (flip) {
					std::swap(p1[Y], p2[Y]);
				}
				
				int c = 0;
				squareVertices[c++] = p1[X];
				squareVertices[c++] = p1[Y];
				
				squareVertices[c++] = p2[X];
				squareVertices[c++] = p1[Y];
				
				squareVertices[c++] = p1[X];
				squareVertices[c++] = p2[Y];
				
				squareVertices[c++] = p2[X];
				squareVertices[c++] = p2[Y];
				
				glVertexPointer(2, GL_FLOAT, 0, squareVertices);
				glEnableClientState(GL_VERTEX_ARRAY);
				
				//glColorPointer(4, GL_UNSIGNED_BYTE, 0, squareColors);
				//glEnableClientState(GL_COLOR_ARRAY);
				
				glTexCoordPointer(2, GL_SHORT, 0, texCoords);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				//glDisableClientState(GL_COLOR_ARRAY);
			}
#endif

			IMPLEMENT_CLASS(PixelBufferRenderer)

			PixelBufferRenderer::PixelBufferRenderer () : m_size(ZERO)
			{
			}

			PixelBufferRenderer::~PixelBufferRenderer ()
			{
			
			}
			
			void PixelBufferRenderer::change (Renderer * renderer, REF(IPixelBuffer) pixels, const Geometry::AlignedBox<2> & box, Vector<2, bool> flip)
			{
				m_size = pixels->size().reduce();
				if (m_texture) {
					if (!m_texture->size().greaterThanOrEqual(m_size))
						m_texture->resize(m_size << 1);
					
					m_texture->updatePixelData(pixels, Vector<3, unsigned>(ZERO));
				} else {
					/// @todo Maybe make constructor for TextureParameters have an argument
					/// i.e. TextureParameters tp(LOW_QUALITY || HIGH_QUALITY || TEXT_QUALITY) etc
					TextureParameters tp;					
					tp.target = GL_TEXTURE_2D;
					tp.minFilter = GL_NEAREST;
					tp.magFilter = GL_NEAREST;
					tp.generateMipMaps = false;
					
					m_texture = renderer->textureController()->createTexture(pixels, tp);
				}
				
				m_texCoords.clear();
				m_vertices.clear();
				
				Vec2 texCoordMax = m_size / m_texture->size().reduce();
				AlignedBox<2> texCoordBox(Vec2(ZERO), texCoordMax);
				
				// N shape triangle strip
				const bool CORNERS[] = {false, false, false, true, true, false, true, true};
				
				for (unsigned i = 0; i < 8; i += 2) {
					Vector<2, bool> c = vec(CORNERS[i], CORNERS[i+1]);
					m_vertices.push_back(box.corner(c));
										
					c[X] = flip[X] ? !c[X] : c[X];
					c[Y] = flip[Y] ? !c[Y] : c[Y];
					
					m_texCoords.push_back(texCoordBox.corner(c));
				}				
			}
			
			void PixelBufferRenderer::render (Renderer * renderer)
			{
				if (!m_texture || m_vertices.size() == 0)
					return;
				
				renderer->textureController()->unit(0)->enable(m_texture);
				
				glVertexPointer(2, GL_FLOAT, 0, &m_vertices[0]);
				glEnableClientState(GL_VERTEX_ARRAY);
			
				glTexCoordPointer(2, GL_FLOAT, 0, &m_texCoords[0]);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				
				renderer->textureController()->unit(0)->disable();
			}

		}
	}
}
