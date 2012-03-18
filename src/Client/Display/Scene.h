//
//  Client/Display/Scene.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 26/06/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CLIENT_DISPLAY_SCENE_H
#define _DREAM_CLIENT_DISPLAY_SCENE_H

#include "../../Core/Timer.h"
#include "../../Resources/Loader.h"

#include "../../Events/Input.h"
#include "Context.h"

#include <set>
#include <list>
#include <functional>

namespace Dream
{
	namespace Client
	{
		namespace Display
		{			
			using namespace Dream::Core;
			using namespace Dream::Numerics;
			
			using Dream::Resources::ILoader;
			using namespace Dream::Events;
			
			class ISceneManager;
			
#pragma mark -
			
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
			class IScene : implements IObject, implements IInputHandler
			{
			public:
				/// Callback from the ISceneManager to indicate that the scene will begin rendering frames.
				virtual void will_become_current (ISceneManager *) abstract;
				
				/// Callback from the ISceneManager to inidicate that the scene has loaded all resources and needs to prepare for rendering.
				/// Called directly after will_become_current.
				virtual void did_become_current () abstract;
				
				/// Callback from the ISceneManager to indicate that the scene will no longer be rendering frames.
				virtual void will_revoke_current (ISceneManager *) abstract;
				
				/// Render a frame which will be displayed at ''time''.
				virtual void render_frame_for_time (TimeT time) abstract;
				
				/// The current scene manager controlling this scene
				virtual ISceneManager * manager () abstract;
				
				/// The loader for resources required by this scene
				virtual ILoader * resource_loader () abstract;
				
				/// Returns the time from the first frame til this frame.
				virtual TimeT current_time () const abstract;
			};
			
			/** Abstract controller of scenes and scene changes.
			 
			 This class should generally be implemented by your Application class in most cases. This class is used to effectively control how scenes progress,
			 for example, you might have the following progression:
				- IntroScene
				- MainMenuScene
				- SinglePlayerGameScene / MultiPlayerGameScene
				- QuitScene
			 
			 */
			class ISceneManager : implements IObject
			{
			protected:
				/// Provide the next scene - this is called when the current_scene_is_finished() method is called.
				virtual Ref<IScene> provide_next_scene () abstract;
				
			public:			
				/// Returns the current scene that is being rendered
				virtual Ref<IScene> current_scene () abstract;
				
				/// Returns the display context which the manager is providing scenes for.
				virtual Ref<IContext> display_context () abstract;
				
				/// Returns the event loop which the manager is running as part of.
				virtual Ref<Loop> event_loop () abstract;
				
				/// Returns the resource loader for the current scene
				virtual Ref<ILoader> resource_loader () abstract;
				
				/// Render a frame which will be displayed at ''time''.
				/// Default implementation uses current_scene().
				virtual void render_frame_for_time (TimeT time);
				
				/// Called by the current scene to notify the manager that it has completed and the next scene should be shown.
				/// The next scene is determined by calling provide_next_scene(). This is done at the beginning of render_frame_for_time().
				virtual void current_scene_is_finished () abstract;
				
				/// Dequeue input from the associated context.
				virtual void process_pending_events (IInputHandler * handler) abstract;
			};
			
#pragma mark -

			/// Scenes are composed of layers of content which can handle input and render graphics.
			class ILayer : implements IObject, implements IInputHandler
			{
				public:
					virtual void render_frame_for_time (IScene * scene, TimeT time);
					
					virtual void did_become_current (ISceneManager * manager, IScene * scene);
					virtual void will_revoke_current (ISceneManager * manager, IScene * scene);
			};

#pragma mark -
	
			/** A group is a non-specific collection of children layers.
			
			A group allows for multiple children layers to be rendered together. This serves
			as the base class for nodes like TransformedGroup or Viewport which allow for
			specific functionality to be applied before further rendering takes place.
			*/
			class Group : public Object, implements ILayer
			{
			public:
				typedef std::vector<Ref<ILayer>> ChildrenT;
			
				protected:
					ChildrenT _children;
				
