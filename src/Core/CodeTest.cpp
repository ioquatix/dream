/*
 *  Core/CodeTest.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 14/09/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#include "CodeTest.h"

#include "Strings.h"
#include "Singleton.h"

#include <iostream>

namespace Dream
{
	namespace Core
	{
		
#pragma mark -
		
		std::ostream & operator<< (std::ostream & out, const std::type_info & rhs)
		{
			return (out << "<typeinfo for " << rhs.name() << ">");
		}
		
#pragma mark -
		
		typedef Singleton<CodeTestRegistry> g_codeTestRegistry;

		CodeTestRegistry::CodeTestRegistry ()
		{
		}

		void CodeTestRegistry::addTest (CodeTest * test)
		{
			//if (test->m_name == "PixelBufferSaver Test")
			m_codeTests.push_back(test);
		}

		void CodeTestRegistry::_performAllTests ()
		{
			using namespace std;

			CodeTest::StatisticsPtr overall = CodeTest::StatisticsPtr(new CodeTest::Statistics("Code Test Registry"));

			foreach (CodeTest * test, m_codeTests)
			{
				test->performTests ();

				CodeTest::StatisticsPtr stats = test->overallStatistics ();
				*overall += *stats;
			}

			cout << endl << center(" Summary ", 60, '_') << endl;
			overall->printSummary();
		}

		void CodeTestRegistry::performAllTests ()
		{
			g_codeTestRegistry::instance()._performAllTests();
		}

		CodeTest::CodeTest (std::string name) : m_name (name)
		{
			g_codeTestRegistry::instance().addTest(this);
		}

		void CodeTest::testing (std::string testName)
		{
			m_tests.push_back (StatisticsPtr(new Statistics(testName)));
		}

		void CodeTest::assertTrue (bool condition, std::string testSummary)
		{
			if (condition == false)
			{
				std::cout << "Failed: " << testSummary << std::endl;
				std::cout << std::string(60, ' ') << "\t***" << std::endl;
				currentTest()->failTest();
			} else
			{
				//std::cout << "Passed: " << testSummary << std::endl;
				currentTest()->passTest();
			}
		}

		void CodeTest::assertFalse (bool condition, std::string testSummary)
		{
			assertTrue(!condition, testSummary);
		}

		void CodeTest::assertTrue (const Assertion & assertion)
		{
			assertTrue(assertion.test(), assertion.summary());
		}

		void CodeTest::assertFalse (const Assertion & assertion)
		{
			assertFalse(assertion.test(), assertion.summary());
		}

		void CodeTest::performTests ()
		{
			using namespace std;
			cout << endl << center((format(" %1% ") % m_name).str(), 60, '_') << endl;

			test();

			printSummaries();
		}

		CodeTest::Statistics::Statistics (std::string testName) : m_name (testName), m_failed (0), m_passed (0)
		{
		}

		void CodeTest::Statistics::failTest ()
		{
			m_failed += 1;
		}

		void CodeTest::Statistics::passTest ()
		{
			m_passed += 1;
		}

		void CodeTest::Statistics::operator+= (const Statistics & other)
		{
			m_passed += other.m_passed;
			m_failed += other.m_failed;
		}

		void CodeTest::Statistics::printSummary () const
		{
			std::cout << "[" << m_name << "] " << m_passed << " Passed";
			if (m_failed > 0)
				std::cout << " " << m_failed << " Failed ";

			std::cout << " out of " << m_failed + m_passed << " total" << std::endl;
		}

		CodeTest::StatisticsPtr CodeTest::currentTest ()
		{
			if (m_tests.size() == 0)
				testing ("Unnamed");

			return m_tests.back();
		}

		CodeTest::StatisticsPtr CodeTest::overallStatistics ()
		{
			StatisticsPtr overall = StatisticsPtr(new Statistics(m_name));

			foreach(StatisticsPtr stats, m_tests)
			{
				*overall += *stats;
			}

			return overall;
		}

		void CodeTest::printSummaries () const
		{
			foreach (StatisticsPtr stats, m_tests)
			{
				stats->printSummary();
			}
		}

		CodeTest::Assertion::~Assertion ()
		{
		}
	}
}