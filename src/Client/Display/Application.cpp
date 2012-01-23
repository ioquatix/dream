//
//  Client/Display/Application.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 14/09/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#include "Application.h"
#include "../../Events/Thread.h"

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
					std::cout << "Build Revision: " << build_revision() << std::endl;
					std::cout << "Compiled at: " << build_date() << std::endl;
					std::cout << "Compiled on: " << build_platform() << std::endl;
				}
			}
			
#pragma mark -
			
			IApplicationDelegate::~IApplicationDelegate ()
			{
			}
					
			void IApplicationDelegate::application_did_finish_launching (IApplication * application)
			{
				_showStartupMessagesIfNeeded();
			}
			
			void IApplicationDelegate::application_will_terminate (IApplication * application)
			{
			}
			
			void IApplicationDelegate::application_will_enter_background (IApplication * application)
			{
			
			}
			
			void IApplicationDelegate::application_did_enter_foreground (IApplication * application)
			{
			
			}
						
			IApplication::~IApplication ()
			{
			}
			
			
#pragma mark -

			class ApplicationDelegate : public Object, implements IApplicationDelegate
			{
				public:
					ApplicationDelegate(PTR(IScene) scene, PTR(Dictionary) config);
					virtual ~ApplicationDelegate();
					
				protected:
					REF(IScene) _scene;
					REF(Dictionary) _config;
					REF(Context) _context;
					REF(Events::Thread) _thread;
					REF(SceneManager) _sceneManager;
					
					virtual void application_did_finish_launching (IApplication * application);
					
					virtual void application_will_enter_background (IApplication * application);
					virtual void application_did_enter_foreground (IApplication * application);
			};

			ApplicationDelegate::ApplicationDelegate(PTR(IScene) scene, PTR(Dictionary) config)
				: _scene(scene), _config(config)
			{
			
			}
			
			ApplicationDelegate::~ApplicationDelegate()
			{
				_context->stop();
				_thread->stop();
			}
			
			void ApplicationDelegate::application_did_finish_launching (IApplication * application)
			{
				_context = application->create_context(_config);
				
				_thread = new Events::Thread;
				REF(ILoader) loader = SceneManager::default_resource_loader();
				
				_sceneManager = new SceneManager(_context, _thread->loop(), loader);
				_sceneManager->push_scene(_scene);
				
				_thread->start();
				_context->start();
			}
			
			void ApplicationDelegate::application_will_enter_background (IApplication * application)
			{
				std::cerr << "Application entering background..." << std::endl;
				
				EventInput suspend_event(EventInput::PAUSE);
				_sceneManager->process_input(_context, suspend_event);
				
				//_context->stop();
				//_thread->stop();
			}
			
			void ApplicationDelegate::application_did_enter_foreground (IApplication * application)
			{
				std::cerr << "Application entering foreground..." << std::endl;
				
				//_thread->start();
				//_context->start();
				
				EventInput resume_event(EventInput::RESUME);
				_sceneManager->process_input(_context, resume_event);
			}
			
			void IApplication::run_scene(PTR(IScene) scene, PTR(Dictionary) config)
			{
				REF(ApplicationDelegate) application_delegate = new ApplicationDelegate(scene, config);
				
				IApplication::start(application_delegate);
			}
			
		}
	}
}
