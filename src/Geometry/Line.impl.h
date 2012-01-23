//
//  Geometry/Line.impl.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 16/11/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_GEOMETRY_LINE_H
#error This header should not be included manually. Include Line.h instead.
#endif

#include "Intersection.h"

namespace Dream
{
	namespace Geometry
	{
	
#pragma mark -
#pragma mark class LineTranslationTraits
		
		template <typename NumericT>
		typename LineTranslationTraits<3, NumericT>::MatrixT LineTranslationTraits<3, NumericT>::transformationToMateWithLine (const LineT & other, const VectorT & normal) const 
		{
			const LineT * t = static_cast<const LineT*> (this);
			
			// Optimization
			if (t->direction() == other.direction())
				return translationToMateWithPoint(other.point());
			else {
				MatrixT translationToOrigin = Mat44::translatingMatrix(-t->point());
				MatrixT translationFromOrigin = Mat44::translatingMatrix(t->point());
				return translationToMateWithPoint(other.point()) * translationFromOrigin * rotationToMateWithDirection(other.direction(), normal) * translationToOrigin;
			}
		}
		
		template <typename NumericT>
		typename LineTranslationTraits<3, NumericT>::MatrixT LineTranslationTraits<3, NumericT>::translationToMateWithPoint (const VectorT & point) const
		{
			const LineT * t = static_cast<const LineT*> (this);
			
			return MatrixT::translatingMatrix(point - t->point());			
		}
		
		template <typename NumericT>
		typename LineTranslationTraits<3, NumericT>::MatrixT LineTranslationTraits<3, NumericT>::rotationToMateWithDirection (const VectorT & direction, const VectorT & normal) const
		{
			const LineT * t = static_cast<const LineT*> (this);
			
			return MatrixT::rotatingMatrix(direction, t->direction(), normal);
		}
		
		template <typename NumericT>
		void LineTranslationTraits<3, NumericT>::rotate (const VectorT & rotationNormal, const NumericRealT & angle)
		{
			LineT * t = static_cast<LineT*> (this);
			
			MatrixT rotation = MatrixT::rotatingMatrix(angle, rotationNormal);
			
			t->setDirection(rotation * t->direction());
		}
		
		template <typename NumericT>
		typename LineTranslationTraits<3, NumericT>::LineT LineTranslationTraits<3, NumericT>::rotatedLine (const VectorT & rotationNormal, const NumericRealT & angle) const
		{
			const LineT * t = static_cast<const LineT*> (this);
			
			LineT result(*t);
			
			result.rotate (rotationNormal, angle);
			
			return result;
		}
		
#pragma mark -
#pragma mark class Line
		
		template <unsigned D, typename NumericT>
		Line<D, NumericT>::Line ()
		{
		}
		
		template <unsigned D, typename NumericT>
		Line<D, NumericT>::Line (const Zero &) : m_point(ZERO), m_direction(ZERO)
		{
		}
		
		template <unsigned D, typename NumericT>
		Line<D, NumericT>::Line (const Identity &, const NumericT & n) : m_point(ZERO), m_direction(IDENTITY, n)
		{
		}
		
		template <unsigned D, typename NumericT>
		Line<D, NumericT>::Line (const VectorT & direction) : m_point(ZERO), m_direction(direction)
		{
		}
		
		template <unsigned D, typename NumericT>
		Line<D, NumericT>::Line (const VectorT & point, const VectorT & direction) : m_point(point), m_direction(direction)
		{
		}
		
		template <unsigned D, typename NumericT>
		NumericT Line<D, NumericT>::timeForClosestPoint (const VectorT & p3) const
		{
			const VectorT & p1 = m_point;
			VectorT p2 = m_point + m_direction;
			
			NumericT d = m_direction.length2();
			NumericT t = 0;
			
			for (unsigned i = 0; i < D; ++i)
				t += (p3[i] - p1[i]) * (p2[i] - p1[i]);
			
			return t / d;
		}
		
