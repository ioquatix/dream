//
//  Client/Display/X11/Context.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 06/11/2012.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//

#include "Context.h"

#import <mutex>

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			namespace X11
			{
				
				using namespace Events::Logging;
								
// MARK: -

				void WindowContext::setup_graphics_context(Ptr<Dictionary> config, Vec2u size)
				{
					int frame_buffer_attributes[] = {
						GLX_RGBA,
						GLX_RED_SIZE, 1,
						GLX_GREEN_SIZE, 1,
						GLX_BLUE_SIZE, 1,
						GLX_DOUBLEBUFFER,
						GLX_DEPTH_SIZE, 1,
						None
					};

					int screen_number = DefaultScreen(_display);
					XWindow root_window = RootWindow(_display, screen_number);

					int count = 0;
					GLXFBConfig * frame_buffer_config = glXChooseFBConfig(_display, screen_number, NULL, &count);

					if (!frame_buffer_config)
						throw ContextInitializationError("No valid frame buffer configurations found!");

					XVisualInfo * visual_info = glXChooseVisual(_display, screen_number, frame_buffer_attributes);

					if (!visual_info)
						throw ContextInitializationError("Couldn't get a visual buffer.");

					XSetWindowAttributes window_attributes;
					window_attributes.background_pixel = 0;
					window_attributes.border_pixel = 0;
					window_attributes.colormap = XCreateColormap(_display, root_window, visual_info->visual, AllocNone);
					window_attributes.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
					unsigned long mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

					_window = XCreateWindow(_display, root_window, 0, 0, size[WIDTH], size[HEIGHT], 0, visual_info->depth, InputOutput, visual_info->visual, mask, &window_attributes);

					int context_attributes[] = {
						GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
						GLX_CONTEXT_MINOR_VERSION_ARB, 2,
						GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
						None
					};

					_glx_context = glXCreateContextAttribsARB(_display, frame_buffer_config[0], nullptr, true, context_attributes);
					_glx_window = glXCreateWindow(_display, *frame_buffer_config, _window, nullptr);

					glXMakeCurrent(_display, _glx_window, _glx_context);

					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					glPixelStorei(GL_PACK_ALIGNMENT, 1);

					LogBuffer buffer;
					buffer << "OpenGL Context Initialized..." << std::endl;
					buffer << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
					buffer << "OpenGL Renderer: " << glGetString(GL_RENDERER) << " " << glGetString(GL_VERSION) << std::endl;
					buffer << "OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
					logger()->log(LOG_INFO, buffer);

					glXMakeContextCurrent(_display, None, None, nullptr);
				}

				void WindowContext::flush_buffers() {
					glXSwapBuffers(_display, _glx_window);
				}

				WindowContext::WindowContext(Ptr<Dictionary> config) : _glx_context(nullptr)
				{
					Vec2u initial_size {1024, 768};

					config->get("Context.Size", initial_size);
					config->get("Context.XDisplay", _display);

					if (!_display) {
						XOpenDisplay(nullptr);
					}

					setup_graphics_context(config, initial_size);
				}

				WindowContext::~WindowContext() {
					stop();

					if (_renderer_thread) {
						_renderer_thread->stop();
					}

					if (_glx_context) {
						glXDestroyContext(_display, _glx_context);
						glXDestroyWindow(_display, _glx_window);

						_glx_context = nullptr;
					}

					if (_display) {
						XCloseDisplay(_display);
					}
				}

				void WindowContext::render_frame() {
					glXMakeContextCurrent(_display, _glx_window, _glx_window, _glx_context);

					_context_delegate->render_frame_for_time(this, system_time());
					flush_buffers();

					glXMakeContextCurrent(_display, None, None, nullptr);
				}

				void WindowContext::start() {
					logger()->log(LOG_DEBUG, "Starting context...");

					if (!_renderer_thread) {
						_renderer_thread = new Events::Thread;
						_renderer_timer = new Events::TimerSource(std::bind(&WindowContext::render_frame, this), 1.0/60.0, true, true);
						_renderer_thread->loop()->schedule_timer(_renderer_timer);
					}
				}

				void WindowContext::stop() {
					logger()->log(LOG_DEBUG, "Stopping context...");

					_renderer_thread->stop();
				}

				Vec2u WindowContext::size() {
					return ZERO;
				}

				void WindowContext::set_cursor_mode(CursorMode mode) {

				}
			}
		}
	}
}
