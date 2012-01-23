//
//  Core/CodeTest.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 14/09/06.
//  Copyright 2006 Samuel WilliamsWilliams. All rights reserved.
//
//

#ifndef _DREAM_CORE_CODETEST_H
#define _DREAM_CORE_CODETEST_H

#include "Core.h"
#include "../Class.h"
#include "../Reference.h"

#include <sstream>
#include <list>

#define UNIT_TEST(name) \
class name##CodeTest : public ::Dream::Core::CodeTest \
{ \
public: \
name##CodeTest () : ::Dream::Core::CodeTest (#name " Test") \
{} \
void test (); \
} name##CodeTestInstance; \
void name##CodeTest::test ()

namespace Dream
{
	namespace Core
	{		
		class CodeTest;

		template <typename AnyT>
		class EquivalentTest;

		/** A singleton class that collects all defined unit tests.
		 */
		class CodeTestRegistry
		{
			std::list<CodeTest *> _code_tests;
			void add_test (CodeTest *);

			void _perform_all_tests ();

			friend class CodeTest;
		public:
			CodeTestRegistry ();

			/// Run all registered unit tests.
			static void perform_all_tests ();
		};

		/** A class that represents a single unit test.

		 A simple unit test might look like the following. Unit tests should be defined in .cpp files.

		 @code
		 UNIT_TEST(MyTestName)
		 {
			testing("Construction");

			MyClassBeingTesting t;
			check(t.good()) << "Object is good";
		 }
		 @endcode

		 Unit tests are run by compiling and linking the Support/CoreTestMain.cpp application.

		 @sa UNIT_TEST
		 */
		class CodeTest
		{
		public:
			std::string _name;

			class Statistics : public SharedObject {
			protected:
				int _failed;
				int _passed;
				std::string _name;

			public:
				Statistics (std::string test_name);
				void fail_test ();
				void pass_test ();
				void print_summary () const;

				void operator+= (const Statistics & other);
			};
			
			std::list<Ref<Statistics>> _tests;

			Ptr<Statistics> current_test ();
			Ref<Statistics> overall_statistics ();

			void print_summaries () const;

		protected:
			/// The actual function which is overridden to do the test.
			/// @sa UNIT_TEST
			virtual void test () abstract;

		public:
			CodeTest (std::string name);
			virtual ~CodeTest ();

			/// Used to indicate the next set of assertions are testing a particular feature or functionality.
			void testing (std::string test_name);
			
			/// A helper function to perform the test and print out statistics.
			virtual void perform_tests ();
			
			/// Logs errors that occur when performing unit tests
			class ErrorLogger
			{
				protected:
					bool _error;
				
				public:
					ErrorLogger (bool error);
					~ErrorLogger ();
					
					template <typename AnyT>
					ErrorLogger& operator<< (const AnyT & fragment)
					{
						if (_error)
							std::cerr << fragment;
						
						return *this;
					}
			};
			
			/// Assert that the value is true, and mark a failed test if it is not.
			class ErrorLogger check (bool condition);
		};
		
		std::ostream & operator<< (std::ostream & out, const std::type_info & rhs);
	}
}

#endif
