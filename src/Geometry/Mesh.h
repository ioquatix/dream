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
	
		template <typename ElementT, unsigned N>
		std::size_t array_count(const ElementT(&)[N]) {
			return N;
		}
		
		struct VertexP3N3M2 {
			Vec3 position;
			Vec3 normal;
			Vec2 mapping;
			
			void apply(const Mat44 & position);
		};
		
		struct VertexP3N3M2C4 : public VertexP3N3M2 {
			Vec4 color;
		};
		
		enum Layout {
			POINTS = 0,
			LINES = 1,
			LINE_LOOP = 2,
			LINE_STRIP = 3,
			TRIANGLES = 4,
			TRIANGLE_STRIP = 5,
			TRIANGLE_FAN = 6
		};
		
		/// A mesh is a list of vertices and an ordered list of indices which make up a set of triangles. We assume that all meshes are made up of triangle strips. These assumptions and limitations are primarily to keep the generation of Mesh objects simple.
		template <typename _VertexT = VertexP3N3M2, typename _IndexT = uint16_t>
		class Mesh {
		public:
			typedef _IndexT IndexT;
			typedef _VertexT VertexT;
			
			Layout layout;
			std::vector<IndexT> indices;
			std::vector<VertexT> vertices;
			
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
