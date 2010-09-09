/*
 *  Path-Unix.cpp
 *  Dream
 *
 *  Created by Samuel Williams on 3/09/10.
 *  Copyright 2010 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Path.h"

#include <cstddef>

// For stat()
#include <sys/stat.h>

// For directory access
#include <dirent.h>

// For errno
#include <sys/errno.h>

// MAXPATHLEN
#include <sys/param.h>

namespace Dream
{
	namespace Core
	{
		Path::FileType Path::fileStatus() const {
			StringT path = toLocalPath();
			struct stat fileInfo;
			
			// Clear the structure.
			memset(&fileInfo, 0, sizeof(fileInfo));
			
			if (stat(path.c_str(), &fileInfo) != 0) {
				// If there was an error other than not found...
				if (errno != ENOENT) {
					std::cerr << path << ":";
					perror(__PRETTY_FUNCTION__);
				}
				
				std::cerr << path << " is unknown" << std::endl;
				return UNKNOWN;
			}
			
			mode_t mode = fileInfo.st_mode & S_IFMT;
			
			if (S_ISDIR(fileInfo.st_mode)) {
				std::cerr << path << " is directory" << std::endl;
				return DIRECTORY;
			}
			
			std::cerr << path << " is storage : " << S_ISREG(fileInfo.st_mode) << std::endl;
			
			// It isn't a directory so it must be some kind of storage.
			return STORAGE;
		}
		
		Path::DirectoryListingT Path::list (FileType filter) const {
			DirectoryListingT entries;
			StringT path = toLocalPath();
			DIR * directory = opendir(path.c_str());
			
			struct dirent * entry = NULL;
			int entryType = 0;
			
			if (filter == DIRECTORY)
				entryType = DT_DIR;
			else if (filter == STORAGE)
				entryType = DT_REG;
						
			while ((entry = readdir(directory)) != NULL) {
				//std::cout << "Checking entry: " << entry->d_name << std::endl;
				
				if (entryType && (entryType != entry->d_type))
					continue;
				
				entries.push_back(entry->d_name);
			}
			
			closedir(directory);
			
			return entries;
		}
		
		void Path::remove () const {
			StringT path = toLocalPath();
			
			if (::remove(path.c_str()) != 0)
				perror(__PRETTY_FUNCTION__);
		}
		
		void Path::move (const Path & newName) const {
			StringT from = toLocalPath(), to = newName.toLocalPath();
			if (rename(from.c_str(), to.c_str()) != 0)
				perror(__PRETTY_FUNCTION__);
		}
		
		Path Path::temporaryFilePath () {
			char * path = tmpnam(NULL);
			
			return Path(path);
		}
		
		Path Path::currentWorkingDirectory () {
			char buffer[MAXPATHLEN];
			getcwd(buffer, MAXPATHLEN);
			
			return Path((const char *)buffer);
		}
				
	}
}

