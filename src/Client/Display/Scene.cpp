//
//  Client/Display/Scene.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 26/06/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#include "Scene.h"
#include "Context.h"

#include "../../Geometry/AlignedBox.h"

// Resource loader
#include "../../Imaging/Image.h"
#include "../../Text/Font.h"
#include "../Audio/Sound.h"
#include "../Audio/OggResource.h"

#include <numeric>
#include <algorithm>

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			void ISceneManager::render_frame_for_time (TimeT time)
			{
				Ref<IScene> s = current_scene();
				
				if (s) 
					s->render_frame_for_time(time);
			}
			
	// MARK: mark -
			
			Ref<Resources::ILoader> SceneManager::default_resource_loader ()
			{
				Ref<Resources::Loader> loader = new Resources::Loader;
				
				loader->add_loader(new Imaging::Image::Loader);
				loader->add_loader(new Client::Audio::Sound::Loader);
				loader->add_loader(new Client::Audio::OggResource::Loader);
				loader->add_loader(new Text::Font::Loader);
				
				return loader;
			}
			
			SceneManager::SceneManager (Ref<IContext> display_context, Ref<Loop> event_loop, Ref<ILoader> resource_loader)
				: _display_context(display_context), _event_loop(event_loop), _resource_loader(resource_loader), _current_scene_is_finished(true)
			{
				_display_context->set_delegate(this);
				
				_stopwatch.start();
			}
			
			SceneManager::~SceneManager ()
			{
				
			}
			
			void SceneManager::push_scene (Ref<IScene> scene)
			{
				// Save the current scene on top of the queue.
				if (_current_scene)
					_scenes.push_front(_current_scene);
				
				replace_scene(scene);
			}
			
			void SceneManager::replace_scene (Ref<IScene> scene)
			{
				_scenes.push_front(scene);
				
				// Clear the current scene. Any attempt to render will load up the first scene.
				_current_scene_is_finished = true;
			}
			
			void SceneManager::append_scene (Ref<IScene> scene)
			{
				_scenes.push_back(scene);
			}
			
			Ref<IScene> SceneManager::current_scene ()
			{
				if (_current_scene)
					return _current_scene;
				else
					return VoidScene::shared_instance();
			}
			
			Ref<IContext> SceneManager::display_context ()
			{
				return _display_context;
			}
			
			Ref<Loop> SceneManager::event_loop ()
			{
				return _event_loop;
			}
			
			Ref<ILoader> SceneManager::resource_loader ()
			{
				return _resource_loader;
			}
			
			void SceneManager::current_scene_is_finished ()
			{
				_current_scene_is_finished = true;
			}
			
			void SceneManager::update_current_scene ()
			{
				_current_scene_is_finished = false;
				
				Ref<IScene> s = provide_next_scene();
				
				if (_current_scene) {
					_current_scene->will_revoke_current(this);
				}
				
				if (s) {
					_current_scene = s;
					_current_scene->will_become_current(this);
					_current_scene->did_become_current();
				} else {
					if (_finished_callback)
						_finished_callback(this);
					
					_current_scene = NULL;
				}
			}
			
			Ref<IScene> SceneManager::provide_next_scene ()
			{
				Ref<IScene> s;
				
				if (!_scenes.empty()) {
					s = _scenes.front();
					_scenes.pop_front();
				}
				
				return s;
			}
			
			void SceneManager::render_frame_for_time(Ptr<IContext> context, TimeT time)
			{
				_stats.begin_timer(_stopwatch.time());

				if (!_current_scene || _current_scene_is_finished)
					update_current_scene();
				
				ISceneManager::render_frame_for_time(time);
								
				_stats.update(_stopwatch.time());
				
				if (_stats.update_count() > (60 * 10)) {
					LogBuffer buffer;
					buffer << "FPS: " << _stats.updates_per_second();
					buffer << std::setw(4) << " (Max: " << (1.0 / _stats.minimum_duration()) << " Min: " << (1.0 / _stats.maximum_duration()) << ")";
					logger()->log(LOG_INFO, buffer);
					
					_stats.reset();
				}				
			}
			
			void SceneManager::process_input (Ptr<IContext> context, const Input & input)
			{
				if (!process(input)) {				
					// Add the event to the thread-safe queue.
					_input_queue.process(input);
				}
			}
			
			void SceneManager::process_pending_events (IInputHandler * handler)
			{
				// Remove a block of events from the input queue and pass to the handler for processing.
				_input_queue.dequeue(handler);
			}
			
			bool SceneManager::event (const Display::EventInput & ipt)
			{
				if (ipt.event() == EventInput::EXIT) {
					event_loop()->stop();
					_display_context->stop();
					
					return true;
				}
				
				return false;
			}
			
			void SceneManager::set_finished_callback (FinishedCallbackT callback)
			{
				_finished_callback = callback;
			}
			
