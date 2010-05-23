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

namespace Dream
{	
	namespace Client
	{
		namespace Display
		{
			
			class IContext;
			
			/**
			 Controller class for running an application.
			 
			 This class is an incredibly simple interface for building complete apps with only a few lines of code. It 
			 is not designed to be flexible, and you shouldn't implement much code inside your IApplication. Typically,
			 you'd do most of this in your Scenes, so that it can be easily reused.
				
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
				/// Creates an instance of the supplied application class and 
				static void start (IApplication::Class * appKlass);
								
				/// Start the event loop and begin processing events
				virtual void run () abstract;
			};
		}
	}
}

#endif
