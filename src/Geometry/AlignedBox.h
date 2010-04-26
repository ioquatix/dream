/*
 *  Geometry/AlignedBox.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 3/01/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_GEOMETRY_ALIGNEDBOX_H
#define _DREAM_GEOMETRY_ALIGNEDBOX_H

#include "Geometry.h"

namespace Dream {

	namespace Geometry {
		
		enum SubtractResolution {
			SUBTRACT_SMALLEST = 0,
			SUBTRACT_BIGGEST = 1,
			SUBTRACT_NEGATIVE = 2,
			SUBTRACT_POSITIVE = 3
		};
		
		enum BoxEdge {
			NEGATIVE_EDGE = 2,
			POSITIVE_EDGE = 3
		};
		
		/** An Axis Aligned Bounding Box.
		 
		 This class provides many different kinds of functionality.
		 
		 @sa Line
		 @sa LineSegment
		 */
		template <unsigned D, typename NumericT>
		class AlignedBox
		{
		public:
			typedef Vector<D, NumericT> VectorT;
			typedef Vector<D, bool> BoolVectorT;
		
		protected:			
			VectorT m_min, m_max;
			
			// This could be optimised by using a function ptr rather than a bool, but need to profile!
			inline bool compareEdge (const NumericT &a, const NumericT &b, bool allowEquality) const {
				if (allowEquality)
					return a <= b;
				else
					return a < b;
			}
			
		public:
#pragma mark Constructors
			AlignedBox ()
			{
			}
			
			AlignedBox (const Zero &) : m_min(ZERO), m_max(ZERO)
			{
			}
			
			AlignedBox (const Identity &, const NumericT &n = 1) : m_min(ZERO), m_max(IDENTITY, n)
			{
			}
			
			AlignedBox (const VectorT &min, const VectorT &max) : m_min(min), m_max(max)
			{
			}
			
			static AlignedBox fromCenterAndSize (const VectorT &center, const VectorT &size)
			{
				AlignedBox r;
				r.setCenterAndSize(center, size);
				return r;
			}
			
			// Bounds m_min and m_max must infact be minima and maxima.
			// This function establishes this condition.
			void normalizeBounds () {
				VectorT t(ZERO);
				VectorT c(m_min);
				
				m_min.constrain(m_max, t - 1);
				m_max.constrain(c, t + 1);	
			}
			
			// Copy Semantics
			template <typename M>
			AlignedBox (const AlignedBox<D,M> &other) : m_min(other.min()), m_max(other.max()) {}
			
			template <typename M>
			AlignedBox & operator= (const AlignedBox<D,M> &other) {
				m_min = other.min(); m_max = other.max();
			}
			
			/// Returns true if the box does not enclose any space.
			bool isDegenerate () const { return m_min == m_max; }
			
			/// Returns true if the box is in normal form - i.e. the minimum point is lesser than the maximum point.
			bool isNormal () const { return m_min.lessThan(m_max); }
			
			/// Test for exact equivalence
			bool operator== (const AlignedBox & other) const
			{
				return min() == other.min() && max() == other.max();
			}
			
			bool operator!= (const AlignedBox & other) const
			{
				return !(*this == other);
			}
			
			bool equivalent (const AlignedBox & other) const
			{
				return min().equivalent(other.min()) && max().equivalent(other.max());
			}
			
			/// Returns the minimum point of the box.
			const VectorT &min () const { return m_min; }
			
			/// Returns the maximum point of the box.
			VectorT &min () { return m_min; }
			
			/// Returns the minimum point of the box.
			const VectorT &max () const { return m_max; }
			
			/// Returns the maximum point of the box.
			VectorT &max () { return m_max; }
			
			/// Returns the origin of the box.
			/// @sa min()
			const VectorT& origin () const { return m_min; }
			
			/// Returns the center of the box.
			VectorT center () const { return (m_min + m_max) / NumericT(2); }
			
			/// Returns the size of the box.
			VectorT size () const { return m_max - m_min; }
			
			/// Set a particular corner.
			void setCorner (const BoolVectorT & cnr, const VectorT & adj);
			
			/// Set the value for a particular axis for a given corner.
			void setCorner (const BoolVectorT & cnr, const unsigned & axis, const NumericT & amnt);
			
			/// Set the center and size of the box.
			/// @sa fromCenterAndSize()
			void setCenterAndSize (const VectorT &center, const VectorT &size)
			{
				m_min = center - (size/2);
				m_max = center + (size/2);
			}
			
			/// Resize the box but keep the same origin.
			void setSizeFromOrigin (const VectorT & size)
			{
				m_max = m_min + size;
			}
			
			/// Set a new origin and size.
			void setOriginAndSize (const VectorT &origin, const VectorT &size)
			{
				m_min = origin;
				this->setSizeFromOrigin(size);
			}
			
			/// Set a new origin, maintain the current size.
			/// @sa setOriginAndSize
			void setOrigin (const VectorT & origin) {
				this->setOriginAndSize(origin, this->size());
			}
			
			/// Recenter the box, maintaining current size.
			AlignedBox& recenterAt (const VectorT & center)
			{
				Vector<D> halfCurrentSize = size() / 2;
				m_min = center - halfCurrentSize;
				m_max = center + halfCurrentSize;
				
				return *this;
			}
			
			/// Translate the box in-place, maintaining the current size.
			AlignedBox& translateBy (const VectorT & offset)
			{
				m_min += offset;
				m_max += offset;
				
				return *this;
			}
			
			/// Return a copy of this box, translated by the given offset.
			AlignedBox translatedBy (const VectorT & offset) const {
				return AlignedBox(*this).translateBy(offset);
			}
			
			/// Adjust an individual axis of the box by the given amount
			void shiftAxis (unsigned axis, const NumericT &amount) {
				m_min[axis] += amount;
				m_max[axis] += amount;
			}
			
			/// Return a particular corner of the box, given by an index vector
			/// The components of d must be either -1 or +1, and this will select the value from either min or max respectively.
			inline VectorT constrainedCorner (const Vector<D, int> & d) const {
				Vector<D> tmp (m_min);
				tmp.constrain(m_max, d);
				return tmp;
			}
			
			inline VectorT corner (const BoolVectorT & d) const {
				VectorT result;
				
				for (IndexT i = 0; i < D; i += 1)
					result[i] = d[i] ? m_max[i] : m_min[i];
				
				return result;
			}
			
			/// Expand the box to include the other point.
			AlignedBox& unionWithPoint (const VectorT & point) {
				Vector<D> t(ZERO);
				
				m_min.constrain(point, t - 1);
				m_max.constrain(point, t + 1);
				
				return *this;
			}
			
			/// Expand the box to include the other box.
			AlignedBox& unionWithBox (const AlignedBox & other) {
				Vector<D> t(ZERO);
				
				m_min.constrain(other.min(), t - 1);
				m_max.constrain(other.max(), t + 1);
				
				return *this;
			}
			
			/// Clip the current box to the given box. This function is very simple.
			/// Case 1: this and other do not intersect; result is box the same as other
			/// Case 2: this and other intersect partially; the result is that this will be resized to fit completely within other
			/// Case 3: this is completely within other; no change will occur.
			/// @sa intersectsWith()
			AlignedBox& clipToBox (const AlignedBox & other)
			{
				Vector<D> t(ZERO);
				
				m_min.constrain(other.m_min, t + 1);
				m_max.constrain(other.m_max, t - 1);
				
				return *this;
			}
			
			/// Given an orientation, aligns this box within a superbox.
			void alignWithinSuperBox (const AlignedBox & superBox, const Vector<D> & orientation)
			{
				for (unsigned i = 0; i < D; ++i) {
					RealT width = m_max[i] - m_min[i];
					RealT superWidth = superBox.m_max[i] - superBox.m_min[i];
					RealT scale = superWidth - width;
					RealT offset = orientation[i] * scale;
					RealT distance = (superBox.m_min[i] + offset) - m_min[i];
					
					shiftAxis(i, distance);
				}
			}
			
			/// Returns the orientation of one box relative to another.
			Vector<D> orientationOf (const AlignedBox &other) const {
				Vector<D> o;
				
				for (unsigned i = 0; i < D; ++i) {
					if (other.m_min[i] < m_min[i])
						o[i] = 0;
					else if (other.m_max[i] > m_max[i])
						o[i] = 1;
					else {
						RealT minWidth = other.m_max[i] - other.m_min[i];
						RealT thisWidth = m_max[i] - m_min[i];
						RealT s = thisWidth - minWidth;
						RealT offset = other.m_min[i] - m_min[i];
						
						if (s == 0)
							o[i] = 0.5;
						else
							o[i] = offset / s;
					}
				}
				
				return o;
			}
			
			/// Returns a sphere that encloses the entire box.
			Sphere<D, NumericT> boundingSphere () const;
			
			/// Tests whether this is completely within other.
			/// @returns true when this is within other, depending on the includesEdges parameter.
			/// @sa containsPoint()
			bool containsBox (const AlignedBox<D,NumericT> & other, bool includesEdges = true) const {
				return containsPoint(other.min(), includesEdges) && containsPoint(other.max(), includesEdges);
			}
			
			/// Tests whether the box contains a point.
			/// @returns true when the point is within the box, or on an edge, depending on the includesEdges parameter.
			bool containsPoint (const Vector<D>& point, bool includesEdges = true) const
			{
				bool result = false;
				
				if (!includesEdges)
					result = point.lessThan(max()) && point.greaterThan(min());
				else
					result = point.lessThanOrEqual(max()) && point.greaterThanOrEqual(min());
				
				return result;
			}
			
			/// Tests whether this box intersects with another box.
			/// @returns true when the two boxes overlap, or edges touch, depending on includesEdges parameter.
			bool intersectsWith (const AlignedBox<D,NumericT> & other, bool includesEdges = true) const
			{
				for (unsigned i = 0; i < D; ++i) {
					if (compareEdge(m_max[i], other.m_min[i], !includesEdges) || compareEdge(other.m_max[i], m_min[i], !includesEdges))
						return false;
				}
				
				return true;	
			}
			
			// Ordered subtraction methods
			void subtractInOrder (const AlignedBox & other, const Vector<D, unsigned> & order);
			void subtractInOrder (const AlignedBox & other, const Vector<D, unsigned> & order, const Vector<D, SubtractResolution> & cuts);
			
			// Translation based subtraction methods
			// These methods assume that only the edges specified by the orientation may overlap. For a more general
			// approach, the old methods may be more useful.
			// These methods remove the need for a lot of complex maths, and thus are faster. However, subtractInOrder
			// Is guaranteed to work in ALL cases.
			AlignedBox subtractUsingTranslation (const AlignedBox & from, const AlignedBox & to, const NumericT & offset = 0);
			
			// This just subtracts a single edge from another box, essentially a helper for subtractUsingTranslation
			bool subtractEdge (const AlignedBox & other, unsigned axis, const BoxEdge & edge, const NumericT & offset = 0);		
		};
		
		template <unsigned D, typename NumericT>
		std::ostream &operator<<(std::ostream &o, const AlignedBox<D,NumericT> &space) {
			o << "(" << space.min() << " -> " << space.max() << ")";
			
			return o;
		}
		
		typedef AlignedBox<2, RealT> AlignedBox2;
		typedef AlignedBox<3, RealT> AlignedBox3;
		typedef AlignedBox<2, int> AlignedBox2i;
		typedef AlignedBox<3, int> AlignedBox3i;
		typedef AlignedBox<2, unsigned> AlignedBox2u;
		typedef AlignedBox<3, unsigned> AlignedBox3u;
	}
}

#include "AlignedBox.impl.h"

#endif