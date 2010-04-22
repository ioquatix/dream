/*
 *  Geometry/Line.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 15/03/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_GEOMETRY_LINE_H
#define _DREAM_GEOMETRY_LINE_H

#include "Geometry.h"
#include "../Numerics/Matrix.h"

namespace Dream {
	namespace Geometry {
		
		template <unsigned D, typename NumericT> 
		class LineTranslationTraits
		{
		};
		
		template <typename NumericT>
		class LineTranslationTraits<3, NumericT> {				
		public:
			typedef typename RealType<NumericT>::RealT NumericRealT;
			typedef Matrix<4, 4, NumericRealT> MatrixT;
			typedef Vector<3, NumericRealT> VectorT;
			typedef Line<3, NumericT> LineT;
			
			MatrixT transformationToMateWithLine (const LineT & other, const VectorT & normal) const;
			MatrixT translationToMateWithPoint (const VectorT & point) const;
			MatrixT rotationToMateWithDirection (const VectorT & direction, const VectorT & normal) const;
			
			void rotate (const VectorT & rotationNormal, const NumericRealT & angle);
			LineT rotatedLine (const VectorT & rotationNormal, const NumericRealT & angle) const;
		};
		
		template <unsigned D, typename NumericT>
		class Line : public LineTranslationTraits<D, NumericT> {			
		protected:
			typedef typename RealType<NumericT>::RealT NumericRealT;
			typedef Vector<D, NumericT> VectorT;
		
			VectorT m_point;
			VectorT m_direction;
			
		public:
			Line ();
			Line (const Zero &);
			Line (const Identity &, const NumericT & n = 1);
			
			Line (const VectorT & direction);
			Line (const VectorT & point, const VectorT & direction);
			
			const VectorT & point () const
			{
				return m_point;
			}
			
			const VectorT & direction () const
			{
				return m_direction;
			}
			
			void setPoint (const VectorT & point)
			{
				m_point = point;
			}
			
			void setDirection (const VectorT & direction)
			{
				m_direction = direction;
			}
			
			VectorT pointAtTime (const NumericT & t) const
			{
				return point() + (direction() * t);	
			}
			
			/// Returns the time on the line where a point is closest to the given point.
			NumericT timeForClosestPoint (const VectorT & p3) const;
			
			VectorT pointForClosestPoint (const VectorT & p) const
			{
				return pointAtTime(timeForClosestPoint(p));
			}
			
			NumericRealT shortestDistanceToPoint (const VectorT &p) const
			{			
				return (p - pointForClosestPoint(p)).length();
			}
			
			// Calculates the factor for line equations
			NumericRealT factor (const NumericRealT & v, IndexT i) const
			{
				return (v + m_point[i]) / (NumericRealT)m_direction[i];
			}
			
			Line<D-1, NumericT> reduce () const
			{
				return Line<D-1, NumericT> (point().reduce(), direction().reduce());
			}
			
			bool equivalent (const Line<D, NumericT> & other)
			{
				// Are we pointing in the same direction
				if (!m_direction.equivalent(other.m_direction))
					return false;
				
				// Is the distance between the parallel lines equivalent to zero?
				return Number<NumericT>::equivalent(shortestDistanceToPoint(other.m_point), 0);
			}
			
			bool intersectsWith (const Line<D, NumericT> & other, NumericT & thisTime, NumericT & otherTime) const;
			bool intersectsWith (const AlignedBox<D, NumericT> & other, NumericT & t1, NumericT & t2) const;
			
			/// Helper function for intersection testing where less information is needed.
			bool intersectsWith (const AlignedBox<D, NumericT> & other, VectorT & at) const
			{
				NumericT t1, t2;
				
				bool result = intersectsWith(other, t1, t2);
				if (result) at = pointAtTime(t1);
				
				return result;
			}
			
			///@todo Implement this function
			/// bool clipToBox (const AlignedBox<D, NumericT> & other, LineSegment<D, NumericT> & segment) const;
			
			/// Construct a line given two points
			static Line from (const VectorT & from, const VectorT & to) {
				return Line(from, (to - from).normalize());
			}
		};

		typedef Line<2> Line2;
		typedef Line<3> Line3;

		template <unsigned D, typename NumericT>
		inline Line<D, NumericT> operator+ (const Line<D, NumericT> & l, const Vector<D, NumericT> & v);
		
		template <unsigned D, typename NumericT>
		Line<D, NumericT> operator- (const Line<D, NumericT> & l, const Vector<D, NumericT> & v);

		template <unsigned D, typename NumericT>
		class LineSegment {
		public:
			typedef typename RealType<NumericT>::RealT NumericRealT;
			typedef Vector<D, NumericT> VectorT;
			
		protected:
			VectorT m_start;
			VectorT m_end;
			
		public:
			LineSegment ();
			LineSegment (const Zero &);
			LineSegment (const Identity &, const NumericT & n = 1);
			
			LineSegment (const Line<D, NumericT> & line, const NumericT & startTime, const NumericT & endTime);
			LineSegment (const VectorT & start, const VectorT & end);
			
			VectorT pointAtTime(const RealT & t) const
			{
				return m_start + (offset() * t);
			}
			
			Line<D, NumericT> toLine () const
			{
				return Line<D, NumericT>(start(), offset().normalize()); 
			}
			
			/// Is this segment zero-length?
			bool isDegenerate ()
			{
				return m_start.equivalent(m_end);
			}
			
			bool intersectsWith (const AlignedBox<D, NumericT> & other, VectorT & at) const;
			bool intersectsWith (const LineSegment<D, NumericT> & other, NumericT & thisTime, NumericT & otherTime) const;
			bool intersectsWith (const LineSegment<D, NumericT> & other, LineSegment<D, NumericT> & overlap) const;
			
			const VectorT & start () const
			{
				return m_start;
			}
			
			const VectorT & end () const
			{
				return m_end;
			}
			
			VectorT & start ()
			{
				return m_start;
			}
			
			VectorT & end ()
			{
				return m_end;
			}
			
			VectorT center ()
			{
				return (m_start + m_end) / 2.0;
			}
			
			VectorT offset () const
			{
				return m_end - m_start;
			}
			
			Vector<D> direction () const
			{
				return (m_end - m_start).normalizedVector();
			}

		};

		typedef LineSegment<2> LineSegment2;
		typedef LineSegment<3> LineSegment3;
	}
}

#include "Line.impl.h"

#endif