/*
 *  Client/Display/UIKitContext/Context.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 20/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_UIKITCONTEXT_CONTEXT_H
#define _DREAM_CLIENT_DISPLAY_UIKITCONTEXT_CONTEXT_H

// This is a private header, and should not be used as public API.

#include "../Context.h"
#include "../Renderer.h"
#include "../Application.h"
#include "../FrameTimerSource.h"

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			namespace UIKitContext
			{
				// This functions assist with UIKit event handling
				IInputHandler * globalInputHandler ();
				
				// These functions assist with the DreamAppDelegate starting the application
				void setApplicationInstance (REF(IApplication) application);
				void runApplicationCallback ();
				
				class Context : public Object, IMPLEMENTS(Context)
				{
					EXPOSE_CLASS(Context)
					
					class Class : public Object::Class, IMPLEMENTS(Context::Class) {
						EXPOSE_CLASSTYPE
						
						Class ();
						
						virtual REF(IContext) init (PTR(Dictionary) config);
					};
					
				protected:
					struct ContextImpl;
					ContextImpl * m_impl;
					
					REF(Dictionary) m_config;
					REF(FrameTimerSource) m_timerSource;
					
				public:
					Context (PTR(Dictionary) config);
					virtual ~Context ();
					
					virtual void setTitle (String title);
					
					virtual void setFrameSync (bool vsync);
					
					virtual void show ();
					virtual void hide ();
					
					virtual ResolutionT resolution ();
					
					virtual void flipBuffers ();
					
					virtual void processPendingEvents (IInputHandler * handler);
					
					virtual void scheduleFrameNotificationCallback (REF(Events::Loop) loop, FrameCallbackT callback);
					
#ifdef DREAM_USE_OPENGLES11
					virtual REF(OpenGLES11::Renderer) renderer ();
#endif
				};
			}
		}
	}
}

#endif
