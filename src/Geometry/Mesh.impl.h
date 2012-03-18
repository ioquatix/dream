//
//  Geometry/Mesh.impl.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 9/03/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_GEOMETRY_MESH_H
#error This header should not be included manually. Include Mesh.h instead.
#endif

#include <algorithm>

namespace Dream {	
	namespace Geometry {
		namespace Generate {
			
			/// Size is actually a mesure from the center to the outside.
			template <typename MeshT>
			void plane(MeshT & mesh, Vec2 size) {
				// Draw with GL_TRIANGLE_FAN
				
				// We make a plane out of four triangles.

				// Center point:
				typename MeshT::VertexT vertex;
				
				vertex.position = ZERO;
				vertex.mapping = Vec2(0.5, 0.5);
				
				mesh.vertices.push_back(vertex);
				
				// The four surrounding points:
				RealT coordinates[4][2] = {
					{-1, -1}, {1, -1}, {1, 1}, {-1, 1}
				};
				
				RealT mappings[4][2] = {
					{0, 0}, {1, 0}, {1, 1}, {0, 1}
				};
				
				for (std::size_t i = 0; i < 4; i += 1) {
					typename MeshT::VertexT vertex;
					
					vertex.position.zero();
					vertex.position.set(size * Vec2(coordinates[i][0], coordinates[i][1]));
					
					vertex.mapping = Vec2(mappings[i][0], mappings[i][1]);
					
					mesh.vertices.push_back(vertex);
				}
			
				// The indices for the triangle fan:
				typename MeshT::IndexT indices[] = {
					0, 1, 2, 3, 4, 1
				};
				
				std::copy(indices, indices + (sizeof(indices) / sizeof(*indices)), std::back_inserter(mesh.indices));
			}
			
			template <typename MeshT>
			void plane(MeshT & mesh, AlignedBox2 box, Vec2u divisions) {
				for (std::size_t y = 0; y <= divisions[Y]; y += 1) {
					for (std::size_t x = 0; x <= divisions[X]; x += 1) {
						typename MeshT::VertexT vertex;
						
						vertex.position = box.min() + ((box.size() / divisions) * Vec2u(x, y));
						vertex.mapping = Vec2(x, y);
						
						mesh.vertices.push_back(vertex);
					}
				}
				
				std::size_t width = divisions[Y] + 1;
				
				for (std::size_t y = 0; y < divisions[Y]; y += 1) {
					if (y != 0) {
						// Create two degenerate triangles for the move from the end of the row back to the start:
						mesh.indices.push_back(row_major_offset(y, divisions[X], width));
						mesh.indices.push_back(row_major_offset(y, 0, width));
					}
					
					for (std::size_t x = 0; x < divisions[X]; x += 1) {
						mesh.indices.push_back(row_major_offset(y, x, width));
						mesh.indices.push_back(row_major_offset(y+1, x, width));
						mesh.indices.push_back(row_major_offset(y, x+1, width));
						mesh.indices.push_back(row_major_offset(y+1, x+1, width));
					}					
				}
			}
			
			template <typename MeshT>
			void grid(MeshT & mesh, Vec2u count, Vec2 spacing) {
				Vec2 size = (count * spacing) / 2.0;
				AlignedBox2 surface = AlignedBox2::from_center_and_size(ZERO, size * 2.0);
				
				for (RealT y = -size[Y]; y <= size[Y]; y += spacing[Y]) {
					typename MeshT::VertexT first, second;
					first.position = Vec3(-size[X], y, 0.0);
					first.normal = Vec3(0, 0, 1);
					first.mapping = surface.relative_offset_of(first.position.reduce());
					mesh.vertices.push_back(first);
					
					second.position = Vec3(size[X], y, 0.0);
					second.normal = Vec3(0, 0, 1);
					second.mapping = surface.relative_offset_of(second.position.reduce());
					mesh.vertices.push_back(second);
					
					mesh.indices.push_back(mesh.vertices.size() - 1);
					mesh.indices.push_back(mesh.vertices.size() - 2);
				}
				
				for (RealT x = -size[X]; x <= size[X]; x += spacing[X]) {
					typename MeshT::VertexT first, second;
					first.position = Vec3(x, -size[Y], 0.0);
					first.normal = Vec3(0, 0, 1);
					first.mapping = surface.relative_offset_of(first.position.reduce());
					mesh.vertices.push_back(first);
					
					second.position = Vec3(x, size[Y], 0.0);
					second.normal = Vec3(0, 0, 1);
					second.mapping = surface.relative_offset_of(second.position.reduce());
					mesh.vertices.push_back(second);

					mesh.indices.push_back(mesh.vertices.size() - 1);
					mesh.indices.push_back(mesh.vertices.size() - 2);
				}
			}
			
