//
//  Resources/Loader.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 4/05/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "Loader.h"

#include "../Events/Logger.h"

#include <iostream>
#include <map>

namespace Dream {
	namespace Resources {
		using namespace Events::Logging;

		void Loader::set_loader_for_extension (Ptr<ILoadable> loadable, StringT ext) {
			_loaders[ext] = loadable;
		}

		Ptr<ILoadable> Loader::loader_for_extension (StringT ext) const {
			LoadersT::const_iterator loader = _loaders.find(ext);

			if (loader != _loaders.end()) {
				return loader->second;
			} else {
				return NULL;
			}
		}

		void Loader::add_loader(Ptr<ILoadable> loader) {
			loader->register_loader_types(this);
		}

		Loader::Loader () {
			_current_path = application_working_path();
		}

		Loader::Loader (Path in) {
			if (in.is_absolute())
				_current_path = in;
			else
				_current_path = application_working_path() + in;

			// Check that the path is actually a directory that exists.
			DREAM_ASSERT(_current_path.file_status() == Path::DIRECTORY);
		}

		Loader::~Loader ()
		{
			// logger()->log(LOG_INFO, LogBuffer() << "Loader being deallocated: " << this);

			double total_size = 0.0;
			for (auto cache : _data_cache) {
				total_size += cache.second->size();
			}

			total_size /= (1024 * 1024);

			logger()->log(LOG_INFO, LogBuffer() << "Freeing: " << total_size << "Mbytes.");
		}

		Ref<IData> Loader::fetch_data_for_path (const Path & path) const
		{
			CacheT::iterator c = _data_cache.find(path);

			if (c != _data_cache.end())
				return c->second;


			if (path.exists()) {
				Ref<IData> data = new LocalFileData(path);

				// logger()->log(LOG_INFO, LogBuffer() << "Adding " << path << " to cache.");

				_data_cache[path] = data;

				return data;
			} else {
				return NULL;
			}
		}

		void Loader::preload_resource (const Path & path)
		{
			logger()->log(LOG_INFO, LogBuffer() << "Preloading " << path << "...");

			fetch_data_for_path(path_for_resource(path));
		}

		void Loader::preload_resources (std::vector<Path> & paths)
		{
			for (auto path : paths)
			{
				preload_resource(path);
			}
		}

		Path Loader::path_for_resource (Path p) const {
			Path::NameComponents name_components = p.last_name_components();

			return path_for_resource(name_components.basename, name_components.extension, p.parent_path());
		}

		void Loader::resources_for_type(StringT ext, Path subdir, std::vector<Path> &paths) const {
			Path full_path = _current_path + subdir;

			if (full_path.exists()) {
				Path::DirectoryListingT entries = full_path.list(Path::STORAGE);

				for (std::size_t i = 0; i < entries.size(); i++) {
					if (Path(entries[i]).last_name_components().extension == ext)
						paths.push_back(entries[i]);
				}
			}
		}

		Path Loader::path_for_resource(StringT name, StringT ext, Path dir) const {
			Path full_path = _current_path + dir;

			//std::cerr << "Looking for: " << name << " ext: " << ext << " in: " << full_path << std::endl;

			if (!full_path.exists())
				return Path();

			if (ext.empty()) {
				// Find all named resources
				Path::DirectoryListingT resource_paths;
				Path::DirectoryListingT entries = full_path.list(Path::STORAGE);

				for (std::size_t i = 0; i < entries.size(); i++) {
					//std::cerr << "Looking at: " << entries[i] << std::endl;

					if (Path(entries[i]).last_name_components().basename == name) {
						//std::cerr << "\t_found: " << entries[i] << std::endl;
						resource_paths.push_back(entries[i]);
					}
				}

				if (resource_paths.size() > 1) {
					logger()->log(LOG_WARN, LogBuffer() << "Multiple paths found for resource: " << name << " in " << full_path);

					for (auto path : resource_paths) {
						logger()->log(LOG_WARN, LogBuffer() << "\t" << path);
					}
				}

				if (resource_paths.size() >= 1) {
					full_path = full_path + resource_paths[0];
				} else {
					full_path = Path();
				}
			} else {
				full_path = full_path + (name + "." + ext);
			}

			//std::cerr << "Full Path = " << full_path << std::endl;

			// Does a file exist?
			if (full_path.file_status() == Path::STORAGE)
				return full_path;

			return Path();
		}

		Ref<Object> Loader::load_path (const Path &p) const {
			if (!p.exists()) {
				logger()->log(LOG_WARN, LogBuffer() << "File does not exist at path: " << p);

				return Ref<Object>();
			}

			StringT ext = p.last_name_components().extension;
			Ptr<ILoadable> loader = loader_for_extension(ext);

			if (!loader) {
				// No loader for this type

				logger()->log(LOG_WARN, LogBuffer() << "No loader found for type: " << ext << " while loading path:" << p);

				return Ref<Object>();
			}

			Ref<IData> data = fetch_data_for_path(p);
			Ref<Object> resource = NULL;

			try {
				resource = loader->load_from_data(data, this);
			} catch (LoadError & e) {
				logger()->log(LOG_WARN, LogBuffer() << "Exception thrown while loading resource " << p << ": " << e.what());

				throw;
			}

			if (!resource) {
				logger()->log(LOG_WARN, LogBuffer() << "Resource " << p << " failed to load!");
			}

			return resource;
		}
	}
}
