//
//  Client/Display/UIKit/Context.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 20/04/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CLIENT_DISPLAY_UIKIT_CONTEXT_H
#define _DREAM_CLIENT_DISPLAY_UIKIT_CONTEXT_H

// This is a private header, and should not be used as public API.

#include "../Context.h"
#include "../Application.h"
#import <UIKit/UIKit.h>

@class DWindowDelegate;
@class EAGLView;
@class DOpenGLView;

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			namespace UIKit
			{
			
				/// Manages a context for a specific view without touching the associated window.
				class ViewContext : public Context
				{
					protected:
						DOpenGLView * _graphics_view;
						
						ViewContext ();
						
					public:
						ViewContext (DOpenGLView * graphics_view);
						virtual ~ViewContext ();
						
						virtual void start ();
						virtual void stop ();
						
						virtual Vec2u size ();
						
						virtual void make_current ();
						virtual void flush_buffers ();
				};
				
				/// This context manages a window which can be used to display content.
				class WindowContext : public ViewContext, implements IInputHandler {
					protected:
						UIWindow * _window;
						
						// Setup the graphics view
						void setup_graphics_view (PTR(Dictionary) config, CGRect frame);
											
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
