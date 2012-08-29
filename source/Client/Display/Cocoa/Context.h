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

#import <condition_variable>

@class DWindowDelegate;
@class DOpenGLView;

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			namespace Cocoa {
				/// Manages a context for a specific view without touching the associated window.
				class ViewContext : public Context {
				protected:
					std::condition_variable _frame_refresh;
					std::mutex _frame_refresh_mutex;

					bool _initialized;
					unsigned _skip_frame;

					DOpenGLView * _graphics_view;

					// Display link callback
					static CVReturn display_link_callback(CVDisplayLinkRef display_link, const CVTimeStamp* now, const CVTimeStamp* output_time, CVOptionFlags flags_in, CVOptionFlags* flags_out, void* display_link_context);
					CVReturn display_link_callback(CVDisplayLinkRef display_link, const CVTimeStamp* now, const CVTimeStamp* output_time, CVOptionFlags flags_in, CVOptionFlags* flags_out);

					// Display link state for managing rendering thread
					CVDisplayLinkRef _display_link;
					void setup_for_current_display();
					void setup_display_link ();
					void setup_render_thread();

					// Need to manually set _graphics_view and call setup_display_link()
					ViewContext();

				public:
					ViewContext(DOpenGLView * graphics_view);
					virtual ~ViewContext();

					virtual void start();
					virtual void stop();

					virtual Vec2u size();

					virtual void set_cursor_mode(CursorMode mode);

					void wait_for_refresh();
					void screen_configuration_changed();
				};

				/// This context manages a window which can be used to display content.
				class WindowContext : public ViewContext {
				protected:
					NSWindow * _window;
					DWindowDelegate * _window_delegate;

					// Setup the graphics view
					void setup_graphics_view (Ptr<Dictionary> config, NSRect frame);

				public:
					WindowContext (Ptr<Dictionary> config);
					virtual ~WindowContext ();

					virtual void start ();
				};
			}
		}
	}
}

#endif