			template <typename MeshT>
			void sphere(MeshT & mesh, RealT radius, std::size_t stacks = 8, std::size_t slices = 8) {
				typename MeshT::IndexT index = 0;
				
				for (std::size_t stack = 0; stack <= stacks; stack += 1) {
					for (std::size_t slice = 0; slice < slices; slice += 1) {
						RealT theta = stack * (R180 / stacks);
						RealT phi = slice * (R360 / (slices - 1));
						
						RealT sin_theta = std::sin(theta);
						RealT sin_phi = std::sin(phi);
						RealT cos_theta = std::cos(theta);
						RealT cos_phi = std::cos(phi);
						
						typename MeshT::VertexT vertex;
						vertex.position = Vec3(radius * cos_phi * sin_theta, radius * sin_phi * sin_theta, radius * cos_theta);
						vertex.normal = vertex.position.normalized_vector();
						vertex.mapping = Vec2(RealT(slice) / RealT(slices - 1), RealT(stack) / RealT(stacks));
						
						mesh.vertices.push_back(vertex);
					}
				}
				
				for (std::size_t stack = 0; stack < stacks; stack += 1) {
					for (std::size_t slice = 0; slice <= slices; slice += 1) {
						mesh.indices.push_back((stack * slices) + (slice % slices));
						mesh.indices.push_back(((stack + 1) * slices) + (slice % slices));
					}
				}
			}
			
			template <typename VertexT>
			void simple_cube(Mesh<VertexT> & mesh, const AlignedBox<3> & box)
			{
				const IndexT CUBE_3X2[] = {
					1, 0, 3, 2,
					3, 2, 7, 6,
					7, 6, 5, 4,
					4, 6, 6, 6, // Degenerate
					6, 2, 4, 0,
					4, 0, 5, 1,
					5, 1, 7, 3
				};
				
				const Vector<3, unsigned> K(IDENTITY, 2);			
				const IndexT SIZE = sizeof(CUBE_3X2) / sizeof(IndexT);
				
				mesh.vertices.resize(SIZE);
				mesh.indices.resize(SIZE);

				for (IndexT c = 0; c < SIZE; c += 1) {
					//std::cout << "c: " << c << " = "  << K.distribute(c) << std::endl;
					Vector<3, bool> current_corner(K.distribute(c));

					mesh.vertices[c].position = box.corner(current_corner);
					mesh.indices[c] = CUBE_3X2[c];
				}
			}
			
			template <typename VertexT>
			void simple_cube_shade(Mesh<VertexT> & mesh)
			{
				// Look at each face
				for (IndexT c = 0; c < mesh.vertices.size(); c += 4) {
					Vec3 normal = surface_normal(mesh.vertices[c].position, mesh.vertices[c+1].position, mesh.vertices[c+2].position);
					
					mesh.vertices[c+0].normal = normal;
					mesh.vertices[c+1].normal = normal;
					mesh.vertices[c+2].normal = normal;
					mesh.vertices[c+3].normal = normal;
					
					mesh.vertices[c+0].mapping = Vec2(1, 0);
					mesh.vertices[c+1].mapping = Vec2(0, 0);
					mesh.vertices[c+2].mapping = Vec2(1, 1);
					mesh.vertices[c+3].mapping = Vec2(0, 1);
				}
			}
			
			template <typename MeshT, typename ColorT>
			void solid_color(MeshT & mesh, const ColorT & color)
			{
				for (auto & vertex : mesh.vertices) {
					vertex.color = color;
				}				
			}
			
			template <typename MeshT, typename NormalT>
			void solid_normal(MeshT & mesh, const NormalT & normal) {
				for (auto & vertex : mesh.vertices) {
					vertex.normal = normal;
				}
			}
		}
	}
}
