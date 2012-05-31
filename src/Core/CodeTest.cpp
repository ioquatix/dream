//
//  Core/CodeTest.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 14/09/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#include "CodeTest.h"

#include "Strings.h"

#include <iostream>

namespace Dream
{
	namespace Core
	{
		
// MARK: mark -
		
		std::ostream & operator<< (std::ostream & out, const std::type_info & rhs)
		{
			return (out << "<typeinfo for " << rhs.name() << ">");
		}
		
// MARK: mark -
		
		CodeTestRegistry * shared_code_test_registry () {
			static CodeTestRegistry * code_test_registry = NULL;
			
			if (!code_test_registry) {
				code_test_registry = new CodeTestRegistry;
			}
			
			return code_test_registry;
		}

		CodeTestRegistry::CodeTestRegistry ()
		{
		}
		
		void CodeTestRegistry::add_test (CodeTest * test)
		{
			_code_tests.push_back(test);
		}

		void CodeTestRegistry::_perform_all_tests ()
		{
			using namespace std;
			
			Ref<CodeTest::Statistics> overall = new CodeTest::Statistics("Code Test Registry");
			
			foreach (test, _code_tests)
			{
				if ((*test)->_name != "Object Test") continue;
				
				(*test)->perform_tests ();

				Ref<CodeTest::Statistics> stats = (*test)->overall_statistics();
				*overall += *stats;
			}

			cout << endl << center(" Summary ", 60, '_') << endl;
			overall->print_summary();
		}
		
		void CodeTestRegistry::perform_all_tests ()
		{
			shared_code_test_registry()->_perform_all_tests();
		}

		CodeTest::CodeTest (std::string name) : _name (name)
		{
			shared_code_test_registry()->add_test(this);
		}
		
		CodeTest::~CodeTest ()
		{
		}

		void CodeTest::testing (std::string test_name)
		{
			_tests.push_back(new Statistics(test_name));
		}

		CodeTest::ErrorLogger CodeTest::check (bool condition)
		{
			if (condition == false) {
				current_test()->fail_test();
				
				std::cerr << "Test Failed: ";
				
				return ErrorLogger(true);
			} else {
				current_test()->pass_test();
				return ErrorLogger(false);
			}
		}

		void CodeTest::perform_tests ()
		{
			using namespace std;
			cout << endl << center(" " + _name + " ", 60, '=') << endl;

			test();

			print_summaries();
		}

		CodeTest::Statistics::Statistics (std::string test_name) : _name (test_name), _failed (0), _passed (0)
		{
		}

		void CodeTest::Statistics::fail_test ()
		{
			_failed += 1;
		}

		void CodeTest::Statistics::pass_test ()
		{
			_passed += 1;
		}

		void CodeTest::Statistics::operator+= (const Statistics & other)
		{
			_passed += other._passed;
			_failed += other._failed;
		}

		void CodeTest::Statistics::print_summary () const
		{
			std::cout << "[" << _name << "] " << _passed << " Passed";
			if (_failed > 0)
				std::cout << " " << _failed << " Failed ";

			std::cout << " out of " << _failed + _passed << " total" << std::endl;
		}
		
		CodeTest::ErrorLogger::ErrorLogger (bool error)
			: _error(error)
		{
		
		}
		
		CodeTest::ErrorLogger::~ErrorLogger ()
		{
			if (_error)
				std::cerr << std::endl;
		}

		Ptr<CodeTest::Statistics> CodeTest::current_test ()
		{
			if (_tests.size() == 0)
				testing ("Unnamed");

			return _tests.back();
		}

		Ref<CodeTest::Statistics> CodeTest::overall_statistics ()
		{
			Ref<Statistics> overall = new Statistics(_name);

			foreach(stats, _tests)
			{
				*overall += **stats;
			}

			return overall;
		}

		void CodeTest::print_summaries () const
		{
			foreach (stats, _tests)
			{
				(*stats)->print_summary();
			}
		}

	}
}
