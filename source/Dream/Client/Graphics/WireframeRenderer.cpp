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
			WireframeRenderer::WireframeRenderer() {
				{
					auto binding = _vertex_array.binding();

					auto attributes = binding.attach(_vertex_buffer);
					attributes[POSITION] = &Vertex::position;
				}

				check_graphics_error();
			}

			WireframeRenderer::~WireframeRenderer() {
			}

			void WireframeRenderer::render(const std::vector<Vec3> & line, Layout layout) {
				{
					// Upload data:
					auto binding = _vertex_buffer.binding<Vec3>();
					binding.set_data(line);
				}

				{
					// Render data:
					auto binding = _vertex_array.binding();
					binding.draw_arrays((GLenum)layout, 0, line.size());
				}
			}

			void WireframeRenderer::render(const LineSegment<2> & segment) {
				std::vector<Vec3> line;
				line.push_back(segment.start() << 0.0);
				line.push_back(segment.end() << 0.0);
				render(line);
			}

			void WireframeRenderer::render(const LineSegment<3> & segment) {
				std::vector<Vec3> line;
				line.push_back(segment.start());
				line.push_back(segment.end());
				render(line);
			}

			void WireframeRenderer::render(const AlignedBox<2> & box, RealT z, Layout layout) {
				std::vector<Vec3> edges;
				edges.push_back(box.corner(Vec2b(false, false)) << z);
				edges.push_back(box.corner(Vec2b(true, false)) << z);
				edges.push_back(box.corner(Vec2b(true, true)) << z);
				edges.push_back(box.corner(Vec2b(false, true)) << z);
				edges.push_back(box.corner(Vec2b(false, false)) << z);
				render(edges, layout);
			}

			void WireframeRenderer::render(const AlignedBox<3> & box) {
				AlignedBox<2> profile(box.min().reduce(), box.max().reduce());

				render(profile, box.min()[Z]);
				render(profile, box.max()[Z]);
			}

			void WireframeRenderer::render_axis() {
				render(LineSegment<3>(ZERO, Vec3(1.0, 0.0, 0.0)));
				render(LineSegment<3>(ZERO, Vec3(0.0, 1.0, 0.0)));
				render(LineSegment<3>(ZERO, Vec3(0.0, 0.0, 1.0)));
			}
		}
	}
}
