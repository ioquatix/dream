//
//  Client/Display/X11/Context.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 06/11/2012.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//

#ifndef _DREAM_CLIENT_DISPLAY_X11_CONTEXT_H
#define _DREAM_CLIENT_DISPLAY_X11_CONTEXT_H

// This is a private header, and should not be used as public API.

#include "../Context.h"

#define GLX_GLXEXT_PROTOTYPES

extern "C" {
	#include <X11/Xlib.h>
	#include <GL/glx.h>
	#include <GL/glxext.h>
}

#include <condition_variable>

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			namespace X11
			{
				typedef ::Display XDisplay;
				typedef ::Window XWindow;

				/// Manages a context for a specific view without touching the associated window.
				class WindowContext : public Context {
				protected:
					XDisplay * _display;
					XWindow _window;

					GLXContext _glx_context;
					GLXWindow _glx_window;

					Ref<Events::Thread> _renderer_thread;
					Ref<Events::TimerSource> _renderer_timer;
					void render_frame();

					void setup_graphics_context(Ptr<Dictionary> config, Vec2u size);
					void flush_buffers();

					bool _done;
					void display_event_loop();
					void cancel_display_event_loop();

				public:
					WindowContext(Ptr<Dictionary> config);
					virtual ~WindowContext();

					virtual void start();
					virtual void stop();

					virtual Vec2u size();

					virtual void set_cursor_mode(CursorMode mode);
				};
			}
		}
	}
}

#endif
