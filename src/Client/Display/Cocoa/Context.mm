//
//  Client/Display/Cocoa/Context.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 27/03/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "Context.h"

#import "DWindowDelegate.h"
#import "DOpenGLView.h"

#import <mutex>

@interface NSAppleMenuController : NSObject
- (void)controlMenu:(NSMenu *)aMenu;
@end

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			namespace Cocoa
			{
				
				using namespace Events::Logging;
								
#pragma mark -
							
				// This is the renderer output callback function
				CVReturn ViewContext::display_link_callback(CVDisplayLinkRef display_link, const CVTimeStamp* now, const CVTimeStamp* output_time, CVOptionFlags flags_in, CVOptionFlags* flags_out, void* display_link_context)
				{					
					ViewContext * context = (ViewContext*)display_link_context;
					
					return context->display_link_callback(display_link, now, output_time, flags_in, flags_out);
				}
				
				// This is the renderer output callback function
				CVReturn ViewContext::display_link_callback(CVDisplayLinkRef display_link, const CVTimeStamp* now, const CVTimeStamp* output_time, CVOptionFlags flags_in, CVOptionFlags* flags_out)
				{					
					if (!_initialized) {
						logger()->set_thread_name("Renderer");
						
						_initialized = true;
					}
					
					//logger()->log(LOG_DEBUG, LogBuffer() << "*** Rendering frame for context " << _graphics_view.openGLContext.CGLContextObj);
					
					TimeT time = (TimeT)(output_time->hostTime) / (TimeT)CVGetHostClockFrequency();
					
					@autoreleasepool {
						// Lock the context so that no other thread can be accessing it:
						CGLLockContext((CGLContextObj)_graphics_view.openGLContext.CGLContextObj);

						// Prepare the context for rendering:
						[_graphics_view.openGLContext makeCurrentContext];
						
						// Ask the client to render a frame:
						_context_delegate->render_frame_for_time(this, time);
						
						// Flush the buffer:
						[[_graphics_view openGLContext] flushBuffer];
						
						// Unlock the context:
						CGLUnlockContext((CGLContextObj)_graphics_view.openGLContext.CGLContextObj);
					}
					
					//logger()->log(LOG_DEBUG, "... Finished rendering frame.");
					
					_frame_refresh.notify_all();
					
					return kCVReturnSuccess;
				}
				
				void ViewContext::start() {
					logger()->log(LOG_DEBUG, "Enter ViewContext::start");
					
					setup_display_link();
					
					_initialized = false;
					
					ensure(_display_link != nil);
					ensure(_graphics_view != nil);
					
					CVReturn result = CVDisplayLinkStart(_display_link);
										
					if (result != kCVReturnSuccess) {
						logger()->log(LOG_ERROR, LogBuffer() << "CVDisplayLinkStart error #" << result);
					}
					
					logger()->log(LOG_DEBUG, "Exit ViewContext::start");
				}
				
				void ViewContext::stop() {
					logger()->log(LOG_DEBUG, "Enter ViewContext::stop");
										
					ensure(_display_link != nil);
					
					CVReturn result = CVDisplayLinkStop(_display_link);
										
					if (result != kCVReturnSuccess) {
						logger()->log(LOG_ERROR, LogBuffer() << "CVDisplayLinkStop error #" << result);
					}
					
					logger()->log(LOG_DEBUG, "Exit ViewContext::stop");
				}
				
				void ViewContext::wait_for_refresh()
				{
					std::unique_lock<std::mutex> lock(_frame_refresh_mutex);
					
					if (_display_link && CVDisplayLinkIsRunning(_display_link)) {
						// Wait for two frames:
						_frame_refresh.wait(lock);
					}
				}
				
				void ViewContext::setup_display_link ()
				{
					if (!_display_link) {
						// Synchronize buffer swaps with vertical refresh rate
						GLint swap = 1;
						[[_graphics_view openGLContext] setValues:&swap forParameter:NSOpenGLCPSwapInterval];
						
						// Create a display link capable of being used with all active displays
						CVDisplayLinkCreateWithActiveCGDisplays(&_display_link);
						
						// Set the renderer output callback function
						CVDisplayLinkSetOutputCallback(_display_link, &ViewContext::display_link_callback, this);
					
						// Set the display link for the current renderer
						CGLContextObj cglContext = (CGLContextObj)_graphics_view.openGLContext.CGLContextObj;
						CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)_graphics_view.pixelFormat.CGLPixelFormatObj;
						CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(_display_link, cglContext, cglPixelFormat);
						
						logger()->log(LOG_DEBUG, LogBuffer() << "Creating display link " << _display_link);
					}
				}
				
				ViewContext::ViewContext() : _graphics_view(NULL), _display_link(NULL)
				{
				}
				
				ViewContext::ViewContext(DOpenGLView * graphicsView) : _graphics_view(graphicsView), _display_link(NULL)
				{
				}
				
				ViewContext::~ViewContext () {
					if (_display_link) {
						CVDisplayLinkStop(_display_link);
						CVDisplayLinkRelease(_display_link);
						_display_link = nil;
					}
					
					//[m_pool release];				
				}
				
				Vec2u ViewContext::size ()
				{
					if (_graphics_view) {
						NSSize size = [_graphics_view frame].size;
					
						return Vec2u(size.width, size.height);
					} else {
						return Vec2u(0, 0);
					}
				}
				
				void ViewContext::set_cursor_mode(CursorMode mode) {
					Context::set_cursor_mode(mode);
					
					if (mode == CURSOR_GRAB) {
						CGDisplayHideCursor(kCGNullDirectDisplay);
						CGAssociateMouseAndMouseCursorPosition(false);
						
						// Warp the mouse cursor to the center of the view.
						NSRect bounds = _graphics_view.bounds;
						NSPoint view_center = NSMakePoint(bounds.origin.x + bounds.size.width / 2.0, bounds.origin.y + bounds.size.height / 2.0);
						NSPoint window_center = [_graphics_view convertPoint:view_center toView:nil];
						NSPoint screen_offset = [[_graphics_view window] convertBaseToScreen:window_center];
						
						NSScreen * screen = [[_graphics_view window] screen];
						CGDirectDisplayID display = [[screen.deviceDescription objectForKey:@"NSScreenNumber"] unsignedIntValue];
						
						CGDisplayMoveCursorToPoint(display, CGPointMake(screen_offset.x, screen.frame.size.height - screen_offset.y));
					} else {
						CGAssociateMouseAndMouseCursorPosition(true);
						CGDisplayShowCursor(kCGNullDirectDisplay);
					}
				}
				
