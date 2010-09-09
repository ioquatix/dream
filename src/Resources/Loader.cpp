/*
 *  Resources/Loader.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 4/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "Loader.h"
#include "../Core/Singleton.h"

#include <iostream>
#include <map>

namespace Dream {
	namespace Resources {
		
		IMPLEMENT_INTERFACE(Loader)
		
#pragma mark -
		
		IMPLEMENT_CLASS(Loader)
		
		void Loader::setLoaderForExtension (ILoadable::Class* cls, String ext) {
			m_loaders[ext] = cls;
		}
		
		ILoadable::Class* Loader::loaderForExtension (String ext) const {
			LoadersT::const_iterator loader = m_loaders.find(ext);
			
			if (loader != m_loaders.end()) {
				return loader->second;
			} else {
				return NULL;
			}
		}
		
		Loader::Loader () {
			m_currentPath = applicationWorkingPath();
		}
		
		Loader::Loader (Path in) {
			if (in.isAbsolute())
				m_currentPath = in;
			else
				m_currentPath = applicationWorkingPath() + in;
			
			// Check that the path is actually a directory that exists.
			ensure(m_currentPath.fileStatus() == Path::DIRECTORY);
		}
		
		Loader::~Loader ()
		{
			std::cerr << "Loader being deallocated: " << this << std::endl;
			
			double totalSize = 0;
			for (iterateEach(m_dataCache, cache)) {
				totalSize += cache->second->size();
			}
			
			ensure(totalSize != 0);
			totalSize /= (1024 * 1024);
			std::cerr << "Freeing " << totalSize << " Mbytes" << std::endl;
		}
		
		REF(Data) Loader::fetchDataForPath (const Path & path) const
		{
			CacheT::iterator c = m_dataCache.find(path);
			
			if (c != m_dataCache.end())
				return c->second;
			
 			REF(Data) data = Data::klass.initWithPath(path);
			
			std::cerr << "Adding " << path << " to cache" << std::endl;
			
			m_dataCache[path] = data;
			
			return data;
		}
		
		void Loader::preloadResource (const Path & path)
		{
			std::cerr << "Preloading " << path << "..." << std::endl;
			fetchDataForPath(pathForResource(path));
		}
		
		void Loader::preloadResources (std::vector<Path> & paths)
		{
			foreach (const Path & p, paths)
			{
				preloadResource(p);
			}
		}
		
		Path Loader::pathForResource (Path p) const {
			Path::NameInfo nameInfo = p.splitFileName();
			
			return pathForResource(nameInfo.basename, nameInfo.extension, p.dirname());
		}
		
		void Loader::resourcesForType(String ext, Path subdir, std::vector<Path> &paths) const {
			Path fullPath = m_currentPath + subdir;
			
			if (fullPath.exists()) {
				Path::DirectoryListingT entries = fullPath.list(Path::STORAGE);
				
				for (std::size_t i = 0; i < entries.size(); i++) {
					if (Path(entries[i]).splitFileName().extension == ext)
						paths.push_back(entries[i]);
				}
			}
		}
		
		Path Loader::pathForResource(String name, String ext, Path dir) const {
			Path fullPath = m_currentPath + dir;
			
			std::cerr << "Looking for: " << name << " ext: " << ext << " in: " << fullPath << std::endl;
			
			if (!fullPath.exists())
				return Path();
			
			if (ext.empty()) {
				// Find all named resources
				Path::DirectoryListingT resourcePaths;
				Path::DirectoryListingT entries = fullPath.list(Path::STORAGE);
				
				for (std::size_t i = 0; i < entries.size(); i++) {
					std::cerr << "Looking at: " << entries[i] << std::endl;
					
					if (Path(entries[i]).splitFileName().basename == name) {
						std::cerr << "\tFound: " << entries[i] << std::endl;
						resourcePaths.push_back(entries[i]);
					}
				}
				
				if (resourcePaths.size() > 1) {
					std::cerr << "Multiple paths found for resource: " << name << " in " << fullPath << std::endl;
					foreach(Path p, resourcePaths)
						std::cerr << "\t" << p << std::endl;
				}
				
				if (resourcePaths.size() >= 1) {
					fullPath = fullPath + resourcePaths[0];
				} else {
					fullPath = Path();
				}
			} else {
				fullPath = fullPath + (name + "." + ext);
			}
			
			std::cerr << "Full Path = " << fullPath << std::endl;
			
			// Does a file exist?
			if (fullPath.fileStatus() == Path::STORAGE)
				return fullPath;
			
			return Path();
		}
		
		REF(Object) Loader::loadPath (const Path &p) const {
			if (!p.exists()) {
				std::cerr << "File does not exist at path '" << p << "'" << std::endl;
				return REF(Object)();
			}
			
			String ext = p.splitFileName().extension;
			ILoadable::Class *loader = loaderForExtension(ext);
			
			if (!loader) {
				// No loader for this type
				std::cerr << "No loader found for type '" << ext << "' (" << p << ")." << std::endl;
				return REF(Object)();
			}
			
			REF(Data) data = fetchDataForPath(p);
			
			REF(Object) resource = NULL;
			
			try {
				resource = loader->initFromData(data, this);
			} catch (LoadError & e) {
				std::cerr << "Could not load resource " << p << ": " << e.what() << std::endl;
				throw;
			}
			
			if (!resource) {
				std::cerr << "Resource " << p << " failed to load!" << std::endl;
			}
			
			return resource;
		}
		
		REF(Loader) applicationLoader () {
			static REF(Loader) s_applicationLoader;
			
			if (!s_applicationLoader)
				s_applicationLoader = new Loader;
			
			return s_applicationLoader;
		}
	}
}