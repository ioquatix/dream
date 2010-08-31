/*
 *  Framework.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 29/09/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_FRAMEWORK_H
#define _DREAM_FRAMEWORK_H

#include <boost/utility.hpp>

#define abstract = 0

#ifdef __GNUC__
#define PACKED __attribute__((__packed__))
#else
#error Cannot define PACKED. Please supply a macro for packing structs.
#endif

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

// Make iteration of std containers easier
#define iterateEach(c, iter) typeof(c.begin()) iter = c.begin(); iter != c.end(); ++iter

/**
	Dream is a framework for creating and enhancing C++ applications.

	The Dream framework provides several high level components which assist with the design and development of C++ appications. It is specifically targeted at real
	time simulations such as games and scientific visualizations.
 */
namespace Dream
{
#pragma mark -
#pragma mark Types

	typedef std::size_t IndexT;
	typedef unsigned char ByteT;
	
	/// Represents a open file.
	typedef int FileDescriptorT;
	
	const char * buildDate ();
	const char * buildPlatform ();
	const char * buildRevision ();
}

#include "Assertion.h"
#include "Class.h"

#ifdef ENABLE_TESTING
#include "Core/CodeTest.h"
#endif

#endif