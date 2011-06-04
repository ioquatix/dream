/*
 *  Client/Display/Layer.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 26/07/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_LAYER_H
#define _DREAM_CLIENT_DISPLAY_LAYER_H

#include "../../Core/Timer.h"
#include "../../Numerics/Matrix.h"
#include "Input.h"

#include <set>

namespace Dream {
	namespace Client {
		namespace Display {
		
			class IScene;
			class ISceneManager;
			
			class ILayer : implements IObject, implements IInputHandler
			{
				public:
					virtual void renderFrameForTime (IScene * scene, TimeT time);
					
					virtual void didBecomeCurrent (ISceneManager * manager, IScene * scene);
					virtual void willRevokeCurrent (ISceneManager * manager, IScene * scene);
			};
			
#pragma mark -
						
			class Group : public Object, implements ILayer
			{
			public:
				typedef std::set<REF(ILayer)> ChildrenT;
			
				protected:
					ChildrenT m_children;
				
				public:
					virtual void renderFrameForTime (IScene * scene, TimeT time);
					
					virtual void didBecomeCurrent (ISceneManager * manager, IScene * scene);
					virtual void willRevokeCurrent (ISceneManager * manager, IScene * scene);
					
					void add(PTR(ILayer) child);
					void remove(PTR(ILayer) child);
					
					ChildrenT & children() { return m_children; }
					const ChildrenT & children() const { return m_children; }
			};
			
		}
	}
}

#endif
