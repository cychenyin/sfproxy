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
		name="wake";
		interval_in_ms = 1000;
	}
	~F() {
	}

	string name;
	int interval_in_ms;
};

BOOST_FIXTURE_TEST_CASE( empty , F) {
	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE( run_directly, F) {
	// Runnable *pr;
//	shared_ptr<ServerHandler> server =shared_ptr<ServerHandler> (new ServerHandler("localhost:2181", 9009));
	// KeepWakeTask task(server, name, interval_in_ms);
//	task.run();
	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE( run_by_runnalbe, F) {
	Runnable *pr;
	shared_ptr<ServerHandler> server =shared_ptr<ServerHandler> (new ServerHandler("localhost:2181", 9009));
//	KeepWakeTask task(server, name, interval_in_ms);
//	pr  = &task;
//	pr->run();
//	delete pr;
//	pr = NULL;

	BOOST_CHECK(true);
}


BOOST_AUTO_TEST_SUITE_END()
