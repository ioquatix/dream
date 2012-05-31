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
			
// MARK: mark -
// MARK: mark Startup Message

			void _show_startup_message_if_needed()
			{
				static bool _startup_messages_displayed = false;
			
				if (_startup_messages_displayed == false) {
					_startup_messages_displayed = true;

					std::cout << "Dream Framework. Copyright © 2006-2011 Samuel Williams. All Rights Reserved." << std::endl;
					std::cout << "For more information visit http://www.oriontransfer.co.nz/research/dream" << std::endl;
					std::cout << "Build Revision: " << build_revision() << std::endl;
					std::cout << "Compiled at: " << build_date() << std::endl;
					std::cout << "Compiled on: " << build_platform() << std::endl;
				}
			}
			
// MARK: mark -
			
			IApplication::IApplication()
			{
				logger()->set_thread_name("Application");
				
				_show_startup_message_if_needed();
			}
			
			IApplicationDelegate::~IApplicationDelegate ()
			{
			}
					
			void IApplicationDelegate::application_did_finish_launching (IApplication * application)
			{
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
			
			
// MARK: mark -

			class ApplicationDelegate : public Object, implements IApplicationDelegate {
			public:
				ApplicationDelegate(Ptr<IScene> scene, Ptr<Dictionary> config);
				virtual ~ApplicationDelegate();
				
			protected:
				Ref<IScene> _scene;
				Ref<Dictionary> _config;
				Ref<Context> _context;
				Ref<Events::Thread> _thread;
				Ref<SceneManager> _scene_manager;
				
				virtual void application_did_finish_launching (IApplication * application);
				
				virtual void application_will_enter_background (IApplication * application);
				virtual void application_did_enter_foreground (IApplication * application);
			};

			ApplicationDelegate::ApplicationDelegate(Ptr<IScene> scene, Ptr<Dictionary> config)
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
				Ref<ILoader> loader = SceneManager::default_resource_loader();
				
				_scene_manager = new SceneManager(_context, _thread->loop(), loader);
				_scene_manager->push_scene(_scene);
				
				_thread->start();
				_context->start();
			}
			
			void ApplicationDelegate::application_will_enter_background (IApplication * application)
			{
				logger()->log(LOG_INFO, "Application entering background...");
				
				EventInput suspend_event(EventInput::PAUSE);
				_scene_manager->process_input(_context, suspend_event);
				
				//_context->stop();
				//_thread->stop();
			}
			
			void ApplicationDelegate::application_did_enter_foreground (IApplication * application)
			{
				logger()->log(LOG_INFO, "Application entering foreground...");
				
				//_thread->start();
				//_context->start();
				
				EventInput resume_event(EventInput::RESUME);
				_scene_manager->process_input(_context, resume_event);
			}
			
			void IApplication::run_scene(Ptr<IScene> scene, Ptr<Dictionary> config)
			{
				Ref<ApplicationDelegate> application_delegate = new ApplicationDelegate(scene, config);
				
				IApplication::start(application_delegate);
			}
			
		}
	}
}
