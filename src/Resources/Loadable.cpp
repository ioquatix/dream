/*
 *  Resources/Loadable.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 4/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "Loadable.h"

namespace Dream {
	namespace Resources {
		
		LoadError::LoadError (String what) : m_what(what) {
			
		}
		
		LoadError::~LoadError () throw () {
			
		}
		
		const char * LoadError::what () throw () {
			return m_what.c_str();
		}

		REF(Object) ILoadableFromStream::Class::initFromData(const REF(Data) data, const ILoader * loader) {
			StaticBuffer buffer(data->start(), data->size());

			BufferStream ds(buffer);
			
			return initFromStream (ds, loader);
		}
		
#pragma mark -
		
		IMPLEMENT_INTERFACE(Loadable)
		IMPLEMENT_INTERFACE(LoadableFromStream)
		
	}
}