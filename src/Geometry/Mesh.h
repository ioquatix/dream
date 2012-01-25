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
	
		struct Vertex {
			Vec3 position;
			Vec3 normal;
			Vec4 colour;
			Vec2 mapping;
			
			void apply(const Mat44 & position);
		};
		
		template <typename ValueT>
		class Array : public std::vector<ValueT> {
		public:
			std::size_t length() const { return this->size() * sizeof(ValueT); }
		};
		
		/// A mesh is a list of vertices and an ordered list of indices which make up a set of triangles. We assume that all meshes are made up of triangle strips. These assumptions and limitations are primarily to keep the generation of Mesh objects simple.
		template <typename _VertexT = Vertex, typename _IndexT = uint16_t>
		class Mesh {
		public:
			typedef _IndexT IndexT;
			typedef _VertexT VertexT;
			
			Array<IndexT> indices;
			Array<VertexT> vertices;
			
			// Apply some kind of tranform to all vertices.
			template <typename TransformT>
			void apply(const TransformT & transform) {
				for (std::size_t i = 0; i < vertices.size(); i++)
					vertices[i].apply(transform);
			}
		};
	}
}

#include "Mesh.impl.h"

#endif
