/*
 *  Spline.cpp
 *  Dream
 *
 *  Created by Samuel Williams on 1/03/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */


#include "Spline.h"

#include <Dream/Geometry/Triangle.h>
#include <Dream/Geometry/Line.h>

namespace Dream {
	namespace Geometry {	
#pragma mark -
#pragma mark ISpline
			
		template <unsigned D>
		ISpline<D>::~ISpline () {
		
		}

		template <unsigned D>
		Vector<D> ISpline<D>::tangentAtTime(RealT t) const {
			// TODO: Need to write a better function !!
			const RealT f = 0.001;
			RealT t1 = Math::wrap(t-f, 1.0), t2 = Math::wrap(t+f, 1.0);
					
			assert(t1 >= 0.0 && t1 <= 1.0);
			assert(t2 >= 0.0 && t2 <= 1.0);
			
			return (pointAtTime(t2) - pointAtTime(t1)).normalize();
		}
		
		template <>
		Vec3 ISpline<3>::normalAtTime(RealT t) const {
			Vec3 tg = tangentAtTime(t);
			Vec3 pt = pointAtTime(t);
			
			Vec4 tg4 = tg << 0.0;
			Vec4 orth (0.0, 0.0, 0.0, 1.0);
			Vec4 ori = (tangentAtTime(t-0.2) - tangentAtTime(t+0.2)) << 1.0;
				
			Vec4 res (crossProduct(tg4, orth, ori));
			
			return res.reduce().normalize();
		}
		
		template <>
		Vec2 ISpline<2>::normalAtTime(RealT t) const {
			Vec2 tg = tangentAtTime(t);
			
			Vec3 tg3 = tg << 0.0;
			Vec3 orth (0.0, 0.0, 1.0);
			
			return tg3.cross(orth).reduce().normalize();		
		}
		
		template <unsigned D>
		std::vector<RealT> ISpline<D>::nominalTimes () const {
			RealT d = (RealT)1.0 / this->segments();
			std::vector<RealT> times;
			
			for (RealT t = 0; t <= (RealT)1.0; t += d)
				times.push_back(t);
			
			return times;
		}
		
		template <unsigned D>
		std::vector<RealT> ISpline<D>::divideAndAppend(int n, RealT res) const {
			// Entry point for the function below.
			// The higher res is, the more detail will be present.
			std::vector<RealT> times;
			
			divideAndAppend(0.0, 1.0, n, 1.0 - (1.0 / res), true, times);
			
			return times;
		}
		
		template <unsigned D>
		void ISpline<D>::divideAndAppend(RealT t, RealT d, int n, RealT res2, bool first, std::vector<RealT> &times) const {
			RealT hd = d / 2.0;
			
			Vector<D> t1(tangentAtTime(t)), t2(tangentAtTime(t+d));
			
			bool divide = n > 0 || t1.dot(t2) < res2 && n > -10 /* Sanity check */;
			
			// First point of segment
			if (divide) divideAndAppend(t, hd, n - 1, res2, true & first, times);
			
			if (!divide) {
				if (first) times.push_back(t);

				times.push_back(t+d);
			}
			
			// Last point of segment
			if (divide) divideAndAppend(t + hd, hd, n - 1, res2, false, times);
		}
		
		template <unsigned D>
		std::vector<RealT> ISpline<D>::timesAtResolution(RealT resolution, unsigned divisions) const {
			// We must divide at least once per point.
			int d = (int)Math::ceil(Math::sqrt(this->points().size()) + 1);
			divisions = (unsigned)Math::max(d, divisions);
			
			return divideAndAppend(divisions, resolution);
		}
		
		template <unsigned D>
		unsigned ISpline<D>::segments () const {
			// The number of segments between points
			return this->segmentPoints().size() - 1;
		}

		template <unsigned D>
		unsigned ISpline<D>::startingPoint(RealT t) const {
			// The starting point for a given t in [0.0, 1.0]
			return (unsigned)Math::floor(this->segments() * t);
		}
		
		template <unsigned D>
		RealT ISpline<D>::fractionalComponent(RealT t) const {
			// The fractional component (ie, in [0.0, 1.0]) of a particular segment.
			RealT m = this->segments() * t;
			return m - Math::floor(m);	
		}

		template class ISpline<3>;	
		template class ISpline<2>;

#pragma mark -
#pragma mark SplineWithNormal

