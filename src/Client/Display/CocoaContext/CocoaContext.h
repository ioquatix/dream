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
#include "Renderer.h"
#include "FrameNotificationSource.h"

#import <Cocoa/Cocoa.h>

#import <CoreVideo/CoreVideo.h>
#import <CoreVideo/CVDisplayLink.h>

#import "CocoaContextDelegate.h"
#import "CocoaScreenManager.h"

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			
			class CocoaContext : public Object, implements IContext {
			protected:

#ifdef DREAM_USE_OPENGL20
				typedef OpenGL20::Renderer MacOSXOpenGLRenderer;
#endif
				REF(MacOSXOpenGLRenderer) m_renderer;
				
				NSWindow * m_window;
				NSOpenGLView * m_view;
				NSAutoreleasePool * m_pool;
				CocoaContextDelegate * m_delegate;
				
				// Sending frame notifications to main thread
				REF(FrameNotificationSource) m_notificationSource;
				REF(Events::Loop) m_loop;
				
				// Display link callback
				static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext);
				CVReturn displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut);
				
				// Display link state for managing rendering thread
				CVDisplayLinkRef m_displayLink;
				bool m_displayWillRefresh;
				unsigned m_displayRefreshStallCount;
				
				// Setup the context
				NSString * applicationName();
				void createApplicationMenus();
				void transformToForegroundApplication();
				
				// Event handling
				unsigned buttonFromEvent(NSEvent * theEvent);
				bool handleMouseEvent(NSEvent * theEvent, unsigned button, IInputHandler * handler);
				
				// For global availability of display modes.
				class Modes {
				public:
					Modes ();
				};
				
				static Modes s_modes;
				
			public:
				static std::vector<REF(IContextMode)> availableModes ();
				
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

				virtual REF(MacOSXOpenGLRenderer) renderer ();
			};
		}
	}
}

#endif