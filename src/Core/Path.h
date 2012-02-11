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
			ComponentsT _components;

		public:
			Path() {}
			Path(const ComponentsT & components);

			Path(const char * string_rep);
			Path(const StringT & string_rep);
			Path(const StringT & string_rep, const char separator);
			
			bool empty () const { return _components.size() == 0; }
			
			StringT to_local_path() const;

			/// Does the path components begin with ""
			bool is_absolute() const;
			/// Returns an absolute path
			Path to_absolute();

			/// Does the path components end with ""
			bool is_directory() const;			
			/// Returns a path representing a directory (i.e. trailing slash)
			Path to_directory();
			
			const ComponentsT & components() const { return _components; }
			
			// Remove "." and ".."
			Path simplify() const;
			
			NameInfo split_file_name() const;
			
			/// Pop n components off the path
			Path dirname (std::size_t n = 1) const;
			
			Path operator+(const Path & other) const;
			Path operator+(const NameInfo & other) const;
			
			bool operator<(const Path & other) const;
			bool operator==(const Path & other) const;
			
			enum FileType {
				UNKNOWN = 0,
				
				// Include directories:
				DIRECTORY = 1 << 0,
				
				// Regular readable files:
				STORAGE = 1 << 2,
			
				// Include entries that start with dots:
				HIDDEN = 1 << 8,
				
				ANY = 0xFFFFFFFF,
			};
			
			// ** Local File Operations **
			FileType file_status() const;
			
			// Does this exist?
			bool exists() const { return file_status() != UNKNOWN; }
			
			FileSizeT file_size() const;
			
			/// Removes a single file / directory
			void remove () const;
			
			/// Rename a file to the new name
			void move (const Path & new_name) const;
			
			static Path temporary_file_path ();
			static Path current_working_directory ();
			
			DirectoryListingT list (FileType filter) const;
		};
		
		std::ostream & operator<<(std::ostream & output, const Path & path);
	}
}

#endif
