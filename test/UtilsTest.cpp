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
#include <time.h>
#include <boost/test/unit_test.hpp>

#include "../frproxy.h"
#include "../core/ClientPool.h"

using namespace std;
using namespace FinagleRegistryProxy;

BOOST_AUTO_TEST_SUITE( utils_suite )

class F {
public:
	F() {
		// BOOST_TEST_MESSAGE("setup fixture");
	}
	~F() {
	}

};

BOOST_FIXTURE_TEST_CASE( multi_times_localtime_r , F) {
	BOOST_CHECK(true);

	time_t tt;
	struct tm vtm;
	time(&tt);
	localtime_r(&tt, &vtm);

//	cout << utils::tm_2_local(vtm) << " ========================" << endl;

	time_t tt2 = (time_t) mktime(const_cast<tm*>(&vtm));
	localtime_r(&tt2, &vtm);
//	cout << utils::tm_2_local(vtm) << " ========================" << endl;

	tt2 = (time_t) mktime(const_cast<tm*>(&vtm));
	localtime_r(&tt2, &vtm);
//	cout << utils::tm_2_local(vtm) << " ========================" << endl;

	tt2 = (time_t) mktime(const_cast<tm*>(&vtm));
	localtime_r(&tt2, &vtm);
//	cout << utils::tm_2_local(vtm) << " ========================" << endl;

	tt2 = (time_t) mktime(const_cast<tm*>(&vtm));
	localtime_r(&tt2, &vtm);
//	cout << utils::tm_2_local(vtm) << " ========================" << endl;
}

BOOST_FIXTURE_TEST_CASE( now_case , F) {
	string now1 = utils::time_stamp();
	string now2 = utils::get_current_time();
//	cout <<"now " << now1 << endl;
//	cout <<"cur " << now2 << endl;
	BOOST_CHECK_EQUAL(now1, now2);
}

BOOST_FIXTURE_TEST_CASE( multi_now_case, F) {

	for (int i = 0; i < 1000; i++) {
		utils::now_in_ms();
		// cout << "now_in_ms=" << utils::now_in_ms() << endl;
	}

	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE( unixtime_case, F) {

	// cout << "unixtime " << time(NULL) << endl;
	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE( from_unixtime_case, F) {
	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE( from_timestamp_case, F) {
	BOOST_CHECK(true);
}

BOOST_FIXTURE_TEST_CASE( now_us_case, F) {
	struct timeval tv;
	struct timezone tz;
	tz.tz_minuteswest = 0;
	tz.tz_dsttime = 0;
	gettimeofday(&tv, &tz);
	uint64_t t1 = (tv.tv_sec * (uint64_t) 1000000 + tv.tv_usec) / 1000LL;
	uint64_t t2 = (tv.tv_sec * (uint64_t) 1000000 + tv.tv_usec) / 1000;
	BOOST_CHECK_EQUAL(t1, t2);

	t1 = (tv.tv_sec * (uint64_t) 1000000 + tv.tv_usec) / 1000;
	t2 = tv.tv_sec * (uint64_t) 1000 + tv.tv_usec / 1000;
	BOOST_CHECK_EQUAL(t1, t2);

}
BOOST_FIXTURE_TEST_CASE( now_ms_case, F) {
	uint64_t now1 = utils::now_in_ms();
	now1 = now1 / 1000 * 1000;

	uint64_t now2 = utils::unix_time() * 1000;
//	cout << "now0 " << utils::now_in_ms() << endl;
//	cout << "now0 " << utils::now_in_us() << endl;
//	cout << "now0 " << utils::unix_time() << endl;
//	cout << "now1 " << now1 << endl;
//	cout << "now2 " << now2 << endl;
	BOOST_CHECK_EQUAL(now1, now2);

	struct timeval tv;
	struct timezone tz;
	tz.tz_minuteswest = 0;
	tz.tz_dsttime = 0;
	gettimeofday(&tv, &tz);
	// uint64_t time = tv.tv_sec * (uint64_t) 1000000 + tv.tv_usec;

	BOOST_CHECK_EQUAL(utils::unix_time(), tv.tv_sec);

}
BOOST_FIXTURE_TEST_CASE( unixtime_vs_now_in_ms, F) {
	uint64_t now_in_ms = utils::now_in_ms();
	uint64_t unixtime = utils::unix_time();
	BOOST_CHECK_EQUAL(unixtime , now_in_ms / 1000);
}

BOOST_AUTO_TEST_SUITE_END()
