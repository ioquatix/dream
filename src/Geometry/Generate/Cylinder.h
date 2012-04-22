//
//  Geometry/Generate/Cylinder.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 9/03/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_GEOMETRY_GENERATE_CYLINDER_H
#define _DREAM_GEOMETRY_GENERATE_CYLINDER_H

#include "Basic.h"
#include "../Spline.h"

namespace Dream {	
	namespace Geometry {
		namespace Generate {
			
			RealT calculate_rotation_length (const unsigned rotation, const std::vector<Vec3> &left, const std::vector<Vec3 > &right) {
				RealT length = 0;
				
				Vec3 centers[2];
				centers[0].zero(); centers[1].zero();
				
				for (unsigned i = 0; i < left.size(); i += 1) {
					centers[0] += left[i];
					centers[1] += right[i];
				}
				
				centers[0] /= left.size();
				centers[1] /= right.size();
				
				Vec3 segment_direction = (centers[1] - centers[0]).normalize();
				
				for (unsigned i = 0; i < left.size(); i += 1) {
					RealT cosl = segment_direction.dot((right[(i + rotation) % right.size()] - left[i]).normalize());
					length += 1.0 - cosl;
				}
				
				return length;
			}
			
			int calculate_rotation_offset (const std::vector<Vec3> &left, const std::vector<Vec3> &right) {
				assert(left.size() == right.size());
				std::vector<RealT> lengths(left.size());
				
				for (unsigned i = 0; i < right.size(); i++) {
					lengths.at(i) = calculate_rotation_length(i, left, right);
				}
				
				int index = 0;
				RealT maximum = lengths[0];
				for (unsigned i = 1; i < lengths.size(); ++i) {
					if (maximum > lengths[i]) {
						index = i;
						maximum = lengths[i];
					}
				}
				
				return index;
			}
			
			template <typename MeshT>
			void cylinder(const MeshT & mesh, const ISpline<3> * spline, RealT radius, unsigned slices, bool cap_start, bool cap_end) {
				std::vector<RealT> times = spline->times_at_resolution(20, 2);
				std::vector<std::vector<std::size_t>> indices(slices);
				
				for (unsigned c = 0; c < slices; ++c) {
					RealT rotation = ROTATION_360 * ((RealT)c / (RealT)slices);
					
					for (auto time : times) {
						Vec3 v = spline->point_at_time(time);
						Vec3 tangent (spline->tangent_at_time(time));
						
						Vec3 pt = Matrix4x4::rotatingMatrix(-rotation, tangent) * spline->normal_at_time(time) * radius;
						
						std::size_t index = mesh->addVertex(v + pt);
						indices[c].push_back(index);
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
			}
			
		}
	}
}

#endif