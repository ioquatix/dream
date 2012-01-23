//
//  Client/Display/Cocoa/Context.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 27/03/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

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
						NSOpenGLView * _graphics_view;
						
						// Display link callback
						static CVReturn display_link_callback(CVDisplayLinkRef display_link, const CVTimeStamp* now, const CVTimeStamp* output_time, CVOptionFlags flags_in, CVOptionFlags* flags_out, void* display_link_context);
						CVReturn display_link_callback(CVDisplayLinkRef display_link, const CVTimeStamp* now, const CVTimeStamp* output_time, CVOptionFlags flags_in, CVOptionFlags* flags_out);
						
						// Display link state for managing rendering thread
						CVDisplayLinkRef _display_link;
						void setup_display_link ();
						
						// Need to manually set _graphics_view and call setup_display_link()
						ViewContext ();
						
					public:
						ViewContext (NSOpenGLView * graphics_view);
						virtual ~ViewContext ();
						
						virtual void start ();
						virtual void stop ();
						
						virtual Vec2u size ();
						
						virtual void make_current ();
						virtual void flush_buffers ();
				};
				
				/// This context manages a window which can be used to display content.
				class WindowContext : public ViewContext {
					protected:
						NSWindow * _window;
						DWindowDelegate * _window_delegate;
					
						// Setup the graphics view
						void setup_graphics_view (PTR(Dictionary) config, NSRect frame);
											
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
