//
//  Client/Display/UIKit/Context.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 20/04/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#include "Context.h"
#import "DOpenGLView.h"

#include "Renderer.h"

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			namespace UIKit
			{
			
				ViewContext::ViewContext ()
					: m_graphicsView(nil)
				{
				
				}
				
				ViewContext::ViewContext (DOpenGLView * graphicsView)
					: m_graphicsView(graphicsView)
				{
					[m_graphicsView retain];
					[m_graphicsView setDisplayContext:this];
				}
				
				ViewContext::~ViewContext ()
				{
					if (m_graphicsView) {
						[m_graphicsView release];
						m_graphicsView = nil;
					}
				}
				
				void ViewContext::start ()
				{
					[m_graphicsView start];
				}
				
				void ViewContext::stop ()
				{
					[m_graphicsView stop];
				}
				
				Vec2u ViewContext::size ()
				{
					CGRect frame = [m_graphicsView frame];
					
					return Vec2u(frame.size.width, frame.size.height);
				}
				
				void ViewContext::makeCurrent ()
				{
					[m_graphicsView makeCurrentContext];
				}
				
				const char * getSymbolicError (GLenum error) {
					switch (error) {
						case GL_NO_ERROR:
							return "No error has occurred.";
						case GL_INVALID_ENUM:
							return "An invalid value has been specified (GL_INVALID_ENUM)!";
						case GL_INVALID_VALUE:
							return "A numeric argument is out of range (GL_INVALID_VALUE)!";
						case GL_INVALID_OPERATION:
							return "The specified operation is not allowed (GL_INVALID_OPERATION)!";
						//case GL_STACK_OVERFLOW:
						//	return "The specified command would cause a stack overflow (GL_STACK_OVERFLOW)!";
						//case GL_STACK_UNDERFLOW:
						//	return "The specified command would cause a stack underflow (GL_STACK_UNDERFLOW)!";
						case GL_OUT_OF_MEMORY:
							return "There is not enough free memory left to run the command (GL_OUT_OF_MEMORY)!";
						default:
							return "An unknown error has occurred!";
					}
				}
				
				void ViewContext::flushBuffers ()
				{
					[m_graphicsView flushBuffers];
				
					GLenum error;
					
					while ((error = glGetError()) != GL_NO_ERROR)
						std::cerr << "OpenGL Error " << "#" << error << ": " << getSymbolicError(error) << std::endl;
				}
			
#pragma mark -
			
				void WindowContext::setupGraphicsView (PTR(Dictionary) config, CGRect frame)
				{
					if (config->get("Cocoa.View", m_graphicsView)) {
						// Graphics view from configuration.
						[m_graphicsView retain];
					} else {
						m_graphicsView = [[DOpenGLView alloc] initWithFrame:frame];
					}

					// Create the OpenGLES context:
					EAGLContext *graphicsContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
					
					ensure(graphicsContext != NULL);
					
					[m_graphicsView setContext:graphicsContext];
					[graphicsContext release];

					this->makeCurrent();
					
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					glPixelStorei(GL_PACK_ALIGNMENT, 1);
					
					std::cerr << "OpenGL Context Initialized..." << std::endl;
					std::cerr << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
					std::cerr << "OpenGL Renderer: " << glGetString(GL_RENDERER) << " " << glGetString(GL_VERSION) << std::endl;
					std::cerr << "OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
					
					// Clear current context.
					[EAGLContext setCurrentContext:nil];
				}
				
				WindowContext::WindowContext (PTR(Dictionary) config)
				{
					m_window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
					
					CGRect frame = [[UIScreen mainScreen] applicationFrame];
					setupGraphicsView(config, frame);
					
					if (m_graphicsView) {
						[m_graphicsView setDisplayContext:this];
						[m_window addSubview:m_graphicsView];
					} else {
						std::cerr << "Couldn't initialize graphics view!" << std::endl;
					}
				}
				
				WindowContext::~WindowContext ()
				{
					if (m_window) {
						[m_window release];
						m_window = nil;
					}
				}
			
				void WindowContext::start ()
				{
					std::cerr << "Starting graphics context: " << m_graphicsView << " window: " << m_window << std::endl;
					[m_window makeKeyAndVisible];
					ViewContext::start();
				}
				
				void WindowContext::stop ()
				{
					ViewContext::stop();
					[m_window resignKeyWindow];
				}
				
			}
		}
	}
}
