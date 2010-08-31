/*
 *  Mesh.h
 *  Model Builder
 *
 *  Created by Samuel Williams on 15/03/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_GEOMETRY_MESH_H
#define _DREAM_GEOMETRY_MESH_H

#include "Geometry.h"
#include "AlignedBox.h"

#include <set>
#include <vector>

namespace Dream {
	namespace Geometry {
	
		struct BasicMeshTraits {
			struct SurfaceProperties {
				Vec3 normal;
				
				enum Mode {
					TRIANGLES,
					TRIANGLE_STRIP,
					TRIANGLE_FAN
				};
				
				Mode mode;
			};
			
			struct PointProperties {
				Vec4 color;
				Vec2 texCoord;
				Vec3 normal;
			};
			
			struct VertexProperties {
				Vec3 vertex;
				
				void apply (const Mat44 & transform);
			};
			
			class MeshProperties : public Object {
				
			};
		};
	
		// A mesh is made out of triangle strips.
		template <typename _TraitsT = BasicMeshTraits>
		class Mesh : public _Traits::MeshProperties {
			public:
				typedef _TraitsT TraitsT;
				typedef std::size_t VertexIndexT;
								
				/* Mesh Points don't maintain explicit reference to the Mesh, so any alterations
					can be made by changing the index of the vertex they refer to. MeshPoints
					maintain a connection to the surface they are defining.
				*/
				class Point : public TraitsT::PointProperties {
					protected:
						/* The vertex which contains this point. */
						VertexIndexT m_vertexIndex;
				};
				
				/** The vertex represents a set of properties that can be shared between multiple
					points, and thus surfaces. You can adjust a vertex and have it affect multiple
					surfaces' shapes.
				*/
				class Vertex : public TraitsT::VertexProperties {
					protected:
						std::set<Point> m_points;
				};
				
				class Surface : public TraitsT::SurfaceProperties {
					protected:
						std::vector<Point> m_points;
					public:
						void addPoint(Mesh & m, VertexIndexT v, const TraitsT::PointProperties & properties) {
							m_points.push_back(Point(properties));
							
						}
				};
			
		protected:
			std::vector<Vertex> m_vertices;
			std::vector<Surface> m_surfaces;
		
		public:
			VertexIndexT addVertex (const TraitsT::VertexProperties & properties) {
				m_vertices.push_back(properties);
			}
			
			template <unsigned D>
			Surface & addSurface (const TraitsT::SurfaceProperties & properties, Vector<D, VertexIndexT> indices) {
				m_surfaces.push_back(properties);
				Surface & s = m_surfaces.back();
				
				for (std::size_t i = 0; i < D; i++) {
					s.points.push_back();
				}
				
				return s;
			}
			
			// Apply some kind of tranform to all vertices.
			template <typename OperatorT>
			void apply (const OperatorT & op) {
				for (std::size_t i = 0; i < m_vertices.size(); i++)
					m_vertices[i].apply(op);
			}
		};
		
	}
}

#endif
