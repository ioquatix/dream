/*
 *  Mesh.cpp
 *  Model Builder
 *
 *  Created by Samuel Williams on 15/03/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#include "Mesh.h"

#include "Triangle.h"
#include "Line.h"

namespace Dream {
	namespace Geometry {
		void BasicMeshTraits::VertexProperties::apply (const Mat44 & transform) {
			vertex = transform * vertex;
		}
	}
}
