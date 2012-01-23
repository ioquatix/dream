//
//  UnitTests.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 25/09/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//

#include <Dream/Core/CodeTest.h>
#include <iostream>

int main (int argc, char** args)
{
	using namespace std;

	cout << "Performing Unit Tests..." << endl;

	Dream::Core::CodeTestRegistry::performAllTests ();
}
