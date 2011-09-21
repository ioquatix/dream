/*
 *  Entity.cpp
 *  Model Builder
 *
 *  Created by Samuel Williams on 21/03/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#include "Entity.h"

namespace Dream {
	namespace Simulation {
		Entity::~Entity() {
			
		}
		
		void Entity::render (Renderer* r) const {		
			foreach(root, m_roots) {						
				(*root)->render(r);
			}
		}
		
#pragma mark -
		
		EntityComponent::EntityComponent (StringT name, REF(IRenderable) renderable) 
			: m_name(name), m_renderable(renderable), m_useColor(false) {
			
		}
		
		EntityComponent::EntityComponent (StringT name) 
			: m_name(name), m_useColor(false) {
			m_renderable = NULL;
		}
		
		void EntityComponent::render (Renderer* r) const {		
			if (m_renderable) {
				m_renderable->render(r);
			}
		}
		
		EntityComponent::~EntityComponent () {
		}
		
#pragma mark -
		EntityNode::EntityNode (EntityNode *parent, index_t parentAxisIndex, index_t childAxisIndex, EntityComponent *component) {
			m_component = component;
			m_parent = parent;
			
			m_entity = parent->m_entity;
			
			assert (parent != NULL && "parent node must not be null using this constructor");
			
			m_parentAxisIndex = parentAxisIndex;
			m_childAxisIndex = childAxisIndex;
			
			m_parent->m_children.push_back(this);
			m_entity->addNode(this);
		}
		
		EntityNode::EntityNode (Entity *entity, index_t entityAxisIndex, EntityComponent *component) {
			m_entity = entity;
			m_component = component;
			m_parent = NULL;
			
			m_parentAxisIndex = 0;
			m_childAxisIndex = entityAxisIndex;
			
			m_entity->addNode(this, true /* = root node */);
		}
		
		EntityNode::~EntityNode () {
			
		}

		const IAxis* EntityNode::parentAxis () const {
			if (parent() != NULL)
				return parent()->axis(m_parentAxisIndex);
			else
				return m_entity->axis();
		}
		
		const IAxis* EntityNode::childAxis() const {
			return axis(m_childAxisIndex);
		}
		
		void EntityNode::render (Renderer *r) const {
			//if (!visible())
			//	return;
			
			//Push a translation matrix onto the stack
			glPushMatrix();
			
			//Reposition ourselves with the parent
			childAxis()->mateWith(parentAxis()).glMultMatrix();
			
			//Render the mesh
			m_component->render(r);
			
			//Render all children
			foreach(child, m_children) {
				(*child)->render(r);
			}
			
			//Restore the state
			glPopMatrix();
		}

	}
}
