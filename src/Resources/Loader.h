//
//  Resources/Loader.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 29/09/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

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
		
		StringT extension(const Path &s, bool dot);
		
		typedef std::map<StringT, REF(ILoadable)> LoadersT;
		
		class ILoader : implements IObject {
		public:
			virtual REF(Object) loadPath (const Path &res) const abstract;
			// This resource loader's current base path
			virtual Path resourcePath () const abstract;
			
			/// Normalize a resource request
			virtual Path pathForResource(Path) const abstract;
			virtual Path pathForResource(StringT name, StringT ext, Path dir) const abstract;
			virtual void resourcesForType(StringT ext, Path subdir, std::vector<Path> & paths) const abstract;
			
			/// Primary interface for loading resources
			template <typename InterfaceT>
			REF(InterfaceT) load (const Path &res) const {
				REF(Object) ptr = loadPath(pathForResource(res));
				
				if (!ptr) std::cerr << "Resource failed to load: '" << res << "'" << std::endl;
				
				REF(InterfaceT) result = ptr;
				
				if (!result) std::cerr << "Resource was not correct type: '" << res << std::endl;
				
				return result;
			}
			
			virtual void preloadResource (const Path & path) abstract;
			virtual void preloadResources (std::vector<Path> & paths) abstract;

			virtual void setLoaderForExtension (PTR(ILoadable) loadable, StringT ext) abstract;
			virtual PTR(ILoadable) loaderForExtension (StringT ext) const abstract;
			virtual void addLoader(PTR(ILoadable) loader) abstract;
						
			/// Load the raw data for a given path.
			virtual REF(IData) fetchDataForPath (const Path & path) const abstract;
			
			/// Useful for loading buffers of data.
			REF(IData) dataForResource (const Path & resource) {
				return fetchDataForPath(pathForResource(resource));
			}
		};
		
		class Loader : public Object, implements ILoader {
		protected:
			// Mapping can be simple file extensions
			// Such as "png" or "tga" or "shader"

			LoadersT m_loaders;
			
			Path m_currentPath;
			
			typedef std::map<Path, REF(IData)> CacheT;
			mutable CacheT m_dataCache;
			
		public:
			virtual void setLoaderForExtension (PTR(ILoadable) loadable, StringT ext);
			virtual PTR(ILoadable) loaderForExtension (StringT ext) const;
			virtual void addLoader(PTR(ILoadable) loader);
			
			Loader ();
			Loader (Path);
			
			virtual ~Loader ();
			
			// This resource loader's current base path
			Path resourcePath () const { return m_currentPath; }
			
			// Normalize a resource request
			Path pathForResource(Path) const;
			Path pathForResource(StringT name, StringT ext, Path dir) const;
			
			// Load a path directly with no processing
			virtual REF(Object) loadPath (const Path &res) const;
			virtual REF(IData) fetchDataForPath (const Path & path) const;
						
			void resourcesForType(StringT ext, Path subdir, std::vector<Path> &paths) const;

			virtual void preloadResource (const Path & path);
			virtual void preloadResources (std::vector<Path> & paths);
		};
		
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
