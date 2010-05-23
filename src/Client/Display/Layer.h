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
#include "Input.h"

namespace Dream {
	namespace Client {
		namespace Display {
		
			class IScene;
			class ISceneManager;
			
			class ILayer : IMPLEMENTS(Object), IMPLEMENTS(InputHandler)
			{
				EXPOSE_INTERFACE(Layer)
				
				class Class : IMPLEMENTS(Object::Class), IMPLEMENTS(InputHandler::Class)
				{	
				};
				
				public:
					virtual void renderFrameForTime (IScene * scene, TimeT time) abstract;
					
					virtual void didBecomeCurrent (ISceneManager * manager, IScene * scene) abstract;
					virtual void willRevokeCurrent (ISceneManager * manager, IScene * scene) abstract;
			};
			
		}
	}
}

#endif
