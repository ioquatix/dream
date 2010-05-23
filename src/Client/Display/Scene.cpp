/*
 *  Client/Display/Scene.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 26/06/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#include "Scene.h"
#include "Layer.h"

#include "../../Geometry/AlignedBox.h"

#include <numeric>
#include <algorithm>

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			
	#pragma mark -
			
			IMPLEMENT_INTERFACE(Scene)
			IMPLEMENT_INTERFACE(SceneManager)
			
			void ISceneManager::renderFrameForTime (TimeT time)
			{
				REF(IScene) s = currentScene();
				
				if (s) 
					s->renderFrameForTime(time);
			}
			
	#pragma mark -
			
			IMPLEMENT_CLASS(SceneManager)
			
			SceneManager::SceneManager (REF(IContext) displayContext, REF(Loop) eventLoop, REF(ILoader) resourceLoader) : m_displayContext(displayContext),
			m_eventLoop(eventLoop), m_currentSceneIsFinished(true), m_resourceLoader(resourceLoader)
			{
				
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
			
			void SceneManager::renderFrameForTime (TimeT time)
			{
				if (!m_currentScene || m_currentSceneIsFinished)
					updateCurrentScene();
				
				ISceneManager::renderFrameForTime(time);
			}
			
			void SceneManager::processPendingEvents ()
			{
				m_displayContext->processPendingEvents(currentScene().get());
			}
			
			void SceneManager::setFinishedCallback (FinishedCallbackT callback)
			{
				m_finishedCallback = callback;
			}

			
	#pragma mark -
			
			IMPLEMENT_CLASS(Scene)
			
			Scene::Scene () : m_sceneManager(NULL), m_firstFrame(true), m_startTime(0), m_currentTime(0)
			{
				
			}
			
			Scene::~Scene()
			{
				
			}
			
			RendererT * Scene::renderer ()
			{
				ensure(m_sceneManager);
				
				return m_sceneManager->displayContext()->renderer().get();
			}
			
			ISceneManager * Scene::manager ()
			{
				return m_sceneManager;
			}
			
			ILoader * Scene::loader ()
			{
				return m_sceneManager->resourceLoader().get();
			}
			
			void Scene::willBecomeCurrent (ISceneManager * sceneManager)
			{
				std::cerr << "Scene will become current: " << className() << std::endl;
				
				m_sceneManager = sceneManager;
				m_firstFrame = true;
			}
			
			void Scene::didBecomeCurrent () {
				Display::ResizeInput initialSize(Vec2(ZERO), m_sceneManager->displayContext()->resolution());
				process(initialSize);
			}
			
			void Scene::willRevokeCurrent (ISceneManager * sceneManager)
			{
				std::cerr << "Scene will revoke current: " << className() << std::endl;
				
				m_sceneManager = NULL;
			}
			
			bool Scene::resize (const Display::ResizeInput &ipt)
			{
				using namespace Geometry;
				
				renderer()->setViewport(Vec2(ZERO), ipt.newSize());
				renderer()->setOrthographicView(AlignedBox<2>::fromCenterAndSize(Vec2(ZERO), ipt.newSize()), 0, 1024);
				
				return true;
			}
			
			void Scene::renderFrameForTime (TimeT time)
			{
				if (m_firstFrame) {
					m_startTime = time;
					m_firstFrame = false;
				}
				
				m_currentTime = time;
			}
			
			TimeT Scene::currentTime () const
			{
				return m_currentTime - m_startTime;
			}
			
	#pragma mark -
			
			IMPLEMENT_CLASS(VoidScene)
			
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
