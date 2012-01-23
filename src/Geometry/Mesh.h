//
//  Geometry/Mesh.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 15/03/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_GEOMETRY_MESH_H
#define _DREAM_GEOMETRY_MESH_H

#include "Geometry.h"
#include "AlignedBox.h"

#include <set>
#include <vector>

namespace Dream {
	namespace Geometry {
	
		class SimpleMesh
		{
			protected:
				std::vector<IndexT> _indices;
				std::vector<Vec3> _vertices;
				std::vector<Vec3> _normals;
				std::vector<Vec2> _texcoords;
				
			public:
				void construct(const std::vector<IndexT> & indices, const std::vector<Vec3> & vertices, const std::vector<Vec3> & normals, const std::vector<Vec2> & texcoords)
				{
					_indices = indices;
					_vertices = vertices;
					_normals = normals;
					_texcoords = texcoords;
				}
				
				const std::vector<IndexT> & indices() const { return _indices; }
				const std::vector<Vec3> & vertices() const { return _vertices; }
				const std::vector<Vec3> & normals() const { return _normals; }				
				const std::vector<Vec2> & texcoords() const { return _texcoords; }				
		};
		
	}
}

#include "Mesh.impl.h"

#endif
