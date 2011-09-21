/*
 *  Client/Display/Cocoa/Context.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 27/03/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_COCOA_CONTEXT_H
#define _DREAM_CLIENT_DISPLAY_COCOA_CONTEXT_H

// This is a private header, and should not be used as public API.

#include "../Context.h"

#import <Cocoa/Cocoa.h>

#import <CoreVideo/CoreVideo.h>
#import <CoreVideo/CVDisplayLink.h>

@class DWindowDelegate;

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			namespace Cocoa {
			
				/// Manages a context for a specific view without touching the associated window.
				class ViewContext : public Context
				{
					protected:
						NSOpenGLView * m_graphicsView;
						
						// Display link callback
						static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext);
						CVReturn displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut);
						
						// Display link state for managing rendering thread
						CVDisplayLinkRef m_displayLink;
						void setupDisplayLink ();
						
						// Need to manually set m_graphicsView and call setupDisplayLink()
						ViewContext ();
						
					public:
						ViewContext (NSOpenGLView * graphicsView);
						virtual ~ViewContext ();
						
						virtual void start ();
						virtual void stop ();
						
						virtual Vec2u size ();
						
						virtual void makeCurrent ();
						virtual void flushBuffers ();
				};
				
				/// This context manages a window which can be used to display content.
				class WindowContext : public ViewContext {
					protected:
						NSWindow * m_window;
						DWindowDelegate * m_windowDelegate;
					
						// Setup the graphics view
						void setupGraphicsView (PTR(Dictionary) config, NSRect frame);
											
					public:
						WindowContext (PTR(Dictionary) config);
						virtual ~WindowContext ();
					
						virtual void start ();
						virtual void stop ();
				};
			}
		}
	}
}

#endif
