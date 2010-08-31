/*
 *  Mesh-shapes.h
 *  Dream
 *
 *  Created by Samuel Williams on 9/03/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_MESH_SHAPES_H
#define _DREAM_MESH_SHAPES_H

#include "Mesh.h"

namespace Dream {	
	int calculateRotationOffset (const std::vector<Vec3 > &pts, const Vec3 &pt);
	
	template <typename spline_t>
	Mesh* Mesh::cylinder (std::vector<spline_t> splines, RealT resolution, bool capStart, bool capEnd) {
		Mesh* mesh = new Mesh();
		
		//resolution = 1.0 / resolution;
		
		std::vector< std::vector<index_t> > indices(splines.size());
		
		for (unsigned c = 0; c < splines.size(); c += 1) {			
			for (RealT i = 0.0; i < 1.00001; i += resolution) {
				Vec3 v (splines[c].pointAtTime(i));
				
				//std::cout << "c: " << c << " i: " << i << " = " << v << std::endl;
				
				indices[c].push_back(mesh->addVertex(v));
			}
		}
				
		for (unsigned i = 0; i < (indices[0].size() - 1); i += 1) {
			for (unsigned c = 0; c < indices.size(); c += 1) {
				unsigned c1 = (c+1) % indices.size();
				unsigned i1 = (i+1) % indices[0].size();
				
				mesh->addTriangleSurface(indices[c][i], indices[c1][i], indices[c][i1]);
				mesh->addTriangleSurface(indices[c1][i1], indices[c][i1], indices[c1][i]);
			}
		}
		
		/* This code currently does not work, and needs to be fixed. */
		if (capStart || capEnd) {			
			Vec3 start(ZERO), end(ZERO);
			
			for (unsigned c = 0; c < splines.size(); ++c) {
				start += splines[c].pointAtTime(0.0);
				end += splines[c].pointAtTime(1.0);
			}
			
			start /= splines.size();
			end /= splines.size();
			
			index_t baseCenter = mesh->addVertex(start);
			index_t topCenter = mesh->addVertex(end);
			
			unsigned slices = indices[0].size();
			for (index_t slice = 0; slice < slices; ++slice) {
				/* Add in triangles for the caps */
				if (capStart)
					mesh->addTriangleSurface(baseCenter, indices[0][(slice+1) % slices], indices[0][slice]);
				
				unsigned stacks = indices.size() - 1;
				
				if (capEnd)
					mesh->addTriangleSurface(topCenter, indices[stacks][slice], indices[stacks][(slice+1) % slices]);
			}
		}
		
		return mesh;
	}

}

namespace Dream {
	// Given an input point pt, find the index of the closest point in pts.
	RealT calculateRotationLength (const unsigned rotation, const std::vector<Vec3 > &left, const std::vector<Vec3 > &right) {
		RealT l = 0;
		
		Vec3 centers[2];
		centers[0].zero(); centers[1].zero();
		
		for (unsigned i = 0; i < left.size(); i += 1) {
			centers[0] += left[i];
			centers[1] += right[i];
		}
		
		centers[0] /= left.size();
		centers[1] /= right.size();
		
		Vec3 segmentDirection = (centers[1] - centers[0]).normalize();
		
		for (unsigned i = 0; i < left.size(); i += 1) {
			RealT cosl = segmentDirection.dot((right[(i + rotation) % right.size()] - left[i]).normalize());
			l += 1.0 - cosl;
		}
		
		return l;
	}
	
	int calculateRotationOffset (const std::vector<Vec3 > &left, const std::vector<Vec3 > &right) {
		assert(left.size() == right.size());
		std::vector<RealT> lengths(left.size());
		
		for (unsigned i = 0; i < right.size(); i++) {
			lengths.at(i) = calculateRotationLength(i, left, right);
			//std::cout << "Length(" << i << "):" << lengths.at(i) << std::endl;
		}
		
		int idx = 0;
		RealT mx = lengths[0];
		for (unsigned i = 1; i < lengths.size(); ++i) {
			if (mx > lengths[i]) {
				idx = i;
				mx = lengths[i];
			}
		}
				
		return idx;
	}
	
	Mesh* Mesh::cylinder(const ISpline<3> *spline, RealT radius, unsigned slices, bool capStart, bool capEnd) {
		Mesh* mesh = new Mesh();
		std::vector<RealT> times = spline->timesAtResolution(20, 2);
		
		std::vector< std::vector<index_t> > indices(slices);
		
		for (unsigned c = 0; c < slices; ++c) {
			RealT rotation = ROTATION_360 * ((RealT)c / (RealT)slices);
		
			for (iterateEach(times, t)) {
				Vec3 v (spline->pointAtTime(*t));
				Vec3 tang (spline->tangentAtTime(*t));
				
				Vec3 pt = Matrix4x4::rotatingMatrix(-rotation, tang) * spline->normalAtTime(*t) * radius;
				
				index_t idx = mesh->addVertex(v + pt);
				
				indices[c].push_back(idx);
			}
		}
		
		int rotation = 0;
		///iterate over each stack idx
		for (unsigned i = 0; i < (indices[0].size() - 1); i += 1) {
			std::vector<index_t> left, right;
			//indices[rotation around][stack idx]
			
			for (unsigned j = 0; j < indices.size(); j++) {
				left.push_back(indices.at(j).at(i));
				right.push_back(indices.at(j).at((i+1) % indices[0].size()));
			}

			rotation = calculateRotationOffset(mesh->vertices(left), mesh->vertices(right));
			
			for (unsigned c = 0; c < indices.size(); c += 1) {
				unsigned c2 = (c+1) % indices.size(); //rotation bottom
				unsigned i2 = (i+1) % indices[0].size();
				
				index_t idxs[2][2];
				idxs[0][0] = indices.at((c2) % indices.size()).at(i);
				idxs[0][1] = indices.at((c) % indices.size()).at(i);
				idxs[1][0] = indices.at((c2+rotation) % indices.size()).at(i2); 
				idxs[1][1] = indices.at((c+rotation) % indices.size()).at(i2);
				
				mesh->addTriangleSurface(idxs[0][0], idxs[1][0], idxs[0][1]);
				mesh->addTriangleSurface(idxs[0][1], idxs[1][0], idxs[1][1]);
				
				mesh->vertex(idxs[0][1])->setColor(vec(0.0, 1.0, 0.0, 1.0));
			}
		}
		
		for (unsigned k = 0; k < indices[0].size(); k++) {
			mesh->vertex(indices[0][k])->setColor(vec(1.0, 0.0, 0.0, 1.0));
		}
		
		return mesh;
	}