		template <unsigned D>
		SplineWithNormal<D>::SplineWithNormal(const SplineT * spline, const SplineT * normalSpline) :
			m_spline(spline),
			m_normalSpline(normalSpline)
		{
			
		}

		template <unsigned D>
		SplineWithNormal<D>::~SplineWithNormal () {
			
		}

		template <unsigned D>
		Vector<D> SplineWithNormal<D>::pointAtTime(RealT t) const {
			return m_spline->pointAtTime(t);
		}
		
		template <unsigned D>
		Vector<D> SplineWithNormal<D>::tangentAtTime(RealT t) const {
			return m_spline->tangentAtTime(t);
		}
		
		template <unsigned D>
		Vector<D> SplineWithNormal<D>::normalAtTime(RealT t) const {
			// We will assume we don't need to normalize this result, but
			// that could be dangerous.
			Vector<D> n1 ((m_normalSpline->pointAtTime(t) - m_spline->pointAtTime(t)).normalize());
			Vector<D> t1 (m_spline->tangentAtTime(t));
			
			n1.normalize();
			
			Vector<D> n2 = t1.cross(n1);
			return n2.cross(t1).normalize();
		}
		
		template <unsigned D>
		const std::vector<Vector<D> > & SplineWithNormal<D>::points() const {
			return m_spline->points();
		}
			
		template <unsigned D>
		class _SplineWithNormalGenerator {
		public:
			struct Intersection {
				bool parallel;
				Vector<D> intersection;
				bool tmp;
			};
			
			struct PlaneAtTime {
				RealT time;
				Plane<D> plane;
				bool interpolated;
			};
			
			struct NormalAtTime {
				RealT time;
				bool fixed;
				Vector<D> normal;
				bool tmp;
			};
			
			static std::vector<PlaneAtTime> interpolatedPlanesForSpline(const ISpline<D> * spline, const std::vector<RealT> & times) {
				std::list<PlaneAtTime> planes;
				
				//Generate the plane for each point in the spline
				foreach(t, times) {
					PlaneAtTime pat;
					pat.interpolated = false;
					pat.time = *t;
					pat.plane = Plane<D>(spline->pointAtTime(*t), spline->tangentAtTime(*t));
					
					planes.push_back(pat);
				}
				
				//Subdivide parallel planes that are in opporsite directions
				foreach(pl, planes) {
					PlaneAtTime &p1 = *pl; pl++; // Fetch the current plane
					PlaneAtTime &p2 = *pl; // Fetch the next plane
					
					if (p1.plane.normal() == -p2.plane.normal()) {
						PlaneAtTime p2h;
						p2h.interpolated = true;
						p2h.time = p1.time + p2.time / (RealT)2.0;
						p2h.plane = Plane<D>(spline->pointAtTime(p2h.time), spline->tangentAtTime(p2h.time));
						
						// Insert p2h after p1, before p2 (pl points to p2 atm).
						planes.insert(pl, p2h);
					}
					
					pl--; // We want to step from the next back to the current
				}
				
				return std::vector<PlaneAtTime> (planes.begin(), planes.end());
			}
			
