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
			
			void WireframeRenderer::render(const std::vector<Vec3> & line) {
				_program->enable();
				
				_program->set_uniform(_major_color_uniform, _major_color);
				_program->set_uniform(_minor_color_uniform, _minor_color);
				
				{
					// Upload data:
					auto binding = _vertex_buffer.binding();
					binding.set_data(line);
				}
				
				{
					// Render data:
					auto binding = _vertex_array.binding();
					binding.draw_arrays(GL_LINE_LOOP, 0, line.size());
				}
				
				_program->disable();
			}
			
			void WireframeRenderer::render(const Geometry::LineSegment<2> & segment) {
				std::vector<Vec3> line;
				line.push_back(segment.start() << 0.0);
				line.push_back(segment.end() << 1.0);
				render(line);
			}
			
			void WireframeRenderer::render(const Geometry::LineSegment<3> & segment) {
				std::vector<Vec3> line;
				line.push_back(segment.start());
				line.push_back(segment.end());
				render(line);				
			}
			
			void WireframeRenderer::render(const Geometry::AlignedBox<2> & box, RealT z) {
				std::vector<Vec3> edges;
				edges.push_back(box.corner(Vec2b(false, false)) << z);
				edges.push_back(box.corner(Vec2b(true, false)) << z);
				edges.push_back(box.corner(Vec2b(true, true)) << z);
				edges.push_back(box.corner(Vec2b(false, true)) << z);
				edges.push_back(box.corner(Vec2b(false, false)) << z);
				render(edges);
			}
			
			void WireframeRenderer::render(const Geometry::AlignedBox<3> & box) {
				AlignedBox<2> profile(box.min().reduce(), box.max().reduce());
				
				render(profile, box.min()[Z]);
				render(profile, box.max()[Z]);
			}
			
			void WireframeRenderer::render_axis() {
				Vec4 major_color = _major_color;
				
				set_major_color(Vec4(1.0, 0.0, 0.0, 1.0));
				render(LineSegment<3>(ZERO, Vec3(1.0, 0.0, 0.0)));
				
				set_major_color(Vec4(0.0, 1.0, 0.0, 1.0));
				render(LineSegment<3>(ZERO, Vec3(0.0, 1.0, 0.0)));
				
				set_major_color(Vec4(0.0, 0.0, 1.0, 1.0));
				render(LineSegment<3>(ZERO, Vec3(0.0, 0.0, 1.0)));
				
				set_major_color(major_color);
			}
		}
	}
}
