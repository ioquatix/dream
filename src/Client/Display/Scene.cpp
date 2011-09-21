/*
 *  Client/Display/Scene.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 26/06/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#include "Scene.h"
#include "Context.h"

#include "../../Geometry/AlignedBox.h"

// Resource loader
#include "../../Imaging/Image.h"
#include "../Text/Font.h"
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
			void ISceneManager::renderFrameForTime (TimeT time)
			{
				REF(IScene) s = currentScene();
				
				if (s) 
					s->renderFrameForTime(time);
			}
			
	#pragma mark -
			
			REF(Resources::ILoader) SceneManager::defaultResourceLoader ()
			{
				REF(Resources::Loader) loader = new Resources::Loader;
				
				loader->addLoader(new Imaging::Image::Loader);
				loader->addLoader(new Client::Audio::Sound::Loader);
				loader->addLoader(new Client::Audio::OggResource::Loader);
				loader->addLoader(new Client::Text::Font::Loader);
				
				return loader;
			}
			
			SceneManager::SceneManager (REF(IContext) displayContext, REF(Loop) eventLoop, REF(ILoader) resourceLoader)
				: m_displayContext(displayContext), m_eventLoop(eventLoop), m_currentSceneIsFinished(true), m_resourceLoader(resourceLoader)
			{
				m_displayContext->setDelegate(this);
				
				m_stopwatch.start();
			}
			
			SceneManager::~SceneManager ()
			{
				
			}
			
			void SceneManager::pushScene (REF(IScene) scene)
			{
				// Save the current scene on top of the queue.
				if (m_currentScene)
					m_scenes.push_front(m_currentScene);
				
				
				replaceScene(scene);
			}
			
			void SceneManager::replaceScene (REF(IScene) scene)
			{
				m_scenes.push_front(scene);
				
				// Clear the current scene. Any attempt to render will load up the first scene.
				m_currentScene = NULL;
			}
			
			void SceneManager::appendScene (REF(IScene) scene)
			{
				m_scenes.push_back(scene);
			}
			
			REF(IScene) SceneManager::currentScene ()
			{
				if (m_currentScene)
					return m_currentScene;
				else
					return VoidScene::sharedInstance();
			}
			
			REF(IContext) SceneManager::displayContext ()
			{
				return m_displayContext;
			}
			
			REF(Loop) SceneManager::eventLoop ()
			{
				return m_eventLoop;
			}
			
			REF(ILoader) SceneManager::resourceLoader ()
			{
				return m_resourceLoader;
			}
			
			void SceneManager::currentSceneIsFinished ()
			{
				m_currentSceneIsFinished = true;
			}
			
			void SceneManager::updateCurrentScene ()
			{
				m_currentSceneIsFinished = false;
				
				REF(IScene) s = provideNextScene();
				
				if (m_currentScene) {
					m_currentScene->willRevokeCurrent(this);
				}
				
				if (s) {
					m_currentScene = s;
					m_currentScene->willBecomeCurrent(this);
					m_currentScene->didBecomeCurrent();
				} else {
					m_finishedCallback(this);
					m_currentScene = NULL;
				}
			}
			
			REF(IScene) SceneManager::provideNextScene ()
			{
				REF(IScene) s;
				
				if (!m_scenes.empty()) {
					s = m_scenes.front();
					m_scenes.pop_front();
				}
				
				return s;
			}
			
			void SceneManager::renderFrameForTime (PTR(IContext) context, TimeT time)
			{
				context->makeCurrent();
				
				m_stats.beginTimer(m_stopwatch.time());

				if (!m_currentScene || m_currentSceneIsFinished)
					updateCurrentScene();
				
				ISceneManager::renderFrameForTime(time);
								
				m_stats.update(m_stopwatch.time());
				
				if (m_stats.updateCount() > (60 * 20))
				{
					std::cerr << "FPS: " << m_stats.updatesPerSecond() << std::endl;
					m_stats.reset();
				}
				
				context->flushBuffers();
			}
			
			void SceneManager::processInput (PTR(IContext) context, const Input & input)
			{
				// Add the event to the thread-safe queue.
				m_inputQueue.process(input);
			}
			
			void SceneManager::processPendingEvents (IInputHandler * handler)
			{
				// Remove a block of events from the input queue and pass to the handler for processing.
				m_inputQueue.dequeue(handler);
			}
			
			void SceneManager::setFinishedCallback (FinishedCallbackT callback)
			{
				m_finishedCallback = callback;
			}
			
#pragma mark -
			
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
				
				result |= IInputHandler::process(input);
				
				return result;
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
				ChildrenT::iterator pos = std::find(m_children.begin(), m_children.end(), child);
				
				if (pos != m_children.end()) {
					m_children.erase(pos);
				}
			}
			
			void Group::removeAll ()
			{
				m_children.resize(0);
			}
			
#pragma mark -
			
			Scene::Scene () : m_sceneManager(NULL), m_firstFrame(true), m_startTime(0), m_currentTime(0)
			{
				
			}
			
			Scene::~Scene()
			{
				
			}
			
			ISceneManager * Scene::manager ()
			{
				return m_sceneManager;
			}
			
			ILoader * Scene::resourceLoader ()
			{
				return m_sceneManager->resourceLoader().get();
			}
			
			void Scene::willBecomeCurrent (ISceneManager * sceneManager)
			{
				m_sceneManager = sceneManager;
				m_firstFrame = true;
			}
			
			void Scene::didBecomeCurrent () {
				Display::ResizeInput initialSize(m_sceneManager->displayContext()->size());
				process(initialSize);
				
				Group::didBecomeCurrent(m_sceneManager, this);
			}
			
			void Scene::willRevokeCurrent (ISceneManager * sceneManager)
			{
				m_sceneManager = NULL;
			}
			
			bool Scene::resize (const Display::ResizeInput &ipt)
			{				
				std::cout << "Resizing to " << ipt.newSize() << std::endl;
				
				return true;
			}
			
			bool Scene::event (const Display::EventInput & ipt)
			{
				if (ipt.event() == EventInput::EXIT && m_sceneManager)
				{
					m_sceneManager->eventLoop()->stop();
					return true;
				}
				
				return false;
			}
			
			void Scene::renderFrameForTime (TimeT time)
			{
				if (m_firstFrame) {
					m_startTime = time;
					m_firstFrame = false;
				}
				
				m_currentTime = time;
				
				Group::renderFrameForTime(this, time);
			}
			
			TimeT Scene::currentTime () const
			{
				return m_currentTime - m_startTime;
			}
			
	#pragma mark -
			
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
			
			void VoidScene::renderFrameForTime (TimeT time)
			{
				
			}
			
			REF(VoidScene) VoidScene::sharedInstance ()
			{
				static REF(VoidScene) s_voidSceneSharedInstance;
				
				if (!s_voidSceneSharedInstance)
					s_voidSceneSharedInstance = new VoidScene;
				
				return s_voidSceneSharedInstance;
			}
			
		}
	}
}
