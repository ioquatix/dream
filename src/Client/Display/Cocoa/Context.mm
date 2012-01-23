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
#import "DScreenManager.h"
#import "DOpenGLView.h"

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
						
#pragma mark -
							
				// This is the renderer output callback function
				CVReturn ViewContext::displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
				{
					ViewContext * context = (ViewContext*)displayLinkContext;
					
					return context->displayLinkCallback(displayLink, now, outputTime, flagsIn, flagsOut);
				}
				
				// This is the renderer output callback function
				CVReturn ViewContext::displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut)
				{
					TimeT time = (TimeT)(outputTime->hostTime) / (TimeT)CVGetHostClockFrequency();
					
					m_contextDelegate->renderFrameForTime(this, time);

					return kCVReturnSuccess;
				}
				
				void ViewContext::start() {
					setupDisplayLink();
					
					ensure(m_displayLink != nil);
					ensure(m_graphicsView != nil);
					
					CVDisplayLinkStart(m_displayLink);
				}
				
				void ViewContext::stop() {
					ensure(m_displayLink != nil);
					
					CVDisplayLinkStop(m_displayLink);
				}
				
				void ViewContext::setupDisplayLink ()
				{
					if (m_displayLink) return;
					
					// Synchronize buffer swaps with vertical refresh rate
					GLint swap = 1;
					[[m_graphicsView openGLContext] setValues:&swap forParameter:NSOpenGLCPSwapInterval];
					
					// Create a display link capable of being used with all active displays
					CVDisplayLinkCreateWithActiveCGDisplays(&m_displayLink);
					
					// Set the renderer output callback function
					CVDisplayLinkSetOutputCallback(m_displayLink, &ViewContext::displayLinkCallback, this);
					
					// Set the display link for the current renderer
					CGLContextObj cglContext = (CGLContextObj)[[m_graphicsView openGLContext] CGLContextObj];
					CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[m_graphicsView pixelFormat] CGLPixelFormatObj];
					CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(m_displayLink, cglContext, cglPixelFormat);
				}
				
				ViewContext::ViewContext ()
					: m_graphicsView(nil), m_displayLink(nil)
				{
				}
				
				ViewContext::ViewContext(NSOpenGLView * graphicsView)
					: m_graphicsView(graphicsView), m_displayLink(nil)
				{
				}
				
				ViewContext::~ViewContext () {
					if (m_displayLink) {
						CVDisplayLinkStop(m_displayLink);
						CVDisplayLinkRelease(m_displayLink);
						m_displayLink = nil;
					}
					
					//[m_pool release];				
				}
				
				Vec2u ViewContext::size ()
				{
					if (m_graphicsView) {
						NSSize size = [m_graphicsView frame].size;
					
						return Vec2u(size.width, size.height);
					} else {
						return Vec2u(0, 0);
					}
				}
				
				void ViewContext::makeCurrent ()
				{
					[[m_graphicsView openGLContext] makeCurrentContext];
				}
				
				void ViewContext::flushBuffers ()
				{
					// We assume that if m_graphicsView is nil, this function will have no effect.
					[[m_graphicsView openGLContext] flushBuffer];
				}
				
#pragma mark -

				void WindowContext::setupGraphicsView (PTR(Dictionary) config, NSRect frame)
				{					
					NSOpenGLView * graphicsView = NULL;
					if (config->get("Cocoa.View", graphicsView)) {
						m_graphicsView = graphicsView;
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

						NSOpenGLPixelFormat * pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes.data()];
						
						DOpenGLView * graphicsView = [[DOpenGLView alloc] initWithFrame:frame pixelFormat:pixelFormat];
						[graphicsView setDisplayContext:this];
												
						m_graphicsView = graphicsView;						
					}

					// Tight alignment
					[[m_graphicsView openGLContext] makeCurrentContext];
					
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					glPixelStorei(GL_PACK_ALIGNMENT, 1);
					
					std::cerr << "OpenGL Context Initialized..." << std::endl;
					std::cerr << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
					std::cerr << "OpenGL Renderer: " << glGetString(GL_RENDERER) << " " << glGetString(GL_VERSION) << std::endl;
					std::cerr << "OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
					
					[NSOpenGLContext clearCurrentContext];
				}

				WindowContext::WindowContext (PTR(Dictionary) config)
				{
					NSRect windowRect = NSMakeRect(50, 50, 1024, 768);
					
					Vec2u initialSize;
					if (config->get("Context.Size", initialSize)) {
						windowRect.size.width = initialSize[WIDTH];
						windowRect.size.height = initialSize[HEIGHT];
					}
					
					unsigned windowStyle = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;

					m_window = [[NSWindow alloc] initWithContentRect:windowRect styleMask:windowStyle backing:NSBackingStoreBuffered defer:YES];
					[m_window setAcceptsMouseMovedEvents:YES];
					
					m_windowDelegate = [DWindowDelegate new];
					[m_windowDelegate setInputHandler:this];
					
					[[m_windowDelegate screenManager] setPartialScreenWindow:m_window];
					
					[m_window setDelegate:m_windowDelegate];
					
					setupGraphicsView(config, windowRect);
					ensure(m_graphicsView);
					
					[m_window setContentView:m_graphicsView];
					[m_window makeFirstResponder:m_graphicsView];
				}
				
				WindowContext::~WindowContext ()
				{
					[m_window release];
				}
			
				void WindowContext::start ()
				{
					[m_window makeKeyAndOrderFront:nil];
					ViewContext::start();
				}
				
				void WindowContext::stop ()
				{
					ViewContext::stop();
					[m_window close];
				}

			}
		}
	}
}
