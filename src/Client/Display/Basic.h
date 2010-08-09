/*
 *  Dream/Client/Display/Basic.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 14/05/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_BASIC_H
#define _DREAM_CLIENT_DISPLAY_BASIC_H

#include "Application.h"
#include "Scene.h"
#include "Layer.h"

#include "../../Events/Loop.h"
#include "../../Resources/Loader.h"

namespace Dream {
	namespace Client {
		namespace Display {
			
			class BasicApplication : public Object, IMPLEMENTS(Application)
			{
				EXPOSE_CLASS(BasicApplication)
				
				class Class : public Object::Class, IMPLEMENTS(Application::Class)
				{
					EXPOSE_CLASSTYPE
					
					virtual REF(IApplication) init (PTR(Dictionary) config);
				};
			
				private:
					void setupResourceLoader ();
				
				protected:
					TimerStatistics m_stats;
				
					REF(Events::Loop) m_eventLoop;
					REF(IContext) m_displayContext;
					REF(Resources::ILoader) m_resourceLoader;
					
					REF(SceneManager) m_sceneManager;
					
					virtual void finishedCallback ();
					
					virtual void frameCallback (TimeT at);
					virtual void run ();

				public:
					BasicApplication (PTR(Dictionary) config);
					
					void append (PTR(IScene) scene);
					void push (PTR(IScene) scene);
					void replace (PTR(IScene) scene);
			};
			
			class BasicScene : public Scene
			{
				EXPOSE_CLASS(BasicScene)
				
				class Class : public Scene::Class
				{
					EXPOSE_CLASSTYPE
				};
			
				protected:
					/// Layers should (generally) be allocated in the Scene constructor, so that
					/// they will be invoked correctly for willBecomeCurrent and willRevokeCurrent.
					std::vector<REF(ILayer)> m_layers;
				
					void renderLayersForTime (TimeT time);
					bool processInputForLayers (const Input & input);
					
				public:
					BasicScene ();
					virtual ~BasicScene ();
					
					virtual bool process (const Input & input);
					
					virtual void didBecomeCurrent ();
					virtual void willRevokeCurrent (ISceneManager *);
					
					virtual void renderFrameForTime (TimeT time);
					
					void add (PTR(ILayer) layer);
			};
			
		}
	}
}

#endif