#pragma mark -

				void WindowContext::setup_graphics_view(Ptr<Dictionary> config, NSRect frame)
				{					
					DOpenGLView * graphics_view = NULL;
					if (config->get("Cocoa.View", graphics_view)) {
						_graphics_view = graphics_view;
					} else {
						std::vector<NSOpenGLPixelFormatAttribute> attributes;
						
						if (config->get("Cocoa.OpenGLAttributes", attributes)) {
							// Attributes provided by configuration.
						} else {
							// OpenGL 3.2 (Core Profile)
							attributes.push_back(NSOpenGLPFAOpenGLProfile);
							attributes.push_back(NSOpenGLProfileVersion3_2Core);
						
							//attributes.push_back(NSOpenGLPFAWindow);
							attributes.push_back(NSOpenGLPFADoubleBuffer);
							attributes.push_back(NSOpenGLPFAAccelerated);
							//attributes.push_back(NSOpenGLPFANoRecovery);
							
							// Anti-aliasing
							//attributes.push_back(NSOpenGLPFASampleBuffers);
							//attributes.push_back(3);
							
							//attributes.push_back(NSOpenGLPFASamples);
							//attributes.push_back(4);
							
							// Buffer size
							attributes.push_back(NSOpenGLPFAColorSize);
							attributes.push_back(24);
							
							attributes.push_back(NSOpenGLPFADepthSize);
							attributes.push_back(24);
							
							attributes.push_back(NSOpenGLPFAAlphaSize);
							attributes.push_back(8);
							
							attributes.push_back(0);
						}

						NSOpenGLPixelFormat * pixel_format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes.data()];
						
						DOpenGLView * graphics_view = [[DOpenGLView alloc] initWithFrame:frame pixelFormat:pixel_format];
						
						[graphics_view setDisplayContext:this];
												
						_graphics_view = graphics_view;						
					}

					// Tight alignment
					CGLLockContext((CGLContextObj)_graphics_view.openGLContext.CGLContextObj);
					[[_graphics_view openGLContext] makeCurrentContext];
					
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					glPixelStorei(GL_PACK_ALIGNMENT, 1);
					
					LogBuffer buffer;
					buffer << "OpenGL Context Initialized..." << std::endl;
					buffer << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
					buffer << "OpenGL Renderer: " << glGetString(GL_RENDERER) << " " << glGetString(GL_VERSION) << std::endl;
					buffer << "OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
					logger()->log(LOG_INFO, buffer);
					
					CGLUnlockContext((CGLContextObj)_graphics_view.openGLContext.CGLContextObj);
					
					[NSOpenGLContext clearCurrentContext];
				}

				WindowContext::WindowContext(Ptr<Dictionary> config)
				{
					NSRect window_rect = NSMakeRect(50, 50, 1024, 768);
					
					Vec2u initial_size;
					if (config->get("Context.Size", initial_size)) {
						window_rect.size.width = initial_size[WIDTH];
						window_rect.size.height = initial_size[HEIGHT];
					}
					
					unsigned window_style = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;

					_window = [[NSWindow alloc] initWithContentRect:window_rect styleMask:window_style backing:NSBackingStoreBuffered defer:NO];
					[_window setAcceptsMouseMovedEvents:YES];
					[_window setReleasedWhenClosed:NO];
					
					// Enable Lion full-screen support
					[_window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
					
					_window_delegate = [DWindowDelegate new];
					[_window_delegate setDisplayContext:this];
										
					[_window setDelegate:_window_delegate];
					
					setup_graphics_view(config, window_rect);
					ensure(_graphics_view);
					
					[_window setContentView:_graphics_view];
					[_window setInitialFirstResponder:_graphics_view];
				}
				
				WindowContext::~WindowContext ()
				{
					[_window release];
				}
			
				void WindowContext::start ()
				{					
					// It is essential that the window is shown _AFTER_ the display link is started.
					if (![_window isVisible]) {
						[_window makeKeyAndOrderFront:nil];
					}
					
					ViewContext::start();
				}
			}
		}
	}
}
