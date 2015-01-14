/*
 * testtemplate.cpp
 *
 *  Created on: Jan 14, 2015
 *      Author: asdf
 */

#define BOOST_TEST_NO_MAIN

#include <iostream>
#include <sstream>
#include <stdio.h>

#include <boost/test/unit_test.hpp>

// using namespace FinagleRegistryProxy;
using namespace std;

BOOST_AUTO_TEST_SUITE( your_suite_name )

class F {
public:
	F() {
		// BOOST_TEST_MESSAGE("setup fixture");
	}
	~F() {
	}

};

BOOST_FIXTURE_TEST_CASE( empty , F) {
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
