/*
 *  Client/Display/Context.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 27/03/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "Context.h"
#include "../../Resources/Loader.h"

namespace Dream {
	namespace Client {
		namespace Display {
			
			IMPLEMENT_INTERFACE(Context)
			
			unsigned IContext::Class::priority () const
			{
				return m_priority;
			}
			
			//List of available window classes
			std::vector<IContext::Class*> IContext::s_registeredWindowClasses;
			
			void IContext::registerContextClass (IContext::Class *rwc) {
				s_registeredWindowClasses.push_back(rwc);
			}
			
			IContext::Class::Class (int priority) : m_priority(priority) {
				IContext::registerContextClass(this);
			}
			
			IContext::Class* IContext::bestContextClass () {
				unsigned priority = 0;
				unsigned best = 0;
				
				for (unsigned i = 0; i < s_registeredWindowClasses.size(); i++) {
					if (s_registeredWindowClasses[i]->priority() > priority) {
						best = i;
						priority = s_registeredWindowClasses[i]->priority();
					}
				}
				
				if (s_registeredWindowClasses.size() > 0) {
					return s_registeredWindowClasses[best];
				} else {
					return NULL;
				}
			}
			
		}
	}
}