			static void forSpline (const ISpline<D> * spline, const ISpline<D> * normalSpline) {			
				std::vector<RealT> nts = spline->nominalTimes();
				nts.pop_back();
				std::vector<Intersection> its;
						
				std::vector<PlaneAtTime> planes = interpolatedPlanesForSpline(spline, nts);
				
				// Calculate plane intersection for non-parallel planes
				for (unsigned i = 0; i < planes.size(); i += 1) {
					PlaneAtTime &pat1 = planes[i];
					PlaneAtTime &pat2 = planes[(i+1) % planes.size()];
					
					Intersection it;
					it.intersection.zero();
					
					if (pat1.plane.isParallel(pat2.plane)) {
						//These points are in sequence and in the same direction, ie '|' straight line
						it.parallel = true;
					} else {
						it.parallel = false;
						RealT t1 = nts[i], t3 = nts[i+1 % nts.size()];
						RealT t2 = (t1 + t3) / 2.0;
						
						Vector<D> pt1 = spline->pointAtTime(t1);
						Vector<D> pt3 = spline->pointAtTime(t3);
						
						// If two of the same points on the line.
						if (pt1 == pt3) continue;
						
						//Vector<D> pt3 = pt1 + spline->tangentAtTime(nts[i]) + spline->tangentAtTime(nts[i+1 % nts.size()]);
						Vector<D> pt2 = spline->pointAtTime(t2);
						
						Triangle<D> tri (pt1, pt2, pt3);
						Plane<D> p3 (pt1, tri.normal());
						Line<D> lineIntersection;
						
						// There must be an intersection
						assert(pat1.plane.intersectsWith(pat2.plane, lineIntersection));
						assert(p3.intersectsWith(lineIntersection, it.intersection));
						
						// it.intersection = (lineIntersection.pointForClosestPoint(pt1) + lineIntersection.pointForClosestPoint(pt2)) / 2.0;
						// This point gives us the best (shortest) normal from pt1 to pt2
					}
					
					it.tmp = pat1.interpolated;
					its.push_back(it);
				}
				
				// We need to find all fixed normals		
				std::vector<NormalAtTime> normals;
				for (unsigned i = 0; i < its.size(); i += 1) {
					PlaneAtTime pat = planes[i];
					
					NormalAtTime nat;
					// If the previous intersection was parallel, and the next intersection is parallel, we are not fixed
					nat.fixed = !(its[i-1 % its.size()].parallel & its[i].parallel);
					nat.time = pat.time;
					
					if (nat.fixed) {
						nat.normal = (its[i].intersection - spline->pointAtTime(pat.time)).normalize();
					} else {
						nat.normal.zero();
					}
					
					normals.push_back(nat);
				}
				
				// We need to check that there is atleast one non-parallel
				
				/*// Now we need to interpolate any non-fixed normals (ie, parallel)
				for (unsigned i = 0; i < normals.size(); i += 1) {
					unsigned k;
					
					//Find next non-fixed normal
					for (k = 1; k < normals.size(); k += 1) {
						if (normals[i+k % normals.size()].fixed) break;
					}
					
					if (k != 1) {
						// We have found 1 or more non-fixed normals
						Vector<D> start(normals[i].normal), end(normals[i+k % normals.size()].normal);
						
						for (unsigned j = 1; j < k; j += 1) {
							RealT f = ((RealT)j / (RealT)k);
							normals[i+j % normals.size()] = start * f + end * (1.0 - f);
						}
					}
				}
				*/
				
				// What spline should we use for the new spline???
				foreach(nml, normals) {
					if (!(*nml).fixed) continue; // Skip temporary normals
					// This next line is important but doesn't compile right now
					// normalSpline->points().push_back(spline->pointAtTime((*nml).time) + ((*nml).normal.normalize() * 100));
				}
			}
		};
		
		template <unsigned D>
		void SplineWithNormal<D>::forSpline (const ISpline<D> * spline, const ISpline<D> * normalSpline) {			
			_SplineWithNormalGenerator<D>::forSpline(spline, normalSpline);
		}

		template class SplineWithNormal<3>;	

#pragma mark -
#pragma mark Spline
		
		template <unsigned D>
		Spline<D>::Spline () : m_closed(true) {
		
		}
		
		template <unsigned D>
		Spline<D>::~Spline () {
		
		}
		
		template <unsigned D>
		void Spline<D>::resetSegmentPointsCache () {
			m_segmentPoints.clear();
		}
		
		template <unsigned D>
		bool Spline<D>::closed () const {
			return m_closed;
		}
		
		template <unsigned D>
		void Spline<D>::setClosed (bool closed) {
			if (m_closed != closed) {
				m_closed = closed;
				resetSegmentPointsCache();
			}
		}
		
		template <unsigned D>
		const std::vector<Vector<D> >& Spline<D>::segmentPoints() const {
			if (m_segmentPoints.empty()) {
				m_segmentPoints = generateSegmentPoints();
			}
			
			return m_segmentPoints;
		}
		
		template <unsigned D>
		std::vector<Vector<D> > Spline<D>::generateSegmentPoints() const {
			if (!m_closed) {
				return m_points;
			}
		
			// We need to add the first point last so that the line is closed.
			assert(m_points.size() > 0);
			PointsT segmentPoints(m_points);
			segmentPoints.push_back(m_points[0]);
			
			return segmentPoints;
		}
		
#pragma mark -
#pragma mark LinearSpline

