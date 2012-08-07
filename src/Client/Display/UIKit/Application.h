//
//  Client/Display/UIKit/Application.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 14/09/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#ifndef _DREAM_CLIENT_DISPLAY_UIKIT_APPLICATION_H
#define _DREAM_CLIENT_DISPLAY_UIKIT_APPLICATION_H

#include "../Application.h"

namespace Dream {
	namespace Client {
		namespace Display {
			namespace UIKit {
				class Application : public Object, implements IApplication {
				protected:
					Ref<IApplicationDelegate> _application_delegate;

				public:
					Application(Ptr<IApplicationDelegate> delegate);
					virtual ~Application ();

					void run();

					virtual Ref<IContext> create_context (Ref<Dictionary> config);

					virtual IApplicationDelegate * delegate () const;
				};
			}
		}
	}
}

#endif
