/*
 *  Client/Display/Scene.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 26/06/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_SCENE_H
#define _DREAM_CLIENT_DISPLAY_SCENE_H

#include "../../Core/Timer.h"
#include "../../Resources/Loader.h"

#include "Input.h"
#include "Context.h"
#include "Renderer.h"

#include <set>

namespace Dream
{
	namespace Client
	{
		namespace Display
		{			
			using namespace Dream::Core;
			using namespace Dream::Numerics;
			
			using Dream::Resources::ILoader;
			using Dream::Events::Loop;
			
			class ISceneManager;
			
	#pragma mark -
	#pragma mark Base Scene Interfaces
			
			/** Abstract representation of graphical sequence.
			 
			 The IScene class represents the following sequential process:
				- Loading and processing some resources
				- Displaying some frames for a period of time (models, user interface, etc)
				- Eventually finishing after a period of time or after some kind of input
				
			 The scene class should itself be a "controller" and a model should be built which is utilized. A model should have an appropriate renderer "view".
			 Therefore, the logic in the scene class is generally limited to delegation and coordination of the following:
				- Local user input
				- Remote user input (network, ai, etc)
			 */
			class IScene : IMPLEMENTS(Object), IMPLEMENTS(InputHandler)
			{
				EXPOSE_INTERFACE(Scene)
				
				class Class : IMPLEMENTS(Object::Class), IMPLEMENTS(InputHandler::Class)
				{
				};
				
			public:
				/// Callback from the ISceneManager to indicate that the scene will begin rendering frames.
				virtual void willBecomeCurrent (ISceneManager *) abstract;
				
				/// Callback from the ISceneManager to inidicate that the scene has loaded all resources and needs to prepare for rendering.
				/// Called directly after willBecomeCurrent.
				virtual void didBecomeCurrent () abstract;
				
				/// Callback from the ISceneManager to indicate that the scene will no longer be rendering frames
				virtual void willRevokeCurrent (ISceneManager *) abstract;
				
				/// Render a frame which will be displayed at ''time''.
				virtual void renderFrameForTime (TimeT time) abstract;
				
				/// The renderer for the scene
				virtual RendererT * renderer () abstract;
				
				/// The current scene manager controlling this scene
				virtual ISceneManager * manager () abstract;
				
				/// The loader for resources required by this scene
				virtual ILoader * resourceLoader () abstract;
				
				/// Returns the time from the first frame til this frame.
				virtual TimeT currentTime () const abstract;
			};
			
			/** Abstract controller of scenes and scene changes.
			 
			 This class should generally be implemented by your Application class in most cases. This class is used to effectively control how scenes progress,
			 for example, you might have the following progression:
				- IntroScene
				- MainMenuScene
				- SinglePlayerGameScene / MultiPlayerGameScene
				- QuitScene
			 
			 */
			class ISceneManager : IMPLEMENTS(Object)
			{
				EXPOSE_INTERFACE(SceneManager)
				
				class Class : IMPLEMENTS(Object::Class)
				{
				};

			protected:
				/// Provide the next scene - this is called when the currentSceneIsFinished() method is called.
				virtual REF(IScene) provideNextScene () abstract;
				
			public:			
				/// Returns the current scene that is being rendered
				virtual REF(IScene) currentScene () abstract;
				
				/// Returns the display context which the manager is providing scenes for.
				virtual REF(IContext) displayContext () abstract;
				
				/// Returns the event loop which the manager is running as part of.
				virtual REF(Loop) eventLoop () abstract;
				
				/// Returns the resource loader for the current scene
				virtual REF(ILoader) resourceLoader () abstract;
				
				/// Render a frame which will be displayed at ''time''.
				/// Default implementation uses currentScene().
				virtual void renderFrameForTime (TimeT time);
				
				/// Called by the current scene to notify the manager that it has completed and the next scene should be shown.
				/// The next scene is determined by calling provideNextScene(). This is done at the beginning of renderFrameForTime().
				virtual void currentSceneIsFinished () abstract;
			};
			
	#pragma mark -
			
			class SceneManager : public Object, IMPLEMENTS(SceneManager)
			{
				EXPOSE_CLASS(SceneManager)
				
				class Class : public Object::Class, IMPLEMENTS(SceneManager::Class)
				{
					EXPOSE_CLASSTYPE
				};
			
				typedef std::list<REF(IScene)> ScenesT;
				typedef boost::function<void (ISceneManager *)> FinishedCallbackT;
				
			protected:			
				ScenesT m_scenes;
				REF(IScene) m_currentScene;
				
				REF(IContext) m_displayContext;
				REF(Loop) m_eventLoop;
				REF(ILoader) m_resourceLoader;
				
				virtual REF(IScene) provideNextScene ();
				
				FinishedCallbackT m_finishedCallback;
				
				bool m_currentSceneIsFinished;
				
				void updateCurrentScene ();
				
			public:
				SceneManager (REF(IContext) displayContext, REF(Loop) eventLoop, REF(ILoader) resourceLoader);
				virtual ~SceneManager ();
				
				/// Pushses a scene on to the top of the current stack - i.e. it will replace the current scene, but the current scene will be retained.
				void pushScene (REF(IScene) scene);
				/// Replaces the current scene - it will not be retained.
				void replaceScene (REF(IScene) scene);
				/// Adds a scene on to the back of the scene queue. It will be displayed after all prior scenes have finished.
				void appendScene (REF(IScene) scene);
				
				ScenesT & scenes () { return m_scenes; }
				const ScenesT & scenes () const { return m_scenes; }
							
				virtual REF(IScene) currentScene ();
				virtual REF(IContext) displayContext ();
				virtual REF(Loop) eventLoop ();
				virtual REF(ILoader) resourceLoader ();
				
				virtual void currentSceneIsFinished ();
				
				/// Calls provideNextScene if there is no current scene.
				virtual void renderFrameForTime (TimeT time);
				
				/// Process any events available from the display context and pass them on to the current scene.
				virtual void processPendingEvents ();
				
				virtual void setFinishedCallback (FinishedCallbackT callback);
			};
						
			class Scene : public Object, IMPLEMENTS(Scene)
			{
				EXPOSE_CLASS(Scene)
				
				class Class : public Object::Class, IMPLEMENTS(Scene::Class)
				{
					EXPOSE_CLASSTYPE
				};
				
			protected:
				ISceneManager * m_sceneManager;
				
				bool m_firstFrame;
				TimeT m_startTime, m_currentTime;
				
			public:
				Scene ();
				virtual ~Scene();
								
				virtual void willBecomeCurrent (ISceneManager *);
				
				/// Used to process layers which have been created in willBecomeCurrent.
				virtual void didBecomeCurrent ();
				
				virtual void willRevokeCurrent (ISceneManager *);
				
				virtual bool resize (const Display::ResizeInput & ipt);
				virtual bool event (const Display::EventInput & ipt);
				
				virtual RendererT * renderer ();
				virtual ISceneManager * manager ();
				virtual ILoader * resourceLoader ();
				virtual TimeT currentTime () const;
				
				virtual void renderFrameForTime (TimeT time);
			};
			
			class VoidScene : public Scene
			{
				EXPOSE_CLASS(VoidScene)
				
				class Class : public Scene::Class
				{
					EXPOSE_CLASSTYPE
				};
				
			public:
				VoidScene ();
				virtual ~VoidScene ();
				
				virtual bool process (const Display::Input & input);
				virtual void renderFrameForTime (TimeT time);
				
				static REF(VoidScene) sharedInstance ();
			};
		}
	}
}

#endif
