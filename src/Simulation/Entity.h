/*
 *  Entity.h
 *  Model Builder
 *
 *  Created by Samuel Williams on 21/03/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#ifndef __DREAM_ENTITY__
#define __DREAM_ENTITY__

#include "Framework.h"
#include "Renderer.h"
#include "Mesh.h"
#include "EntityNode.h"

#include <map>

namespace Dream {
	namespace Simulation {
	
		class EntityNode;
		
		class Entity {
	protected:
			typedef std::vector<EntityNode*> NodeArray;
			
			/* An array of all nodes */
			NodeArray m_nodes;
			
			/* An array of the root nodes */
			NodeArray m_roots;
			
			StringT m_name;

			const IAxis *m_location;
			
	public:
			Entity (StringT name) : m_name(name) {
				
			}
			
			Entity (StringT name, const IAxis *location) : m_name(name), m_location(location) {
				
			}
			
			virtual ~Entity();
			
			NodeArray rootNodes () {
				return m_roots;
			}
			
			const IAxis* axis() const {
				return m_location;
			}
			
			void render (Renderer* r) const;
			
			const StringT name() const {
				return m_name;
			}
			
			void setName(StringT newName) {
				m_name = newName;
			}

	protected:
			friend class Dream::EntityNode;
			
			index_t addNode (EntityNode* node, bool root = false) {
				m_nodes.push_back (node);
				
				if (root)
					m_roots.push_back(node);
				
				return m_nodes.size() - 1;
			}
		};
		
#pragma mark -

		class EntityComponent {
			public:
				typedef std::vector<IAxis*> AxisArray;
		
			protected:
				StringT m_name;
				
				REF(IRenderable) m_renderable;
				AxisArray m_axes;
				
				bool m_useColor;
				Vec3 m_color;
				
			public:
				EntityComponent (StringT name, REF(IRenderable) renderable);
				EntityComponent (StringT name);
				virtual ~EntityComponent();
				
				REF(IRenderable) renderable () {
					return m_renderable;
				}
				
				const REF(const IRenderable) renderable() const {
					return m_renderable;
				}
				
				const StringT name () {
					return m_name;
				}
				
				void setName (StringT name) {
					m_name = name;
				}
				
				void setColor (const Vec3 &color) {
					m_useColor = true;
					m_color = color;
				}
				
				index_t addAxis (IAxis* axis) {
					m_axes.push_back(axis);
					
					return m_axes.size() - 1;
				}
				
				AxisArray& axes () {
					return m_axes;
				}
				
				const AxisArray& axes () const {
					return m_axes;
				}
				
				IAxis* operator[] (index_t i) {
					return m_axes[i];
				}
				
				const IAxis* operator[] (index_t i) const {
					return m_axes[i];
				}
						
				void render (Renderer* r) const;
		};

	}
}

#endif
