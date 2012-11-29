//
//  Core/Path-Unix.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 3/09/10.
//  Copyright (c) 2010 Samuel Williams. All rights reserved.
//
//

#include "Path.h"
#include "System.h"

#include <cstddef>

// For stat()
#include <sys/stat.h>

// For directory access
#include <dirent.h>

// For errno
#include <sys/errno.h>

// MAXPATHLEN
#include <sys/param.h>

// getcwd
#include <unistd.h>

// memset
#include <string.h>

namespace Dream
{
	namespace Core
	{
		Path::FileType Path::file_status() const {
			SystemError::reset();

			StringT path = to_local_path();
			struct stat file_info;

			// Clear the structure.
			memset(&file_info, 0, sizeof(file_info));

			if (stat(path.c_str(), &file_info) != 0) {
				// If there was an error other than not found...
				if (errno != ENOENT) {
					SystemError::check(path);
				}

				//std::cerr << path << " is unknown" << std::endl;
				return UNKNOWN;
			}

			//mode_t mode = file_info.st_mode & S_IFMT;

			if (S_ISDIR(file_info.st_mode)) {
				//std::cerr << path << " is directory" << std::endl;
				return DIRECTORY;
			}

			//std::cerr << path << " is storage : " << S_ISREG(file_info.st_mode) << std::endl;

			// It isn't a directory so it must be some kind of storage.
			return STORAGE;
		}

		Path::FileSizeT Path::file_size() const {
			SystemError::reset();

			StringT path = to_local_path();
			struct stat file_info;

			memset(&file_info, 0, sizeof(file_info));

			if (stat(path.c_str(), &file_info) != 0) {
				SystemError::check(path);
			}

			return FileSizeT(file_info.st_blocks) * FileSizeT(file_info.st_blksize);
		}

		Path::DirectoryListingT Path::list (FileType filter) const {
			DirectoryListingT entries;
			StringT path = to_local_path();
			DIR * directory = opendir(path.c_str());

			struct dirent storage, * entry;
			int entry_type = 0;

			if (filter & DIRECTORY)
				entry_type |= DT_DIR;
			else if (filter & STORAGE)
				entry_type |= DT_REG;

			while (1) {
				int error = readdir_r(directory, &storage, &entry);

				// Did some error occur?
				if (error != 0)
					throw std::runtime_error("Could not enumerate directory");

				// Are we at the end of the directory list?
				if (entry == NULL)
					break;

				if (entry_type && !(entry_type & entry->d_type))
					continue;

				// Skip "hidden" files.
				if (!(filter & HIDDEN) && entry->d_name[0] == '.')
					continue;

				entries.push_back(entry->d_name);
			}

			closedir(directory);

			return entries;
		}

		void Path::remove () const {
			StringT path = to_local_path();

			if (::remove(path.c_str()) != 0) {
				SystemError::check(__func__);
			}
		}

		void Path::move (const Path & new_name) const {
			StringT from = to_local_path(), to = new_name.to_local_path();
			if (::rename(from.c_str(), to.c_str()) != 0) {
				SystemError::check(__func__);
			}
		}

		Path Path::temporary_file_path () {
			char * path = ::tmpnam(NULL);

			if (path == nullptr) {
				SystemError::check(__func__);
			}

			return Path(path);
		}

		Path Path::current_working_directory () {
			char buffer[MAXPATHLEN];
			getcwd(buffer, MAXPATHLEN);

			return Path((const char *)buffer);
		}
	}
}
