/*
 *  Core/CodeTest.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 14/09/06.
 *  Copyright 2006 Samuel WilliamsWilliams. All rights reserved.
 *
 */

#ifndef _DREAM_CORE_CODETEST_H
#define _DREAM_CORE_CODETEST_H

#include "Core.h"
#include "../Class.h"

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

#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/mpl/if.hpp>

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
			std::list<CodeTest *> m_codeTests;
			void addTest (CodeTest *);

			void _performAllTests ();

			friend class CodeTest;
		public:
			CodeTestRegistry ();

			/// Run all registered unit tests.
			static void performAllTests ();
		};

		/** A class that represents a single unit test.

		 A simple unit test might look like the following. Unit tests should be defined in .cpp files.

		 @code
		 UNIT_TEST(MyTestName)
		 {
			testing("Construction");

			MyClassBeingTesting t;
			assertTrue(t.good(), "Object is good");
		 }
		 @endcode

		 Unit tests are run by compiling and linking the Support/CoreTestMain.cpp application.

		 @sa UNIT_TEST
		 */
		class CodeTest
		{
		public:
			std::string m_name;

			class Statistics
			{
			protected:
				int m_failed;
				int m_passed;
				std::string m_name;

			public:
				Statistics (std::string testName);
				void failTest ();
				void passTest ();
				void printSummary () const;

				void operator+= (const Statistics & other);
			};

			typedef shared_ptr<Statistics> StatisticsPtr;

			std::list<StatisticsPtr> m_tests;

			StatisticsPtr currentTest ();
			StatisticsPtr overallStatistics ();

			void printSummaries () const;

		protected:
			/// The actual function which is overridden to do the test.
			/// @sa UNIT_TEST
			virtual void test () abstract;

		public:
			class Assertion
			{
			public:
				virtual ~Assertion ();

				virtual std::string summary () const abstract;

				virtual bool test () const abstract;
			};

			CodeTest (std::string name);

			/// Used to indicate the next set of assertions are testing a particular feature or functionality.
			void testing (std::string testName);

			/// Assert that the value is true, and mark a failed test if it is not.
			void assertTrue (bool condition, std::string testSummary);

			void assertTrue (const Assertion & assertion);

			/// Assert that the value is false, and mark a failed test if it is not.
			void assertFalse (bool condition, std::string testSummary);

			void assertFalse (const Assertion & assertion);

			/// Assert that the values are equal, and mark a failed test if they are not.
			template <typename LhsT, typename RhsT>
			void assertEqual (const LhsT & lhs, const RhsT & rhs, std::string testSummary);

			/// Assert that the values are not equal, and mark a failed test if they are.
			template <typename LhsT, typename RhsT>
			void assertNotEqual (const LhsT & lhs, const RhsT & rhs, std::string testSummary);
			
			/// A helper function to perform the test and print out statistics.
			virtual void performTests ();

		};
		
		template <typename LhsT, typename RhsT>
		bool testEquivalency (const LhsT & lhs, const RhsT & rhs)
		{
			return lhs == rhs;
		}
		
		std::ostream & operator<< (std::ostream & out, const std::type_info & rhs);
		
		template <typename LhsT, typename RhsT>
		void CodeTest::assertEqual (const LhsT & lhs, const RhsT & rhs, std::string testSummary)
		{
			if (testEquivalency(lhs, rhs))
			{
				assertTrue(true, testSummary);
				return;
			}

			std::stringstream s;
			s << lhs << " == " << rhs << " failed: " << testSummary;

			assertTrue(false, s.str());
		}

		template <typename LhsT, typename RhsT>
		void CodeTest::assertNotEqual (const LhsT & lhs, const RhsT & rhs, std::string testSummary)
		{
			if (!testEquivalency(lhs, rhs))
			{
				assertTrue(true, testSummary);
				return;
			}

			std::stringstream s;
			s << lhs << " != " << rhs << " failed: " << testSummary;

			assertTrue(false, s.str());
		}

	}
}

#endif