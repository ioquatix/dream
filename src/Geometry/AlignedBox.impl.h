//
//  Geometry/AlignedBox.impl.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/12/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_GEOMETRY_ALIGNEDBOX_H
#error This header should not be included manually. Include AlignedBox.h instead.
#endif

#include "Line.h"
#include "Sphere.h"

namespace Dream
{
	namespace Geometry
	{
		
		template <unsigned D, typename NumericT>
		void AlignedBox<D, NumericT>::setCorner (const BoolVectorT & cnr, const VectorT & adj)
		{
			for (IndexT axis = 0; axis < D; axis += 1)
				setCorner(cnr, axis, adj[axis]);
		}
		
		template <unsigned D, typename NumericT>
		void AlignedBox<D, NumericT>::setCorner (const BoolVectorT & cnr, const unsigned & axis, const NumericT & amnt)
		{
			if (cnr[axis])
				m_max[axis] = amnt;
			else
				m_min[axis] = amnt;
		}
		
#pragma mark Line Intersection Tests
		
		template <unsigned D, typename NumericT>
		Sphere<D, NumericT> AlignedBox<D, NumericT>::boundingSphere () const
		{
			return Sphere<D, NumericT>(center(), (center() - m_min).length()); 
		}
		
		template <unsigned D, typename NumericT>
		void AlignedBox<D, NumericT>::subtractInOrder (const AlignedBox & other, const Vector<D, unsigned> & order) {
			subtractInOrder(other, order, Vector<D, SubtractResolution> (SUBTRACT_SMALLEST, SUBTRACT_SMALLEST, SUBTRACT_SMALLEST));
		}
		
		template <unsigned D, typename NumericT>
		void AlignedBox<D, NumericT>::subtractInOrder (const AlignedBox & other, const Vector<D, unsigned> & order, const Vector<D, SubtractResolution> & cuts) 
		{
			// This function is fairly complex, for a good reason - it does a fairly complex geometric operation.
			// This operation can be summarised as subtracting one box from another. The reason why this is complex is because there are many edge cases to
			// consider, and this function works for boxes in n dimensions.
			
			Vector<D, unsigned> k(IDENTITY, 2);
			
			// Total number of corners for a given D.
			const IndexT CORNERS = 1 << D;
			
			// We consider every corner and its opporsite.
			// We do this because we need to consider if the corner intersects the other shape.
			for (IndexT c = 0; c < CORNERS; c += 1) {
				// We consider both the given corner and its opporsite
				BoolVectorT currentCorner(k.distribute(c));
				BoolVectorT opporsiteCorner(!currentCorner);
				
				VectorT thisCurrentCorner(corner(currentCorner)), thisOpporsiteCorner(corner(opporsiteCorner));
				VectorT otherCurrentCorner(other.corner(currentCorner)), otherOpporsiteCorner(other.corner(opporsiteCorner));
				
				bool intersects = containsPoint(otherCurrentCorner, true) || other.containsPoint(thisOpporsiteCorner, true);
				
				// We consider each axis for these corners
				for (IndexT axisIndex = 0; axisIndex < D; axisIndex += 1) {
					// We pick out the current axis
					unsigned axis = order[axisIndex];
					
					// We consider the lines on this axis
					NumericT a1 = thisCurrentCorner[axis];
					NumericT a2 = thisOpporsiteCorner[axis];
					NumericT b1 = otherCurrentCorner[axis];
					NumericT b2 = otherOpporsiteCorner[axis];
					
					// We copy the corner vectors just in case we need to swap them
					BoolVectorT c1(currentCorner), c2(opporsiteCorner);
					
					// We need to compare things relatively, so a1 should be smaller than a2.
					// if not, we swap everything around.
					if (a1 > a2) {
						std::swap(a1, a2);
						std::swap(b1, b2);
						std::swap(c1, c2);
					}
					
					if (b1 > a1 && b2 >= a2 && intersects) {
						// Remove the right hand segment a2 = b1
						setCorner(c2, axis, b1);
						break;
					}
					
					if (b1 <= a1 && b2 < a2 && intersects) {
						// Remove the left hand segment a1 = b2
						setCorner(c1, axis, b2);
						break;
					}
					
					if (a1 < b1 && a2 > b2 && intersectsWith(other)) {
						// The line is being split in two by the subtracted segment.
						// We will choose the larger segment
						RealT leftSide = b1 - a1;
						RealT rightSide = a2 - b2;
						
						// We use cuts to determine how to resolve these cases with more than one result
						if (cuts[axis] == SUBTRACT_POSITIVE || (leftSide > rightSide && cuts[axis] == SUBTRACT_SMALLEST)) {
							// remove right side (positive side)
							setCorner(c2, axis, b1);
						} else {
							// remove left side (negative side)
							setCorner(c1, axis, b2);
						}
						
						break;
					}
					
					if (a1 > b1 && a2 < b2) {
						// This case is when we are subtracting more than we have available,
						// ie, both ends of the line are within the subtracting bounds
						// There is nothing we can do reasonably within this case, so we just ignore it
						// and hope for the best ^_^.
					}
				}
			}			
		}
		
		template <unsigned D, typename NumericT>
		bool AlignedBox<D, NumericT>::subtractEdge (const AlignedBox<D, NumericT> & other, unsigned axis, const BoxEdge & edge, const NumericT & offset)
		{
			NumericT a, b, c;
			
			// Offset indicates the distance the edge must be from the other box if they are on top of each other, ie a == b, then a -> a + offset
			
			if (edge == NEGATIVE_EDGE) {
				a = m_min[axis];
				b = other.m_max[axis];
				c = m_max[axis];
				
				if ((b + offset) < c && compareEdge(a, b, !Numerics::equalWithinTolerance(offset, (NumericT)0.0))) {
					//std::cout << "Adjusting [" << axis << "] from " << a << " to " << b - offset << std::endl;
					m_min[axis] = b + offset;
					return true;
				}
			} else {
				a = m_max[axis];
				b = other.m_min[axis];
				c = m_min[axis];
				
				if ((b + offset) > c && compareEdge(b, a, !Numerics::equalWithinTolerance(offset, (NumericT)0.0))) {
					//std::cout << "Adjusting [" << axis << "] from " << a << " to " << b - offset << std::endl;
					m_max[axis] = b - offset;
					return true;
				}
			}
			
			//std::cout << "Did not adjust [" << axis << "] from " << a << " to " << b - offset << " other " << c << std::endl;
			return false;
		}
		
		template <unsigned D, typename NumericT>
		AlignedBox<D, NumericT> AlignedBox<D, NumericT>::subtractUsingTranslation (const AlignedBox<D, NumericT> & from, const AlignedBox<D, NumericT> & to, 
																				   const NumericT & offset) 
		{
			VectorT orientation = from.orientationOf(to);
			AlignedBox translation = from;
			
			for (unsigned i = 0; i < D; ++i) {
				AlignedBox toCopy = to;
				bool result = false;
				
				if (orientation[i] == 0.0) {
					result = toCopy.subtractEdge(*this, i, NEGATIVE_EDGE, offset);
				} else if (orientation[i] == 1.0) {
					result = toCopy.subtractEdge(*this, i, POSITIVE_EDGE, offset);				
				}
				
				if (result) {
					toCopy.unionWithBox(translation);
					
					result = toCopy.intersectsWith(*this, false);
					
					if (!result)
						translation.unionWithBox(toCopy);
				}
			}
			
			return translation;
		}
		
	}
}
