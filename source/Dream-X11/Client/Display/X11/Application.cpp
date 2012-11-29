//
//  Client/Display/X11/Application.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 06/11/2012.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#include "Application.h"
#include "Context.h"

namespace Dream {
	namespace Client {
		namespace Display {
		
			void IApplication::start (Ptr<IApplicationDelegate> delegate)
			{
				Ref<X11::Application> application = new X11::Application(delegate);
				
				application->run();
			}
		
			namespace X11 {
				Application::Application(Ptr<IApplicationDelegate> applicationDelegate) : _application_delegate(applicationDelegate)
				{
				
				}
				
				Application::~Application ()
				{
				
				}

				void Application::run()
				{
					_application_delegate->application_did_finish_launching(this);
				}
				
				Ref<IContext> Application::create_context(Ref<Dictionary> config)
				{
					return new WindowContext(config);
				}
				
				IApplicationDelegate * Application::delegate () const
				{
					return _application_delegate.get();
				}
			}
		}
	}
}
