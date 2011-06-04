/*
 *  AlignedTree.cpp
 *  Dream
 *
 *  Created by Samuel Williams on 2/01/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "AlignedTree.h"
#include <Dream/Assertion.h>

namespace Dream {
	namespace Geometry {

		unsigned Quadrants::indexForPartition(const Vec2 &point, const Vec2 &center) {
			if (point[Y] < center[Y]) {
				if (point[X] < center[X])
					return 0;
				else
					return 1;
			} else {
				if (point[X] < center[X])
					return 2;
				else
					return 3;
			}
		}
		
		Vec2 Quadrants::normalOriginForPartitionIndex(const IndexT &i) {
			ensure(i < Q && "Invalid partition index!");
			
			switch (i) {
			case 0:
				return Vec2 (0.0, 0.0);
			case 1:
				return Vec2 (0.5, 0.0);
			case 2:
				return Vec2 (0.0, 0.5);
			case 3:
				return Vec2 (0.5, 0.5);
			}
			
			ensure(false && "Invalid partition index!");

			return ZERO;
		}
		
		Vec2 Quadrants::offsetOriginForPartitionIndex(const IndexT &i) {
			return ((normalOriginForPartitionIndex(i) + 0.25) * 2.0) - 0.5;
		}
		
		Quadrants::PartitionLocation Quadrants::locationForDirection (const Direction &dir) {
			switch (dir) {
			case (BOTTOM | LEFT):
				return BottomLeft;
			case (BOTTOM | RIGHT):
				return BottomRight;
			case (TOP | LEFT):
				return TopLeft;
			case (TOP | RIGHT):
				return TopRight;
			default:
				break;
			}
			
			ensure(false && "Invalid partition direction!");
			
			return BottomLeft;			
		}
		
		unsigned Octants::indexForPartition(const Vec3 &point, const Vec3 &center) {
			unsigned v = point[Z] < center[Z] ? 0 : 4;
			
			if (point[Y] < center[Y]) {
				if (point[X] < center[X])
					return v + 0;
				else
					return v + 1;
			} else {
				if (point[X] < center[X])
					return v + 2;
				else
					return v + 3;
			}
		}
		
		Vec3 Octants::normalOriginForPartitionIndex(const IndexT &i) {			
			switch (i) {
			case 0:
				return Vec3 (0.0, 0.0, 0.0);
			case 1:
				return Vec3 (0.5, 0.0, 0.0);
			case 2:
				return Vec3 (0.0, 0.5, 0.0);
			case 3:
				return Vec3 (0.5, 0.5, 0.0);
			case 4:
				return Vec3 (0.0, 0.0, 0.5);
			case 5:
				return Vec3 (0.5, 0.0, 0.5);
			case 6:
				return Vec3 (0.0, 0.5, 0.5);
			case 7:
				return Vec3 (0.5, 0.5, 0.5);
			}
			
			ensure(false && "Invalid partition index!");
			
			return ZERO;
		}
		
		Vec3 Octants::offsetOriginForPartitionIndex(const IndexT &i) {
			return ((normalOriginForPartitionIndex(i) + 0.25) * 2.0) - 0.5;
		}
		
		Octants::PartitionLocation Octants::locationForDirection (const Direction &dir) {
			switch (dir) {
			case (BOTTOM | LEFT | NEAR):
				return BottomLeftNear;
			case (BOTTOM | RIGHT | NEAR):
				return BottomRightNear;
			case (TOP | LEFT | NEAR):
				return TopLeftNear;
			case (TOP | RIGHT | NEAR):
				return TopRightNear;
				
			case (BOTTOM | LEFT | FAR):
				return BottomLeftFar;
			case (BOTTOM | RIGHT | FAR):
				return BottomRightFar;
			case (TOP | LEFT | FAR):
				return TopLeftFar;
			case (TOP | RIGHT | FAR):
				return TopRightFar;
			
			default:
				break;
			}
			
			ensure(false && "Invalid partition direction!");
			
			return BottomLeftNear;
		}
		
	}
}
