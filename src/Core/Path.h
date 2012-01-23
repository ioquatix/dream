//
//  Core/Path.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/09/10.
//  Copyright (c) 2010 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CORE_PATH_H
#define _DREAM_CORE_PATH_H

#include "Strings.h"
#include <vector>

namespace Dream
{
	namespace Core
	{
		/**
			A simple class for accessing the local filesystem.
		 */
		class Path
		{
		public:
			typedef uint64_t FileSizeT;
			
			typedef std::vector<StringT> DirectoryListingT;
			typedef std::vector<StringT> ComponentsT;
			struct NameInfo {
				StringT basename;
				StringT extension;
			};
			
			static const char SEPARATOR = '/';
		
		protected:
			ComponentsT m_components;

		public:
			Path() {}
			Path(const ComponentsT & components);

			Path(const char * stringRep);
			Path(const StringT & stringRep);
			Path(const StringT & stringRep, const char separator);
			
			bool empty () const { return m_components.size() == 0; }
			
			StringT toLocalPath() const;

			/// Does the path components begin with ""
			bool isAbsolute() const;
			/// Returns an absolute path
			Path toAbsolute();

			/// Does the path components end with ""
			bool isDirectory() const;			
			/// Returns a path representing a directory (i.e. trailing slash)
			Path toDirectory();
			
			const ComponentsT & components() const { return m_components; }
			
			// Remove "." and ".."
			Path simplify() const;
			
			NameInfo splitFileName() const;
			
			/// Pop n components off the path
			Path dirname (std::size_t n = 1) const;
			
			Path operator+(const Path & other) const;
			Path operator+(const NameInfo & other) const;
			
			bool operator<(const Path & other) const;
			bool operator==(const Path & other) const;
			
			enum FileType {
				UNKNOWN = 0,
				DIRECTORY = 0x4,
				STORAGE = 0x8
			};
			
			// ** Local File Operations **
			FileType fileStatus() const;
			
			// Does this exist?
			bool exists() const { return fileStatus() != UNKNOWN; }
			
			FileSizeT fileSize() const;
			
			/// Removes a single file / directory
			void remove () const;
			
			/// Rename a file to the new name
			void move (const Path & newName) const;
			
			static Path temporaryFilePath ();
			static Path currentWorkingDirectory ();
			
			DirectoryListingT list (FileType filter) const;
		};
		
		std::ostream & operator<<(std::ostream & output, const Path & path);
	}
}

#endif
