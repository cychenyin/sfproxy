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

#include "../frproxy.h"

using namespace FinagleRegistryProxy;
using namespace std;

BOOST_AUTO_TEST_SUITE( your_suite_name )
static int counter = 0;
class F {
public:
	F() {
	}
	~F() {
		BOOST_TEST_MESSAGE("============== end of test method " << ++ counter);
	}
};

BOOST_FIXTURE_TEST_CASE( empty , F) {
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
