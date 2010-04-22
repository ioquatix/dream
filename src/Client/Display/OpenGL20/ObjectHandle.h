/*
 *  Client/Display/OpenGL20/ObjectHandle.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 1/05/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_OPENGL20_OBJECTHANDLE_H
#define _DREAM_CLIENT_DISPLAY_OPENGL20_OBJECTHANDLE_H

#include "OpenGL20.h"

namespace Dream {
	namespace Client {
		namespace Display {
			namespace OpenGL20 {
				
				class ObjectHandle : public Object {
					EXPOSE_CLASS(ObjectHandle)
					
					class Class : public Object::Class {
					public:
						EXPOSE_CLASSTYPE
					};
					
				protected:
#ifdef GL_VERSION_2_0
					typedef GLuint HandleT;
#else
					typedef GLhandleARB HandleT;
#endif
					
					HandleT m_handle;
				public:
					virtual ~ObjectHandle ();
					
					inline HandleT oid () const {
						return get();
					}
					
					HandleT get () const {
						return m_handle;
					}
				};
				
			}
		}
	}
}
#endif
