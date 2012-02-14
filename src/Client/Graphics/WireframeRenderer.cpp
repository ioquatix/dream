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
				_vertex_array = new VertexArray;
				_vertex_buffer = new VertexBuffer;
				
				_vertex_array->bind();
				_vertex_buffer->attach(*_vertex_array);
				
				VertexArray::Attributes attributes(*_vertex_array, *_vertex_buffer);
				attributes[POSITION] = &Vertex::position;
					
				_vertex_array->unbind();
				
				check_graphics_error();
			}
			
			WireframeRenderer::~WireframeRenderer() {
				
			}
			
			void WireframeRenderer::set_program(Ptr<Program> program) {
				_program = program;
				
				if (_program) {
					_major_color_uniform = _program->uniform_location("major_color");
					_minor_color_uniform = _program->uniform_location("minor_color");
					
					_program->set_attribute_location("position", POSITION);
					
					set_major_color(Vec4(1.0, 1.0, 1.0, 1.0));
					set_minor_color(Vec4(0.4, 0.4, 0.4, 1.0));
				}
			}
			
			void WireframeRenderer::set_major_color(const Vec4 & color) {
				_major_color = color;
			}
			
			void WireframeRenderer::set_minor_color(const Vec4 & color) {
				_minor_color = color;
			}
			
			void WireframeRenderer::render(const std::vector<Vec3> & line) const {
				_program->enable();
				
				_program->set_uniform(_major_color_uniform, _major_color);
				_program->set_uniform(_minor_color_uniform, _minor_color);
				
				_vertex_array->bind();
				
				// Upload data:
				_vertex_buffer->attach(*_vertex_array);
				_vertex_buffer->assign(line, GL_STREAM_DRAW);
				
				// Render data:
				_vertex_array->draw_arrays(GL_LINE_LOOP, 0, line.size());
				
				_vertex_array->unbind();
				_program->disable();
			}
			
			void WireframeRenderer::render(const Geometry::LineSegment<2> & segment) const {
				std::vector<Vec3> line;
				line.push_back(segment.start() << 0.0);
				line.push_back(segment.end() << 1.0);
				render(line);
			}
			
			void WireframeRenderer::render(const Geometry::LineSegment<3> & segment) const {
				std::vector<Vec3> line;
				line.push_back(segment.start());
				line.push_back(segment.end());
				render(line);				
			}
			
			void WireframeRenderer::render(const Geometry::AlignedBox<2> & box, RealT z) const {
				std::vector<Vec3> edges;
				edges.push_back(box.corner(Vec2b(false, false)) << z);
				edges.push_back(box.corner(Vec2b(true, false)) << z);
				edges.push_back(box.corner(Vec2b(true, true)) << z);
				edges.push_back(box.corner(Vec2b(false, true)) << z);
				edges.push_back(box.corner(Vec2b(false, false)) << z);
				render(edges);
			}
			
			void WireframeRenderer::render(const Geometry::AlignedBox<3> & box) const {
				AlignedBox<2> profile(box.min().reduce(), box.max().reduce());
				
				render(profile, box.min()[Z]);
				render(profile, box.max()[Z]);
			}
		}
	}
}