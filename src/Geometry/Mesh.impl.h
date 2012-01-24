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
			
			template <typename VertexT>
			void cube(Mesh<VertexT> & mesh, const AlignedBox<3> & box)
			{
				const IndexT CUBE_3X2[] = {
					1, 0, 3, 2,
					3, 2, 7, 6,
					7, 6, 5, 4,
					4, 6, 6, 6,
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
			void shade_square_cube(Mesh<VertexT> & mesh)
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
