/*
 *  Path-Cocoa.cpp
 *  Dream
 *
 *  Created by Samuel Williams on 3/09/10.
 *  Copyright 2010 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Path.h"

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
		Path::FileType Path::fileStatus() const {
			NSAutoreleasePool * pool = [NSAutoreleasePool new];			

			NSString * path = [[[NSString alloc] initWithUTF8String:toLocalPath().c_str()] autorelease];
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
		
		Path::FileSizeT Path::fileSize() {
			FileSizeT fileSize = 0;
						
			NSAutoreleasePool * pool = [NSAutoreleasePool new];			

			NSString * path = [[[NSString alloc] initWithUTF8String:toLocalPath().c_str()] autorelease];
			NSError * error = NULL;
			NSDictionary * attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:&error];
			
			if (error) {
				SystemError systemError([[error domain] UTF8String], [error code], [[error description] UTF8String], toLocalPath());
				
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
			NSString * path = [[NSString alloc] initWithUTF8String:toLocalPath().c_str()];
			NSDirectoryEnumerator * directory = [[NSFileManager defaultManager] enumeratorAtPath:path];

			[directory skipDescendents];
			
			NSString * entry;
			while (entry = [directory nextObject]) {
				StringT filename([entry UTF8String]);
				
				if (filter) {
					Path fullPath = *this + filename;
					
					if (fullPath.fileStatus() != filter)
						continue;
				}
				
				entries.push_back([entry UTF8String]);
			}
			
			[path release];
			[pool release];
			
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
			NSAutoreleasePool * pool = [NSAutoreleasePool new];			
			Path path([[[NSFileManager defaultManager] currentDirectoryPath] UTF8String]);
			[pool release];
			
			return path;
		}
	}
}

