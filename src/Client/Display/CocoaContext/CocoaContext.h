/*
 *  Client/Display/CocoaContext/CocoaContext.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 27/03/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_COCOACONTEXT_COCOACONTEXT_H
#define _DREAM_CLIENT_DISPLAY_COCOACONTEXT_COCOACONTEXT_H

// This is a private header, and should not be used as public API.

#include "Context.h"
#include "FrameNotificationSource.h"

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			
			class CocoaContext : public Object, implements IContext {
			protected:
				//bool m_allowResize;
				//String m_windowTitle;
				
				//Vector<2, uint32_t> currentSize;
								
				struct CocoaContextImpl;
				CocoaContextImpl *m_impl;
				
			public:				
				virtual void setTitle (String title);
				
				virtual void setFrameSync (bool vsync);
				
				virtual void show ();
				virtual void hide ();
				
				virtual ResolutionT resolution ();
				
				virtual void flipBuffers ();
				
				CocoaContext (PTR(Dictionary) config);
				virtual ~CocoaContext ();
				
				virtual void processPendingEvents (IInputHandler * handler);
				
				virtual void scheduleFrameNotificationCallback (REF(Events::Loop) loop, FrameCallbackT callback);

#ifdef DREAM_USE_OPENGL20
				virtual REF(OpenGL20::Renderer) renderer ();
#endif
			};
		}
	}
}

#endif