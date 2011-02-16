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
			static std::vector<IContext::Class*> * s_registeredWindowClasses = NULL;
							
			void IContext::registerContextClass (IContext::Class *rwc) {
				if (s_registeredWindowClasses == NULL) {
					s_registeredWindowClasses = new std::vector<IContext::Class*>;
				}
				
				s_registeredWindowClasses->push_back(rwc);
			}
			
			IContext::Class::Class (int priority) : m_priority(priority) {				
				IContext::registerContextClass(this);
			}
			
			IContext::Class* IContext::bestContextClass () {
				Class * bestContextClass = NULL;
				
				for (unsigned i = 0; i < s_registeredWindowClasses->size(); i++) {
					Class * contextClass = s_registeredWindowClasses->at(i);
					
					if (bestContextClass == NULL || contextClass->priority() > bestContextClass->priority()) {
						bestContextClass = contextClass;
					}
				}
				
				return bestContextClass;
			}
			
		}
	}
}