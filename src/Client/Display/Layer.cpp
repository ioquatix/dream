/*
 *  Client/Display/Layer.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 26/07/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Layer.h"

namespace Dream {
	namespace Client {
		namespace Display {

			void ILayer::renderFrameForTime (IScene * scene, TimeT time) {
			
			}
			
			void ILayer::didBecomeCurrent (ISceneManager * manager, IScene * scene) {
			
			}
			
			void ILayer::willRevokeCurrent (ISceneManager * manager, IScene * scene) {
			
			}

#pragma mark -

			void Group::renderFrameForTime (IScene * scene, TimeT time)
			{
				for (ChildrenT::iterator i = m_children.begin(); i != m_children.end(); i++)
				{
					(*i)->renderFrameForTime(scene, time);
				}
			}
			
			bool Group::process (const Input & input)
			{
				bool result = false;
				
				for (ChildrenT::iterator i = m_children.begin(); i != m_children.end(); i++)
				{
					result |= (*i)->process(input);
				}
				
				return false;
			}
			
			void Group::didBecomeCurrent (ISceneManager * manager, IScene * scene)
			{
				for (ChildrenT::iterator i = m_children.begin(); i != m_children.end(); i++)
				{
					(*i)->didBecomeCurrent(manager, scene);
				}
			}
			
			void Group::willRevokeCurrent (ISceneManager * manager, IScene * scene)
			{
				for (ChildrenT::iterator i = m_children.begin(); i != m_children.end(); i++)
				{
					(*i)->willRevokeCurrent(manager, scene);
				}
			}
			
			void Group::add(PTR(ILayer) child)
			{
				m_children.push_back(child);
			}
			
			void Group::remove(PTR(ILayer) child)
			{
				//m_children.erase(child);
			}
			
		}
	}
}