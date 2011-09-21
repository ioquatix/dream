//
//  Application.cpp
//  Dream
//
//  Created by Samuel Williams on 14/09/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

#include "Application.h"

namespace Dream {
	namespace Client {
		namespace Display {
			
#pragma mark -
#pragma mark Startup Message

			void _showStartupMessagesIfNeeded ()
			{
				static bool _startupMessagesDisplayed = false;
			
				if (_startupMessagesDisplayed == false) {
					_startupMessagesDisplayed = true;

					std::cout << "Dream Framework. Copyright © 2006-2011 Samuel Williams. All Rights Reserved." << std::endl;
					std::cout << "For more information visit http://www.oriontransfer.co.nz/research/dream" << std::endl;
					std::cout << "Build Revision: " << buildRevision() << std::endl;
					std::cout << "Compiled at: " << buildDate() << std::endl;
					std::cout << "Compiled on: " << buildPlatform() << std::endl;
				}
			}
			
#pragma mark -
			
			IApplicationDelegate::~IApplicationDelegate ()
			{
			}
					
			void IApplicationDelegate::applicationDidFinishLaunching (IApplication * application)
			{
				_showStartupMessagesIfNeeded();
			}
			
			void IApplicationDelegate::applicationWillTerminate (IApplication * application)
			{
			}
			
			void IApplicationDelegate::applicationWillEnterBackground (IApplication * application)
			{
			
			}
			
			void IApplicationDelegate::applicationDidEnterForeground (IApplication * application)
			{
			
			}
						
			IApplication::~IApplication ()
			{
			}
															
		}
	}
}
