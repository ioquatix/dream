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
			m_codeTests.push_back(test);
		}

		void CodeTestRegistry::_performAllTests ()
		{
			using namespace std;
			
			REF(CodeTest::Statistics) overall = new CodeTest::Statistics("Code Test Registry");
			
			foreach (CodeTest * test, m_codeTests)
			{
				//if (test->m_name != "Cascade Test") continue;
				
				test->performTests ();

				REF(CodeTest::Statistics) stats = test->overallStatistics();
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
			m_tests.push_back(new Statistics(testName));
		}

		CodeTest::ErrorLogger CodeTest::check (bool condition)
		{
			if (condition == false) {
				currentTest()->failTest();
				
				std::cerr << "Test Failed: ";
				
				return ErrorLogger(true);
			} else {
				currentTest()->passTest();
				return ErrorLogger(false);
			}
		}

		void CodeTest::performTests ()
		{
			using namespace std;
			cout << endl << center(" " + m_name + " ", 60, '=') << endl;

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
		
		CodeTest::ErrorLogger::ErrorLogger (bool error)
			: m_error(error)
		{
		
		}
		
		CodeTest::ErrorLogger::~ErrorLogger ()
		{
			if (m_error)
				std::cerr << std::endl;
		}

		PTR(CodeTest::Statistics) CodeTest::currentTest ()
		{
			if (m_tests.size() == 0)
				testing ("Unnamed");

			return m_tests.back();
		}

		REF(CodeTest::Statistics) CodeTest::overallStatistics ()
		{
			REF(Statistics) overall = new Statistics(m_name);

			foreach(PTR(Statistics) stats, m_tests)
			{
				*overall += *stats;
			}

			return overall;
		}

		void CodeTest::printSummaries () const
		{
			foreach (PTR(Statistics) stats, m_tests)
			{
				stats->printSummary();
			}
		}

	}
}