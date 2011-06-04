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

			PixelBufferRenderer::PixelBufferRenderer () : m_size(ZERO)
			{
				m_textureParameters.target = GL_TEXTURE_2D;
				m_textureParameters.minFilter = GL_NEAREST;
				m_textureParameters.magFilter = GL_NEAREST;
				m_textureParameters.generateMipMaps = false;
			}

			PixelBufferRenderer::~PixelBufferRenderer ()
			{
			}
			
			void PixelBufferRenderer::change (Renderer * renderer, REF(IPixelBuffer) pixels, const Geometry::AlignedBox<2> & box, Vector<2, bool> flip, int rotate)
			{
				m_size = pixels->size().reduce();
				//if (m_texture) {
					// updatePixelData will resize the texture as required.
					//if (!m_texture->size().greaterThanOrEqual(m_size))
					//	m_texture->resize(m_size << 1);
					
				//	m_texture->updatePixelData(pixels, ZERO);
				//} else {
					m_texture = renderer->textureController()->createTexture(pixels, m_textureParameters);
				//}
				
				m_texCoords.resize(4);
				m_vertices.resize(4);
				
				const Vec2b CORNERS[] = {
					Vec2b(false, false),
					Vec2b(false, true),
					Vec2b(true, true),
					Vec2b(true, false)
				};
				
				/*
				// Possible optimisation
				if (flip[X] && flip[Y]) {
					rotation += 2;
					flip[X] = false;
					flip[Y] = false;
				}
				*/

				AlignedBox<2> texCoordBox(ZERO, m_size / m_texture->size().reduce());
				
				for (unsigned i = 0; i < 4; i++) {
					m_texCoords[i] = texCoordBox.corner(CORNERS[(i+rotate) % 4]);
					m_vertices[i] = box.corner(CORNERS[i]);
				}

				if (flip[X]) {
					std::swap(m_texCoords[0], m_texCoords[3]);
					std::swap(m_texCoords[1], m_texCoords[2]);
				}
				
				if (flip[Y]) {
					std::swap(m_texCoords[0], m_texCoords[1]);
					std::swap(m_texCoords[2], m_texCoords[3]);				
				}
				
				std::swap(m_vertices[0], m_vertices[1]);
				std::swap(m_texCoords[0], m_texCoords[1]);
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
