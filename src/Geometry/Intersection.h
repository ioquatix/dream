/*
 *  Geometry/Intersection.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 17/11/08.
 *  Copyright 2008 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_GEOMETRY_INTERSECTION_H
#define _DREAM_GEOMETRY_INTERSECTION_H

#include "Geometry.h"

namespace Dream
{
	namespace Geometry
	{
	
		/// Intersection between two line segments in 1-space.
		/// See http://eli.thegreenplace.net/2008/08/15/intersection-of-1d-segments/ for more information.
		/// @returns true if segments overlapped, and the overlapping portion in <tt>overlap</tt>
		template <typename NumericT>
		bool segmentsIntersect (const Vector<2, NumericT> & s1, const Vector<2, NumericT> & s2, Vector<2, NumericT> & overlap)
		{
			if (s1[1] >= s2[0] && s2[1] >= s1[0])
			{
				overlap[0] = std::max(s1[0], s2[0]);
				overlap[1] = std::min(s1[1], s2[1]);
				
				return true;
			}
			else
			{
				return false;
			}
		}
		
	}
}

#endif