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

#include <mutex>
#include <iomanip>

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
				using namespace Euclid::Numerics::Constants;
				
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
					
					TimeT hostClockFrequency = CVGetHostClockFrequency();
					
					if (_skip_frame) {
						_skip_frame -= 1;
						// Eat the first frame (or subsequent frames), which may not be an entire time slice.
						return kCVReturnSuccess;
					}
					
					//logger()->log(LOG_DEBUG, LogBuffer() << "*** Rendering frame for context " << _graphics_view.openGLContext.CGLContextObj);
					
					uint64_t start_host_time, submit_host_time, end_host_time;
					TimeT time = (TimeT)(output_time->hostTime) / hostClockFrequency;
					
					@autoreleasepool {
						CGLContextObj graphicsContext = (CGLContextObj)_graphics_view.openGLContext.CGLContextObj;
						
						// Lock the context so that no other thread can be accessing it:
						CGLLockContext(graphicsContext);
						
						// Prepare the context for rendering:
						CGLSetCurrentContext(graphicsContext);
						
						start_host_time = CVGetCurrentHostTime();
						
						// Ask the client to render a frame:
						_context_delegate->render_frame_for_time(this, time);
						
						submit_host_time = CVGetCurrentHostTime();
						
						// Flush the buffer:
						CGLFlushDrawable(graphicsContext);
						
						end_host_time = CVGetCurrentHostTime();
						
						// Unlock the context:
						CGLUnlockContext((CGLContextObj)_graphics_view.openGLContext.CGLContextObj);
					}
					
					// Print out information relating to the missed frame:
					if (end_host_time > output_time->hostTime) {
						// We calculate these times in seconds:
						TimeT frame_period = (TimeT)output_time->videoRefreshPeriod / (TimeT)output_time->videoTimeScale;
						
						TimeT start_frame_offset = ((TimeT)start_host_time - output_time->hostTime) / hostClockFrequency;
						TimeT submit_frame_offset = ((TimeT)submit_host_time - output_time->hostTime) / hostClockFrequency;
						TimeT end_frame_offset = ((TimeT)end_host_time - output_time->hostTime) / hostClockFrequency;
						
						TimeT end_offset = (end_frame_offset / frame_period) * 100.0;
						TimeT submit_offset = (submit_frame_offset / frame_period) * 100.0;
						TimeT start_offset = (start_frame_offset / frame_period) * 100.0;
						
						logger()->log(LOG_DEBUG, LogBuffer() << std::setprecision(3) << "Frame missed vertical sync (start=" << start_offset << "%, submit=" << submit_offset << "%, end=" << end_offset << "%)");
						
						// Skip one frame since we have missed the VSync and are currently at least one frame ahead.
						_skip_frame = 1;
					}
					
					_frame_refresh.notify_all();
					
					return kCVReturnSuccess;
				}
				
				void ViewContext::setup_render_thread() {
					CGLContextObj context = CGLGetCurrentContext();
					
					// Enable the multi-threading
					CGLError error = CGLEnable(context, kCGLCEMPEngine);
					
					if (error != kCGLNoError) {
						logger()->log(LOG_WARN, "Multi-threading OpenGL not available!");
					}    
				}
				
				void ViewContext::start() {
					setup_display_link();
					
					_initialized = false;
					_skip_frame = 1;
					
					DREAM_ASSERT(_display_link != nil);
					DREAM_ASSERT(_graphics_view != nil);
					
					CVReturn result = CVDisplayLinkStart(_display_link);
										
					if (result != kCVReturnSuccess) {
						logger()->log(LOG_ERROR, LogBuffer() << "CVDisplayLinkStart error #" << result);
					}
					
					logger()->log(LOG_DEBUG, "Starting context...");
				}
				
				void ViewContext::stop() {										
					DREAM_ASSERT(_display_link != nil);
					
					CVReturn result = CVDisplayLinkStop(_display_link);
										
					if (result != kCVReturnSuccess) {
						logger()->log(LOG_ERROR, LogBuffer() << "CVDisplayLinkStop error #" << result);
					}
					
					logger()->log(LOG_DEBUG, "Stopping context...");
				}
				
				void ViewContext::wait_for_refresh()
				{
					std::unique_lock<std::mutex> lock(_frame_refresh_mutex);
					
					if (_display_link && CVDisplayLinkIsRunning(_display_link)) {
						// Wait for two frames:
						_frame_refresh.wait(lock);
					}
				}
				
				void ViewContext::setup_for_current_display() {
					// Set the display link for the current renderer
					CGLContextObj cglContext = (CGLContextObj)_graphics_view.openGLContext.CGLContextObj;
					CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)_graphics_view.pixelFormat.CGLPixelFormatObj;
					CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(_display_link, cglContext, cglPixelFormat);
					
					// VSync? Is this really useful with CVDisplayLink?
					GLint value = 1;
					CGLSetParameter(cglContext, kCGLCPSwapInterval, &value);
				}
				
				void ViewContext::setup_display_link ()
				{
					if (!_display_link) {
						// Synchronize buffer swaps with vertical refresh rate
						//GLint swap = 1;
						//[[_graphics_view openGLContext] setValues:&swap forParameter:NSOpenGLCPSwapInterval];
						
						// Create a display link capable of being used with all active displays
						CVDisplayLinkCreateWithActiveCGDisplays(&_display_link);
						
						// Set the renderer output callback function
						CVDisplayLinkSetOutputCallback(_display_link, &ViewContext::display_link_callback, this);
					
						setup_for_current_display();
						
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
					if (mode == cursor_mode())
						return;
					
					Context::set_cursor_mode(mode);
					
					if (mode == CURSOR_GRAB) {						
						CGDisplayHideCursor(kCGNullDirectDisplay);
						
						CGAssociateMouseAndMouseCursorPosition(false);
						
						[_graphics_view setAcceptsTouchEvents:YES];
						[_graphics_view warpCursorToCenter];
					} else {
						[_graphics_view setAcceptsTouchEvents:NO];
						
						CGAssociateMouseAndMouseCursorPosition(true);
						
						CGDisplayShowCursor(kCGNullDirectDisplay);
					}
				}
				
				void ViewContext::screen_configuration_changed() {
					setup_for_current_display();
				}
				
// MARK: -

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
							//attributes.push_back(2);
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
					
					// Enable multiple-threaded OpenGL if possible.
					// setup_render_thread();
					
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
					
					[_window setDisplaysWhenScreenProfileChanges:NO];
					[_window setRestorable:NO];
					
					setup_graphics_view(config, window_rect);
					DREAM_ASSERT(_graphics_view);
					
					[_window setContentView:_graphics_view];
					
					
					[_window orderBack:nil];
					[_window makeFirstResponder:_window];
					//[_window setInitialFirstResponder:_window.contentView];
				}
				
				WindowContext::~WindowContext ()
				{
					[_window release];
				}
			
				void WindowContext::start ()
				{					
					// It is essential that the window is shown _AFTER_ the display link is started.
					if (![_window isVisible]) {
						[_window makeFirstResponder:_window];
						[_window makeKeyAndOrderFront:nil];
					}
										
					ViewContext::start();
				}
			}
		}
	}
}
