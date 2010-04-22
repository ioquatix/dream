/*
 *  Client/Display/Application.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 27/12/08.
 *  Copyright 2008 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_APPLICATION_H
#define _DREAM_CLIENT_DISPLAY_APPLICATION_H

#include "Input.h"
#include "Context.h"

#include "../../Events/Loop.h"
#include "../../Resources/Loader.h"

namespace Dream
{	
	namespace Client
	{
		namespace Display
		{
			
			class IContext;
			
			using namespace Dream::Events;
			using namespace Dream::Resources;
			
			/**
			 Controller class for running an application.
			 */
			class IApplication : IMPLEMENTS(Object), IMPLEMENTS(InputHandler)
			{
				EXPOSE_INTERFACE(Application)
				
				class Class : IMPLEMENTS(Object::Class), IMPLEMENTS(InputHandler::Class)
				{
				public:
					virtual REF(IApplication) init () abstract;
				};
			
			public:
				/// Creates an instance of the supplied application class and invokes run().
				static void start (IApplication::Class * appKlass);
				
				/// You generally shouldn't call run directly.
				virtual void run () abstract;
			};
			
			class ApplicationBase : public Object, IMPLEMENTS(Application)
			{
				EXPOSE_CLASS(ApplicationBase)
				
				class Class : public Object::Class, IMPLEMENTS(Application::Class)
				{
					EXPOSE_CLASSTYPE
					
					virtual REF(IApplication) init ();
				};
				
			protected:
				REF(Events::Loop) m_eventLoop;
				REF(IContext) m_displayContext;
				REF(Resources::ILoader) m_resourceLoader;
				
				virtual void frameCallback (TimeT at) abstract;
			
			private:
				void setupResourceLoader ();
				
			public:
				ApplicationBase ();
				
			protected:
				virtual void run ();
			};		
		}
	}
}

#endif
