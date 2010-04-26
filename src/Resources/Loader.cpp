/*
 *  Resources/Loader.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 4/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "Loader.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/pool/detail/singleton.hpp>

#include <boost/filesystem/convenience.hpp>

#include "../Core/Singleton.h"

#include <iostream>
#include <map>

namespace Dream {
	namespace Resources {
		
		IMPLEMENT_INTERFACE(Loader)
		
#pragma mark -
		
		using namespace std;
		using namespace boost::filesystem;
		
		void setNativePathFormat () {
			static bool setNative = false;
			if (!setNative) {
				Path::default_name_check(native);
				setNative = true;
			}
		}
		
		String extension(const Path &s, bool dot) {
			String ext = boost::filesystem::extension(s);
			
			if (!dot && !ext.empty())
				return ext.substr(1);
			else
				return ext;
		}
		
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
			m_currentPath = workingPath();
		}
		
		Loader::Loader (Path in) {
			if (in.has_root_directory())
				m_currentPath = in;
			else
				m_currentPath = workingPath() / in;
			
			ensure(is_directory(m_currentPath));
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
			return pathForResource(basename(p), extension(p, false), p.parent_path());
		}
		
		void Loader::resourcesForType(String ext, Path subdir, std::vector<Path> &paths) const {
			Path subp = m_currentPath / subdir;
			
			if (exists(subp)) {
				directory_iterator end_itr;
				
				for (directory_iterator itr(subp); itr != end_itr; ++itr) {
					if (extension(itr->leaf(), false) == ext)
						paths.push_back(*itr);
				}
			}
		}
		
		Path Loader::pathForResource(String name, String ext, Path dir) const {
			Path subp = m_currentPath / dir;
			
			//cerr << "Looking for: " << name << " ext: " << ext << " in: " << dir.string() << endl;
			
			if (!exists(subp)) {
				return Path();
			}
			
			if (ext.empty()) {
				// Find all named resources
				std::vector<Path> paths;
				
				directory_iterator end_itr; // default construction yields past-the-end
				
				for (directory_iterator itr(subp); itr != end_itr; ++itr) {
					if (itr->leaf()[0] == '.') continue;
					
					//cerr << "Looking at: " << itr->leaf() << endl;
					
					if (is_directory(*itr)) {
						continue; // We don't care about directory
					} else if (basename(itr->leaf()) == name) {
						//cerr << "Found: " << itr->leaf() << endl;
						paths.push_back(*itr);
					}
				}
				
				if (paths.size() > 1) {
					cerr << "Multiple paths found for resource: " << name << " in " << subp.string() << endl;
					foreach(Path p, paths)
						cerr << "-- " << p.string() << endl;
				}
				
				if (paths.size() >= 1) {
					subp = paths[0];
				} else {
					subp = Path();
				}
			} else {
				subp /= name + "." + ext;
			}
			
			// Does a file exist?
			if (exists(subp) && !is_directory(subp))
				return subp;
			
			return Path();
		}
		
		REF(Object) Loader::loadPath (const Path &p) const {
			if (!exists(p)) {
				cerr << "File does not exist at path '" << p.string() << "'" << endl;
				return REF(Object)();
			}
			
			String ext = extension(p, false);
			ILoadable::Class *loader = loaderForExtension(ext);
			
			if (!loader) {
				// No loader for this type
				cerr << "No loader found for type '" << ext << "' (" << p.string() << ")." << endl;
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
				cerr << "Resource " << p.string() << " failed to load!" << endl;
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