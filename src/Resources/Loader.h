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
#include "../Events/Logger.h"

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
	 Ref<MyResourcesFactory> factory = loader->load <MyResourcesFactory> ("path/to/resource");
	 </code>
	 
	 Factory classes should process the loaded data accordingly. A resource factory should provide a set of creation methods, such as <tt>create_my_resource</tt>.
	 This means that once a resource is loaded, the factory can be cached (i.e. the loaded data), and actual "resources" can be created as needed.
	 
	 It might be necessary to load data from more than one path. If this is the case, an AggregateLoader should be used. Generally, an application should have
	 need for only one top level loader, and it should be constructed so that it has access to all resources needed.
	 
	 A resource factory should retain an instance to its original loader if needed.
	 */
	namespace Resources {
		using namespace Dream::Core;
		
		/// Default application resources path.
		Path application_working_path();
		
		StringT extension(const Path &s, bool dot);
		
		typedef std::map<StringT, Ref<ILoadable>> LoadersT;
		
		class ILoader : implements IObject {
		public:
			virtual Ref<Object> load_path (const Path &res) const abstract;
			// This resource loader's current base path
			virtual Path resource_path () const abstract;
			
			/// Normalize a resource request
			virtual Path path_for_resource(Path) const abstract;
			virtual Path path_for_resource(StringT name, StringT ext, Path dir) const abstract;
			virtual void resources_for_type(StringT ext, Path subdir, std::vector<Path> & paths) const abstract;
			
			/// Primary interface for loading resources
			template <typename InterfaceT>
			Ref<InterfaceT> load (const Path &res) const {
				using namespace Events::Logging;
				
				Ref<Object> ptr = load_path(path_for_resource(res));
				
				if (!ptr) { 
					logger()->log(LOG_ERROR, LogBuffer() << "Resource failed to load: " << res);
				}
				
				Ref<InterfaceT> result = ptr;
				
				if (!result) {
					logger()->log(LOG_ERROR, LogBuffer() << "Invalid resource class: " << res);
				}
				
				return result;
			}
			
			virtual void preload_resource (const Path & path) abstract;
			virtual void preload_resources (std::vector<Path> & paths) abstract;

			virtual void set_loader_for_extension (Ptr<ILoadable> loadable, StringT ext) abstract;
			virtual Ptr<ILoadable> loader_for_extension (StringT ext) const abstract;
			virtual void add_loader(Ptr<ILoadable> loader) abstract;
						
			/// Load the raw data for a given path.
			virtual Ref<IData> fetch_data_for_path (const Path & path) const abstract;
			
			/// Useful for loading buffers of data.
			Ref<IData> data_for_resource (const Path & resource) {
				return fetch_data_for_path(path_for_resource(resource));
			}
		};
		
		class Loader : public Object, implements ILoader {
		protected:
			// Mapping can be simple file extensions
			// Such as "png" or "tga" or "shader"

			LoadersT _loaders;
			
			Path _current_path;
			
			typedef std::map<Path, Ref<IData>> CacheT;
			mutable CacheT _data_cache;
			
		public:
			virtual void set_loader_for_extension (Ptr<ILoadable> loadable, StringT ext);
			virtual Ptr<ILoadable> loader_for_extension (StringT ext) const;
			virtual void add_loader(Ptr<ILoadable> loader);
			
			Loader ();
			Loader (Path);
			
			virtual ~Loader ();
			
			// This resource loader's current base path
			Path resource_path () const { return _current_path; }
			
			// Normalize a resource request
			Path path_for_resource(Path) const;
			Path path_for_resource(StringT name, StringT ext, Path dir) const;
			
			// Load a path directly with no processing
			virtual Ref<Object> load_path (const Path &res) const;
			virtual Ref<IData> fetch_data_for_path (const Path & path) const;
						
			void resources_for_type(StringT ext, Path subdir, std::vector<Path> &paths) const;

			virtual void preload_resource (const Path & path);
			virtual void preload_resources (std::vector<Path> & paths);
		};
		
		/*
		class AggregateLoader {
		protected:
			std::vector<Loader> _loaders;
		public:
			
		};
		*/
	}
}

#endif
