#define BOOST_TEST_NO_MAIN

#include <iostream>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include <stdio.h>

#include "../core/ServerHandler.h"

using namespace FinagleRegistryProxy;
using namespace std;

BOOST_AUTO_TEST_SUITE( task_suite )

struct F {
	F() {
	}
	~F() {
	}

	// RegistryCache* p;

};

BOOST_FIXTURE_TEST_CASE( empty , F) {
	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE( run_directly, F) {
	// Runnable *pr;
	ServerHandler*server = new ServerHandler("localhost:2181", 9009);

	ResetTask reload(server);
	reload.run();

	delete server;
	server = NULL;
	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE( run_by_runnalbe, F) {
	Runnable *pr;
	ServerHandler*server = new ServerHandler("localhost:2181", 9009);

	ResetTask reload(server);

	pr  = &reload;

	pr->run();

	delete pr;
	pr = NULL;

	delete server;
	server = NULL;
	BOOST_CHECK(true);
}


BOOST_AUTO_TEST_SUITE_END()
