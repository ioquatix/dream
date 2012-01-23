//
//  Geometry/Frustum.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 18/10/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_GEOMETRY_FRUSTUM_H
#define _DREAM_GEOMETRY_FRUSTUM_H

#include "Plane.h"
#include "../Numerics/Matrix.h"

namespace Dream {
	namespace Geometry {
		
		template <typename NumericT = RealT>
		class Frustum {
	public:			
			typedef Plane<3, NumericT> PlaneT;
	protected:
			PlaneT m_planes[6];

	public:
			const PlaneT & operator[] (unsigned i) const
			{
				return m_planes[i];
			}
			
			void buildFrustrumFromMatrix (const Matrix<4, 4, NumericT> &);
			
			bool intersectsWith (const Sphere<3, NumericT> &s) const;
			bool intersectsWith (const AlignedBox<3, NumericT> &b) const;
			//bool intersectsWith (const Triangle &t) const;
			
			//bool containsPoint (const Vec3 &p) const;
		};
	
	}
}

#include "Frustrum.impl.h"

#endif