	Mesh* Mesh::cube (const AlignedBox<3> &box) {
		Mesh* mesh = new Mesh();
		index_t i[8];
		
		i[0] = mesh->addVertex(box.constrainedCorner(vec<int>(-1, 1, -1))); //near top left
		i[1] = mesh->addVertex(box.constrainedCorner(vec<int>(1, 1, -1))); //near top right
		i[2] = mesh->addVertex(box.constrainedCorner(vec<int>(-1, 1, 1))); //far top left
		i[3] = mesh->addVertex(box.constrainedCorner(vec<int>(1, 1, 1))); //far top right
		
		i[4] = mesh->addVertex(box.constrainedCorner(vec<int>(-1, -1, -1))); //near bottom left
		i[5] = mesh->addVertex(box.constrainedCorner(vec<int>(1, -1, -1))); //near bottom right
		i[6] = mesh->addVertex(box.constrainedCorner(vec<int>(-1, -1, 1))); //far bottom left
		i[7] = mesh->addVertex(box.constrainedCorner(vec<int>(1, -1, 1))); //far bottom right
		
		// far left, far right, near right, near left
		mesh->addQuadSurface(i[2], i[3], i[1], i[0]);
		mesh->addQuadSurface(i[4], i[5], i[7], i[6]);
		
		mesh->addQuadSurface(i[0], i[1], i[5], i[4]);
		mesh->addQuadSurface(i[6], i[7], i[3], i[2]);
		
		mesh->addQuadSurface(i[4], i[6], i[2], i[0]);
		mesh->addQuadSurface(i[1], i[3], i[7], i[5]);
		
		return mesh;
	}
	
	Mesh* Mesh::cylinder (const RealT &baseRadius, const RealT &topRadius, const RealT &height, const index_t &slices, const index_t &stacks, bool capStart, bool capEnd) {
		Vec3 base (0, baseRadius, 0);
		Vec3 top (height, topRadius, 0);
		Vec3 center(1, 0, 0);
		
		assert((baseRadius != 0.0 || topRadius != 0.0) && "Both baseRadius and topRadius are zero!");
				
		Matrix<4,4> rotation = Matrix<4,4>::rotatingMatrix(-ROTATION_360 / slices, center);
		index_t divisions = stacks + 1; /* N stacks has n+1 divisions, one for the start, one for the end */
		
		index_t points[divisions][slices];
		
		Mesh *mesh = new Mesh();
		
		/* Generate all the points for the stacks and slices of the cylinder */
		for (index_t stk = 0; stk < divisions; ++stk) {
			/* Generate the offset vector for the current stack */
			RealT position = (RealT)stk / (RealT)(stacks);
			Vec3 current = top * position + base * (1.0 - position);
			Vec3 last (0, 0, 0);
			
			//std::cout << stk << "(" << position << ")" << " = " << current << std::endl;
				
			for (index_t slice = 0; slice < slices; ++slice) {
				/* Add points for that offset vector */
				if (slice > 0 && current == last)
					points[stk][slice] = points[stk][0];
				else
					points[stk][slice] = mesh->addVertex(current);

				
				/* Rotate the offset vector */
				last = current;
				current = rotation * current;
			}
		}
		
		/* If we are capping, add two more points for the center points at each end */
		if (capStart || capEnd) {
			index_t baseCenter = mesh->addVertex(vec<RealT>(0.0, 0, 0));
			index_t topCenter = mesh->addVertex(vec(height, 0, 0));
			
			for (index_t slice = 0; slice < slices; ++slice) {
				/* Add in triangles for the caps */
				if (capStart && baseRadius > 0.0)
					mesh->addTriangleSurface(baseCenter, points[0][(slice+1) % slices], points[0][slice]);
				
				if (capEnd && topRadius > 0.0)
					mesh->addTriangleSurface(topCenter, points[stacks][slice], points[stacks][(slice+1) % slices]);
			}
		}
		
		/* Add in the quad surfaces for the cylinder */
		for (index_t stk = 0; stk < stacks; ++stk) {
			for (index_t slice = 0; slice < slices; slice += 1) {
				if (baseRadius == 0.0 && stk == 0)
					mesh->addTriangleSurface(points[stk+1][slice], points[stk][slice], points[stk+1][(slice+1) % slices]);
				else if (topRadius == 0.0 && stk == (stacks-1))
					mesh->addTriangleSurface(points[stk+1][slice], points[stk][slice], points[stk][(slice+1) % slices]);
				else
					mesh->addQuadSurface(points[stk+1][slice], points[stk][slice], points[stk][(slice+1) % slices], points[stk+1][(slice+1) % slices]);
			}
		}
		
		return mesh;
	}

}


#endif
