/*
 *  Dream/Client/Display/Basic.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 14/05/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Basic.h"

#include "Context.h"
#include "../../Imaging/Image.h"
#include "../../Client/Text/Font.h"
#include "../Audio/Sound.h"
#include "../Graphics/MaterialLibraryParser.h"
#include "../Audio/OggResource.h"

namespace Dream {
	namespace Client {
		namespace Display {

#pragma mark -
#pragma mark Startup Message

			bool _startupMessagesDisplayed = false;
			
			void _showStartupMessagesIfNeeded ()
			{
				if (_startupMessagesDisplayed == false) {
					_startupMessagesDisplayed = true;
					
					std::cout << "Dream Framework. Copyright © 2006-2010 Samuel Williams. All Rights Reserved." << std::endl;
					std::cout << "For more information visit http://www.oriontransfer.co.nz/research/dream" << std::endl;
					std::cout << "Compiled at: " << buildDate() << std::endl;
					std::cout << "Compiled on: " << buildPlatform() << std::endl;
				}
			}

#pragma mark -
#pragma mark class BasicApplication

			BasicApplication::BasicApplication (PTR(Dictionary) config)
			{
				m_eventLoop = new Events::Loop;
				setupResourceLoader();

				REF(IContextMode) mode = ContextManager::sharedManager()->bestContextMode();
				
				ensure(mode);
				
				m_displayContext = mode->setup(config);
				m_displayContext->setTitle(String("Dream Framework (") + String(buildRevision()) + ")");

				// Setup the display context notification
				m_displayContext->scheduleFrameNotificationCallback(m_eventLoop, boost::bind(&BasicApplication::frameCallback, this, _1));

				m_sceneManager = new SceneManager(m_displayContext, m_eventLoop, m_resourceLoader);
				m_sceneManager->setFinishedCallback(boost::bind(&BasicApplication::finishedCallback, this));
			}
			
			void BasicApplication::setup () {
			
			}
			
			void BasicApplication::runScene (PTR(IScene) scene, PTR(Dictionary) config) {
				REF(BasicApplication) application;
				
				application = new BasicApplication(config);
				
				application->push(scene);
				
				application->run();
			}
			
			void BasicApplication::setupResourceLoader ()
			{
				REF(Resources::Loader) resourceLoader = Resources::applicationLoader();
				
				resourceLoader->addLoader(new Imaging::Image::Loader);
				resourceLoader->addLoader(new Client::Audio::Sound::Loader);
				resourceLoader->addLoader(new Client::Audio::OggResource::Loader);
				resourceLoader->addLoader(new Client::Text::Font::Loader);
				resourceLoader->addLoader(new Client::Graphics::MaterialLibrary::Loader);

				m_resourceLoader = resourceLoader;
			}
			
			void BasicApplication::finishedCallback ()
			{
				m_eventLoop->stop();
			}

			void BasicApplication::frameCallback (TimeT at)
			{
				m_stats.beginTimer(m_eventLoop->stopwatch().time());
	
				m_sceneManager->processPendingEvents();
				m_sceneManager->renderFrameForTime(at);
				
				m_stats.update(m_eventLoop->stopwatch().time());
				
				m_displayContext->flipBuffers();
				
				if (m_stats.updateCount() > (60 * 20))
				{
					std::cerr << "FPS: " << m_stats.updatesPerSecond() << std::endl;
					m_stats.reset();
				}
			}

			void BasicApplication::run ()
			{
				m_displayContext->show();
				
				m_eventLoop->setStopWhenIdle(false);
				m_eventLoop->runForever();
				
				m_displayContext->hide();
			}
			
			void BasicApplication::append (PTR(IScene) scene)
			{
				m_sceneManager->appendScene(scene);
			}
			
			void BasicApplication::push (PTR(IScene) scene)
			{
				m_sceneManager->pushScene(scene);
			}
			
			void BasicApplication::replace(PTR(IScene) scene)
			{
				m_sceneManager->replaceScene(scene);
			}

#pragma mark -
#pragma mark class BasicScene
			
			
			
			BasicScene::BasicScene ()
			{
			
			}
			
			BasicScene::~BasicScene ()
			{
			
			}

			bool BasicScene::process (const Input & input)
			{
				bool result = processInputForLayers(input);
				return Scene::process(input) || result;
			}
			
			void BasicScene::didBecomeCurrent () {				
				for (unsigned i = 0; i < m_layers.size(); i += 1)
					m_layers[i]->didBecomeCurrent(m_sceneManager, this);

				Scene::didBecomeCurrent();
			}
			
			void BasicScene::willRevokeCurrent (ISceneManager * sceneManager)
			{
				for (unsigned i = 0; i < m_layers.size(); i += 1)
					m_layers[i]->willRevokeCurrent(sceneManager, this);
				
				m_layers.clear();
				
				Scene::willRevokeCurrent(sceneManager);
			}
			
			void BasicScene::renderLayersForTime (TimeT time)
			{
				for (unsigned i = 0; i < m_layers.size(); i += 1)
					m_layers[i]->renderFrameForTime(this, time);
			}
			
			bool BasicScene::processInputForLayers (const Input & input)
			{
				bool result = false;
				
				for (unsigned i = 0; i < m_layers.size(); i += 1)
					result = m_layers[i]->process(input) | result;
				
				return result;
			}
			
			void BasicScene::renderFrameForTime (TimeT time)
			{
				Scene::renderFrameForTime(time);
				
				renderLayersForTime(m_currentTime);
			}

			void BasicScene::add (PTR(ILayer) layer)
			{
				m_layers.push_back(layer);
			}
			
			void BasicScene::removeAll ()
			{
				m_layers.resize(0);
			}
		}
	}
}