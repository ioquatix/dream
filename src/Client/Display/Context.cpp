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
			
#pragma mark -
#pragma mark IContextManager

			IContextMode::~IContextMode() {
				
			}
			
			ContextManager::ContextManager() {
			
			}
			
			ContextManager::~ContextManager() {
			
			}
			
			ContextManager * ContextManager::sharedManager() {
				static ContextManager * contextManager = NULL;
				
				if (!contextManager) {
					contextManager = new ContextManager;
				}
				
				return contextManager;
			}
			
			void ContextManager::registerContextMode (PTR(IContextMode) mode) {
				m_modes.push_back(mode);
			}
			
			/// Return the best context class for the given operating system and library compilation.
			REF(IContextMode) ContextManager::bestContextMode() const {
				if (m_modes.size() > 0) {
					return m_modes.front();
				} else {
					return NULL;
				}
			}
			
		}
	}
}