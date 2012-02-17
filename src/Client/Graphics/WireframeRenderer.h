/*
 *  Client/Graphics/WireframeRenderer.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 22/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_WIREFRAMERENDERER_H
#define _DREAM_CLIENT_GRAPHICS_WIREFRAMERENDERER_H

#include "Graphics.h"
#include "ShaderManager.h"
#include "MeshBuffer.h"

#include "../../Geometry/AlignedBox.h"

namespace Dream
{
	namespace Client
	{
		namespace Graphics
		{
			using namespace Dream::Numerics;
			
			/// These primatives are really designed for debugging purposes.
			class WireframeRenderer : public Object {
			protected:				
				Ref<Program> _program;
				GLuint _major_color_uniform;
				GLuint _minor_color_uniform;
				
				enum Attributes {
					POSITION = 0
				};
				
				struct Vertex {
					Vec3 position;
				};
				
				Vec4 _major_color, _minor_color;
				
				VertexArray _vertex_array;
				VertexBuffer<Vertex> _vertex_buffer;
				
			public:
				WireframeRenderer();
				virtual ~WireframeRenderer();
				
				void set_program(Ptr<Program> program);
				Ref<Program> program() { return _program; }
				
				void set_major_color(const Vec4 & color);
				void set_minor_color(const Vec4 & color);
				
				void render(const std::vector<Vec3> &);
				
				void render(const Geometry::LineSegment<2> &);
				void render(const Geometry::LineSegment<3> &);
								
				void render(const Geometry::AlignedBox<2> &, RealT z = 0.0);
				void render(const Geometry::AlignedBox<3> &);
				
				void render_axis();
			};
			
		}
	}
}

#endif
