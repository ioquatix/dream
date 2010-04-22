/*
	  CodeTestMain.cpp
	  Dream / Support

	  Created by Samuel Williams on 25/09/06.
	  Copyright 2006 Samuel WilliamsWilliams. All rights reserved.
 */

#include <Dream/Core/CodeTest.h>
#include <iostream>

int main (int argc, char** args)
{
	using namespace std;

	cout << "Performing Unit Tests..." << endl;

	Dream::Core::CodeTestRegistry::performAllTests ();
}