				public:
					virtual void render_frame_for_time (IScene * scene, TimeT time);
					
					virtual void did_become_current (ISceneManager * manager, IScene * scene);
					virtual void will_revoke_current (ISceneManager * manager, IScene * scene);
					
					virtual bool process (const Input & input);
					
					void add (Ptr<ILayer> child);
					void remove (Ptr<ILayer> child);
					void remove_all ();
					
					ChildrenT & children() { return _children; }
					const ChildrenT & children() const { return _children; }
			};
			
#pragma mark -
			
			/// A stack-based scene manager which can support typical game logic.
			class SceneManager : public Object, implements ISceneManager, implements IContextDelegate, implements IInputHandler
			{
			public:
				typedef std::list<Ref<IScene>> ScenesT;
				typedef std::function<void (ISceneManager *)> FinishedCallbackT;
				
			protected:
				Stopwatch _stopwatch;
				TimerStatistics _stats;
				
				ScenesT _scenes;
				Ref<IScene> _current_scene;
				
				Ref<IContext> _display_context;
				Ref<Loop> _event_loop;
				Ref<ILoader> _resource_loader;
				
				virtual Ref<IScene> provide_next_scene ();
				
				FinishedCallbackT _finished_callback;
				
				bool _current_scene_is_finished;
				
				void update_current_scene ();
				
				/// Queue input from display context.
				InputQueue _input_queue;
								
			public:
				static Ref<Resources::ILoader> default_resource_loader ();
				
				SceneManager (Ref<IContext> display_context, Ref<Loop> event_loop, Ref<ILoader> resource_loader);
				virtual ~SceneManager ();
				
				/// Pushses a scene on to the top of the current stack - i.e. it will replace the current scene, but the current scene will be retained.
				void push_scene (Ref<IScene> scene);
				/// Replaces the current scene - it will not be retained.
				void replace_scene (Ref<IScene> scene);
				/// Adds a scene on to the back of the scene queue. It will be displayed after all prior scenes have finished.
				void append_scene (Ref<IScene> scene);
				
				ScenesT & scenes () { return _scenes; }
				const ScenesT & scenes () const { return _scenes; }
							
				virtual Ref<IScene> current_scene();
				virtual Ref<IContext> display_context();
				virtual Ref<Loop> event_loop();
				virtual Ref<ILoader> resource_loader();
				
				virtual void current_scene_is_finished();
				
				/// Calls provide_next_scene if there is no current scene.
				virtual void render_frame_for_time(Ptr<IContext> context, TimeT time);
				
				using ISceneManager::render_frame_for_time;
				
				/// Process any events available from the display context and pass them on to the current scene.
				virtual void process_input(Ptr<IContext> context, const Input & input);
				virtual void process_pending_events(IInputHandler * handler);
				
				virtual bool event(const Display::EventInput & input);
				
				virtual void set_finished_callback(FinishedCallbackT callback);
			};
			
			/// A basic scene implementation which manages the context for a single logical part of the application.
			class Scene : public Group, implements IScene
			{
			protected:
				ISceneManager * _scene_manager;
				
				bool _first_frame;
				TimeT _start_time, _current_time;
				
			public:
				Scene ();
				virtual ~Scene();
				
				virtual void will_become_current (ISceneManager *);
				using Group::will_revoke_current;
				
				/// Used to process layers which have been created in will_become_current.
				virtual void did_become_current();
				using Group::did_become_current;
				
				virtual void will_revoke_current(ISceneManager *);
				
				virtual ISceneManager * manager ();
				virtual ILoader * resource_loader ();
				virtual TimeT current_time () const;
				
				virtual void render_frame_for_time (TimeT time);
				using Group::render_frame_for_time;
			};
			
			/// A place-holder scene that does nothing.
			class VoidScene : public Scene 
			{
			public:
				VoidScene ();
				virtual ~VoidScene ();
				
				virtual bool process (const Display::Input & input);
				virtual void render_frame_for_time (TimeT time);
				
				static Ref<VoidScene> shared_instance ();
			};
		}
	}
}

#endif
