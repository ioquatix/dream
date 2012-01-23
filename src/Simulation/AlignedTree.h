//
//  Simulation/AlignedTree.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/01/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_ALIGNED_TREE_H
#define _DREAM_ALIGNED_TREE_H

#include "../Numerics/Vector.h"
#include "../Geometry/AlignedBox.h"

#include <vector>
#include <set>
#include <iostream>

namespace Dream {
	namespace Geometry {
		// An aligned tree is a generic name for either a quad-tree or an oct-tree
		//	AlignedTree<QuadrantTraits> or AlignedTree<OctantTraits> respectively
		
		// An aligned tree subdivides a 3 dimensional space into aligned partitions.
		// Each partition can be thus subdivided recursively, and 
		// so on. Collision tests only need to be done with objects in all
		// overlapping partitions.
		
		// Template specialization for 2-dimentional quad-tree
		class Quadrants {
		public:
			enum { Q = 4, D = 2 };
			
			typedef Vector<D> VecT;
			typedef AlignedBox<D> SpaceT;
			
			template <typename ObjectT>
			static SpaceT calculateBoundingBox (const ObjectT & obj) {
				return obj.boundingBox();
			}
												
			static unsigned indexForPartition(const Vec2 &point, const Vec2 &center);
			static Vec2 normalOriginForPartitionIndex(const IndexT &i);
			static Vec2 offsetOriginForPartitionIndex(const IndexT &i);
			
			enum PartitionLocation {
				BottomLeft = 0,
				BottomRight = 1,
				TopLeft = 2,
				TopRight = 3
			};
			
			static PartitionLocation locationForDirection (const Direction &dir);	
		};
		
		// Template specialization for 3-dimentional oct-tree
		class Octants {
		public:
			enum { Q = 8, D = 3 };
			
			typedef Vector<D> VecT;
			typedef AlignedBox<D> SpaceT;
			
			template <typename ObjectT>
			static SpaceT calculateBoundingBox (const ObjectT & obj) {
				return obj.boundingBox();
			}
			
			static unsigned indexForPartition(const Vec3 &point, const Vec3 &center);
			static Vec3 normalOriginForPartitionIndex(const IndexT &i);
			static Vec3 offsetOriginForPartitionIndex(const IndexT &i);
			
			enum PartitionLocation {
				BottomLeftNear = 0,
				BottomRightNear = 1,
				TopLeftNear = 2,
				TopRightNear = 3,
				BottomLeftFar = 4,
				BottomRightFar = 5,
				TopLeftFar = 6,
				TopRightFar = 7
			};
			
			static PartitionLocation locationForDirection (const Direction &dir);
		};
		
		// An aligned space partitioning tree.
		template <typename _TraitsT, typename ObjectT>
		class AlignedTree {	
		public:
			typedef _TraitsT TraitsT;
			typedef typename TraitsT::VecT VecT;
			typedef typename TraitsT::SpaceT SpaceT;
			typedef std::set<ObjectT> ObjectSetT;
			
			void debug () {
				if (m_top) {
					std::cout << "Dumping tree..." << std::endl;
					m_top->debug("");
				} else
					std::cout << "No top level partition!" << std::endl;
			};
			
			class Partition {
			protected:
				ObjectSetT m_objects;
				
				AlignedTree* m_base;
				typename TraitsT::PartitionLocation m_location;
				
				Partition* m_parent;
				Partition* m_children[TraitsT::Q];
				
				VecT m_origin;
				VecT m_size;
				unsigned m_level;
				
				void computePosition () {
					m_origin = m_parent->origin() + (TraitsT::normalOriginForPartitionIndex(m_location) * m_parent->size());
					m_size = m_parent->size() / 2;
					m_level = m_parent->level() + 1;
				}
				
			public:
				Partition (AlignedTree * base, const VecT & origin, const VecT & size)
					: m_origin(origin), m_size(size), m_level(0)
				{
					m_parent = NULL;
					m_location = (typename TraitsT::PartitionLocation)0;
					m_base = base;
					
					bzero(&m_children, sizeof(m_children));
				}
				
