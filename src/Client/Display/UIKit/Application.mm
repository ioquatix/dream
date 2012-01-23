//
//  Client/Display/UIKit/Application.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 14/09/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#include "Application.h"
#include "Context.h"
#import "DApplicationDelegate.h"
#import <UIKit/UIKit.h>

namespace Dream {
	namespace Client {
		namespace Display {
		
			void IApplication::start (PTR(IApplicationDelegate) delegate)
			{
				REF(UIKit::Application) application = new UIKit::Application(delegate);
				
				application->run();
			}
		
			namespace UIKit {				
				Application::Application(PTR(IApplicationDelegate) applicationDelegate)
					: m_applicationDelegate(applicationDelegate)
				{
				
				}
				
				Application::~Application ()
				{
				
				}
				
				void Application::run()
				{
					[DApplicationDelegate startWithApplication:this];
				}
				
				REF(IContext) Application::createContext (REF(Dictionary) config)
				{
					return new WindowContext(config);
				}
				
				IApplicationDelegate * Application::delegate () const
				{
					return m_applicationDelegate.get();
				}
			}
		}
	}
}
