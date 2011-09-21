//
//  Application.h
//  Dream
//
//  Created by Samuel Williams on 14/09/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

#ifndef _DREAM_CLIENT_DISPLAY_APPLICATION_H
#define _DREAM_CLIENT_DISPLAY_APPLICATION_H

#include "Context.h"
#include "Scene.h"

namespace Dream {
	namespace Client {
		namespace Display {
			
			/// An abstract interface to platform specific application implementation.
			// This part of the framework is optional.
			/// You can also depend on IContext directly and embed the rendering
			/// process directly into a host-specific application for enhanced
			/// functionality.
			class IApplication;
			
			class IApplicationDelegate : implements IObject {
				public:
					virtual ~IApplicationDelegate ();
					
					virtual void applicationDidFinishLaunching (IApplication * application);
					virtual void applicationWillTerminate (IApplication * application);
					
					virtual void applicationWillEnterBackground (IApplication * application);
					virtual void applicationDidEnterForeground (IApplication * application);
			};
			
			class IApplication : implements IObject {
				public:
					virtual ~IApplication ();
					
					static void start (PTR(IApplicationDelegate) delegate);
					
					/// Create a display context for rendering.
					virtual REF(IContext) createContext (REF(Dictionary) config) abstract;
					
					virtual IApplicationDelegate * delegate () const abstract;
			};
			
		}
	}
}

#endif