				Partition (Partition *parent, typename TraitsT::PartitionLocation location)
					: m_parent(parent), m_location(location)
				{
					ensure(m_parent->m_children[m_location] == NULL);
					
					m_parent->m_children[m_location] = this;
					m_base = parent->m_base;
					
					bzero(&m_children, sizeof(m_children));
					
					computePosition();
				}
				
				~Partition () {
					for (unsigned i = 0; i < TraitsT::Q; i += 1) {
						if (m_children[i] != NULL) {
							delete m_children[i];
						}
					}
				}
					
				VecT origin () const {
					return m_origin;
				}
				
				VecT size () const {
					return m_size;
				}

				SpaceT boundingBox() const {
					return SpaceT(m_origin, m_origin + m_size);
				}
				
				unsigned level () const {
					return m_level;
				}

			public:
				// Returns a given child partition.
				Partition* child (unsigned i) { return m_children[i]; }
				const Partition* child (unsigned i) const { return m_children[i]; }
				
				// Returns the parent partition.
				Partition* parent () { return m_parent; }
				const Partition* parent () const { return m_parent; }
				
				// Returns the objects in this partition.
				ObjectSetT& objects() { return m_objects; }
				const ObjectSetT& objects() const { return m_objects; }
				
				void debug (std::string indent) {
					std::cout << indent << "Partition " << m_location << " has " << objects().size() << " objects." << std::endl;
					
					for (unsigned i = 0; i < TraitsT::Q; i++) {
						if (m_children[i] != NULL) {
							m_children[i]->debug(indent + "\t");
						}
					}
				}
				
				// Visitor pattern.
				template <typename VisitorT>
				void visit (VisitorT & visitor) {
					for (unsigned i = 0; i < TraitsT::Q; i++) {
						if (m_children[i] != NULL) {
							m_children[i]->visit(visitor);
						}
					}
					
					visitor.accept(this);
				}
				
				// Core function to redistribute objects into children subdivisions if possible.
				// Can potentially take a long time to execute. O(QN)
				void redistribute () {
					for (unsigned i = 0; i < TraitsT::Q; i += 1) {
						if (m_children[i] == NULL) new Partition(this, (typename TraitsT::PartitionLocation)i);
					}
					
					ObjectSetT resort;
					std::swap(m_objects, resort);

					foreach(object, resort) {
						this->insert(*object);
					}
				}
				
				// Join this partition with its parent.
				void coalesce () {
					//m_parent->m_children[m_location] = NULL;
					//delete this;
				}
				
				// Insert an object in this partition or a child.
				Partition * insert (ObjectT object)
				{	
					Partition * cur = find(object);
					
					cur->objects().insert(object);
					
					return cur;
				}
				
				// Erase an object in this partition or a child.
				Partition * erase (ObjectT object)
				{
					Partition * cur = find(object);
					
					cur->objects().erase(object);
					
					return cur;
				}
				
				// Find an object in this partition or a child.
				Partition * find (ObjectT object) {
					SpaceT b = TraitsT::calculateBoundingBox(object);
					Partition *cur = this;
					
					unsigned i = 0;
					while (i < TraitsT::Q) {
						Partition *c = cur->child(i);
						
						if (c && c->boundingBox().containsBox(b)) {
							// Descend into the current partition
							cur = c;
							i = 0;
						} else {
							// Try the next partition
							i += 1;
						}
					}
					
					return cur;
				}
				
				// Return the smallest partition for the given point.
				Partition* partitionForPoint (const Vec2 &point) {
					if (boundingBox().containsPoint(point, true)) {
						// A child potentially contains the point
						Partition *t = NULL;
						for (unsigned i = 0; i < TraitsT::Q; i += 1) {
							if (child(i)) {
								t = child(i).partitionForPoint(point);
							
								if (t) return t;
							}
						}
					}
					
					// Otherwise, we must contain the point
					return this;
				}
				
				// Return the smallest partition for the given rect.
				Partition* partitionForRect (const SpaceT &rect) {
					if (boundingBox().containsBox(rect, true)) {
						// A child potentially contains the point
						Partition *t = NULL;
						for (unsigned i = 0; i < TraitsT::Q; i += 1) {
							if (child(i)) {
								t = child(i)->partitionForRect(rect);
							
								if (t) return t;
							}
						}
					}
					
					return this;
				}
				