		template <typename NumericT>
		bool lineIntersectionTest (const Line<2, NumericT> & lhs, const Line<2, NumericT> & rhs, NumericT & leftTime, NumericT & rightTime)
		{
			Vector<2, NumericT> t = lhs.direction();
			Vector<2, NumericT> o = rhs.direction();
			
			// http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/
			float denom = (o[Y] * t[X]) - (o[X] * t[Y]);
			
			// Quick return
			if (denom == (NumericT)0) return false;
			
			NumericT na = o[X] * (lhs.point()[Y] - rhs.point()[Y]) - o[Y] * (lhs.point()[X] - rhs.point()[X]);
			leftTime = na / denom;
			
			NumericT nb = t[X] * (lhs.point()[Y] - rhs.point()[Y]) - t[Y] * (lhs.point()[X] - rhs.point()[X]);
			rightTime = nb / denom;
			
			return true;
		}
		
		template <unsigned D, typename NumericT>
		bool lineIntersectionTest (const Line<D, NumericT> & lhs, const Line<D, NumericT> & rhs, NumericT & leftTime, NumericT & rightTime)
		{
			Vector<2, NumericT> lhsPoint, lhsDir, rhsPoint, rhsDir;
			lhsPoint.set(lhs.point());
			lhsDir.set(lhs.direction());
			
			rhsPoint.set(rhs.point());
			rhsDir.set(rhs.direction());
			
			Line<2, NumericT> lhs2d(lhsPoint, lhsDir), rhs2d(rhsPoint, rhsDir);
			if (lineIntersectionTest(lhs2d, rhs2d, leftTime, rightTime))
			{
				// Collision occurred in 2-space, check in n-space
				Vector<D, NumericT> lhsPt, rhsPt;
				
				lhsPt = lhs.pointAtTime(leftTime);
				rhsPt = rhs.pointAtTime(rightTime);
				
				return lhsPt.equivalent(rhsPt);
			}
			else
			{
				// No collision occurred.
				return false;
			}
		}
		
		template <unsigned D, typename NumericT>
		bool Line<D, NumericT>::intersectsWith (const Line<D, NumericT> & other, NumericT & thisTime, NumericT & otherTime) const
		{
			return lineIntersectionTest(*this, other, thisTime, otherTime);
		}
		
		template <typename NumericT>
		bool raySlabsIntersection(NumericT start, NumericT dir, NumericT min, NumericT max, NumericT& tfirst, NumericT& tlast)
		{
			if (Number<NumericT>::equivalent(dir, 0))
				return (start < max && start > min);
			
			NumericT tmin = (min - start) / dir;
			NumericT tmax = (max - start) / dir;
			
			if (tmin > tmax) std::swap(tmin, tmax);
			
			if (tmax < tfirst || tmin > tlast)
				return false;
			
			if (tmin > tfirst) tfirst = tmin;
			if (tmax < tlast) tlast = tmax;
			
			return true;
		}
		
		template <unsigned D, typename NumericT>
		bool Line<D, NumericT>::intersectsWith(const AlignedBox<D, NumericT> &a, NumericT & t1, NumericT & t2) const {
			t1 = (NumericT)0;
			t2 = (NumericT)1;
			
			for (unsigned i = 0; i < D; i += 1) {
				if (!raySlabsIntersection(m_point[i], m_direction[i], a.min()[i], a.max()[i], t1, t2)) return false;	
			}
			
			return true;
		}
				
		template <unsigned D, typename NumericT>
		inline Line<D, NumericT> operator+ (const Line<D, NumericT> &l, const Vector<D, NumericT> &v) {
			return Line<D, NumericT>(l.point() + v, l.direction());
		}
		
		template <unsigned D, typename NumericT>
		inline Line<D, NumericT> operator- (const Line<D, NumericT> &l, const Vector<D, NumericT> &v) {
			return Line<D, NumericT>(l.point() - v, l.direction());
		}
		
#pragma mark -
#pragma mark class LineSegment

		template <unsigned D, typename NumericT>
		LineSegment<D, NumericT>::LineSegment ()
		{	
		}

		template <unsigned D, typename NumericT>
		LineSegment<D, NumericT>::LineSegment (const Zero &) : m_start(ZERO), m_end(ZERO)
		{	
		}
		
		template <unsigned D, typename NumericT>
		LineSegment<D, NumericT>::LineSegment (const Identity &, const NumericT & n) : m_start(ZERO), m_end(IDENTITY, n)
		{	
		}
		
		template <unsigned D, typename NumericT>
		LineSegment<D, NumericT>::LineSegment (const Line<D, NumericT> & line, const NumericT & startTime, const NumericT & endTime)
		{
			m_start = line.pointAtTime(startTime);
			m_end = line.pointAtTime(endTime);
		}
		
