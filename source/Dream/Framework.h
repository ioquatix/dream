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

// MARK: -
// MARK: Target operating system conditionals
// http://stackoverflow.com/questions/6649936/c-compiling-on-windows-and-linux-ifdef-switch
#ifdef __APPLE__
#include <TargetConditionals.h>
#elif __ANDROID_API__
#define TARGET_OS_ANDROID 1
#elif __linux__
#define TARGET_OS_LINUX 1
#elif _WIN32
#define TARGET_OS_WIN32 1
#endif

#define abstract = 0
#define implements virtual public

/**
    Dream is a framework for creating and enhancing C++ applications.

    The Dream framework provides several high level components which assist with the design and development of C++ appications. It is specifically targeted at real
    time simulations such as games and scientific visualizations.
 */
namespace Dream
{
// MARK: -
// MARK: Types
	typedef unsigned char ByteT;

	/// Represents a open file.
	typedef int FileDescriptorT;
}

#include "Assertion.h"
#include "Class.h"

#endif
