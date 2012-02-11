//
//  Core/Path-Mac.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 3/09/10.
//  Copyright (c) 2010 Samuel Williams. All rights reserved.
//
//

#include "Path.h"
#include "System.h"

#include <Foundation/NSError.h>
#include <Foundation/NSFileManager.h>
#include <Foundation/NSAutoreleasePool.h>

#include <cstddef>
#include <stdexcept>

// For directory access
#include <dirent.h>

// MAXPATHLEN
#include <sys/param.h>

namespace Dream
{
	namespace Core
	{
		Path::FileType Path::file_status() const {
			NSAutoreleasePool * pool = [NSAutoreleasePool new];			

			NSString * path = [[[NSString alloc] initWithUTF8String:to_local_path().c_str()] autorelease];
			BOOL isDir = NO;
			BOOL exists = [[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&isDir];
			
			[pool release];
			
			if (exists) {
				if (isDir) {
					return DIRECTORY;
				} else {
					return STORAGE;
				}
			} else {
				return UNKNOWN;
			}
		}
		
		Path::FileSizeT Path::file_size() const {
			FileSizeT fileSize = 0;
						
			NSAutoreleasePool * pool = [NSAutoreleasePool new];			

			NSError * error = nil;
			NSString * path = [[[NSString alloc] initWithUTF8String:to_local_path().c_str()] autorelease];
			NSDictionary * attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:&error];
			
			if (error) {
				//StringT domain, ErrorNumberT errorNumber, StringT errorDescription, StringT errorTarge
				SystemError systemError(StringT([[error domain] UTF8String]), ErrorNumberT([error code]), StringT([[error description] UTF8String]), to_local_path());
				
				[pool release];
				
				throw systemError;
			} else {
				fileSize = [attributes fileSize];
				
				[pool release];
			}
			
			return fileSize;
		}
		
		Path::DirectoryListingT Path::list (FileType filter) const {
			DirectoryListingT entries;

			NSAutoreleasePool * pool = [NSAutoreleasePool new];			
			NSString * path = [[NSString alloc] initWithUTF8String:to_local_path().c_str()];
			NSDirectoryEnumerator * directory = [[NSFileManager defaultManager] enumeratorAtPath:path];

			[directory skipDescendents];
			
			NSString * entry;
			while (entry = [directory nextObject]) {
				StringT filename([entry UTF8String]);
				
				if (filter) {
					Path fullPath = *this + filename;
					
					if (!(fullPath.file_status() & filter))
						continue;
				}
				
				if (!(filter & HIDDEN) & [entry characterAtIndex:0] == '.')
					continue;
				
				entries.push_back([entry UTF8String]);
			}
			
			[path release];
			[pool release];
			
			return entries;
		}
		
		void Path::remove () const {
			StringT path = to_local_path();
			
			if (::remove(path.c_str()) != 0)
				perror(__PRETTY_FUNCTION__);
		}
		
		void Path::move (const Path & new_name) const {
			StringT from = to_local_path(), to = new_name.to_local_path();
			
			if (rename(from.c_str(), to.c_str()) != 0)
				perror(__PRETTY_FUNCTION__);
		}
		
		Path Path::temporary_file_path() {
			char * path = tmpnam(NULL);
			
			return Path(path);
		}
		
		Path Path::current_working_directory() {
			NSAutoreleasePool * pool = [NSAutoreleasePool new];			
			Path path([[[NSFileManager defaultManager] currentDirectoryPath] UTF8String]);
			[pool release];
			
			return path;
		}
	}
}
