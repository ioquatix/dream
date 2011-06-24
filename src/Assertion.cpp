/*
 *  Assertion.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 26/10/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#include "Assertion.h"

#include <sstream>
#include <iostream>

// For testing
#include "Core/CodeTest.h"

void assertionFailure() {
	return;
}

namespace Dream
{
	AssertionError::AssertionError (const char * expression, const char * file, unsigned line) throw ()
	: m_expression (expression), m_file (file), m_line (line)
	{
		using namespace std;

		try
		{
			stringstream s;
			s << m_file << ":" << m_line << " assertion failed: " << m_expression;
			m_what = s.str ();
		}
		catch (...)
		{
			m_what = "Could not format assertion text!";
		}
	}

	AssertionError::~AssertionError () throw ()
	{
	}

	const char * AssertionError::what () const throw ()
	{
		return m_what.c_str();
	}

	void AssertionError::assertHandler (bool condition, const char * expression, const char * file, unsigned line)
	{
		if (!condition)
		{
			assertionFailure();
			
			throw AssertionError(expression, file, line);
		}
	}

#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING
	UNIT_TEST(Assertion)
	{
		bool exceptionThrown;

		testing("Ensure");

		exceptionThrown = false;
		try
		{
			ensure(1 == 0);
		}
		catch (AssertionError & err)
		{
			exceptionThrown = true;
		}

		check(exceptionThrown) << "Ensure threw exception";

		exceptionThrown = false;
		try
		{
			ensure(1 == 1);
		}
		catch (AssertionError & err)
		{
			exceptionThrown = true;
		}

		check(!exceptionThrown) << "Ensure didn't throw exception";
	}
#endif
}
