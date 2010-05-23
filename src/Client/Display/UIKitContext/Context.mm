/*
 *  Context.mm
 *  Dream
 *
 *  Created by Samuel Williams on 20/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Context.h"
#include "DreamView.h"
#include "DreamAppDelegate.h"

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			void IApplication::start (IApplication::Class * appKlass)
			{
				UIKitContext::setApplicationInstance(appKlass->init());
				
				[DreamAppDelegate start];
			}
			
			namespace UIKitContext
			{
				static IInputHandler * g_inputHandler;
				
				IInputHandler * globalInputHandler ()
				{
					return g_inputHandler;
				}
				
				static REF(IApplication) g_applicationInstance;
				
				void setApplicationInstance (REF(IApplication) application)
				{
					g_applicationInstance = application;
				}
				
				void runApplicationCallback ()
				{
					g_applicationInstance->run();
				}
				
#pragma mark -
#pragma mark Renderer Subclass
				
				typedef OpenGLES11::Renderer TouchRenderer;
				
#pragma mark -
				
				IMPLEMENT_CLASS(Context)
				
				struct Context::ContextImpl {
					ContextImpl () {}
					
					REF(TouchRenderer) renderer;
					
					UIWindow * window;
					EAGLView * view;
										
					FrameCallbackT frameCallback;
				};
				
				Context::Class::Class() : IContext::Class(30)
				{
					std::cout << "Registering Context..." << std::endl;
				}
				
				REF(IContext) Context::Class::init ()
				{
					return new Context();
				}
				
				void Context::setTitle (String title) {
					
				}
				
				void Context::show () {
					if (!m_impl) {
						m_impl = new ContextImpl;
						
						// Create window
						m_impl->window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
						
						// Set up content view
						m_impl->view = [[[DreamView alloc] initWithFrame:[[UIScreen mainScreen] applicationFrame]] autorelease];
						[m_impl->window addSubview:m_impl->view];
						
						m_impl->renderer = new TouchRenderer();
						
						std::cerr << "OpenGL Context Initialized..." << std::endl;
						std::cerr << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
						std::cerr << "OpenGL Renderer: " << glGetString(GL_RENDERER) << " " << glGetString(GL_VERSION) << std::endl;
						
						glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
						glPixelStorei(GL_PACK_ALIGNMENT, 1);
						
						//[m_impl->view beginDrawing];
					}
				
					[m_impl->window makeKeyAndVisible];
				}
				
				void Context::hide () {
					// N/A
				}
				
				void Context::setFrameSync (bool vsync) {
					//GLint swapInt = vsync ? 1 : 0;
					//[[[m_impl->window contentView] openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
				}
				
				Context::Context () : m_impl(NULL) {
				
				}
				
				Context::~Context () {
					[m_impl->window release];
					
					delete m_impl;
				}
				
				ResolutionT Context::resolution ()
				{
					CGSize size = [m_impl->view frame].size;
					
					return ResolutionT(size.width, size.height);
				}
				
				void Context::flipBuffers ()
				{
					GLenum error;
					
					while ((error = glGetError()) != GL_NO_ERROR)
						std::cerr << "OpenGL Error " << "#" << error << ": " << glGetString(error) << std::endl;
					
					[m_impl->view flipBuffers];
				}
				
				void Context::processPendingEvents (IInputHandler * handler)
				{
					g_inputHandler = handler;
					
					SInt32 result;
					
					do {
						result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, TRUE);
					} while (result == kCFRunLoopRunHandledSource);
					
					g_inputHandler = NULL;
				}
				
				void Context::scheduleFrameNotificationCallback (REF(Events::Loop) loop, FrameCallbackT callback)
				{
					if (!callback && m_timerSource) {
						m_timerSource->cancel();
						m_timerSource = NULL;
					}
					
					if (callback) {
						m_timerSource = new FrameTimerSource(callback, 1.0/60.0);
						loop->scheduleTimer(m_timerSource);
					}
				}
				
				REF(TouchRenderer) Context::renderer ()
				{
					return m_impl->renderer;
				}
				
			}
		}
	}
}
