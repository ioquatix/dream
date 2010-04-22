/*
 *  Client/Display/Context.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 27/03/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_CONTEXT_H
#define _DREAM_CLIENT_DISPLAY_CONTEXT_H

#include "../../Core/Strings.h"
#include "../../Numerics/Vector.h"
#include "../../Events/Loop.h"

#include "Renderer.h"
#include "Input.h"

#include <boost/function.hpp>
#include <vector>

namespace Dream {
	namespace Client {
		namespace Display {
			using namespace Dream::Numerics;
			
			typedef Vector<2, uint32_t> ResolutionT;
			
			typedef boost::function<void (TimeT at)> FrameCallbackT;
			
			/** Simple generic method of showing a window for use with 3D graphics.
			 
			 It turns out that creating a cross-platform API is fairly difficult
			 */
			class IContext : IMPLEMENTS(Object)
			{		
				EXPOSE_INTERFACE(Context)
				
				class Class : IMPLEMENTS(Object::Class) {
				protected:
					unsigned m_priority;			
					Class (int priority);
					
				public:
					unsigned priority () const;
					
					virtual REF(IContext) init () abstract;					
				};
				
			protected:
				static void registerContextClass (IContext::Class *rwc);
				static std::vector<IContext::Class*> s_registeredWindowClasses;
			
			public:				
				/// Return the best context class for the given operating system and library compilation.
				static IContext::Class* bestContextClass ();
				
				/// Synchronize the frame updates to screen updates. This is on by default, and should generally not be adjusted.
				virtual void setFrameSync (bool vsync) abstract;
				
				/// Set the title of the window, if it is possible.
				virtual void setTitle (String title) abstract;
				
				/// Show the display context.
				/// If it is a window, show the window. If it is a full-screen context, take control of the screen.
				virtual void show () abstract;
				
				/// Hide the display context and return control to the system if the context was fullscreen.
				virtual void hide () abstract;
				
				/// Flip the buffers. Generally should be called at the end of rendering to indicate the frame is complete.
				virtual void flipBuffers () abstract;
				
				/// The resolution of the current display window or screen.
				virtual ResolutionT resolution () abstract;
				
				/// This function aggregates any events that have occurred and calls the appropriate methods on the provided handler.
				virtual void processPendingEvents (IInputHandler * handler) abstract;
				
				/// Set the callback that will occur when a frame is to be displayed.
				/// This callback will include the time that the frame will be rendered, and may be called from another thread.
				virtual void scheduleFrameNotificationCallback (REF(Events::Loop) loop, FrameCallbackT callback) abstract;
				
				/// The renderer for this display context.
				virtual REF(DefaultRendererT) renderer () abstract;
			};
			
		}
	}
}

#endif