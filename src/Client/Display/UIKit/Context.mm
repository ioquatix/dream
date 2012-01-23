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
					: _graphics_view(nil)
				{
				
				}
				
				ViewContext::ViewContext (DOpenGLView * graphicsView)
					: _graphics_view(graphicsView)
				{
					[_graphics_view retain];
					[_graphics_view setDisplayContext:this];
				}
				
				ViewContext::~ViewContext ()
				{
					if (_graphics_view) {
						[_graphics_view release];
						_graphics_view = nil;
					}
				}
				
				void ViewContext::start ()
				{
					[_graphics_view start];
				}
				
				void ViewContext::stop ()
				{
					[_graphics_view stop];
				}
				
				Vec2u ViewContext::size ()
				{
					CGRect frame = [_graphics_view frame];
					
					return Vec2u(frame.size.width, frame.size.height);
				}
				
				void ViewContext::make_current ()
				{
					[_graphics_view makeCurrentContext];
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
				
				void ViewContext::flush_buffers ()
				{
					[_graphics_view flushBuffers];
				
					GLenum error;
					
					while ((error = glGetError()) != GL_NO_ERROR)
						std::cerr << "OpenGL Error " << "#" << error << ": " << getSymbolicError(error) << std::endl;
				}
			
#pragma mark -
			
				void WindowContext::setup_graphics_view (Ptr<Dictionary> config, CGRect frame)
				{
					if (config->get("Cocoa.View", _graphics_view)) {
						// Graphics view from configuration.
						[_graphics_view retain];
					} else {
						_graphics_view = [[DOpenGLView alloc] initWithFrame:frame];
					}

					// Create the OpenGLES context:
					EAGLContext *graphicsContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
					
					ensure(graphicsContext != NULL);
					
					[_graphics_view setContext:graphicsContext];
					[graphicsContext release];

					this->make_current();
					
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					glPixelStorei(GL_PACK_ALIGNMENT, 1);
					
					std::cerr << "OpenGL Context Initialized..." << std::endl;
					std::cerr << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
					std::cerr << "OpenGL Renderer: " << glGetString(GL_RENDERER) << " " << glGetString(GL_VERSION) << std::endl;
					std::cerr << "OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
					
					// Clear current context.
					[EAGLContext setCurrentContext:nil];
				}
				
				WindowContext::WindowContext (Ptr<Dictionary> config)
				{
					_window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
					
					CGRect frame = [[UIScreen mainScreen] applicationFrame];
					setup_graphics_view(config, frame);
					
					if (_graphics_view) {
						[_graphics_view setDisplayContext:this];
						[_window addSubview:_graphics_view];
					} else {
						std::cerr << "Couldn't initialize graphics view!" << std::endl;
					}
				}
				
				WindowContext::~WindowContext ()
				{
					if (_window) {
						[_window release];
						_window = nil;
					}
				}
			
				void WindowContext::start ()
				{
					std::cerr << "Starting graphics context: " << _graphics_view << " window: " << _window << std::endl;
					[_window makeKeyAndVisible];
					ViewContext::start();
				}
				
				void WindowContext::stop ()
				{
					ViewContext::stop();
					[_window resignKeyWindow];
				}
				
			}
		}
	}
}
