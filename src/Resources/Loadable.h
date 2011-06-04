/*
 *  Resources/Loadable.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 4/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_RESOURCES_LOADABLE_H
#define _DREAM_RESOURCES_LOADABLE_H

#include "../Framework.h"
#include "../Core/Data.h"
#include "../Core/Strings.h"

#include <vector>
#include <exception>

namespace Dream {
	namespace Resources {
		using namespace Dream::Core;
		/*
		 The resource loader system ideally has two types of resources to load:
		 - Files which correspond directly to resources
		 - Files which index sets of resources
		 
		 Files that directly correspond to resources shouldn't directly load classes
		 that equate to the resource. Generally, you should have a ResourceFactory
		 that contains the required information, and can build instances of the 
		 required data. You can inject the resource loader into the factory so
		 that it is available to load further resources as needed.
		 
		 In the case of files which index a set of resources, you should create a
		 MyDataTypeLibrary which has a function:
			REF(MyDataTypeFactory) findResource (String name);
		 This factory can then be used to create corresponding resources.
		 */
		
		class ILoader;

		class LoadError : public std::exception {
		protected:
			String m_what;
		public:
			LoadError (String what);
			virtual ~LoadError () throw ();
			
			virtual const char * what() throw ();
		};

		class ILoadable : implements IObject 
		{
		public:
			virtual void registerLoaderTypes (ILoader * loader) abstract;
			virtual REF(Object) loadFromData (const PTR(IData) data, const ILoader * loader) abstract;
		};
		
		class ILoadableFromStream : implements ILoadable 
		{
		public:
			virtual REF(Object) loadFromData (const PTR(IData) data, const ILoader * loader);
			virtual REF(Object) loadFromStream (std::istream& input, const ILoader * loader) abstract;
		};

	}
}

#endif