		template <unsigned D>
		Vector<D> LinearSpline<D>::pointAtTime(RealT t) const {			
			unsigned sp = this->startingPoint(t);
			RealT fr = this->fractionalComponent(t);
			
			if (t >= 1.0) return this->m_points[sp];
			else return linearInterpolate(fr, this->m_points[sp], this->m_points[sp+1]);
		}

#pragma mark -
#pragma mark CubicSpline
		
		template <unsigned D>
		unsigned CubicSpline<D>::segments () const {
			// Remove two segments which are used for initial cubic tangent generation
			// (one at each end)
			return Spline<D>::segments() - 2;
		}
		
		template <unsigned D>
		std::vector<Vector<D> > CubicSpline<D>::generateSegmentPoints() const {
			if (!this->m_closed) {
				return Spline<D>::generateSegmentPoints();
			}
			
			assert(this->m_points.size() > 3);
			PointsT pts;
			
			pts.push_back(this->m_points[this->m_points.size() - 1]);
			pts.insert(pts.end(), this->m_points.begin(), this->m_points.end());
			pts.push_back(this->m_points[0]);
			pts.push_back(this->m_points[1]);
			
			return pts;
		}
		
		template <unsigned D>
		Vector<D> CubicSpline<D>::pointAtTime(RealT t) const {
			const PointsT &pts = this->segmentPoints();
			assert(pts.size() > 3 && "CublicSpline<D> requires at least 4 points");
			
			unsigned sp = this->startingPoint(t) + 1;
			RealT fr = this->fractionalComponent(t);
			
			if (t >= 1.0) return pts[sp];
			return cubicInterpolate(fr, pts[sp-1], pts[sp], pts[sp+1], pts[sp+2]);

		}

#pragma mark -
#pragma mark HermiteSpline
		
		template <unsigned D>
		Vector<D> HermiteSpline<D>::pointAtTime(RealT t) const {
			const PointsT &pts = this->segmentPoints();
			
			assert(pts.size() > 3 && "HermiteSpline<D> requires at least 4 points");
			
			unsigned sp = this->startingPoint(t) + 1;
			RealT fr = this->fractionalComponent(t);
			
			if (t >= 1.0) return pts[sp];
			
			Vector<D> t1 (this->tangent(sp));
			Vector<D> t2 (this->tangent(sp+1));
			
			return hermitePolynomial(fr, pts[sp], t1, pts[sp+1], t2);
		}
		
		/// Catmull-Rom tangent function
		template <unsigned D>
		Vector<D> HermiteSpline<D>::catmullRomSpline (const ISpline<D> *s, unsigned n) {
			const PointsT &pts = s->segmentPoints();
			
			return ((pts[n+1 % pts.size()] - pts[n-1 % pts.size()]) * 0.5);
		}
		
		template <unsigned D>
		Vector<D> HermiteSpline<D>::fourPointLinearMu (const ISpline<D> *s, unsigned n) {
			const PointsT &pts = s->segmentPoints();
		
			if (n > 1) n += 1;
			unsigned p1 = n, p2 = n-1;
			
			Vector<D> mu (pts[p1] - pts[p2]);
			mu.normalize() *= (pts[1] - pts[2]).length();
			
			return mu;
		}
		
		template <unsigned D>
		Vector<D> HermiteSpline<D>::fourPointExponentialMu (const ISpline<D>* s, unsigned n) {
			const PointsT &pts = s->segmentPoints();

			if (n > 1) n += 1;
			unsigned p1 = n, p2 = n-1;
			
			Vector<D> mu (pts[p1] - pts[p2]);
			
			// This weight of 30 is arbitray
			mu.normalize() *= (pts[1] - pts[2]).length2() / 30.0;
			
			return mu;
		}
		
		/// Simple linear tangent function
		template <unsigned D>
		Vector<D> HermiteSpline<D>::multiPointLinearMu (const ISpline<D>* s, unsigned n) {
			const PointsT &pts = s->segmentPoints();

			unsigned p1 = (n + 1) % pts.size(), p2 = (n-1) % pts.size();
			
			Vector<D> sg(pts[p1] - pts[n]);
			Vector<D> mu(pts[p1] - pts[p2]);
			
			return mu.normalize(sg.length());
		}
		
	#pragma mark -

		// Explicit template instantiation
		template class LinearSpline<3>;	
		template class LinearSpline<2>;
		
		template class CubicSpline<3>;	
		template class CubicSpline<2>;
		
		template class HermiteSpline<3>;	
		template class HermiteSpline<2>;
	}
}
