/*
 *  Mesh.h
 *  Model Builder
 *
 *  Created by Samuel Williams on 15/03/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

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
				std::vector<IndexT> m_indices;
				std::vector<Vec3> m_vertices;
				std::vector<Vec3> m_normals;
				std::vector<Vec2> m_texcoords;
				
			public:
				void construct(const std::vector<IndexT> & indices, const std::vector<Vec3> & vertices, const std::vector<Vec3> & normals, const std::vector<Vec2> & texcoords)
				{
					m_indices = indices;
					m_vertices = vertices;
					m_normals = normals;
					m_texcoords = texcoords;
				}
				
				const std::vector<IndexT> & indices() const { return m_indices; }
				const std::vector<Vec3> & vertices() const { return m_vertices; }
				const std::vector<Vec3> & normals() const { return m_normals; }				
				const std::vector<Vec2> & texcoords() const { return m_texcoords; }				
		};
		
	}
}

#include "Mesh.impl.h"

#endif
