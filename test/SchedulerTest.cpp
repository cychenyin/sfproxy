#define BOOST_TEST_NO_MAIN

#define TASKSCHEDULER_STOP_WAIT_PERIOD_COUNT 0.01

#include <iostream>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include "../core/ServerHandler.h"

using namespace FinagleRegistryProxy;
using namespace std;

BOOST_AUTO_TEST_SUITE( scheduler )

class F {
public:
	ServerHandler *pserver;

public:
	F() {
		setup();
		count = 0;
	}
	~F() {
		count++;
		// delete pserver;
//		pserver = 0;
	}
	int count;
	void setup() {
		pserver = new ServerHandler("localhost", 9009);
	}
};

BOOST_FIXTURE_TEST_CASE( empty, F) {
//	BOOST_TEST_MESSAGE("test init ");
	BOOST_TEST_MESSAGE("empty start ===========");
	BOOST_CHECK(true);
	// cout << "test init by cout" << endl;
	BOOST_TEST_MESSAGE("empty end -------------");
}

BOOST_FIXTURE_TEST_CASE( init, F) {
	BOOST_TEST_MESSAGE("init start ===========");
	TaskScheduler *pts = pserver->get_scheduler();
	// BOOST_CHECK_EQUAL(pts->size(), 3);

	pts->clear();
	BOOST_CHECK_EQUAL(pts->size(), 0);

	BOOST_TEST_MESSAGE("init end -------------");
}

//BOOST_FIXTURE_TEST_CASE( start, F) {
//	BOOST_TEST_MESSAGE("start start ===========");
//	TaskScheduler *pts = pserver->get_scheduler();
//
//	pts->clear();
//	pserver->start_scheduler();
//
//	BOOST_CHECK(true);
//	BOOST_TEST_MESSAGE("start end -------------");
//}


//BOOST_FIXTURE_TEST_CASE( autosave, F) {
//	BOOST_TEST_MESSAGE("autosave start ===========");
//	TaskScheduler *pts = pserver->get_scheduler();
//	pts->clear();
//	AutoSaveTask* pt = new AutoSaveTask(pserver, "/tmp/frproxy.unittest");
//
//	pts->add(pt, 100); // 100ms
//	pserver->start_scheduler();
//
//	sleep(1);
//
//#ifdef DEBUG_
//	cout << "DEBUG_ definedddddddddddddddddddddddddddddddddddddddddddddd" << endl;
//#endif
//
//	BOOST_CHECK(true);
//
//	BOOST_TEST_MESSAGE("autosave end -------------");
//}

//BOOST_FIXTURE_TEST_CASE( add_taskq ,F) {
//}

//BOOST_FIXTURE_TEST_CASE( commit_autobreak ,F) {
//}
//
//BOOST_FIXTURE_TEST_CASE( commit_reset ,F) {
//}
//
//BOOST_FIXTURE_TEST_CASE( commit_save ,F) {
//}

BOOST_AUTO_TEST_SUITE_END()