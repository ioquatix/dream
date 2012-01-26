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

namespace Dream {	
	namespace Geometry {
		namespace Generate {
			
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
			
			template <typename VertexT, typename ColorT>
			void solid_color(Mesh<VertexT> & mesh, const ColorT & color)
			{
				for (auto vertex : mesh.vertices) {
					vertex.color = color;
				}				
			}
		}
	}
}
