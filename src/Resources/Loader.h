/*
 *  Resources/Loader.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 29/09/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#ifndef _DREAM_RESOURCES_LOADER_H
#define _DREAM_RESOURCES_LOADER_H

#include "Loadable.h"

#include <map>

namespace Dream {
	/**
	 Provides infrastructure for loading resources.
	 
	 Loading resources is a fairly complex task. A number of traits are useful to have:
		
		- Quick loading of data from disk (using mmap for example).
		- The ability to load data without knowning its exact type.
		- The ability to search multiple locations for resources.
	 
	 Classes that are designed to load from resources need to be registered. This is typically done by writing
	 
	 <code>
	 Resources::register("filetype", MyResourceFactory::klass);
	 </code>
	 
	 This means that the user can write
	 
	 <code>
	 REF(MyResourcesFactory) factory = loader->load <MyResourcesFactory> ("path/to/resource");
	 </code>
	 
	 Factory classes should process the loaded data accordingly. A resource factory should provide a set of creation methods, such as <tt>createMyResource</tt>.
	 This means that once a resource is loaded, the factory can be cached (i.e. the loaded data), and actual "resources" can be created as needed.
	 
	 It might be necessary to load data from more than one path. If this is the case, an AggregateLoader should be used. Generally, an application should have
	 need for only one top level loader, and it should be constructed so that it has access to all resources needed.
	 
	 A resource factory should retain an instance to its original loader if needed.
	 */
	namespace Resources {
		using namespace Dream::Core;
		
		/// Default application resources path.
		Path applicationWorkingPath();
		
		String extension(const Path &s, bool dot);
		
		typedef std::map<String, ILoadable::Class*> LoadersT;
		
		class ILoader : IMPLEMENTS(Object)
		{
			EXPOSE_INTERFACE(Loader)
			
			class Class : IMPLEMENTS(Object::Class)
			{
			};
			
			virtual REF(Object) loadPath (const Path &res) const abstract;
			// This resource loader's current base path
			virtual Path resourcePath () const abstract;
			
			/// Normalize a resource request
			virtual Path pathForResource(Path) const abstract;
			virtual Path pathForResource(String name, String ext, Path dir) const abstract;
			virtual void resourcesForType(String ext, Path subdir, std::vector<Path> & paths) const abstract;

			/// Primary interface for loading resources
			template <typename InterfaceT>
			REF(InterfaceT) load (const Path &res) const {
				REF(Object) ptr = loadPath(pathForResource(res));
				
				if (!ptr) std::cerr << "Resource failed to load: '" << res << "'" << std::endl;
				
				REF(InterfaceT) result = ptr;
				
				if (!result) std::cerr << "Resource was not correct type: '" << res << "' -> " << InterfaceT::staticType()->name() << std::endl;
				
				return result;
			}
			
			virtual void preloadResource (const Path & path) abstract;
			virtual void preloadResources (std::vector<Path> & paths) abstract;

			virtual void setLoaderForExtension (ILoadable::Class* cls, String ext) abstract; 
			virtual ILoadable::Class* loaderForExtension (String ext) const abstract;
		};
		
		class Loader : public Object, IMPLEMENTS(Loader) {
		protected:
			// Mapping can be simple file extensions
			// Such as "png" or "tga" or "shader"

			LoadersT m_loaders;
			
			Path m_currentPath;
			
			typedef std::map<Path, REF(Data)> CacheT;
			mutable CacheT m_dataCache;
			
			REF(Data) fetchDataForPath (const Path & path) const;
		public:
			void setLoaderForExtension (ILoadable::Class* cls, String ext);
			ILoadable::Class* loaderForExtension (String ext) const;
		
			class Class : public Object::Class, IMPLEMENTS(Loader::Class) {
			public:
				EXPOSE_CLASSTYPE
			};
			
			EXPOSE_CLASS(Loader)
			
			Loader ();
			Loader (Path);
			
			virtual ~Loader ();
			
			// This resource loader's current base path
			Path resourcePath () const { return m_currentPath; }
			
			// Normalize a resource request
			Path pathForResource(Path) const;
			Path pathForResource(String name, String ext, Path dir) const;
			
			// Load a path directly with no processing
			virtual REF(Object) loadPath (const Path &res) const;
						
			void resourcesForType(String ext, Path subdir, std::vector<Path> &paths) const;

			virtual void preloadResource (const Path & path);
			virtual void preloadResources (std::vector<Path> & paths);
		};
		
		REF(Loader) applicationLoader ();
		
		/*
		class AggregateLoader {
		protected:
			std::vector<Loader> m_loaders;
		public:
			
		};
		*/
	}
}

#endif