		template <unsigned D, typename NumericT>
		LineSegment<D, NumericT>::LineSegment (const VectorT & start, const VectorT & end) : m_start(start), m_end(end)
		{
		}
		
		template <unsigned D, typename NumericT>
		bool LineSegment<D, NumericT>::intersectsWith (const AlignedBox<D, NumericT> &other, VectorT & at) const
		{
			Vector<D, NumericT> d((end() - start()).normalize());
			Line<D, NumericT> l(start(), d);
			
			l.intersectsWith(other, at);
			
			return (end() - at).normalize() == d;
		}
				
		template <typename NumericT>
		bool lineIntersectionTest (const LineSegment<1, NumericT> & lhs, const LineSegment<1, NumericT> & rhs, LineSegment<1, NumericT> & overlap)
		{
			NumericT lmin, lmax, rmin, rmax;
			
			lmin = lhs.start()[X];
			lmax = lhs.end()[X];
			
			if (lmin > lmax) std::swap(lmin, lmax);
			
			rmin = rhs.start()[X];
			rmax = rhs.end()[X];
			
			if (rmin > rmax) std::swap(rmin, rmax);
			
			Vector<2, NumericT> o;
			if (segmentsIntersect(vec(lmin, lmax), vec(rmin, rmax), o))
			{
				overlap = LineSegment<1, NumericT>(vec(o[0]), vec(o[1]));
				return true;
			}
			else
			{
				return false;
			}
		}
		
		template <typename NumericT>
		bool lineIntersectionTest (const LineSegment<2, NumericT> & lhs, const LineSegment<2, NumericT> & rhs, NumericT & leftTime, NumericT & rightTime)
		{
			Vector<2, NumericT> t = lhs.offset();
			Vector<2, NumericT> o = rhs.offset();
			
			// http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/
			float denom = (o[Y] * t[X]) - (o[X] * t[Y]);
			
			// Quick return
			if (denom == 0.0f) return false;
			
			float na = o[X] * (lhs.start()[Y] - rhs.start()[Y]) - o[Y] * (lhs.start()[X] - rhs.start()[X]);
			leftTime = na / denom;
			
			// Quick return
			if (leftTime < 0.0 || leftTime > 1.0) return false;
			
			float nb = t[X] * (lhs.start()[Y] - rhs.start()[Y]) - t[Y] * (lhs.start()[X] - rhs.start()[X]);
			rightTime = nb / denom;
			
			// Quick return
			if (rightTime < 0.0 || rightTime > 1.0) return false;
			
			return true;
		}
		
		template <unsigned D, typename NumericT>
		bool lineIntersectionTest (const LineSegment<D, NumericT> & lhs, const LineSegment<D, NumericT> & rhs, NumericT & leftTime, NumericT & rightTime)
		{
			Vector<2, NumericT> lhsStart, lhsEnd, rhsStart, rhsEnd;
			lhsStart.set(lhs.start());
			lhsEnd.set(lhs.end());
			
			rhsStart.set(rhs.start());
			rhsEnd.set(rhs.end());
			
			LineSegment<2, NumericT> lhs2d(lhsStart, lhsEnd), rhs2d(rhsStart, rhsEnd);
			if (lineIntersectionTest(lhs2d, rhs2d, leftTime, rightTime))
			{
				// Collision occurred in 2-space, check in n-space
				Vector<D, NumericT> lhsPt, rhsPt;
				
				lhsPt = lhs.pointAtTime(leftTime);
				rhsPt = rhs.pointAtTime(rightTime);
				
				return lhsPt.equivalent(rhsPt);
			}
			else
			{
				// No collision occurred.
				return false;
			}
		}
		
		template <unsigned D, typename NumericT>
		bool LineSegment<D, NumericT>::intersectsWith (const LineSegment<D, NumericT> & other, LineSegment<D, NumericT> & overlap) const
		{
			return lineIntersectionTest(*this, other, overlap);
		}
		
		template <unsigned D, typename NumericT>
		bool LineSegment<D, NumericT>::intersectsWith (const LineSegment<D, NumericT> & other, NumericT & leftTime, NumericT & rightTime) const
		{
			return lineIntersectionTest(*this, other, leftTime, rightTime);
		}
		
	}
}