				// Return all objets in the given partition including children.
				ObjectSetT allObjects () {
					ObjectSetT objects;
					Partition * cur = this;
					
					while (cur != NULL) {
						objects.insert(cur->m_objects.begin(), cur->m_objects.end());
						
						cur = cur->m_parent;
					}
					
					return objects;
				};
				
				// Return the set of objects in a given rectangle.
				ObjectSetT objectsInRect (const SpaceT & _rect) {
					SpaceT rect(_rect);
					rect.clipToBox(this->boundingBox());
					
					ObjectSetT selection;
					
					foreach(o, m_objects) {
						SpaceT b = TraitsT::calculateBoundingBox(*o);
						
						if (b.intersectsWith(rect)) {
							selection.insert(*o);
						}
					}
					
					for (unsigned i = 0; i < TraitsT::Q; i += 1) {
						if (child(i) == NULL) continue;
					
						if (child(i)->boundingBox().intersectsWith(rect)) {
							ObjectSetT childSelection = child(i)->objectsInRect(rect);

							selection.insert(childSelection.begin(), childSelection.end());
						}
					}
					
					return selection;
				}
				
				// This function probably needs to be fixed
				ObjectSetT objectsAlongLine (const LineSegment<TraitsT::D> &l) {								
					ObjectSetT selection;
					
					for (unsigned i = 0; i < TraitsT::Q; i += 1) {
						if (child(i) == NULL) continue;
					
						if (child(i)->boundingBox().intersectsWith(l, true)) {
							// Add all current-level objects
							selection.insert(child(i)->objects().begin(), child(i)->objects().end());
							
							// Add any more children objects
							ObjectSetT objs = child(i)->objectsAlongLine(l);
							selection.insert(objs.begin(), objs.end());
						}
					}
					
					return selection;
				}
			};
			
		protected:		
			SpaceT m_bounds;
			Partition *m_top;
			
			void expand (const unsigned & dir) {
				if (dir && LEFT) {
					if (dir && TOP) {
					
					} else {
					
					}
				}
			}
			
		public:
			AlignedTree (const VecT & origin, const VecT & size) : m_bounds(origin, origin + size), m_expanding(true) {
				m_top = new Partition(this, origin, size);
			}
			
			// The top partition in the tree.
			Partition * top () {
				return m_top;
			}
			
			const Partition * top () const {
				return m_top;
			}
			
			bool m_expanding;
			bool expanding () const { return m_expanding; }
			void setExpanding (bool expanding) { m_expanding = expanding; }
			
			// Copy objects from another STL container.
			template <typename IteratorT>
			void insert (IteratorT begin, IteratorT end) {
				for (; begin != end; ++begin) {
					addObject(*begin);
				}
			}
			
			// Insert an object if it exists, redistribute the tree if appropriate.
			Partition * insert (ObjectT o, bool redistribute = true) {
				SpaceT b = TraitsT::calculateBoundingBox(o);
							
				if (!m_top->boundingBox().containsBox(b))
					return NULL;
				
				Partition * p = m_top->insert(o);
				
				if (redistribute && p->objects().size() > 16) {
					p->redistribute();
				}
				
				return p;
			}
			
			// Find an object if it exists.
			Partition * find (ObjectT o) {
				SpaceT b = TraitsT::calculateBoundingBox(o);
							
				if (!m_top->boundingBox().intersectsWith(b))
					return NULL;
				
				return m_top->find(o);
			}
			
			// Erase an object if it exists.
			Partition * erase (ObjectT o, bool coalesce = true) {
				Partition * p = find(o);
				
				if (p) {
					p->objects().erase(o);
				}
				
				if (p->objects().size() == 0) {
					p->coalesce();
				}
				
				return p;
			}
			
			// Find the smallest partition which encloses the given point.
			Partition* partitionForPoint (const Vec2 &point) {
				return m_top->partitionForPoint(point);
			}
			
			// Find the smallest partition which encloses the given rectangle.
			Partition* partitionForRect (const SpaceT &rect) {
				return m_top->partitionForRect(rect);
			}
		};
	}
}

#endif
