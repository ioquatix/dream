/*
 *  Dream/Client/Display/Basic.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 14/05/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Basic.h"

#include "../../Imaging/Image.h"
#include "../../Client/Text/Font.h"
#include "../Audio/Sound.h"
#include "../Graphics/MaterialLibraryParser.h"

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

			IMPLEMENT_CLASS(BasicApplication)

			REF(IApplication) BasicApplication::Class::init (PTR(Dictionary) config)
			{
				return new BasicApplication(config);
			}

			BasicApplication::BasicApplication (PTR(Dictionary) config)
			{
				m_eventLoop = Loop::klass.init();
				setupResourceLoader();

				IContext::Class * contextClass = IContext::bestContextClass();
				ensure(contextClass != NULL);

				m_displayContext = contextClass->init(config);
				m_displayContext->setTitle(String("Dream Framework (") + String(buildRevision()) + ")");

				// Setup the display context notification
				m_displayContext->scheduleFrameNotificationCallback(m_eventLoop, boost::bind(&BasicApplication::frameCallback, this, _1));

				m_sceneManager = new SceneManager(m_displayContext, m_eventLoop, m_resourceLoader);
				m_sceneManager->setFinishedCallback(boost::bind(&BasicApplication::finishedCallback, this));
			}
			
			void BasicApplication::setupResourceLoader ()
			{
				REF(Resources::Loader) resourceLoader = Resources::applicationLoader();
				
				Dream::Imaging::Image::staticClass()->registerLoaderTypes(resourceLoader);
				Dream::Client::Audio::Sound::staticClass()->registerLoaderTypes(resourceLoader);
				Dream::Client::Text::Font::staticClass()->registerLoaderTypes(resourceLoader);
				Dream::Client::Graphics::MaterialLibrary::staticClass()->registerLoaderTypes(resourceLoader);
				
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
				m_displayContext->setFrameSync(false);
				
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
			
			IMPLEMENT_CLASS(BasicScene)
			
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
				for(unsigned i = 0; i < m_layers.size(); i += 1)
					m_layers[i]->didBecomeCurrent(m_sceneManager, this);

				Scene::didBecomeCurrent();
			}
			
			void BasicScene::willRevokeCurrent (ISceneManager * sceneManager)
			{
				for(unsigned i = 0; i < m_layers.size(); i += 1)
					m_layers[i]->willRevokeCurrent(sceneManager, this);
					
				Scene::willRevokeCurrent(sceneManager);
			}
			
			void BasicScene::renderLayersForTime (TimeT time)
			{
				for(unsigned i = 0; i < m_layers.size(); i += 1)
					m_layers[i]->renderFrameForTime(this, time);
			}
			
			bool BasicScene::processInputForLayers (const Input & input)
			{
				bool result = false;
				
				for(unsigned i = 0; i < m_layers.size(); i += 1)
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
		}
	}
}