/*
 *  Client/Graphics/WireframeRenderer.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 22/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "WireframeRenderer.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
		
			WireframeRenderer::WireframeRenderer () : m_color(0.0, 0.0, 0.0, 1.0), m_filled(false)
			{
				
			}
			
			void WireframeRenderer::setFilled (bool filled)
			{
				m_filled = true;
			}
			
#if defined(DREAM_USE_OPENGLES11) || defined(DREAM_USE_OPENGL20)	
			void WireframeRenderer::renderGrid (Display::RendererT * renderer) {
				std::vector<Vec3> points;
				const RealT LOWER = -200;
				const RealT UPPER = 200;
				const RealT STEP = 10;
				
				for (RealT x = LOWER; x <= UPPER; x += STEP) {
					points.push_back(Vec3(x, LOWER, 0));
					points.push_back(Vec3(x, UPPER, 0));
					
					points.push_back(Vec3(LOWER, x, 0));
					points.push_back(Vec3(UPPER, x, 0));
				}
				
				render(renderer, points);
			}
			
			void WireframeRenderer::renderAxis (Display::RendererT *)
			{
				std::vector<Vec3> vertices;
				std::vector<Vec3> colors;
				
				vertices.push_back(Vec3(ZERO));
				colors.push_back(Vec3(1.0, 0.0, 0.0));
				
				vertices.push_back(Vec3(10.0, 0.0, 0.0));
				colors.push_back(Vec3(1.0, 0.0, 0.0));
				
				vertices.push_back(Vec3(ZERO));
				colors.push_back(Vec3(0.0, 1.0, 0.0));
				
				vertices.push_back(Vec3(0.0, 10.0, 0.0));
				colors.push_back(Vec3(0.0, 1.0, 0.0));

				vertices.push_back(Vec3(ZERO));
				colors.push_back(Vec3(0.0, 0.0, 1.0));
				
				vertices.push_back(Vec3(0.0, 0.0, 10.0));
				colors.push_back(Vec3(0.0, 0.0, 1.0));
				
				glLineWidth(5.0);
				
				glColorPointer(3, GL_FLOAT, 0, &colors[0]);
				glEnableClientState(GL_COLOR_ARRAY);
				
				glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
				glEnableClientState(GL_VERTEX_ARRAY);
				
				glDrawArrays(GL_LINES, 0, vertices.size());
				
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);
				
				glLineWidth(1.0);
			}

			void WireframeRenderer::render (Display::RendererT *, const std::vector<Vec3> & vertices) {
				glColor(m_color);

				glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
				glEnableClientState(GL_VERTEX_ARRAY);
				
				glDrawArrays(GL_LINES, 0, vertices.size());
				
				glDisableClientState(GL_VERTEX_ARRAY);
			}
		
			template <unsigned D>
			void WireframeRenderer_renderLineSegment(const Vec4 & color, const Geometry::LineSegment<D> & lineSegment) {
				typedef Vector<D> VectorT;
				typedef Vector<D, bool> BoolVectorT;
				
				std::vector<VectorT> vertices(2);
				std::vector<Vec4> colors(2, color);
							
				vertices[0] = lineSegment.start();
				vertices[1] = lineSegment.end();
				
				glVertexPointer(D, GL_FLOAT, 0, &vertices[0]);
				glEnableClientState(GL_VERTEX_ARRAY);
				
				glColorPointer(4, GL_FLOAT, 0, &colors[0]);
				glEnableClientState(GL_COLOR_ARRAY);
				
				glDrawArrays(GL_LINES, 0, 2);
				
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);
			
			}

			void WireframeRenderer::render (Display::RendererT *, const Geometry::LineSegment<2> & lineSegment) {
				WireframeRenderer_renderLineSegment(m_color, lineSegment);
			}
			
			void WireframeRenderer::render (Display::RendererT *, const Geometry::LineSegment<3> & lineSegment) {
				WireframeRenderer_renderLineSegment(m_color, lineSegment);				
			}

			void WireframeRenderer::render (Renderer *, const Geometry::AlignedBox<2> & b)
			{
				const unsigned D = 2;
				Vector<D, unsigned> k(IDENTITY, 2);
			
				// Total number of corners for a given D.
				const IndexT CORNERS = 1 << D;
				
				typedef Vector<D> VectorT;
				typedef Vector<D, bool> BoolVectorT;
				std::vector<VectorT> vertices(CORNERS);
				std::vector<Vec4> colors(CORNERS, m_color);
			
				const GLushort INDICES[] = {0, 1, 3, 2};
				
				for (IndexT c = 0; c < CORNERS; c += 1) {
					BoolVectorT currentCorner(k.distribute(c));
					Vector<D> vertex = b.corner(currentCorner);
					
					vertices[INDICES[c]] = vertex;
				}
				
				glVertexPointer(D, GL_FLOAT, 0, &vertices[0]);
				glEnableClientState(GL_VERTEX_ARRAY);
				
				glColorPointer(4, GL_FLOAT, 0, &colors[0]);
				glEnableClientState(GL_COLOR_ARRAY);
				
				glDrawArrays(GL_LINE_LOOP, 0, sizeof(INDICES) / sizeof(GLushort));
				
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);
			}
			
			void WireframeRenderer::render (Renderer *, const Geometry::AlignedBox<3> & b)
			{			
				const unsigned D = 3;
				Vector<D, unsigned> k(IDENTITY, 2);
			
				// Total number of corners for a given D.
				const IndexT CORNERS = 1 << D;
				
				typedef Vector<D> VectorT;
				typedef Vector<D, bool> BoolVectorT;
				std::vector<VectorT> vertices(CORNERS);
				std::vector<Vec4> colors(CORNERS, m_color);
			
				for (IndexT c = 0; c < CORNERS; c += 1) {
					BoolVectorT currentCorner(k.distribute(c));
					Vector<D> vertex = b.corner(currentCorner);
					
					vertices[c] = vertex;
				}
				
				const GLushort INDICES[] = {0, 1, 1, 3, 3, 2, 2, 0, 4, 5, 5, 7, 7, 6, 6, 4, 0, 4, 1, 5, 2, 6, 3, 7};
				
				glVertexPointer(D, GL_FLOAT, 0, &vertices[0]);
				glEnableClientState(GL_VERTEX_ARRAY);
				
				glColorPointer(4, GL_FLOAT, 0, &colors[0]);
				glEnableClientState(GL_COLOR_ARRAY);
								
				glDrawElements(GL_LINES, sizeof(INDICES) / sizeof(GLushort), GL_UNSIGNED_SHORT, INDICES);
				
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);
			}
#endif
			
			void WireframeRenderer::setPrimaryColor (const Vec4 & color)
			{
				m_color = color;
			}
			
		}
	}
}