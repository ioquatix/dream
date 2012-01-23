//
//  Client/Display/Context.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 27/03/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CLIENT_DISPLAY_CONTEXT_H
#define _DREAM_CLIENT_DISPLAY_CONTEXT_H

#include "../../Core/Dictionary.h"
#include "../../Core/Strings.h"
#include "../../Numerics/Vector.h"
#include "../../Events/Loop.h"
#include "../../Events/Input.h"

#include <vector>

namespace Dream {
	namespace Client {
		namespace Display {
			using namespace Dream::Core;
			using namespace Dream::Numerics;
			using namespace Dream::Events;
			
#pragma mark -
			
			class IContext;
			
			class IContextDelegate : implements IObject {
			public:
				virtual ~IContextDelegate ();
				
				/// Render a frame with the given context. You should lock the context before rendering as
				/// this function may be called from a separate thread.
				virtual void renderFrameForTime (PTR(IContext) context, TimeT time);
				
				/// Process the given user event. This event may typically come from main thread, so you should
				/// use InputQueue to pass events to main context event loop.
				virtual void processInput (PTR(IContext) context, const Input & input);
			};
			
			/** Simple generic method of showing a window for use with 3D graphics.
			 
			 It turns out that creating a cross-platform API is fairly difficult
			 */
			class IContext : implements IObject {				
			public:
				virtual ~IContext ();
				
				/// Start the display context.
				/// The delegate's IContextDelegate::renderFrameForTime method will begin being called periodically.
				/// If it is a window, show the window. If it is a full-screen context, take control of the screen.
				virtual void start () abstract;
				
				/// Hide the display context and return control to the system if the context was fullscreen.
				virtual void stop () abstract;
				
				/// Make the associated graphics context current for the given thread of execution.
				virtual void makeCurrent () abstract;
				
				/// Flip the buffers. Generally should be called at the end of rendering to indicate the frame is complete.
				virtual void flushBuffers () abstract;
				
				/// The resolution of the current display window or screen.
				virtual Vec2u size () abstract;
				
				/// Set the delegate that will be used to handle frame rendering.
				/// This delegate will typically be called on a separate thread.
				virtual void setDelegate(PTR(IContextDelegate) contextDelegate) abstract;
				
				/// Possibly add some mouse handling functions?
				/// void grabCursor ();
			};
			
			class Context : public Object, implements IContext, implements IInputHandler {
			protected:
				REF(IContextDelegate) m_contextDelegate;
				
			public:
				// Process some input
				virtual bool process (const Input & input);
				
				// Render a frame
				virtual void renderFrameForTime (TimeT time);
			
				virtual ~Context();
				
				virtual void setDelegate(PTR(IContextDelegate) contextDelegate);
			};
		}
	}
}

#endif
