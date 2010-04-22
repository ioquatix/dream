/*
 *  Client/Display/Application.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 27/12/08.
 *  Copyright 2008 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Application.h"

#include "../../Imaging/Image.h"
#include "../../Imaging/Text/Font.h"
#include "../Audio/Sound.h"
#include "../Graphics/MaterialLibraryParser.h"

namespace Dream
{	
	namespace Client
	{
		namespace Display
		{

#pragma mark -
#pragma mark class IApplication
			
			IMPLEMENT_INTERFACE(Application)
			
#pragma mark -
#pragma mark class ApplicationBase
			
			bool _startupMessagesDisplayed = false;
			
			void _showStartupMessagesIfNeeded ()
			{
				if (_startupMessagesDisplayed == false) {
					_startupMessagesDisplayed = true;
					
					std::cout << "Dream Framework. Copyright © 2006-2009 Samuel Williams. All Rights Reserved." << std::endl;
					std::cout << "For more information visit www.oriontransfer.co.nz" << std::endl;
					std::cout << "Compiled at: " << buildDate() << std::endl;
					std::cout << "Compiled on: " << buildPlatform() << std::endl;
				}
			}
			
			IMPLEMENT_CLASS(ApplicationBase)
			
			REF(IApplication) ApplicationBase::Class::init ()
			{
				return NULL;
			}

			ApplicationBase::ApplicationBase ()
			{
				setNativePathFormat();
				_showStartupMessagesIfNeeded();
				
				m_eventLoop = Loop::klass.init();
				setupResourceLoader();
				
				IContext::Class * contextClass = IContext::bestContextClass();
				ensure(contextClass != NULL);
				
				m_displayContext = contextClass->init();
				m_displayContext->setTitle(String("Dream Framework (") + String(buildRevision()) + ")");
				
				// Setup the display context notification
				m_displayContext->scheduleFrameNotificationCallback(m_eventLoop, boost::bind(&ApplicationBase::frameCallback, this, _1));
			}
			
			void ApplicationBase::setupResourceLoader ()
			{
				REF(Loader) resourceLoader = Resources::applicationLoader();
				
				Dream::Imaging::Image::staticClass()->registerLoaderTypes(resourceLoader);
				Dream::Client::Audio::Sound::staticClass()->registerLoaderTypes(resourceLoader);
				Dream::Imaging::Text::Font::staticClass()->registerLoaderTypes(resourceLoader);
				Dream::Client::Graphics::MaterialLibrary::staticClass()->registerLoaderTypes(resourceLoader);
				
				m_resourceLoader = resourceLoader;
			}
			
			void ApplicationBase::run ()
			{
				m_displayContext->show();
				m_eventLoop->runForever();
				m_displayContext->hide();
			}

		}
	}
}
