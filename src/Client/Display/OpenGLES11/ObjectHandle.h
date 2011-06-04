/*
 *  Client/Display/OpenGLES11/ObjectHandle.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 1/05/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_OPENGLES11_OBJECTHANDLE_H
#define _DREAM_CLIENT_DISPLAY_OPENGLES11_OBJECTHANDLE_H

#include "OpenGLES11.h"

namespace Dream {
	namespace Client {
		namespace Display {
			namespace OpenGLES11 {
				
				class ObjectHandle : public Object {
				protected:
					typedef GLuint HandleT;

					HandleT m_handle;
				public:
					virtual ~ObjectHandle ();
					
					inline HandleT handle () const {
						return m_handle;
					}					
				};
				
			}
		}
	}
}
#endif
