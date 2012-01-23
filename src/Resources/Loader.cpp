//
//  Resources/Loader.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 4/05/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "Loader.h"

#include <iostream>
#include <map>

namespace Dream {
	namespace Resources {
		
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
			ensure(_current_path.file_status() == Path::DIRECTORY);
		}
		
		Loader::~Loader ()
		{
			std::cerr << "Loader being deallocated: " << this << std::endl;
			
			double total_size = 0.0;
			foreach(cache, _dataCache) {
				total_size += cache->second->size();
			}
			
			total_size /= (1024 * 1024);
			std::cerr << "Freeing " << total_size << " Mbytes." << std::endl;
		}
		
		Ref<IData> Loader::fetch_data_for_path (const Path & path) const
		{
			CacheT::iterator c = _dataCache.find(path);
			
			if (c != _dataCache.end())
				return c->second;
			
 			Ref<IData> data = new LocalFileData(path);
			
			std::cerr << "Adding " << path << " to cache." << std::endl;
			
			_dataCache[path] = data;
			
			return data;
		}
		
		void Loader::preload_resource (const Path & path)
		{
			std::cerr << "Preloading " << path << "..." << std::endl;
			fetch_data_for_path(path_for_resource(path));
		}
		
		void Loader::preload_resources (std::vector<Path> & paths)
		{
			foreach (path, paths)
			{
				preload_resource(*path);
			}
		}
		
		Path Loader::path_for_resource (Path p) const {
			Path::NameInfo name_info = p.split_file_name();
			
			return path_for_resource(name_info.basename, name_info.extension, p.dirname());
		}
		
		void Loader::resources_for_type(StringT ext, Path subdir, std::vector<Path> &paths) const {
			Path full_path = _current_path + subdir;
			
			if (full_path.exists()) {
				Path::DirectoryListingT entries = full_path.list(Path::STORAGE);
				
				for (std::size_t i = 0; i < entries.size(); i++) {
					if (Path(entries[i]).split_file_name().extension == ext)
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
					
					if (Path(entries[i]).split_file_name().basename == name) {
						//std::cerr << "\t_found: " << entries[i] << std::endl;
						resource_paths.push_back(entries[i]);
					}
				}
				
				if (resource_paths.size() > 1) {
					std::cerr << "Multiple paths found for resource: " << name << " in " << full_path << std::endl;
					foreach(path, resource_paths)
						std::cerr << "\t" << *path << std::endl;
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
				std::cerr << "File does not exist at path '" << p << "'" << std::endl;
				return Ref<Object>();
			}
			
			StringT ext = p.split_file_name().extension;
			Ptr<ILoadable> loader = loader_for_extension(ext);
			
			if (!loader) {
				// No loader for this type
				std::cerr << "No loader found for type '" << ext << "' (" << p << ")." << std::endl;
				return Ref<Object>();
			}
			
			Ref<IData> data = fetch_data_for_path(p);
			Ref<Object> resource = NULL;
			
			try {
				resource = loader->load_from_data(data, this);
			} catch (LoadError & e) {
				std::cerr << "Could not load resource " << p << ": " << e.what() << std::endl;
				throw;
			}
			
			if (!resource) {
				std::cerr << "Resource " << p << " failed to load!" << std::endl;
			}
			
			return resource;
		}
	}
}