// MARK: mark -
			
			void ILayer::render_frame_for_time (IScene * scene, TimeT time) {
			
			}
			
			void ILayer::did_become_current (ISceneManager * manager, IScene * scene) {
			
			}
			
			void ILayer::will_revoke_current (ISceneManager * manager, IScene * scene) {
			
			}

// MARK: mark -

			void Group::render_frame_for_time (IScene * scene, TimeT time)
			{
				for (ChildrenT::iterator i = _children.begin(); i != _children.end(); i++)
				{
					(*i)->render_frame_for_time(scene, time);
				}
			}
			
			bool Group::process (const Input & input)
			{
				//logger()->log(LOG_DEBUG, LogBuffer() << "Processing input: " << typeid(input).name());
				
				bool result = false;
				
				for (ChildrenT::iterator i = _children.begin(); i != _children.end(); i++)
				{
					result |= (*i)->process(input);
				}
				
				result |= IInputHandler::process(input);
				
				return result;
			}
			
			void Group::did_become_current (ISceneManager * manager, IScene * scene)
			{
				for (ChildrenT::iterator i = _children.begin(); i != _children.end(); i++)
				{
					(*i)->did_become_current(manager, scene);
				}
			}
			
			void Group::will_revoke_current (ISceneManager * manager, IScene * scene)
			{
				for (ChildrenT::iterator i = _children.begin(); i != _children.end(); i++)
				{
					(*i)->will_revoke_current(manager, scene);
				}
			}
			
			void Group::add(Ptr<ILayer> child)
			{
				_children.push_back(child);
			}
			
			void Group::remove(Ptr<ILayer> child)
			{
				//_children.erase(child);
				ChildrenT::iterator pos = std::find(_children.begin(), _children.end(), child);
				
				if (pos != _children.end()) {
					_children.erase(pos);
				}
			}
			
			void Group::remove_all ()
			{
				_children.resize(0);
			}
			
// MARK: mark -
			
			Scene::Scene () : _scene_manager(NULL), _first_frame(true), _start_time(0), _current_time(0)
			{
				
			}
			
			Scene::~Scene()
			{
				
			}
			
			ISceneManager * Scene::manager ()
			{
				return _scene_manager;
			}
			
			ILoader * Scene::resource_loader ()
			{
				return _scene_manager->resource_loader().get();
			}
			
			void Scene::will_become_current(ISceneManager * scene_manager)
			{
				_scene_manager = scene_manager;
				_first_frame = true;
			}
			
			void Scene::did_become_current() {
				Group::did_become_current(_scene_manager, this);
				
				Display::ResizeInput initial_size(_scene_manager->display_context()->size());
				process(initial_size);
			}
			
			void Scene::will_revoke_current(ISceneManager * scene_manager)
			{
				_scene_manager = NULL;
			}
			
			void Scene::render_frame_for_time(TimeT time)
			{
				if (_first_frame) {
					_start_time = time;
					_first_frame = false;
				}
				
				_current_time = time;
				
				Group::render_frame_for_time(this, time);
			}
			
			TimeT Scene::current_time () const
			{
				return _current_time - _start_time;
			}
			
	// MARK: mark -
			
			VoidScene::VoidScene ()
			{
				
			}
			
			VoidScene::~VoidScene ()
			{
				
			}
			
			bool VoidScene::process (const Display::Input & input)
			{
				return true;
			}
			
			void VoidScene::render_frame_for_time (TimeT time)
			{
				
			}
			
			Ref<VoidScene> VoidScene::shared_instance ()
			{
				static Ref<VoidScene> _shared_instance;
				
				if (!_shared_instance) {
					_shared_instance = new VoidScene;
				}
				
				return _shared_instance;
			}
			
		}
	}
}
