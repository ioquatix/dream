//
//  Framework.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 29/09/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_FRAMEWORK_H
#define _DREAM_FRAMEWORK_H

#include <cstddef>

#define abstract = 0
#define implements virtual public

#ifdef __GNUC__
#define PACKED __attribute__((__packed__))
#else
#error Cannot define PACKED. Please supply a macro for packing structs.
#endif

// Collection should typically avoid being a function call to avoid unnessary overhead. 
#define foreach(iterator,collection) for(decltype((collection).begin()) iterator = (collection).begin(); iterator != (collection).end(); ++iterator)

/**
	Dream is a framework for creating and enhancing C++ applications.

	The Dream framework provides several high level components which assist with the design and development of C++ appications. It is specifically targeted at real
	time simulations such as games and scientific visualizations.
 */
namespace Dream
{
// MARK: mark -
// MARK: mark Types

	typedef std::size_t IndexT;
	typedef unsigned char ByteT;
	
	/// Represents a open file.
	typedef int FileDescriptorT;
	
	const char * build_date ();
	const char * build_platform ();
	const char * build_revision ();
}

#include "Assertion.h"
#include "Class.h"

#ifdef ENABLE_TESTING
#include "Core/CodeTest.h"
#endif

#endif
