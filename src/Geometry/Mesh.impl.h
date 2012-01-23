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
		
		const IndexT CUBE_3X2[] = {
			1, 0, 3, 2,
			3, 2, 7, 6,
			7, 6, 5, 4,
			4, 6, 6, 6,
			6, 2, 4, 0,
			4, 0, 5, 1,
			5, 1, 7, 3
		};
	
		template <typename MeshT>
		void generate_cube(MeshT & mesh, const AlignedBox<3> & box)
		{
			const Vector<3, unsigned> K(IDENTITY, 2);			
			const IndexT SIZE = sizeof(CUBE_3X2) / sizeof(IndexT);

			std::vector<Vec3> vertices(SIZE);

			for (IndexT c = 0; c < SIZE; c += 1) {
				//std::cout << "c: " << c << " = "  << K.distribute(c) << std::endl;
				Vector<3, bool> current_corner(K.distribute(c));

				vertices[c] = box.corner(current_corner);
			}
			
			mesh.construct(std::vector<IndexT>(CUBE_3X2, CUBE_3X2 + SIZE), vertices);
		}
		
		template <typename MeshT>
		void generate_cubeWithNormals(MeshT & mesh, const AlignedBox<3> & box)
		{
			const Vector<3, unsigned> K(IDENTITY, 2);
			const IndexT SIZE = sizeof(CUBE_3X2) / sizeof(IndexT);

			std::vector<IndexT> indices(SIZE);
			std::vector<Vec3> vertices(SIZE);
			std::vector<Vec3> normals(SIZE, Vec3(ZERO));
			std::vector<Vec2> texcoords(SIZE);

			for (IndexT c = 0; c < SIZE; c += 1) {
				Vector<3, bool> current_corner(K.distribute(CUBE_3X2[c]));
				
				vertices[c] = box.corner(current_corner);
				indices.push_back(c);
			}

			for (IndexT c = 0; c < vertices.size(); c += 4) {
				Vec3 normal = surface_normal(vertices[c], vertices[c+1], vertices[c+2]);
				
				normals[c+0] = normal;
				normals[c+1] = normal;
				normals[c+2] = normal;
				normals[c+3] = normal;
				
				texcoords[c+0] = Vec2(1, 0);
				texcoords[c+1] = Vec2(0, 0);
				texcoords[c+2] = Vec2(1, 1);
				texcoords[c+3] = Vec2(0, 1);				
			}
			
			mesh.construct(indices, vertices, normals, texcoords);
		}
	}
}
